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

#ifndef ORG_W3C_DOM_BOOTSTRAP_CSSRULELISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_CSSRULELISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSRuleList.h>

#include <deque>
#include <list>
#include <map>
#include <set>

#include "CSSImportRuleImp.h"
#include "CSSStyleRuleImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentImp;
class MediaQueryListImp;

class CSSRuleListImp : public ObjectMixin<CSSRuleListImp>
{
public:
    struct Rule
    {
        // TODO: Make sure the following two pointers are valid while they are in use.
        CSSSelector* selector;
        CSSStyleDeclarationImp* declaration;
        unsigned order;
        MediaListImp* mediaList;
    };

    enum Importance
    {
        UserAgent = (1 << 24),
        User = (2 << 24),
        Presentational = (4 << 24), // for non-CSS presentational hints; should be treated like Author
        Author = (8 << 24)
    };

    struct PrioritizedRule
    {
        // TODO: Make sure the rule is valid while this is in use.
        unsigned priority;
        CSSRuleListImp::Rule rule;
        MediaQueryListImp* mql;
    public:
        PrioritizedRule(unsigned priority, const CSSRuleListImp::Rule& rule, MediaQueryListImp* mql) :
            priority(priority),
            rule(rule),
            mql(mql)
        {
            if ((this->priority & 0xff000000) != Presentational)
                this->priority |= rule.selector->getSpecificity();
        }
        PrioritizedRule(unsigned priority, CSSStyleDeclarationImp* decl) :
            priority(priority),
            mql(0)
        {
            rule.selector = 0;
            rule.declaration = decl;
            rule.order = 0;
            rule.mediaList = 0;
        }
        CSSSelector* getSelector() const {
            return rule.selector;
        }
        CSSStyleDeclarationImp* getDeclaration() const {
            return rule.declaration;
        }
        unsigned getOrder() const {
            return rule.order;
        }
        bool isUserStyle() const {
            return (priority & 0xff000000) == User;
        }
        unsigned getPseudoElementID() const {
            CSSSelector* selector = getSelector();
            if (!selector)
                return CSSPseudoElementSelector::NonPseudo;
            if (CSSPseudoElementSelector* pseudo = selector->getPseudoElement())
                return pseudo->getID();
            return CSSPseudoElementSelector::NonPseudo;
        }
        bool getMatches() const;
        bool isActive(Element& element, ViewCSSImp* view) const;
        bool operator <(const PrioritizedRule& decl) const {
            return (priority < decl.priority) || (priority == decl.priority && getOrder() < decl.getOrder()) ;
        }
    };

    typedef std::multiset<PrioritizedRule> RuleSet;

private:
    unsigned importance;
    unsigned order;
    std::deque<css::CSSRule> ruleList;

    std::deque<CSSImportRulePtr> importList;
    std::multimap<std::u16string, Rule> mapID;     // ID selectors
    std::multimap<std::u16string, Rule> mapClass;  // class selectors
    std::multimap<std::u16string, Rule> mapType;   // type selectors
    std::deque<Rule> misc;

    // TODO: avoid using non-const MediaListPtr reference
    void collectRules(RuleSet& set, ViewCSSImp* view, Element& element, std::multimap<std::u16string, Rule>& map, const std::u16string& key, MediaListPtr mediaList);
    void collectRulesByID(RuleSet& set, ViewCSSImp* view, Element& element, const MediaListPtr& mediaList);
    void collectRulesByClass(RuleSet& set, ViewCSSImp* view, Element& element, const MediaListPtr& mediaList);
    void collectRulesByType(RuleSet& set, ViewCSSImp* view, Element& element, const MediaListPtr& mediaList);
    void collectRulesByMisc(RuleSet& set, ViewCSSImp* view, Element& element, MediaListPtr mediaList);

public:
    CSSRuleListImp() :
        importance(0),
        order(0)
    {}

    void append(css::CSSRule rule);   // trivial version for CSSMediaRule
    void append(css::CSSRule rule, const DocumentPtr& document, const MediaListPtr& mediaList);

    void appendMisc(CSSSelector* selector, const CSSStyleDeclarationPtr& declaration, const MediaListPtr& mediaList);
    void appendID(CSSSelector* selector, const CSSStyleDeclarationPtr& declaration, const std::u16string& key, const MediaListPtr& mediaList);
    void appendClass(CSSSelector* selector, const CSSStyleDeclarationPtr& declaration, const std::u16string& key, const MediaListPtr& mediaList);
    void appendType(CSSSelector* selector, const CSSStyleDeclarationPtr& declaration, const std::u16string& key, const MediaListPtr& mediaList);

    void collectRules(RuleSet& set, ViewCSSImp* view, Element& element, unsigned importance, MediaListPtr mediaList);

    css::CSSRuleList getCssRules() {
        return self();
    }

    std::u16string getCssText()
    {
        std::u16string text;

        for (auto i = ruleList.begin(); i != ruleList.end(); ++i)
            text += (*i).getCssText() + u" ";
        return text;
    }

    // for StyleSheet
    unsigned int insertRule(const std::u16string& rule, unsigned int index) {
        // TODO: implement me
        return 0;
    }

    void deleteRule(unsigned int index) {
        // TODO: implement me
    }

    // CSSRuleList
    css::CSSRule item(unsigned int index) {
        if (getLength() <= index)
            return nullptr;
        return ruleList[index];
    }
    unsigned int getLength() {
        return ruleList.size();
    }
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSRuleList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSRuleList::getMetaData();
    }

    static bool hasHover(const RuleSet& set);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_CSSRULELISTIMP_H_INCLUDED
