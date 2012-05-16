/*
 * Copyright 2012 Esrille Inc.
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

#include "CSSRuleListImp.h"

#include "CSSMediaRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSStyleSheetImp.h"

#include "ViewCSSImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

bool CSSRuleListImp::PrioritizedRule::isActive(Element& element, ViewCSSImp* view) const
{
    CSSSelector* selector = getSelector();
    if (!selector)
        return true;
    return selector->match(element, view, true);
}

void CSSRuleListImp::appendMisc(CSSSelector* selector, CSSStyleDeclarationImp* declaration)
{
    misc.push_back(Rule{ selector, declaration, ++order });
}

void CSSRuleListImp::appendID(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key)
{
    mapID.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order }));
}

void CSSRuleListImp::appendClass(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key)
{
    mapClass.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order }));
}

void CSSRuleListImp::appendType(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key)
{
    mapType.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order }));
}

void CSSRuleListImp::append(css::CSSRule rule, DocumentImp* document)
{
    if (!rule)
        return;
    if (CSSStyleRuleImp* styleRule = dynamic_cast<CSSStyleRuleImp*>(rule.self())) {
        if (CSSSelectorsGroup* selectorsGroup = styleRule->getSelectorsGroup()) {
            for (auto j = selectorsGroup->begin(); j != selectorsGroup->end(); ++j) {
                CSSSelector* selector = *j;
                CSSStyleDeclarationImp* declaration = dynamic_cast<CSSStyleDeclarationImp*>(styleRule->getStyle().self());
                selector->registerToRuleList(this, declaration);
            }
        }
    } else if (CSSMediaRuleImp* mediaRule = dynamic_cast<CSSMediaRuleImp*>(rule.self())) {
        MediaListImp* mediaList = dynamic_cast<MediaListImp*>(mediaRule->getMedia().self());
        if (mediaList->hasMedium(MediaListImp::Screen)) {   // TODO: support other mediums, too.
            css::CSSRuleList ruleList = mediaRule->getCssRules();
            unsigned length = ruleList.getLength();
            for (unsigned i = 0; i < length; ++i)
                append(ruleList.getElement(i), document);
        }
    } else if (CSSImportRuleImp* importRule = dynamic_cast<CSSImportRuleImp*>(rule.self())) {
        MediaListImp* mediaList = dynamic_cast<MediaListImp*>(importRule->getMedia().self());
        if (mediaList->hasMedium(MediaListImp::Screen)) {   // TODO: support other mediums, too.
            if (document) {
                importRule->setDocument(document);
                importRule->getStyleSheet();  // to get the CSS file
                importList.push_back(importRule);
            }
        }
    }
    ruleList.push_back(rule);
}

void CSSRuleListImp::find(RuleSet& set, ViewCSSImp* view, Element& element, std::multimap<std::u16string, Rule>& map, const std::u16string& key)
{
    for (auto i = map.find(key); i != map.end() && i->first == key; ++i) {
        CSSSelector* selector = i->second.selector;
        if (!selector->match(element, view, false))
            continue;
        // TODO: emplace() seems to be not ready yet with libstdc++.
        PrioritizedRule rule(importance, i->second);
        set.insert(rule);
    }
}

void CSSRuleListImp::findByID(RuleSet& set, ViewCSSImp* view, Element& element)
{
    Nullable<std::u16string> attr = element.getAttribute(u"id");
    if (attr.hasValue())
        find(set, view, element, mapID, attr.value());
}

void CSSRuleListImp::findByClass(RuleSet& set, ViewCSSImp* view, Element& element)
{
    Nullable<std::u16string> attr = element.getAttribute(u"class");
    if (attr.hasValue()) {
        std::u16string classes = attr.value();
        for (size_t pos = 0; pos < classes.length();) {
            if (isSpace(classes[pos])) {
                ++pos;
                continue;
            }
            size_t start = pos++;
            while (pos < classes.length() && !isSpace(classes[pos]))
                ++pos;
            find(set, view, element, mapClass, classes.substr(start, pos - start));
        }
    }
}

void CSSRuleListImp::findByType(RuleSet& set, ViewCSSImp* view, Element& element)
{
    find(set, view, element, mapType, element.getLocalName());
}

void CSSRuleListImp::findMisc(RuleSet& set, ViewCSSImp* view, Element& element)
{
    for (auto i = misc.begin(); i != misc.end(); ++i) {
        CSSSelector* selector = i->selector;
        if (!selector->match(element, view, false))
            continue;
        // TODO: emplace() seems to be not ready yet with libstdc++.
        PrioritizedRule rule(importance, *i);
        set.insert(rule);
    }
}

void CSSRuleListImp::find(RuleSet& set, ViewCSSImp* view, Element& element, unsigned importance)
{
    this->importance = importance;

    for (auto i = importList.begin(); i != importList.end(); ++i) {
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>((*i)->getStyleSheet().self())) {
            if (CSSRuleListImp* ruleList = dynamic_cast<CSSRuleListImp*>(sheet->getCssRules().self()))
                ruleList->find(set, view, element, importance);
        }
    }

    findMisc(set, view, element);
    findByType(set, view, element);
    findByClass(set, view, element);
    findByID(set, view, element);
}

bool CSSRuleListImp::hasHover(const RuleSet& set)
{
    for (auto i = set.begin(); i != set.end(); ++i) {
        CSSSelector* selector = i->getSelector();
        if (selector && selector->hasHover())
            return true;
    }
    return false;
}

}}}}  // org::w3c::dom::bootstrap
