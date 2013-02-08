/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "HTMLButtonElementImp.h"

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

void HTMLButtonElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();

    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"disabled"):
        if (mutation.getAttrChange() != events::MutationEvent::REMOVAL)
            tabIndex = -1;
        else if (!toInteger(getAttribute(u"tabindex"), tabIndex))
            tabIndex = 0;
        break;
    case Intern(u"tabindex"):
        if (!getDisabled() && !toInteger(value, tabIndex))
            tabIndex = 0;
        break;
    default:
        HTMLFormControlImp::handleMutation(mutation);
        break;
    }
}

}
}
}
}
