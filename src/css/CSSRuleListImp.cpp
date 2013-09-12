/*
 * Copyright 2012, 2013 Esrille Inc.
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

#include "DocumentImp.h"
#include "ViewCSSImp.h"

#include "html/MediaQueryListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

void CSSRuleListImp::append(css::CSSRule rule)
{
    ruleList.push_back(rule);
}

bool CSSRuleListImp::PrioritizedRule::getMatches() const
{
    return !mql || mql->getMatches();
}

bool CSSRuleListImp::PrioritizedRule::isActive(Element& element, ViewCSSImp* view) const
{
    CSSSelector* selector = getSelector();
    if (!selector)
        return true;
    return selector->match(element, view, true);
}

void CSSRuleListImp::appendMisc(CSSSelector* selector, CSSStyleDeclarationImp* declaration, MediaListImp* mediaList)
{
    misc.push_back(Rule{ selector, declaration, ++order, mediaList });
}

void CSSRuleListImp::appendID(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key, MediaListImp* mediaList)
{
    mapID.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order, mediaList }));
}

void CSSRuleListImp::appendClass(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key, MediaListImp* mediaList)
{
    mapClass.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order, mediaList }));
}

void CSSRuleListImp::appendType(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key, MediaListImp* mediaList)
{
    mapType.insert(std::pair<std::u16string, Rule>(key, Rule{ selector, declaration, ++order, mediaList }));
}

void CSSRuleListImp::append(css::CSSRule rule, DocumentImp* document, MediaListImp* mediaList)
{
    if (!rule)
        return;
    if (CSSStyleRuleImp* styleRule = dynamic_cast<CSSStyleRuleImp*>(rule.self())) {
        if (CSSSelectorsGroup* selectorsGroup = styleRule->getSelectorsGroup()) {
            for (auto j = selectorsGroup->begin(); j != selectorsGroup->end(); ++j) {
                CSSSelector* selector = *j;
                CSSStyleDeclarationImp* declaration = dynamic_cast<CSSStyleDeclarationImp*>(styleRule->getStyle().self());
                selector->registerToRuleList(this, declaration, mediaList);
            }
        }
    } else if (CSSMediaRuleImp* mediaRule = dynamic_cast<CSSMediaRuleImp*>(rule.self())) {
        auto ruleList = dynamic_cast<CSSRuleListImp*>(mediaRule->getCssRules().self());
        if (ruleList) {
            MediaListImp* mediaList = dynamic_cast<MediaListImp*>(mediaRule->getMedia().self());
            for (auto i = ruleList->ruleList.begin(); i != ruleList->ruleList.end(); ++i)
                append(*i, document, mediaList);
        }
    } else if (CSSImportRuleImp* importRule = dynamic_cast<CSSImportRuleImp*>(rule.self())) {
        if (document) {
            importRule->setDocument(document);
            importRule->getStyleSheet();  // to get the CSS file
            importList.push_back(importRule);
        }
    }
    if (!rule.getParentRule())
        ruleList.push_back(rule);
}

void CSSRuleListImp::collectRules(RuleSet& set, ViewCSSImp* view, Element& element, std::multimap<std::u16string, Rule>& map, const std::u16string& key, MediaListImp* mediaList)
{
    for (auto i = map.find(key); i != map.end() && i->first == key; ++i) {
        CSSSelector* selector = i->second.selector;
        if (!selector->match(element, view, false))
            continue;
        // TODO: emplace() seems to be not ready yet with libstdc++.
        if (!mediaList)
            mediaList = i->second.mediaList;
        // TODO: else ...
        PrioritizedRule rule(importance, i->second, view->matchMedia(mediaList));
        set.insert(rule);
    }
}

void CSSRuleListImp::collectRulesByID(RuleSet& set, ViewCSSImp* view, Element& element, MediaListImp* mediaList)
{
    Nullable<std::u16string> attr = element.getAttribute(u"id");
    if (attr.hasValue())
        collectRules(set, view, element, mapID, attr.value(), mediaList);
}

void CSSRuleListImp::collectRulesByClass(RuleSet& set, ViewCSSImp* view, Element& element, MediaListImp* mediaList)
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
            collectRules(set, view, element, mapClass, classes.substr(start, pos - start), mediaList);
        }
    }
}

void CSSRuleListImp::collectRulesByType(RuleSet& set, ViewCSSImp* view, Element& element, MediaListImp* mediaList)
{
    collectRules(set, view, element, mapType, element.getLocalName(), mediaList);
}

void CSSRuleListImp::collectRulesByMisc(RuleSet& set, ViewCSSImp* view, Element& element, MediaListImp* mediaList)
{
    for (auto i = misc.begin(); i != misc.end(); ++i) {
        CSSSelector* selector = i->selector;
        if (!selector->match(element, view, false))
            continue;
        // TODO: emplace() seems to be not ready yet with libstdc++.
        if (!mediaList)
            mediaList = i->mediaList;
        // TODO: else ...
        PrioritizedRule rule(importance, *i, view->matchMedia(mediaList));
        set.insert(rule);
    }
}

void CSSRuleListImp::collectRules(RuleSet& set, ViewCSSImp* view, Element& element, unsigned importance, MediaListImp* mediaList)
{
    this->importance = importance;

    // Declarations in imported style sheets are considered to be before any
    // declarations in the style sheet itself.
    // cf. http://www.w3.org/TR/CSS2/cascade.html#cascading-order
    for (auto i = importList.begin(); i != importList.end(); ++i) {
        if (CSSStyleSheetImp* sheet = dynamic_cast<CSSStyleSheetImp*>((*i)->getStyleSheet().self())) {
            if (CSSRuleListImp* ruleList = dynamic_cast<CSSRuleListImp*>(sheet->getCssRules().self()))
                ruleList->collectRules(set, view, element, importance, mediaList);
        }
    }

    collectRulesByMisc(set, view, element, mediaList);
    collectRulesByType(set, view, element, mediaList);
    collectRulesByClass(set, view, element, mediaList);
    collectRulesByID(set, view, element, mediaList);
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
