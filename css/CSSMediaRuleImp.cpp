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

#include "CSSMediaRuleImp.h"
#include "ObjectArrayImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

void CSSMediaRuleImp::append(css::CSSRule rule)
{
    ruleList.push_back(rule);
}

// CSSRule
unsigned short CSSMediaRuleImp::getType()
{
    return CSSRule::MEDIA_RULE;
}

std::u16string CSSMediaRuleImp::getCssText()
{
    std::u16string text = u"@media " + mediaList.getMediaText() + u" {";
    for (auto i = ruleList.begin(); i != ruleList.end(); ++i)
        text += (*i).getCssText();
    return text += u"}";
}

// CSSMediaRule
stylesheets::MediaList CSSMediaRuleImp::getMedia()
{
    return &mediaList;
}

void CSSMediaRuleImp::setMedia(std::u16string media)
{
    mediaList.setMediaText(media);
}

CSSRuleList CSSMediaRuleImp::getCssRules()
{
    return new(std::nothrow) ObjectArrayImp<CSSMediaRuleImp, CSSRule, &CSSMediaRuleImp::ruleList>(this);
}

unsigned int CSSMediaRuleImp::insertRule(std::u16string rule, unsigned int index)
{
    // TODO: implement me!
    return 0;
}

void CSSMediaRuleImp::deleteRule(unsigned int index)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
