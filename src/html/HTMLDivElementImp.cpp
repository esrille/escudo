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

#include "HTMLDivElementImp.h"
#include <boost/concept_check.hpp>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "css/CSSStyleDeclarationImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

css::CSSStyleDeclaration HTMLDivElementImp::getStyle()
{
    if (hasStyle())
        return HTMLElementImp::getStyle();
    css::CSSStyleDeclaration style = HTMLElementImp::getStyle();
    if (CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(style.self())) {
        if (getLocalName() == u"center")
            imp->setHTMLAlign(u"center");
    }
    return style;
}

void HTMLDivElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"align"):
        if (CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(style.self()))
            imp->setHTMLAlign(value);
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

std::u16string HTMLDivElementImp::getAlign()
{
    return getAttribute(u"align");
}

void HTMLDivElementImp::setAlign(const std::u16string& align)
{
    setAttribute(u"align", align);
}

}
}
}
}
