/*
 * Copyright 2011 Esrille Inc.
 * Copyright 2010 Google Inc.
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

#ifndef ORG_W3C_DOM_OBJECTARRAY_INCLUDED
#define ORG_W3C_DOM_OBJECTARRAY_INCLUDED

#include <Object.h>

namespace org
{
namespace w3c
{
namespace dom
{

template <typename E>
class ObjectArray : public Object
{
public:
    unsigned int getLength()
    {
        return static_cast<unsigned int>(message_(0x957e2539, "getLength", GETTER_, 0));
    }
    void setLength(unsigned int length)
    {
        Any argument;
        argument = length;
        message_(0x957e2539, "setLength", SETTER_, &argument);
    }
    E getElement(unsigned int index)
    {
        Any argument;
        argument = index;
        Any e = message_(0x58994e97, "getElement", SPECIAL_GETTER_, &argument);
        return e.as<E>();
    }
    void setElement(unsigned int index, E value)
    {
        Any arguments[2];
        arguments[1] = index;
        arguments[2] = value;
        message_(0x58994e97, "setElement", SPECIAL_SETTER_, &arguments);
    }
    ObjectArray(Object* object) :
        Object(object)
    {
    }
    ObjectArray(const ObjectArray& object) :
        Object(object)
    {
    }
    ObjectArray& operator=(const ObjectArray& object)
    {
        Object::operator =(object);
        return *this;
    }
    template <class IMP>
    static Any dispatch(IMP* self, uint32_t selector, const char* id, int argumentCount, Any* arguments)
    {
        switch (selector)
        {
        case 0x957e2539:
            if (argumentCount == GETTER_)
                return self->getLength();
            if (argumentCount == SETTER_)
                self->setLength(arguments[0]);
            if (argumentCount == HAS_PROPERTY_)
                return true;
            break;
        case 0x58994e97:
            if (argumentCount == SPECIAL_GETTER_)
                return self->getElement(arguments[0]);
            if (argumentCount == SPECIAL_SETTER_)
                self->setElement(arguments[0], arguments[1].as<E>());
            if (argumentCount == HAS_OPERATION_)
                return true;
            break;
        case 0x0:
            if (argumentCount == IS_KIND_OF_)
                return getPrefixedName() == id || !std::strcmp(getPrefixedName(), id);
            break;
        }
        return Any();
    };
    static const char* const getPrefixedName()
    {
        static const char* const prefixedName = "::org::w3c::dom::ObjectArray";  // TODO: include <E>
        return prefixedName;
    }
    static bool hasInstance(Object& object)
    {
        return static_cast<bool>(object.message_(0, getPrefixedName(), IS_KIND_OF_, 0));
    }
    static bool hasInstance(Object* object)
    {
        return object && static_cast<bool>(object->message_(0, getPrefixedName(), IS_KIND_OF_, 0));
    }
};

}
}
}

#endif  // ORG_W3C_DOM_OBJECTARRAY_INCLUDED
