/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "CSSRuleImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

// CSSRule
unsigned short CSSRuleImp::getType()
{
    return 0;  // UNKNOWN_RULE, obsoleted by CSSOM
}

std::u16string CSSRuleImp::getCssText()
{
    // TODO: implement me!
    return u"";
}

void CSSRuleImp::setCssText(std::u16string cssText)
{
    // TODO: implement me!
}

css::CSSRule CSSRuleImp::getParentRule()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

css::CSSStyleSheet CSSRuleImp::getParentStyleSheet()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}}}}  // org::w3c::dom::bootstrap
