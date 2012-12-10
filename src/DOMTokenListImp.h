/*
 * Copyright 2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_DOMTOKENLISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_DOMTOKENLISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/DOMTokenList.h>

#include <vector>
#include "ElementImp.h"

#include <org/w3c/dom/Element.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class ElementImp;

class DOMTokenListImp : public ObjectMixin<DOMTokenListImp>
{
    Element element;
    std::u16string localName;
    std::vector<std::u16string> tokens;

    void update();

public:
    DOMTokenListImp(ElementImp* element, const std::u16string& localName);

    // DOMTokenList
    unsigned int getLength();
    Nullable<std::u16string> item(unsigned int index);
    bool contains(std::u16string token);
    void add(Variadic<std::u16string> tokens = Variadic<std::u16string>());
    void remove(Variadic<std::u16string> tokens = Variadic<std::u16string>());
    bool toggle(std::u16string token);
    bool toggle(std::u16string token, bool force);
    std::u16string toString();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DOMTokenList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DOMTokenList::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_DOMTOKENLISTIMP_H_INCLUDED
