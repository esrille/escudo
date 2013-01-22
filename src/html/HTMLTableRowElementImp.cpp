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

#include "HTMLTableRowElementImp.h"

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLTableCellElementImp.h"
#include "HTMLTableDataCellElementImp.h"
#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

//
// Cells
//

HTMLTableRowElementImp::Cells::Cells(HTMLTableRowElementImp* row) :
    row(row)
{}

unsigned int HTMLTableRowElementImp::Cells::getLength()
{
    return row->getCellCount();
}

Element HTMLTableRowElementImp::Cells::item(unsigned int index)
{
    return row->getCell(index);
}

//
// HTMLTableRowElementImp
//

void HTMLTableRowElementImp::handleMutation(events::MutationEvent mutation)
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
    // Styles
    case Intern(u"height"):
        if (mapToDimension(value = mutation.getNewValue()))
            style.setProperty(u"height", value, u"non-css");
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

unsigned int HTMLTableRowElementImp::getCellCount()
{
    // TODO: Better to keep the result
    unsigned int count = 0;
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (dynamic_cast<HTMLTableCellElementImp*>(child.self()))
            ++count;
    }
    return count;
}

html::HTMLTableCellElement HTMLTableRowElementImp::getCell(unsigned int index)
{
    unsigned int count = 0;
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableCellElementImp* cell = dynamic_cast<HTMLTableCellElementImp*>(child.self())) {
            if (count == index)
                return cell;
            ++count;
        }
    }
    return 0;
}

int HTMLTableRowElementImp::getRowIndex()
{
    // TODO: implement me!
    return 0;
}

int HTMLTableRowElementImp::getSectionRowIndex()
{
    // TODO: implement me!
    return 0;
}

html::HTMLCollection HTMLTableRowElementImp::getCells()
{
    return new(std::nothrow) Cells(this);
}

html::HTMLElement HTMLTableRowElementImp::insertCell(int index)
{
    if (index < -1)
        return 0;   // TODO: throw an IndexSizeError exception
    unsigned int count = 0;
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableCellElementImp* cell = dynamic_cast<HTMLTableCellElementImp*>(child.self())) {
            if (count == index) {
                html::HTMLTableDataCellElement td = new(std::nothrow) HTMLTableDataCellElementImp(getOwnerDocumentImp());
                if (td)
                    insertBefore(td, cell);
                return td;
            }
            ++count;
        }
    }
    if (count < index)
        return 0;   // TODO: throw an IndexSizeError exception
    html::HTMLTableDataCellElement td = new(std::nothrow) HTMLTableDataCellElementImp(getOwnerDocumentImp());
    if (td)
        appendChild(td);
    return td;
}

void HTMLTableRowElementImp::deleteCell(int index)
{
    if (index < 0)
        return;     // TODO: throw an IndexSizeError exception
    unsigned int count = 0;
    for (Element child = getFirstElementChild(); child; child = child.getNextElementSibling()) {
        if (HTMLTableCellElementImp* cell = dynamic_cast<HTMLTableCellElementImp*>(child.self())) {
            if (count == index) {
                removeChild(child);
                return;
            }
            ++count;
        }
    }
    // TODO: throw an IndexSizeError exception
}

std::u16string HTMLTableRowElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getCh()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setCh(const std::u16string& ch)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getChOff()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setChOff(const std::u16string& chOff)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getVAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setVAlign(const std::u16string& vAlign)
{
    // TODO: implement me!
}

}
}
}
}
