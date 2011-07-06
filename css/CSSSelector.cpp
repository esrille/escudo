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

#include "CSSSelector.h"

#include <assert.h>

#include <org/w3c/dom/Element.h>

#include "CSSStyleDeclarationImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace
{

inline bool find(const std::u16string& s, const std::u16string& t)
{
    return s.find(t) != std::u16string::npos;
}

// s is a whitespace-separated list of words
inline bool contains(const std::u16string& s, const std::u16string& t)
{
    size_t pos = 0;
    for (;;) {
        pos = s.find(t, pos);
        if (pos == std::u16string::npos)
            return false;
        if ((pos == 0 || isSpace(s[pos])) && (pos + t.length() == s.length() || isSpace(t[pos])))
            return true;
        ++pos;
    }
}

inline bool startsWith(const std::u16string& s, const std::u16string& t)
{
    return !s.compare(0, t.length(), t);
}

inline bool endsWith(const std::u16string& s, const std::u16string& t)
{
    if (s.length() < t.length())
        return false;
    return !s.compare(s.length() - t.length(), t.length(), t);
}

}

void CSSSelectorsGroup::serialize(std::u16string& result)
{
    for (auto i = selectors.begin(); i != selectors.end(); ++i) {
        if (i != selectors.begin())
            result += u", ";
        (*i)->serialize(result);

        if (CSSSerializeControl.serializeSpecificity) {
            result += u" /* specificity = ";
            result += CSSSerializeHex((*i)->getSpecificity());
            result += u" */ ";
        }
    }
}

void CSSSelector::serialize(std::u16string& result)
{
    for (auto i = simpleSelectors.begin(); i != simpleSelectors.end(); ++i)
        (*i)->serialize(result);
}

void CSSPrimarySelector::serialize(std::u16string& result)
{
    if (combinator) {
        if (combinator != u' ') {
            result += u' ';
            result += static_cast<char16_t>(combinator);
        }
        result += u' ';
    }
    if (name == u"*")
        result += name;
    else
        result += CSSSerializeIdentifier(name);
    for (auto i = chain.begin(); i != chain.end(); ++i)
        (*i)->serialize(result);
}

void CSSAttributeSelector::serialize(std::u16string& result)
{
    result += u'[';
    result += CSSSerializeIdentifier(name);
    if (op) {
        result += static_cast<char16_t>(op);
        if (op != u'=')
            result += u'=';
        result += CSSSerializeString(value);
    }
    result += u']';
}

void CSSNthPseudoClassSelector::serialize(std::u16string& text)
{
    text += u':' + CSSSerializeIdentifier(name) + u'(';
    if (!a)
        text += CSSSerializeInteger(b);
    else {
        if (a == -1)  // TODO: Ask Anne about this.
            text += '-';
        else if (a != 1 && a != -1)
            text += CSSSerializeInteger(a);
        text += u'n';
        if (b) {
            if (0 < b)
                text += u'+';
            text += CSSSerializeInteger(b);
        }
    }
    text += u')';
}

CSSSpecificity CSSPrimarySelector::getSpecificity() {
    CSSSpecificity specificity;
    if (name != u"*")
        specificity += CSSSpecificity(0, 0, 0, 1);
    for (auto i = chain.begin(); i != chain.end(); ++i)
        specificity += (*i)->getSpecificity();
    return specificity;
}

CSSSpecificity CSSSelector::getSpecificity()
{
    CSSSpecificity specificity;
    for (auto i = simpleSelectors.begin(); i != simpleSelectors.end(); ++i)
        specificity += (*i)->getSpecificity();
    return specificity;
}

bool CSSPrimarySelector::match(Element e)
{
    if (name != u"*") {
        if (e.getLocalName() != name)
            return false;
        if (namespacePrefix != u"*") {
            if (!e.getNamespaceURI().hasValue() || e.getNamespaceURI().value() != namespacePrefix)
                return false;
        }
    }
    for (auto i = chain.begin(); i != chain.end(); ++i) {
        if (!(*i)->match(e))
            return false;
    }
    return true;
}

bool CSSIDSelector::match(Element e)
{
    Nullable<std::u16string> id = e.getAttribute(u"id");
    if (!id.hasValue())
        return false;
    return id.value() == name;  // TODO: ignore case
}

bool CSSClassSelector::match(Element e)
{
    Nullable<std::u16string> classes = e.getAttribute(u"class");
    if (!classes.hasValue())
        return false;
    return contains(classes.value(), name);  // TODO: ignore case
}

