/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include <Any.h>
#include <nullable.h>
#include <sequence.h>
#include <Variadic.h>

#include <atomic>
#include <type_traits>

#include <assert.h>
#include <cstring>

class Object
{
protected:
    Object* object;

public:
    virtual unsigned int retain_() {
        if (object && object != this)
            return object->retain_();
        return 1;
    }
    virtual unsigned int release_() {
        if (object && object != this) {
            unsigned count = object->release_();
            if (0 < count)
                return count;
            object = 0;
        }
        return 0;
    }

public:
    Object() :
        object(0)
    {
    }
    Object(Object* other) :
        object(other)
    {
        if (other)
            other->retain_();
    }
    Object(const Object& other) :
        object(other.object)
    {
        const_cast<Object*>(&other)->retain_();
    }
    virtual ~Object() {
        release_();
    }
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        if (object && object != this)
            return object->message_(selector, id, argc, argv);
        assert(!object);    // forwarding a message to self
        return Any();
    }
    bool operator!() const {
        return !object;
    }
    operator void*( ) const {
        return object;
    }
    Object* self() const {
        return object;
    }
    Object& operator=(const Object& other)
    {
        if (this != &other) {
            const_cast<Object*>(&other)->retain_();
            release_();
            object = other.self();
        }
        return *this;
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

class ObjectImp : public Object
{
    std::atomic_uint count;
    void* privateDate;
public:
    unsigned int count_() const {
        return count;
    }
    virtual unsigned int retain_() {
        return ++count;
    };
    virtual unsigned int release_() {
        if (0 < count)
            --count;
        if (count == 0) {
            delete this;
            return 0;
        }
        return count;
    };
    ObjectImp() :
        Object(this),
        count(0),
        privateDate(0) {
    }
    ObjectImp(const ObjectImp& other) :
        Object(this),
        count(0),
        privateDate(0) {
    }
public:
    void* getPrivate() const {
        return privateDate;
    }
    void setPrivate(void* data) {
        privateDate = data;
    }
    virtual void* getStaticPrivate() const = 0;
};

inline void intrusive_ptr_add_ref(ObjectImp* imp)
{
    imp->retain_();
}

inline void intrusive_ptr_release(ObjectImp* imp)
{
    imp->release_();
}

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

template <typename T>
class Retained : public T
{
public:
    Retained()
    {
        static_assert(std::is_base_of<ObjectImp, T>::value, "not ObjectImp");
        T::retain_();
    }
    template<class... As>
    Retained(As&&... as) :
        T(std::forward<As>(as)...)
    {
        static_assert(std::is_base_of<ObjectImp, T>::value, "not ObjectImp");
        T::retain_();
    }
};

template <typename T, typename B>
void* ObjectMixin<T, B>::staticPrivate = 0;

#endif  // ES_OBJECT_H
