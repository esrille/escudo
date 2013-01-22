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

#include "HTMLOListElementImp.h"

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

void HTMLOListElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"start"):
        if (mutation.getAttrChange() == events::MutationEvent::REMOVAL) {
            style.setProperty(u"counter-reset", u"", u"non-css");
            style.setProperty(u"counter-increment", u"", u"non-css");
        } else {
            if (!mapToInteger(value))
                value = u"1";
            style.setProperty(u"counter-reset", u"list-item " + value, u"non-css");
            style.setProperty(u"counter-increment", u"list-item -1", u"non-css");
        }
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

bool HTMLOListElementImp::getReversed()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setReversed(bool reversed)
{
    // TODO: implement me!
}

int HTMLOListElementImp::getStart()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setStart(int start)
{
    // TODO: implement me!
}

bool HTMLOListElementImp::getCompact()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setCompact(bool compact)
{
    // TODO: implement me!
}

std::u16string HTMLOListElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLOListElementImp::setType(const std::u16string& type)
{
    // TODO: implement me!
}

}
}
}
}
