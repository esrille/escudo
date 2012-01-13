/*
 * Copyright 2010-2012 Esrille Inc.
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

#ifndef ES_CSSSELECTOR_H
#define ES_CSSSELECTOR_H

#include <assert.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <deque>
#include <string>

#include <Object.h>
#include <org/w3c/dom/Element.h>

#include "CSSParser.h"
#include "CSSSerialize.h"
#include "utf.h"

namespace org { namespace w3c { namespace dom {

namespace bootstrap {

class ViewCSSImp;

class CSSSpecificity
{
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;

    static unsigned char sum(unsigned char x, unsigned char y) {
        unsigned value = x + y;
        return std::min(value, 255u);
    }
public:
    CSSSpecificity() :
        a(0),
        b(0),
        c(0),
        d(0) {
    }
    CSSSpecificity(unsigned char a, unsigned char b, unsigned char c, unsigned char d) :
        a(a),
        b(b),
        c(c),
        d(d) {
    }
    operator unsigned() const {
        return (a << 24) | (b << 16) | (c << 8) | d;
    }
    CSSSpecificity& operator +=(const CSSSpecificity& value)
    {
        a = sum(a, value.a);
        b = sum(b, value.b);
        c = sum(c, value.c);
        d = sum(d, value.d);
        return *this;
    }
};

inline bool operator<(const CSSSpecificity& a, const CSSSpecificity& b) {
    return static_cast<unsigned>(a) < static_cast<unsigned>(b);
}

class CSSPseudoElementSelector;

// implemented as a type selector or a universal selector with a chain of sub selectors.
class CSSSimpleSelector
{
protected:
    std::u16string name;    // hash, class not including the 1st '#' or '.', attrib ident, or pseudo ident
public:
    CSSSimpleSelector(const std::u16string& name) :
        name(name) {
    }
    const std::u16string& getName() const {
        return name;
    }
    void setName(const std::u16string& name) {
        this->name = name;
    }
    virtual void serialize(std::u16string& text) {
        text += CSSSerializeIdentifier(name);
    }
    virtual CSSSpecificity getSpecificity() = 0;
    virtual bool match(Element element, ViewCSSImp* view) {
        return false;
    }
    virtual bool isValid() const {
        return true;
    }
};

// a type selector, or a universal selector
class CSSPrimarySelector : public CSSSimpleSelector
{
public:
    // combinator
    enum
    {
        None = '\0',
        Descendant = ' ',
        Child = '>',
        AdjacentSibling = '+',
        GeneralSibling = '~'
    };
private:
    int combinator;
    std::u16string namespacePrefix;  // IDENT, '*', or empty
    std::deque<CSSSimpleSelector*> chain;
public:
    CSSPrimarySelector() :
        CSSSimpleSelector(u"*"),
        combinator(None) {
    }
    CSSPrimarySelector(const std::u16string& namespacePrefix) :
        CSSSimpleSelector(u"*"),
        combinator(None),
        namespacePrefix(namespacePrefix) {
    }
    CSSPrimarySelector(const std::u16string& namespacePrefix, const std::u16string& elementName) :
        CSSSimpleSelector(elementName),
        combinator(None),
        namespacePrefix(namespacePrefix) {
    }
    void append(CSSSimpleSelector* selector) {
        if (selector)
            chain.push_back(selector);
    }
    const std::u16string& getNamespacePrefix() const {
        return namespacePrefix;
    }
    void setNamespacePrefix(const std::u16string& namespacePrefix) {
        this->namespacePrefix = namespacePrefix;
    }
    int getCombinator() const {
        return combinator;
    }
    void setCombinator(int combinator) {
        assert(std::strchr(" >+~", combinator));
        this->combinator = combinator;
    }
    virtual void serialize(std::u16string& text);
    virtual CSSSpecificity getSpecificity();
    virtual bool match(Element element, ViewCSSImp* view);
    virtual bool isValid() const;
    CSSPseudoElementSelector* getPseudoElement() const;
};

// '#' IDENT
class CSSIDSelector : public CSSSimpleSelector
{
public:
    CSSIDSelector(const std::u16string& ident) :
        CSSSimpleSelector(ident) {
    }
    virtual void serialize(std::u16string& text) {
        text += u'#' + CSSSerializeIdentifier(name);
    }
    virtual CSSSpecificity getSpecificity() {
        return CSSSpecificity(0, 1, 0, 0);
    }
    virtual bool match(Element element, ViewCSSImp* view);
    virtual bool isValid() const {
        return !name.empty();
    }
};

// '.' IDENT
class CSSClassSelector : public CSSSimpleSelector
{
public:
    CSSClassSelector(const std::u16string& ident) :
        CSSSimpleSelector(ident) {
    }
    virtual void serialize(std::u16string& text) {
        text += u'.' + CSSSerializeIdentifier(name);
    }
    virtual CSSSpecificity getSpecificity() {
            return CSSSpecificity(0, 0, 1, 0);
    }
    virtual bool match(Element element, ViewCSSImp* view);
    virtual bool isValid() const {
        return !name.empty();
    }
};

class CSSAttributeSelector : public CSSSimpleSelector
{
public:
    // op
    enum
    {
        None = '\0',
        // Attribute presence and value selectors
        Equals = '=',         // =
        Includes = '~',       // ~=
        DashMatch = '|',      // |=
        // Substring matching attribute selectors
        PrefixMatch = '^',    // ^=
        SuffixMatch = '$',    // $=
        SubstringMatch = '*', // *=
    };
private:
    int op;
    std::u16string value;
    std::u16string namespacePrefix;
public:
    CSSAttributeSelector(const std::u16string& ident) :
        CSSSimpleSelector(ident),
        op(None) {
    }
    CSSAttributeSelector(const std::u16string& ident, int op, const std::u16string& value) :
        CSSSimpleSelector(ident),
        op(op),
        value(value) {
    }
    CSSAttributeSelector(const std::u16string& namespacePrefix, const std::u16string& ident) :
        CSSSimpleSelector(ident),
        op(None),
        namespacePrefix(namespacePrefix) {
    }
    CSSAttributeSelector(const std::u16string& namespacePrefix, const std::u16string& ident, int op, const std::u16string& value) :
        CSSSimpleSelector(ident),
        op(op),
        value(value),
        namespacePrefix(namespacePrefix) {
    }
    virtual void serialize(std::u16string& text);
    virtual CSSSpecificity getSpecificity() {
            return CSSSpecificity(0, 0, 1, 0);
    }
    virtual bool match(Element element, ViewCSSImp* view);
};

class CSSPseudoSelector : public CSSSimpleSelector
{
    CSSParserExpr* expression;
public:
    CSSPseudoSelector(const std::u16string& ident) :
        CSSSimpleSelector(ident) {
        expression = 0;
    }
    CSSPseudoSelector(const CSSParserTerm& function) :
        CSSSimpleSelector(function.getString(false)),
        expression(function.expr) {
    }

    virtual void serialize(std::u16string& text) {
        text += CSSSerializeIdentifier(name);
        if (expression)
            text += u'(' + expression->getCssText() + u')';
    }

    enum Type {
        PseudoClass,
        PseudoElement
    };
    static CSSPseudoSelector* createPseudoSelector(int type, const std::u16string& ident);
    static CSSPseudoSelector* createPseudoSelector(int type, const CSSParserTerm& function);

    static int getPseudoClassID(const std::u16string& name);
    static const char16_t* getPseudoClassName(int id);
    static int getPseudoElementID(const std::u16string& name);
    static const char16_t* getPseudoElementName(int id);
};

// :
class CSSPseudoClassSelector : public CSSPseudoSelector
{
    int id;

public:
    // Pseudo-class id
    // The order needs to be in sync with pseudoClassNames[]
    enum
    {
        Unknown = -1,
        // CSS 2.1
        Link,
        Visited,
        Hover,
        Active,
        Focus,
        Lang,
        FirstChild,
        // CSS 3
        LastChild,
        Target,
        Enabled,
        Disabled,
        Checked,
        Indeterminate,
        Root,
        Empty,
        FirstOfType,
        LastOfType,
        OnlyChild,
        OnlyOfType,

        MaxPseudoClasses
    };

    CSSPseudoClassSelector(const std::u16string& ident, int id);
    CSSPseudoClassSelector(const CSSParserTerm& function, int id);
    virtual void serialize(std::u16string& text) {
        text += u':';
        CSSPseudoSelector::serialize(text);
    }
    virtual CSSSpecificity getSpecificity() {
        return CSSSpecificity(0, 0, 1, 0);
    }
    virtual bool match(Element element, ViewCSSImp* view);
    virtual bool isValid() const {
        return id != Unknown;
    }

    unsigned getID() const {
        return id;
    }
};

// :lang
class CSSLangPseudoClassSelector : public CSSPseudoClassSelector
{
    std::u16string lang;
public:
    CSSLangPseudoClassSelector(const std::u16string& lang) :
        CSSPseudoClassSelector(u"lang", CSSPseudoClassSelector::Lang),
        lang(lang)
    {
        toLower(this->lang);    // TODO: html only
    }
    virtual void serialize(std::u16string& text);
    virtual bool match(Element element, ViewCSSImp* view);
};

// :nth-
class CSSNthPseudoClassSelector : public CSSPseudoClassSelector
{
    // an+b
    long a;
    long b;
public:
    CSSNthPseudoClassSelector(long a, long b, int id) :
        CSSPseudoClassSelector(u"", id),
        a(a),
        b(b) {
    }
    virtual void serialize(std::u16string& text);
};

// ::
class CSSPseudoElementSelector : public CSSPseudoSelector
{
    int id;

public:
    // Pseudo-element id
    // The order needs to be in sync with pseudoElementNames[]
    enum
    {
        Unknown = -1,
        NonPseudo,
        FirstLine,
        FirstLetter,
        Before,
        After,
        Marker,     // cf. http://www.w3.org/TR/css3-lists/#marker-pseudoelement

        MaxPseudoElements
    };

    CSSPseudoElementSelector(int id);
    virtual void serialize(std::u16string& text) {
        text += u"::";
        CSSPseudoSelector::serialize(text);
    }
    virtual CSSSpecificity getSpecificity() {
        return CSSSpecificity(0, 0, 0, 1);
    }
    virtual bool match(Element element, ViewCSSImp* view) {
        return id != Unknown;
    }
    virtual bool isValid() const {
        return id != Unknown;
    }

    unsigned getID() const {
        return id;
    }
};

// :not
class CSSNegationPseudoClassSelector : public CSSPseudoClassSelector
{
    CSSSimpleSelector* selector;
public:
    CSSNegationPseudoClassSelector(CSSSimpleSelector* selector) :
        CSSPseudoClassSelector(u"not", -1),
        selector(selector) {
    }
    virtual void serialize(std::u16string& text) {
        text += u":not(";
        selector->serialize(text);
        text += u')';
    }
    virtual CSSSpecificity getSpecificity() {
        return selector->getSpecificity();
    }
    virtual bool isValid() const {
        return selector && selector->isValid();
    }
};

class CSSSelector
{
    std::deque<CSSPrimarySelector*> simpleSelectors;
public:
    CSSSelector(CSSPrimarySelector* simpleSelector) {
        simpleSelectors.push_back(simpleSelector);
    }
    void append(int combinator, CSSPrimarySelector* simpleSelector) {
        if (simpleSelector) {
            simpleSelector->setCombinator(combinator);
            simpleSelectors.push_back(simpleSelector);
        }
    }
    void serialize(std::u16string& text);
    CSSSpecificity getSpecificity();

    bool match(Element element, ViewCSSImp* view);
    CSSPseudoElementSelector* getPseudoElement() const;

    bool isValid() const;
};

class CSSSelectorsGroup
{
    std::deque<CSSSelector*> selectors;
public:
    CSSSelectorsGroup(CSSSelector* selector) {
        selectors.push_back(selector);
    }

    std::deque<CSSSelector*>::iterator begin() {
        return selectors.begin();
    }
    std::deque<CSSSelector*>::iterator end() {
        return selectors.end();
    }

    void append(CSSSelector* selector) {
        if (selector)
            selectors.push_back(selector);
    }
    void serialize(std::u16string& text);
    bool isValid() const {
        if (selectors.empty())
            return false;
        for (auto i = selectors.begin(); i != selectors.end(); ++i) {
            if (!(*i)->isValid())
                return false;
        }
        return true;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSSELECTOR_H
