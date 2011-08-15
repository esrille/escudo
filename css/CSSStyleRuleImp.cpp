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

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

CSSSelector* CSSStyleRuleImp::match(dom::Element element, ViewCSSImp* view)
{
    if (selectorsGroup)
        return selectorsGroup->match(element, view);
    return 0;
}

// CSSRule
unsigned short CSSStyleRuleImp::getType()
{
    return CSSRule::STYLE_RULE;
}

std::u16string CSSStyleRuleImp::getCssText()
{
    return getSelectorText() + u": " + styleDeclaration.getCssText();
}

// CSSStyleRule
std::u16string CSSStyleRuleImp::getSelectorText()
{
    std::u16string text;
    if (selectorsGroup)
        selectorsGroup->serialize(text);
    return text;
}

void CSSStyleRuleImp::setSelectorText(std::u16string selectorText)
{
    // TODO: implement me!
}

CSSStyleDeclaration CSSStyleRuleImp::getStyle()
{
    return styleDeclaration;
}

CSSStyleRuleImp::CSSStyleRuleImp(CSSSelectorsGroup* selectorsGroup, css::CSSStyleDeclaration styleDeclaration) :
    selectorsGroup(selectorsGroup),
    styleDeclaration(styleDeclaration)
{
    if (CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(styleDeclaration.self()))
        imp->setParentRule(this);
}

}}}}  // org::w3c::dom::bootstrap
