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

#include "CSSMediaRuleImp.h"

#include <org/w3c/dom/css/CSSRuleList.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

CSSMediaRuleImp::~CSSMediaRuleImp()
{
    unsigned length = ruleList.getLength();
    for (unsigned i = 0; i < length; ++i) {
        CSSRule rule = ruleList.item(i);
        if (auto imp = dynamic_cast<CSSRuleImp*>(rule.self()))
            imp->setParentRule(0);
    }
}

void CSSMediaRuleImp::append(css::CSSRule rule)
{
    if (auto imp = dynamic_cast<CSSRuleImp*>(rule.self())) {
        imp->setParentRule(this);
        ruleList.append(rule);
    }
}

// CSSRule
unsigned short CSSMediaRuleImp::getType()
{
    return CSSRule::MEDIA_RULE;
}

std::u16string CSSMediaRuleImp::getCssText()
{
    return u"@media " + mediaList.getMediaText() + u" { " + ruleList.getCssText() += u'}';
}

// CSSMediaRule
stylesheets::MediaList CSSMediaRuleImp::getMedia()
{
    return &mediaList;
}

void CSSMediaRuleImp::setMedia(const std::u16string& media)
{
    mediaList.setMediaText(media);
}

css::CSSRuleList CSSMediaRuleImp::getCssRules()
{
    return &ruleList;
}

unsigned int CSSMediaRuleImp::insertRule(const std::u16string& rule, unsigned int index)
{
    // TODO: implement me!
    return 0;
}

void CSSMediaRuleImp::deleteRule(unsigned int index)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
