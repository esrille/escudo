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

#include "HTMLEmbedElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLEmbedElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalHeight(this);
    HTMLElementImp::evalWidth(this);
    HTMLElementImp::evalHspace(this);
    HTMLElementImp::evalVspace(this);
}

std::u16string HTMLEmbedElementImp::getSrc()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setSrc(std::u16string src)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setHeight(std::u16string height)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setName(std::u16string name)
{
    // TODO: implement me!
}

}
}
}
}
