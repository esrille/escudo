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

#include "HTMLTableElementImp.h"

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLTableCaptionElementImp.h"
#include "HTMLTableRowElementImp.h"
#include "HTMLTableSectionElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

//
// Rows
//

HTMLTableElementImp::Rows::Rows(HTMLTableElementImp* table) :
    table(table)
{}

unsigned int HTMLTableElementImp::Rows::getLength()
{
    return table->getRowCount();
}

Element HTMLTableElementImp::Rows::item(unsigned int index)
{
    return table->getRow(index);
}

//
// HTMLTableElementImp
//

void HTMLTableElementImp::handleMutation(events::MutationEvent mutation)
{
    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"background"):
        handleMutationBackground(mutation);
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

void HTMLTableElementImp::eval()
{
    HTMLElementImp::evalBgcolor(this);
    HTMLElementImp::evalHeight(this);
    HTMLElementImp::evalWidth(this);

    std::u16string border = getBorder();
    if (!border.empty()) {
        css::CSSStyleDeclaration style = getStyle();
        if (border == u"0")
            style.setProperty(u"border-style", u"none", u"non-css");
        else {
            style.setProperty(u"border-width", border + u"px", u"non-css");
            style.setProperty(u"border-style", u"outset", u"non-css");
        }
    }

    Nullable<std::u16string> attr = getAttribute(u"cellspacing");
    if (attr.hasValue()) {
        std::u16string value = attr.value();
        if (toPx(value)) {
            css::CSSStyleDeclaration style = getStyle();
            style.setBorderSpacing(value);
        }
    }
}

unsigned int HTMLTableElementImp::getRowCount()
{
    // TODO: Better to keep the result
    unsigned int count = 0;
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self()))
            ++count;
        else if (HTMLTableSectionElementImp* section = dynamic_cast<HTMLTableSectionElementImp*>(child.self())) {
            for (Element child = section->getFirstElementChild(); child; child = child.getNextElementSibling()) {
                if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self()))
                    ++count;
            }
        }
    }
    return count;
}

html::HTMLTableRowElement HTMLTableElementImp::getRow(unsigned int index)
{
    unsigned int count = 0;

    // thead
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableSectionElementImp* section = dynamic_cast<HTMLTableSectionElementImp*>(child.self())) {
            if (section->getTagName() == u"thead") {
                for (Element child = section->getFirstElementChild(); child; child = child.getNextElementSibling()) {
                    if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self())) {
                        if (count == index)
                            return row;
                        ++count;
                    }
                }
            }
        }
    }

    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self())) {
            if (count == index)
                return row;
            ++count;
        } else if (HTMLTableSectionElementImp* section = dynamic_cast<HTMLTableSectionElementImp*>(child.self())) {
            if (section->getTagName() == u"tbody") {
                for (Element child = section->getFirstElementChild(); child; child = child.getNextElementSibling()) {
                    if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self())) {
                        if (count == index)
                            return row;
                        ++count;
                    }
                }
            }
        }
    }

    // tfoot
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableSectionElementImp* section = dynamic_cast<HTMLTableSectionElementImp*>(child.self())) {
            if (section->getTagName() == u"tfoot") {
                for (Element child = section->getFirstElementChild(); child; child = child.getNextElementSibling()) {
                    if (HTMLTableRowElementImp* row = dynamic_cast<HTMLTableRowElementImp*>(child.self())) {
                        if (count == index)
                            return row;
                        ++count;
                    }
                }
            }
        }
    }

    return 0;
}

//
// HTMLTableElement
//

html::HTMLTableCaptionElement HTMLTableElementImp::getCaption()
{
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableCaptionElementImp* caption = dynamic_cast<HTMLTableCaptionElementImp*>(child.self()))
            return caption;
    }
    return 0;
}

void HTMLTableElementImp::setCaption(html::HTMLTableCaptionElement caption)
{
    // TODO: implement me!
}

html::HTMLElement HTMLTableElementImp::createCaption()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::deleteCaption()
{
    // TODO: implement me!
}

html::HTMLTableSectionElement HTMLTableElementImp::getTHead()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::setTHead(html::HTMLTableSectionElement tHead)
{
    // TODO: implement me!
}

html::HTMLElement HTMLTableElementImp::createTHead()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::deleteTHead()
{
    // TODO: implement me!
}

html::HTMLTableSectionElement HTMLTableElementImp::getTFoot()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::setTFoot(html::HTMLTableSectionElement tFoot)
{
    // TODO: implement me!
}

html::HTMLElement HTMLTableElementImp::createTFoot()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::deleteTFoot()
{
    // TODO: implement me!
}

html::HTMLCollection HTMLTableElementImp::getTBodies()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableElementImp::createTBody()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLCollection HTMLTableElementImp::getRows()
{
    return new(std::nothrow) Rows(this);
}

html::HTMLElement HTMLTableElementImp::insertRow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableElementImp::insertRow(int index)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableElementImp::deleteRow(int index)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getSummary()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setSummary(const std::u16string& summary)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getBorder()
{
    Nullable<std::u16string> value = getAttribute(u"border");
    if (value.hasValue()) {
        std::u16string px = value.value();
        if (toPx(px))
            return px.substr(0, px.length() - 2);
    }
    return u"";
}

void HTMLTableElementImp::setBorder(const std::u16string& border)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getCellPadding()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setCellPadding(const std::u16string& cellPadding)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getCellSpacing()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setCellSpacing(const std::u16string& cellSpacing)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getFrame()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setFrame(const std::u16string& frame)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getRules()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setRules(const std::u16string& rules)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

}
}
}
}
