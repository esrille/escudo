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

#ifndef CSSMEDIARULE_IMP_H
#define CSSMEDIARULE_IMP_H

#include <Object.h>
#include <org/w3c/dom/css/CSSMediaRule.h>

#include <deque>

#include "CSSRuleImp.h"
#include "MediaListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class MediaListImp;

class CSSMediaRuleImp : public ObjectMixin<CSSMediaRuleImp, CSSRuleImp>
{
    std::deque<css::CSSRule> ruleList;
    Retained<MediaListImp> mediaList;

public:
    void append(css::CSSRule rule) {
        ruleList.push_back(rule);
    }

    // CSSRule
    virtual unsigned short getType();

    // CSSMediaRule
    stylesheets::MediaList getMedia();
    void setMedia(std::u16string media);
    css::CSSRuleList getCssRules();
    unsigned int insertRule(std::u16string rule, unsigned int index);
    void deleteRule(unsigned int index);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSMediaRule::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSMediaRule::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CSSMEDIARULE_IMP_H
