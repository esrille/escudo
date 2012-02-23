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

#include "HTMLTableCellElementImp.h"

#include "HTMLTableElementImp.h"
#include "css/CSSTokenizer.h"

#include "utf.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLTableCellElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalBackground(this);
    HTMLElementImp::evalBgcolor(this);
    HTMLElementImp::evalHeight(this);
    HTMLElementImp::evalWidth(this);
    HTMLElementImp::evalNoWrap(this);

    for (Element e = getParentElement(); e; e = e.getParentElement()) {
        if (html::HTMLTableElement::hasInstance(e)) {
            if (e.hasAttribute(u"border")) {
                css::CSSStyleDeclaration style = getStyle();
                style.setProperty(u"border-width", u"1px", u"non-css");
                style.setProperty(u"border-style", u"inset", u"non-css");
            }
        }
    }
}

unsigned int HTMLTableCellElementImp::getColSpan()
{
    std::u16string value = getAttribute(u"colspan");
    if (value.empty())
        return 1;
    // TODO: Use the rules for parsing non-negative integers of HTML.
    stripLeadingAndTrailingWhitespace(value);
    unsigned int colspan = CSSTokenizer::parseInt(value.c_str(), value.length());
    if (colspan == 0)
        colspan = 1;
    return colspan;
}

void HTMLTableCellElementImp::setColSpan(unsigned int colSpan)
{
    setAttribute(u"colspan", toString(colSpan));
}

unsigned int HTMLTableCellElementImp::getRowSpan()
{
    std::u16string value = getAttribute(u"rowspan");
    if (value.empty())
        return 1;
    // TODO: Use the rules for parsing non-negative integers of HTML.
    stripLeadingAndTrailingWhitespace(value);
    unsigned int rowspan = CSSTokenizer::parseInt(value.c_str(), value.length());
    if (rowspan == 0)
        rowspan = 1;
    return rowspan;
}

void HTMLTableCellElementImp::setRowSpan(unsigned int rowSpan)
{
    setAttribute(u"rowspan", toString(rowSpan));
}

DOMSettableTokenList HTMLTableCellElementImp::getHeaders()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableCellElementImp::setHeaders(std::u16string headers)
{
    // TODO: implement me!
}

int HTMLTableCellElementImp::getCellIndex()
{
    // TODO: implement me!
    return 0;
}

std::u16string HTMLTableCellElementImp::getAbbr()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setAbbr(std::u16string abbr)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getAxis()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setAxis(std::u16string axis)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setBgColor(std::u16string bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getCh()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setCh(std::u16string ch)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getChOff()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setChOff(std::u16string chOff)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setHeight(std::u16string height)
{
    // TODO: implement me!
}

bool HTMLTableCellElementImp::getNoWrap()
{
    // TODO: implement me!
    return 0;
}

void HTMLTableCellElementImp::setNoWrap(bool noWrap)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getVAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setVAlign(std::u16string vAlign)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

}
}
}
}
