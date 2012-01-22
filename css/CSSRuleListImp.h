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

#ifndef CSSRULELIST_IMP_H
#define CSSRULELIST_IMP_H

#include <Object.h>
#include <org/w3c/dom/ObjectArray.h>

#include <deque>
#include <map>
#include <set>

#include "CSSImportRuleImp.h"
#include "CSSStyleRuleImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentImp;

class CSSRuleListImp : public ObjectImp
{
public:
    enum Importance
    {
        UserAgent = (1 << 24),
        User = (2 << 24),
        Author = (4 << 24)
    };

    struct PrioritizedDeclaration
    {
        unsigned priority;
        unsigned order;
        CSSStyleDeclarationImp* decl;
        unsigned pseudoElementID;

        PrioritizedDeclaration(unsigned priority, CSSStyleDeclarationImp* decl, unsigned pseudoElementID, unsigned order) :
            priority(priority),
            order(order),
            decl(decl),
            pseudoElementID(pseudoElementID)
        {
        }
        bool operator <(const PrioritizedDeclaration& decl) const
        {
            return (priority < decl.priority) || (priority == decl.priority && order < decl.order) ;
        }
        bool isUserStyle() const {
            return (priority & 0xff000000) == User;
        }
    };

    typedef std::multiset<PrioritizedDeclaration> DeclarationSet;

private:
    struct Rule
    {
        CSSSelector* selector;
        CSSStyleDeclarationImp* declaration;
        unsigned order;
    };

    unsigned importance;
    unsigned order;
    std::deque<css::CSSRule> ruleList;

    std::deque<CSSImportRuleImp*> importList;
    std::multimap<std::u16string, Rule> mapID;     // ID selectors
    std::multimap<std::u16string, Rule> mapClass;  // class selectors
    std::multimap<std::u16string, Rule> mapType;   // type selectors
    std::deque<Rule> misc;

    void find(DeclarationSet& set, ViewCSSImp* view, Element element, std::multimap<std::u16string, Rule>& map, const std::u16string& key);
    void findByID(DeclarationSet& set, ViewCSSImp* view, Element element);
    void findByClass(DeclarationSet& set, ViewCSSImp* view, Element element);
    void findByType(DeclarationSet& set, ViewCSSImp* view, Element element);
    void findMisc(DeclarationSet& set, ViewCSSImp* view, Element element);

public:
    CSSRuleListImp() :
        importance(0),
        order(0)
    {}

    void append(css::CSSRule rule, DocumentImp* document);

    void appendMisc(CSSSelector* selector, CSSStyleDeclarationImp* declaration);
    void appendID(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key);
    void appendClass(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key);
    void appendType(CSSSelector* selector, CSSStyleDeclarationImp* declaration, const std::u16string& key);

    void find(DeclarationSet& set, ViewCSSImp* view, Element element, unsigned importance);

    css::CSSRuleList getCssRules()
    {
        return this;
    }

    // for StyleSheet
    unsigned int insertRule(std::u16string rule, unsigned int index)
    {
        // TODO: implement me
    }

    void deleteRule(unsigned int index)
    {
        // TODO: implement me
    }

    // ObjectArray
    unsigned int getLength() {
        return ruleList.size();
    }
    void setLength(unsigned int length) {
    }
    css::CSSRule getElement(unsigned int index) {
        if (getLength() <= index)
            return 0;
        return ruleList[index];
    }
    void setElement(unsigned int index, css::CSSRule value) {
    }
    Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return ObjectArray<css::CSSRule>::dispatch(this, selector, id, argc, argv);
    }

    virtual void* getStaticPrivate() const {
        return 0;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CSSRULELIST_IMP_H
