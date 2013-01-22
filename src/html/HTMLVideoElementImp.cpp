/*
 * Copyright 2013 Esrille Inc.
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

#include "HTMLVideoElementImp.h"

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

void HTMLVideoElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"height"):
        if (mapToDimension(value))
            style.setProperty(u"height", value, u"non-css");
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

unsigned int HTMLVideoElementImp::getWidth()
{
    // TODO: implement me!
    return 0;
}

void HTMLVideoElementImp::setWidth(unsigned int width)
{
    // TODO: implement me!
}

unsigned int HTMLVideoElementImp::getHeight()
{
    // TODO: implement me!
    return 0;
}

void HTMLVideoElementImp::setHeight(unsigned int height)
{
    // TODO: implement me!
}

unsigned int HTMLVideoElementImp::getVideoWidth()
{
    // TODO: implement me!
    return 0;
}

unsigned int HTMLVideoElementImp::getVideoHeight()
{
    // TODO: implement me!
    return 0;
}

std::u16string HTMLVideoElementImp::getPoster()
{
    // TODO: implement me!
    return u"";
}

void HTMLVideoElementImp::setPoster(const std::u16string& poster)
{
    // TODO: implement me!
}

}
}
}
}
