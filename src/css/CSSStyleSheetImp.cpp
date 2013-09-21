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

#include "CSSStyleSheetImp.h"

#include "CSSRuleImp.h"
#include "ObjectArrayImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void CSSStyleSheetImp::append(css::CSSRule rule, const DocumentPtr& document)
{
    if (auto imp = std::dynamic_pointer_cast<CSSRuleImp>(rule.self())) {
        imp->setParentStyleSheet(std::static_pointer_cast<CSSStyleSheetImp>(self()));
        ruleList->append(rule, document, 0);
    }
}

// StyleSheet
std::u16string CSSStyleSheetImp::getType()
{
    return u"text/css";
}

// CSSStyleSheet
css::CSSRule CSSStyleSheetImp::getOwnerRule()
{
    // TODO: implement me!
    return nullptr;
}

css::CSSRuleList CSSStyleSheetImp::getCssRules()
{
    return ruleList;
}

unsigned int CSSStyleSheetImp::insertRule(const std::u16string& rule, unsigned int index)
{
    return ruleList->insertRule(rule, index);
}

void CSSStyleSheetImp::deleteRule(unsigned int index)
{
    ruleList->deleteRule(index);
}

}}}}  // org::w3c::dom::bootstrap
