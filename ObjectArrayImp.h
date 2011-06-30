/*
 * Copyright 2010 Esrille Inc.
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

#ifndef OBJECTARRAY_IMP_H
#define OBJECTARRAY_IMP_H

#include <Object.h>
#include <org/w3c/dom/ObjectArray.h>

#include <deque>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

template <typename IMP, typename E, std::deque<E> IMP::* array>
class ObjectArrayImp : public Object
{
    Object imp;
public:
    virtual unsigned int getLength() {
        return (static_cast<IMP*>(imp.self())->*array).size();
    }
    virtual void setLength(unsigned int length) {
    }
    virtual E getElement(unsigned int index) {
        if (getLength() <= index)
            return 0;
        return (static_cast<IMP*>(imp.self())->*array)[index];
    }
    virtual void setElement(unsigned int index, E value) {
    }
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return ObjectArray<E>::dispatch(this, selector, id, argc, argv);
    }
    ObjectArrayImp(IMP* imp) :
        Object(this),
        imp(imp) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // OBJECTARRAY_IMP_H