bool CSSAttributeSelector::match(Element e)
{
    Nullable<std::u16string> attr = e.getAttribute(name);
    if (!attr.hasValue())
        return false;
    switch (op) {
    case None:
        return true;
    case Equals:
        return attr.value() == value;
    case Includes:
        if (attr.value().length() == 0 || contains(attr.value(), u" "))
            return false;
        return contains(attr.value(), value);
    case DashMatch:
        if (!startsWith(attr.value(), value))
            return false;
        return attr.value().length() == value.length() || attr.value()[value.length()] == u'-';
    case PrefixMatch:
        if (attr.value().length() == 0)
            return false;
        return startsWith(attr.value(), value);
    case SuffixMatch:
        if (attr.value().length() == 0)
            return false;
        return endsWith(attr.value(), value);
    case SubstringMatch:
        if (attr.value().length() == 0)
            return false;
        return find(attr.value(), value);
    default:
        return false;
    }
}

bool CSSSelector::match(Element e)
{
    if (!e || simpleSelectors.size() == 0)
        return false;

    auto i = simpleSelectors.rbegin();
    if (!(*i)->match(e))
        return false;
    int combinator = (*i)->getCombinator();
    ++i;
    while (i != simpleSelectors.rend()) {
        switch (combinator) {
        case CSSPrimarySelector::Descendant:
            while (e = e.getParentElement()) {  // TODO: do we need to retry from here upon failure?
                if ((*i)->match(e))
                    break;
            }
            if (!e)
                return false;
            break;
        case CSSPrimarySelector::Child:
            e = e.getParentElement();
            if (!(*i)->match(e))
                return false;
            break;
        case CSSPrimarySelector::AdjacentSibling:
            e = e.getPreviousElementSibling();
            if (!(*i)->match(e))
                return false;
            break;
        case CSSPrimarySelector::GeneralSibling:
            while (e = e.getPreviousElementSibling()) {
                if ((*i)->match(e))
                    break;
            }
            if (!e)
                return false;
        default:
            return false;
        }
        combinator = (*i)->getCombinator();
        ++i;
    }
    if (combinator != CSSPrimarySelector::None)
        return false;
    return true;
}

CSSSelector* CSSSelectorsGroup::match(Element e)
{
    specificity = CSSSpecificity();
    for (auto i = selectors.begin(); i != selectors.end(); ++i) {
        if ((*i)->match(e)) {
            specificity = (*i)->getSpecificity();
            return *i;
        }
    }
    return 0;
}

CSSPseudoElementSelector* CSSPrimarySelector::getPseudoElement() const {
    if (chain.empty())
        return 0;
    if (CSSPseudoElementSelector* pseudo = dynamic_cast<CSSPseudoElementSelector*>(chain.back()))
        return pseudo;
    return 0;
}

CSSPseudoElementSelector* CSSSelector::getPseudoElement() const {
    if (simpleSelectors.empty())
        return 0;
    simpleSelectors.back()->getPseudoElement();
}

CSSPseudoElementSelector::CSSPseudoElementSelector(int id) :
    CSSPseudoSelector(CSSStyleDeclarationImp::getPseudoElementName(id)),
    id(id)
{
}

CSSPseudoSelector* CSSPseudoSelector::createPseudoSelector(int type, const std::u16string& ident)
{
    if (type == PseudoClass) {
        /* Exceptions: :first-line, :first-letter, :before and :after. */
        if (ident == u"first-line" || ident == u"first-letter" || ident == u"before" || ident == u"after")
            type = PseudoElement;
    }
    int id = -1;
    switch (type) {
    case PseudoClass:
        return new(std::nothrow) CSSPseudoClassSelector(ident);
        break;
    case PseudoElement:
        id = CSSStyleDeclarationImp::getPseudoElementID(ident);
        if (0 <= id)
            return new(std::nothrow) CSSPseudoElementSelector(id);
        break;
    default:
        break;
    }
    return 0;
}

CSSPseudoSelector* CSSPseudoSelector::createPseudoSelector(int type, const CSSParserTerm& function)
{
    switch (type) {
    case PseudoClass:
        return new(std::nothrow) CSSPseudoClassSelector(function);
        break;
    case PseudoElement:
        // No functional pseudo element is defined in CSS 2.1
        break;
    default:
        break;
    }
    return 0;
}

}}}}  // org::w3c::dom::bootstrap