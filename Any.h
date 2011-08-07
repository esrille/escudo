/*
 * Copyright 2011 Esrille Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ES_ANY_H
#define ES_ANY_H

#include <new>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <type_traits>

#include <nullable.h>

class Object;

// The Any type for Web IDL
class Any
{
    // The type of the heap for the data stored in an Any instance.
    // Note an Any instance cannot contain an object that is larger
    // than the size of Heap.
    union Heap {
        int32_t  i32;
        uint32_t u32;
        int64_t  i64;
        uint64_t u64;
        float    f32;
        double   f64;
        char     s128[16];  // TODO: This is for Object on x86_64.
    };

    // The virtual table for non-primitive types
    struct VirtualTable {
        const std::type_info& (*getType)();
        void (*destruct)(Heap*);
        void (*clone)(Heap const*, Heap*);
    };

    // The type erasure for non-primitive types
    template<typename T>
    struct TypeErasure {
        static VirtualTable vtable;
        static const std::type_info& getType() {
            return typeid(T);
        }
        static void destruct(Heap* value) {
            reinterpret_cast<T*>(value)->~T();
        }
        static void clone(Heap const* src, Heap* dest) {
            new(dest) T(*reinterpret_cast<T const*>(src));
        }
    };

public:
    enum {
        Undefined,
        Null,
        Bool,
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float32,
        Float64,
        Dynamic  // i.e., not a primitive value
    };

private:
    unsigned type;
    VirtualTable* vtable;
    Heap heap;

#ifdef HAVE_NULLPTR
    void initialize(std::nullptr_t value) {
        type = Null;
    }
#endif

    template<typename T>
    void initialize(T const& value, typename std::enable_if<!std::is_base_of<Object, T>::value>::type* =0) {
        vtable = &TypeErasure<T>::vtable;
        static_assert(sizeof(T) <= sizeof(heap), "size mismatch");
        new(&heap) T(value);
        type = Dynamic;
    }

    template<typename T>
    void initialize(T const& value, typename std::enable_if<std::is_base_of<Object, T>::value>::type* =0) {
        if (!value)
            type = Null;
        else {
            vtable = &TypeErasure<Object>::vtable;
            new(&heap) Object(value.self());
            type = Dynamic;
        }
    }

    template<typename T>
    void initialize(T* value, typename std::enable_if<std::is_base_of<Object, T>::value>::type* =0) {
        if (!value)
            type = Null;
        else {
            vtable = &TypeErasure<Object>::vtable;
            new(&heap) Object(value);
            type = Dynamic;
        }
    }

    template <typename T>
    void initialize(const Nullable<T> nullable) {
        if (!nullable.hasValue())
            type = Null;
        else
            initialize(nullable.value());
    }

    void initialize(const Any& value) {
        type = value.type;
        if (type != Dynamic)
            heap = value.heap;
        else {
            vtable = value.vtable;
            vtable->clone(&value.heap, &heap);
        }
    }

    void initialize(bool value) {
        heap.i32 = value;
        type = Bool;
    }
    void initialize(unsigned char value) {
        heap.u32 = value;
        type = Uint32;
    }
    void initialize(signed char value) {
        heap.i32 = value;
        type = Int32;
    }
    void initialize(unsigned short value) {
        heap.u32 = value;
        type = Uint32;
    }
    void initialize(signed short value) {
        heap.i32 = value;
        type = Int32;
    }
    void initialize(unsigned int value) {
        heap.u32 = value;
        type = Uint32;
    }
    void initialize(signed int value) {
        heap.i32 = value;
        type = Int32;
    }
    void initialize(unsigned long long value) {
        heap.u64 = value;
        type = Uint64;
    }
    void initialize(signed long long value) {
        heap.i64 = value;
        type = Int64;
    }
    void initialize(float value) {
        heap.f32= value;
        type = Float32;
    }
    void initialize(double value) {
        heap.f64 = value;
        type = Float64;
    }
    void initialize(const char* x) {
        if (x)
            initialize(std::string(x));
        else
            type = Null;
    }
    void initialize(char* x) {
        initialize(const_cast<const char*>(x));
    }
    void initialize(const char16_t* x) {
        if (x)
            initialize(std::u16string(x));
        else
            type = Null;
    }
    void initialize(char16_t* x) {
        initialize(const_cast<const char16_t*>(x));
    }

    template<typename T>
    T cast(typename std::enable_if<std::is_arithmetic<T>::value>::type* = 0) const {
        switch (type) {
        case Bool:
        case Int32:
            return static_cast<T>(heap.i32);
        case Uint32:
            return static_cast<T>(heap.u32);
        case Int64:
            return static_cast<T>(heap.i64);
        case Uint64:
            return static_cast<T>(heap.u64);
        case Float32:
            return static_cast<T>(heap.f32);
        case Float64:
            return static_cast<T>(heap.f64);
        default:
            return static_cast<T>(toNumber());
        }
    }

    template<typename T>
    const T cast(typename std::enable_if<std::is_base_of<Object, T>::value>::type* = 0) const {
        if (isObject())
            return toObject();
        if (type == Null)
            return 0;
        throw std::bad_cast();
    }

    template<typename T>
    const T cast(typename std::enable_if<!std::is_base_of<Object, T>::value && !std::is_arithmetic<T>::value>::type* = 0) const {
        if (type == Dynamic && vtable->getType() == typeid(T))
            return *reinterpret_cast<const T*>(&heap);
        throw std::bad_cast();
    }

public:
    Any() :
        type(Undefined)
    {
    }

    Any(const Any& value) {
        initialize(value);
    }

    template <typename T>
    Any(const T& x) {
        initialize(x);
    }

    ~Any() {
        if (type == Dynamic)
            vtable->destruct(&heap);
    }

    Any& operator=(const Any& x) {
        if (this != &x) {
            if (type == Dynamic)
                vtable->destruct(&heap);
            initialize(x);
        }
        return *this;
    }

    template<typename T>
    Any& operator=(const T& x) {
        if (type == Dynamic)
            vtable->destruct(&heap);
        initialize(x);
        return *this;
    }

    template<typename T>
    T as() const {
        return cast<T>();
    }

    template<typename T>
    operator T() const {
        return cast<T>();
    }

    bool isPrimitive() const {
        switch (type) {
        case Bool:
        case Int32:
        case Uint32:
        case Int64:
        case Uint64:
        case Float32:
        case Float64:
            return true;
        default:
            return false;
        }
    }

    bool isObject() const;
    Object* toObject() const;

    bool toBoolean() const;
    double toNumber() const;

    bool isString() const;
    std::u16string toString() const;

    bool isUndefined() const {
        return type == Undefined;
    }
    bool isNull() const {
        return type == Null;
    }

    unsigned getType() const {
        return type;
    }
};

// TypeErasure vtable initializer
template<class T>
Any::VirtualTable Any::TypeErasure<T>::vtable = {
    Any::TypeErasure<T>::getType,
    Any::TypeErasure<T>::destruct,
    Any::TypeErasure<T>::clone
};

template <typename T>
Nullable<T>::Nullable(const Any& any)
{
    hasValue_ = !any.isNull();
    if (hasValue_)
        value_ = any.as<T>();
}

#endif // ESNPAPI_ANY_H_INCLUDED
