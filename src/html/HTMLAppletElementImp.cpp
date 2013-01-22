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

#include "HTMLAppletElementImp.h"

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

void HTMLAppletElementImp::handleMutation(events::MutationEvent mutation)
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

std::u16string HTMLAppletElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getAlt()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setAlt(const std::u16string& alt)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getArchive()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setArchive(const std::u16string& archive)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getCode()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setCode(const std::u16string& code)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getCodeBase()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setCodeBase(const std::u16string& codeBase)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setHeight(const std::u16string& height)
{
    // TODO: implement me!
}

unsigned int HTMLAppletElementImp::getHspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLAppletElementImp::setHspace(unsigned int hspace)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getObject()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setObject(const std::u16string& object)
{
    // TODO: implement me!
}

unsigned int HTMLAppletElementImp::getVspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLAppletElementImp::setVspace(unsigned int vspace)
{
    // TODO: implement me!
}

std::u16string HTMLAppletElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLAppletElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

}
}
}
}
