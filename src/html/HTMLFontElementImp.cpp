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

#include "HTMLFontElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLFontElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalColor(this, u"color", u"color");
}

std::u16string HTMLFontElementImp::getColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLFontElementImp::setColor(std::u16string color)
{
    // TODO: implement me!
}

std::u16string HTMLFontElementImp::getFace()
{
    // TODO: implement me!
    return u"";
}

void HTMLFontElementImp::setFace(std::u16string face)
{
    // TODO: implement me!
}

std::u16string HTMLFontElementImp::getSize()
{
    // TODO: implement me!
    return u"";
}

void HTMLFontElementImp::setSize(std::u16string size)
{
    // TODO: implement me!
}

}
}
}
}
