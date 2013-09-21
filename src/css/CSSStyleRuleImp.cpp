/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

void CSSStyleRuleImp::setStyle(const CSSStyleDeclarationPtr& style)
{
    styleDeclaration = style;
    style->setParentRule(std::static_pointer_cast<CSSStyleRuleImp>(self()));
}

// CSSRule
unsigned short CSSStyleRuleImp::getType()
{
    return CSSRule::STYLE_RULE;
}

std::u16string CSSStyleRuleImp::getCssText()
{
    std::u16string text = getSelectorText() + u" {";
    if (styleDeclaration)
        text += u' ' + styleDeclaration->getCssText() + u' ';
    text +=  u'}';
    return text;
}

// CSSStyleRule
std::u16string CSSStyleRuleImp::getSelectorText()
{
    std::u16string text;
    if (selectorsGroup)
        selectorsGroup->serialize(text);
    return text;
}

void CSSStyleRuleImp::setSelectorText(const std::u16string& selectorText)
{
    // TODO: implement me!
}

CSSStyleDeclaration CSSStyleRuleImp::getStyle()
{
    return styleDeclaration;
}

CSSStyleRuleImp::CSSStyleRuleImp(CSSSelectorsGroup* selectorsGroup) :
    selectorsGroup(selectorsGroup)
{
}

}}}}  // org::w3c::dom::bootstrap
