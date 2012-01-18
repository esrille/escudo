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

#include "HTMLTableRowElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLTableRowElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalBackground(this);
    HTMLElementImp::evalBgcolor(this);
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
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableRowElementImp::insertCell()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableRowElementImp::insertCell(int index)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableRowElementImp::deleteCell(int index)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setBgColor(std::u16string bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getCh()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setCh(std::u16string ch)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getChOff()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setChOff(std::u16string chOff)
{
    // TODO: implement me!
}

std::u16string HTMLTableRowElementImp::getVAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableRowElementImp::setVAlign(std::u16string vAlign)
{
    // TODO: implement me!
}

}
}
}
}
