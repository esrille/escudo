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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLHRELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLHRELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLHRElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLHRElementImp : public ObjectMixin<HTMLHRElementImp, HTMLElementImp>
{
public:
    void eval();

    // HTMLHRElement
    // HTMLHRElement-14
    std::u16string getAlign();
    void setAlign(std::u16string align);
    std::u16string getColor();
    void setColor(std::u16string color);
    bool getNoShade();
    void setNoShade(bool noShade);
    std::u16string getSize();
    void setSize(std::u16string size);
    std::u16string getWidth();
    void setWidth(std::u16string width);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLHRElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLHRElement::getMetaData();
    }
    HTMLHRElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"hr") {
    }
    HTMLHRElementImp(HTMLHRElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLHRELEMENTIMP_H_INCLUDED
