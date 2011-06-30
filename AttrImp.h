/*
 * Copyright 2010, 2011 Esrille Inc.
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

#ifndef ATTR_IMP_H
#define ATTR_IMP_H

#include <Object.h>
#include <org/w3c/dom/Attr.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class AttrImp : public ObjectMixin<AttrImp>
{
private:
    Nullable<std::u16string> namespaceURI;
    Nullable<std::u16string> prefix;
    std::u16string localName;
    std::u16string value;

public:
    // Attr
    virtual Nullable<std::u16string> getNamespaceURI();
    virtual Nullable<std::u16string> getPrefix();
    virtual std::u16string getLocalName();
    virtual std::u16string getName();
    virtual std::u16string getValue();
    virtual void setValue(std::u16string value);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return Attr::dispatch(this, selector, id, argc, argv);
    }

    AttrImp(Nullable<std::u16string> namespaceURI, Nullable<std::u16string> prefix, std::u16string localName, std::u16string value);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ATTR_IMP_H
