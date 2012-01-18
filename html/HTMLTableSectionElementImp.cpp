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

#include "HTMLTableSectionElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLTableSectionElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalBackground(this);
}

html::HTMLCollection HTMLTableSectionElementImp::getRows()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableSectionElementImp::insertRow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::HTMLElement HTMLTableSectionElementImp::insertRow(int index)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLTableSectionElementImp::deleteRow(int index)
{
    // TODO: implement me!
}

std::u16string HTMLTableSectionElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableSectionElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLTableSectionElementImp::getCh()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableSectionElementImp::setCh(std::u16string ch)
{
    // TODO: implement me!
}

std::u16string HTMLTableSectionElementImp::getChOff()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableSectionElementImp::setChOff(std::u16string chOff)
{
    // TODO: implement me!
}

std::u16string HTMLTableSectionElementImp::getVAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLTableSectionElementImp::setVAlign(std::u16string vAlign)
{
    // TODO: implement me!
}

}
}
}
}
