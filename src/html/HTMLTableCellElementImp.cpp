/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLTableElementImp.h"
#include "HTMLUtil.h"
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

void HTMLTableCellElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value;
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"background"):
        handleMutationBackground(mutation);
        break;
    case Intern(u"bgcolor"):
        handleMutationColor(mutation, u"background-color");
        break;
    case Intern(u"height"):
        if (mapToDimension(value = mutation.getNewValue()))
            style.setProperty(u"height", value, u"non-css");
        break;
    case Intern(u"width"):
        if (mapToDimension(value = mutation.getNewValue()))
            style.setProperty(u"width", value, u"non-css");
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

void HTMLTableCellElementImp::eval()
{
    for (Element e = getParentElement(); e; e = e.getParentElement()) {
        if (auto table = dynamic_cast<HTMLTableElementImp*>(e.self())) {
            std::u16string border = table->getBorder();
            if (!border.empty() && border != u"0") {
                css::CSSStyleDeclaration style = getStyle();
                style.setProperty(u"border-width", u"1px", u"non-css");
                style.setProperty(u"border-style", u"inset", u"non-css");
            }
            // TOOD: Check "cellpadding"
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

void HTMLTableCellElementImp::setHeaders(const std::u16string& headers)
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

void HTMLTableCellElementImp::setAbbr(const std::u16string& abbr)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getAxis()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setAxis(const std::u16string& axis)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getCh()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setCh(const std::u16string& ch)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getChOff()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setChOff(const std::u16string& chOff)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setHeight(const std::u16string& height)
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

void HTMLTableCellElementImp::setVAlign(const std::u16string& vAlign)
{
    // TODO: implement me!
}

std::u16string HTMLTableCellElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableCellElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

}
}
}
}
