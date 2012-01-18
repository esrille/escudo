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

#include "HTMLHRElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLHRElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalColor(this, u"color", u"border-color");
    HTMLElementImp::evalWidth(this);
}

std::u16string HTMLHRElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLHRElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLHRElementImp::getColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLHRElementImp::setColor(std::u16string color)
{
    // TODO: implement me!
}

bool HTMLHRElementImp::getNoShade()
{
    // TODO: implement me!
    return 0;
}

void HTMLHRElementImp::setNoShade(bool noShade)
{
    // TODO: implement me!
}

std::u16string HTMLHRElementImp::getSize()
{
    // TODO: implement me!
    return u"";
}

void HTMLHRElementImp::setSize(std::u16string size)
{
    // TODO: implement me!
}

std::u16string HTMLHRElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLHRElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

}
}
}
}
