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

#ifndef CSSSTYLESHEET_IMP_H
#define CSSSTYLESHEET_IMP_H

#include <Object.h>
#include <org/w3c/dom/css/CSSStyleSheet.h>

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/css/CSSRule.h>

#include <deque>

#include "StyleSheetImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CSSStyleSheetImp : public ObjectMixin<CSSStyleSheetImp, StyleSheetImp>
{
    std::deque<css::CSSRule> ruleList;
public:
    void append(css::CSSRule rule) {
        ruleList.push_back(rule);
    }

    // StyleSheet
    virtual std::u16string getType();

    // CSSStyleSheet
    css::CSSRule getOwnerRule() __attribute__((weak));
    css::CSSRuleList getCssRules() __attribute__((weak));
    unsigned int insertRule(std::u16string rule, unsigned int index) __attribute__((weak));
    void deleteRule(unsigned int index) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSStyleSheet::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSStyleSheet::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CSSSTYLESHEET_IMP_H
