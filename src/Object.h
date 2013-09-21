/*
 * Copyright 2011-2013 Esrille Inc.
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

#ifndef ES_OBJECT_H
#define ES_OBJECT_H

#include <cstring>
#include <memory>
#include <type_traits>

#include <Any.h>
#include <nullable.h>
#include <sequence.h>
#include <Variadic.h>

class Imp : public std::enable_shared_from_this<Imp>
{
public:
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return nullptr;
    }
    // Note this->self() cannot be used inside the constructor.
    std::shared_ptr<Imp> self() {
        return shared_from_this();
    }
};

template<class O> class Weak
{
    std::weak_ptr<Imp> pimpl;
public:
    Weak() = default;
    Weak(std::nullptr_t) {}
    Weak(O const& other) : pimpl(other.self()) {}
    Weak& operator=(O const& other) {
        pimpl = other.self();
        return *this;
    }

    void reset() {
        pimpl.reset();
    }
    bool expired() const {
        return pimpl.expired();
    }
    O lock() const {
        return O(pimpl.lock());
    }
};

class Object
{
protected:
    std::shared_ptr<Imp> pimpl;

public:
    Object() = default;
    Object(const Object& object) = default;
    Object(Object&& object) = default;
    Object& operator=(const Object& other) = default;
    Object& operator=(Object&& other) = default;
    ~Object() = default;

    Object(const Object* object) {
        if (object && object->pimpl)
            pimpl = object->pimpl;
    }
    explicit Object(Imp* pimpl) : pimpl(pimpl) {}
    Object(std::nullptr_t) {}

    template <class IMP>
    Object(const std::shared_ptr<IMP>& pimpl) : pimpl(std::static_pointer_cast<Imp>(pimpl)) {}
    Object(const std::shared_ptr<Imp>& pimpl) : pimpl(pimpl) {}
    template <class IMP>
    Object(std::shared_ptr<IMP>&& pimpl) : pimpl(std::static_pointer_cast<Imp>(pimpl)) {}
    Object(std::shared_ptr<Imp>&& pimpl) : pimpl(pimpl) {}

    template <class IMP>
    Object& operator=(const std::shared_ptr<IMP>& other) {
        pimpl = other;
        return *this;
    }
    Object& operator=(const std::shared_ptr<Imp>& other) {
        pimpl = other;
        return *this;
    }
    template <class IMP>
    Object& operator=(std::shared_ptr<IMP>&& other) {
        pimpl = other;
        return *this;
    }
    Object& operator=(std::shared_ptr<Imp>&& other) {
        pimpl = other;
        return *this;
    }

    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        if (pimpl)
            return pimpl->message_(selector, id, argc, argv);
        return Any();
    }
    bool operator!() const {
        return !pimpl;
    }
    explicit operator bool() const {
        return pimpl.get();
    }
    std::shared_ptr<Imp> self() const {
        return pimpl;
    }

    bool operator==(const Object& other) const
    {
        return self() == other.self();
    }
    bool operator!=(const Object& other) const
    {
        return self() != other.self();
    }
    bool operator<(const Object& other) const
    {
        return self() < other.self();
    }

    // Predefined argument count
    static const int GETTER_ = -1;
    static const int SETTER_ = -2;
    static const int SPECIAL_GETTER_ = -3;
    static const int SPECIAL_DELETER_ = -4;
    static const int SPECIAL_SETTER_ = -5;
    static const int SPECIAL_CREATOR_ = -6;
    static const int SPECIAL_SETTER_CREATOR_ = -7;
    static const int HAS_PROPERTY_ = -8;
    static const int HAS_OPERATION_ = -9;
    static const int IS_KIND_OF_ = -10;
    static const int STRINGIFY_ = -11;
    static const int CALLBACK_ = 0x4000000;

    // Max # of variadic arguments
    static const int MAX_VARIADIC_ = 16;
};

template <typename X>
X interface_cast(const Object& object)
{
    return X(object.self());
}

template <typename X>
X interface_cast(const Object* object)
{
    return X(object->self());
}

class ObjectImp : public Imp
{
    void* privateDate;

public:
    ObjectImp() : privateDate(0) {}
    void* getPrivate() const {
        return privateDate;
    }
    void setPrivate(void* data) {
        privateDate = data;
    }
    virtual void* getStaticPrivate() const = 0;
};

template <typename T, typename B = ObjectImp>
class ObjectMixin : public B
{
    static void* staticPrivate;
protected:
    ObjectMixin() :
        B()
    {
    }

    template<class... As>
    ObjectMixin(As&&... as) :
        B(std::forward<As>(as)...)
    {
    }

public:
    virtual void* getStaticPrivate() const {
        return staticPrivate;
    }

    static void setStaticPrivate(void* p) {
        staticPrivate = p;
    }
};

template <typename IMP>
class Retained : public std::shared_ptr<IMP>
{
public:
    Retained() :
        std::shared_ptr<IMP>(std::make_shared<IMP>())
    {
        static_assert(std::is_base_of<Imp, IMP>::value, "not Imp");
    }
    template<class... As>
    Retained(As&&... as) :
        std::shared_ptr<IMP>(std::make_shared<IMP>(std::forward<As>(as)...))
    {
        static_assert(std::is_base_of<Imp, IMP>::value, "not Imp");
    }
};

template <typename T, typename B>
void* ObjectMixin<T, B>::staticPrivate = 0;

template<typename T>
void Any::initialize(T const& value, typename std::enable_if<!std::is_base_of<Object, T>::value>::type*)
{
    vtable = &TypeErasure<T>::vtable;
    static_assert(sizeof(T) <= sizeof(heap), "size mismatch");
    new(&heap) T(value);
    type = Dynamic;
}

template<typename T>
void Any::initialize(T const& value, typename std::enable_if<std::is_base_of<Object, T>::value>::type*)
{
    if (!value)
        type = Null;
    else {
        vtable = &TypeErasure<Object>::vtable;
        new(&heap) Object(value);
        type = Dynamic;
    }
}

template<typename T>
void Any::initialize(T* value, typename std::enable_if<std::is_base_of<Object, T>::value>::type*)
{
    if (!value)
        type = Null;
    else {
        vtable = &TypeErasure<Object>::vtable;
        new(&heap) Object(value);
        type = Dynamic;
    }
}

#endif  // ES_OBJECT_H
