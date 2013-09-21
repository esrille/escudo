/*
 * Copyright 2012, 2013 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLTABLESECTIONELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLTABLESECTIONELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLTableSectionElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLCollection.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLTableSectionElementImp : public ObjectMixin<HTMLTableSectionElementImp, HTMLElementImp>
{
public:
    HTMLTableSectionElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
        ObjectMixin(ownerDocument, localName)
    {
    }

    virtual void handleMutation(events::MutationEvent mutation);

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLTableSectionElementImp>(*this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // HTMLTableSectionElement
    html::HTMLCollection getRows();
    html::HTMLElement insertRow();
    html::HTMLElement insertRow(int index);
    void deleteRow(int index);
    // HTMLTableSectionElement-31
    std::u16string getAlign();
    void setAlign(const std::u16string& align);
    std::u16string getCh();
    void setCh(const std::u16string& ch);
    std::u16string getChOff();
    void setChOff(const std::u16string& chOff);
    std::u16string getVAlign();
    void setVAlign(const std::u16string& vAlign);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLTableSectionElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLTableSectionElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLTABLESECTIONELEMENTIMP_H_INCLUDED
