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

#include "HTMLEmbedElementImp.h"

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLEmbedElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"height"):
        if (mapToDimension(value))
            style.setProperty(u"height", value, u"non-css");
        break;
    case Intern(u"hspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-left", value, u"non-css");
            style.setProperty(u"margin-right", value, u"non-css");
        }
        break;
    case Intern(u"vspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-top", value, u"non-css");
            style.setProperty(u"margin-bottom", value, u"non-css");
        }
        break;
    case Intern(u"width"):
        if (mapToDimension(value))
            style.setProperty(u"width", value, u"non-css");
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

std::u16string HTMLEmbedElementImp::getSrc()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setSrc(const std::u16string& src)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setType(const std::u16string& type)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setHeight(const std::u16string& height)
{
    // TODO: implement me!
}

Any HTMLEmbedElementImp::operator() (Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

std::u16string HTMLEmbedElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLEmbedElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLEmbedElementImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

}
}
}
}
