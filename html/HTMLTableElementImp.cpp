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

#include "HTMLTableElementImp.h"

#include "HTMLTableCaptionElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

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
    // TODO: implement me!
    return static_cast<Object*>(0);
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

void HTMLTableElementImp::setSummary(std::u16string summary)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setBgColor(std::u16string bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setBorder(std::u16string border)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getCellPadding()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setCellPadding(std::u16string cellPadding)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getCellSpacing()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setCellSpacing(std::u16string cellSpacing)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getFrame()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setFrame(std::u16string frame)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getRules()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setRules(std::u16string rules)
{
    // TODO: implement me!
}

std::u16string HTMLTableElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

}
}
}
}
