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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLCOLLECTIONIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLCOLLECTIONIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLCollection.h>

#include <org/w3c/dom/Element.h>

#include <map>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLCollectionImp : public ObjectMixin<HTMLCollectionImp>
{
    std::multimap<const std::u16string, Element> map;
public:
    virtual ~HTMLCollectionImp();
    void addItem(const std::u16string& name, Element element);

    // HTMLCollection
    unsigned int getLength();
    Element item(unsigned int index);
    Object namedItem(std::u16string name);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLCollection::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLCollection::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLCOLLECTIONIMP_H_INCLUDED
