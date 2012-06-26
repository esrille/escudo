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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLTABLECELLELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLTABLECELLELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLTableCellElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/DOMSettableTokenList.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLTableCellElementImp : public ObjectMixin<HTMLTableCellElementImp, HTMLElementImp>
{
public:
    HTMLTableCellElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
        ObjectMixin(ownerDocument, localName)
    {
    }
    HTMLTableCellElementImp(HTMLTableCellElementImp* org, bool deep) :
        ObjectMixin(org, deep)
    {
    }

    virtual void eval();

    // HTMLTableCellElement
    unsigned int getColSpan();
    void setColSpan(unsigned int colSpan);
    unsigned int getRowSpan();
    void setRowSpan(unsigned int rowSpan);
    DOMSettableTokenList getHeaders();
    void setHeaders(std::u16string headers);
    int getCellIndex();
    // HTMLTableCellElement-32
    std::u16string getAbbr();
    void setAbbr(std::u16string abbr);
    std::u16string getAlign();
    void setAlign(std::u16string align);
    std::u16string getAxis();
    void setAxis(std::u16string axis);
    std::u16string getBgColor();
    void setBgColor(std::u16string bgColor);
    std::u16string getCh();
    void setCh(std::u16string ch);
    std::u16string getChOff();
    void setChOff(std::u16string chOff);
    std::u16string getHeight();
    void setHeight(std::u16string height);
    bool getNoWrap();
    void setNoWrap(bool noWrap);
    std::u16string getVAlign();
    void setVAlign(std::u16string vAlign);
    std::u16string getWidth();
    void setWidth(std::u16string width);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLTableCellElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLTableCellElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLTABLECELLELEMENTIMP_H_INCLUDED
