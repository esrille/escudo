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

#ifndef ES_CSSVALUEPARSER_H
#define ES_CSSVALUEPARSER_H

#include "CSSParser.h"

#include <assert.h> // TODO
#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CSSValueParser;

class CSSValueRule
{
public:
    enum
    {
        Nop = 0,
        AB,
        AnyIdent,
        AnyNumber,
        AnyInteger,
        Angle,
        Bar,
        Color,
        Comma,
        DoubleBar,
        Function,
        Ident,
        Juxtapose,
        Length,
        NonNegativeLength,
        Number,
        Percentage,
        NonNegativePercentage,
        Slash,
        String,
        Uri,
        Index // Indicates the keyword index is stored in b as hint
    };
    static const unsigned short Inf = 65535u;
public:
    short op;
    unsigned propertyID;
    unsigned short a;  // used for hint, too
    unsigned short b;
    float number;
    const char16_t* text;
    bool (CSSValueParser::*f)(const CSSValueRule& rule);
    std::list<CSSValueRule> list;
public:
    CSSValueRule() :
        op(Nop),
        propertyID(0),
        a(0),
        b(0),
        number(0.0),
        text(0),
        f(0)
    {
    }
    CSSValueRule(unsigned short a, unsigned short b, const CSSValueRule& rule) :
        op(AB),
        propertyID(0),
        a(a),
        b(b),
        f(0)
    {
        list.push_back(rule);
    }
    CSSValueRule(int op) :
        op(op),
        propertyID(0),
        a(0),
        b(0),
        number(0.0),
        text(0),
        f(0)
    {
    }
    CSSValueRule(const char16_t* function, const CSSValueRule& expr) :
        op(Function),
        propertyID(0),
        text(function),
        f(0)
    {
        list.push_back(expr);
    }
    CSSValueRule(const char16_t* text) :
        op(Ident),
        propertyID(0),
        a(0),
        b(0),
        number(0.0),
        text(text),
        f(0) {
    }
    CSSValueRule(const char16_t* text, int hint) :
        op(Ident),
        propertyID(0),
        a(Index),
        b(hint),
        number(hint),
        text(text),
        f(0) {
    }
    CSSValueRule(const char16_t* text, float hint) :
        op(Ident),
        propertyID(0),
        a(Number),
        b(0),
        number(hint),
        text(text),
        f(0) {
    }
    CSSValueRule(float number) :
        op(Number),
        propertyID(0),
        number(number),
        f(0) {
    }
    CSSValueRule operator+(const CSSValueRule& rule) const {
        CSSValueRule r;
        r.op = Juxtapose;
        if (op == Juxtapose)
            r.list = list;
        else
            r.list.push_back(*this);
        r.list.push_back(rule);
        return r;
    }
    CSSValueRule operator|(const CSSValueRule& rule) const {
        CSSValueRule r;
        r.op = Bar;
        if (op == Bar)
            r.list = list;
        else
            r.list.push_back(*this);
        r.list.push_back(rule);
        return r;
    }
    CSSValueRule operator||(const CSSValueRule& rule) const {
        CSSValueRule r;
        r.op = DoubleBar;
        if (op == DoubleBar)
            r.list = list;
        else
            r.list.push_back(*this);
        r.list.push_back(rule);
        return r;
    }
    CSSValueRule operator[](bool (CSSValueParser::*f)(const CSSValueRule& rule)) {
        CSSValueRule r = *this;
        r.f = f;
        return r;
    }
    CSSValueRule& operator[](unsigned propertyID) {
        this->propertyID = propertyID;
        return *this;
    }
    bool isValid(CSSValueParser* parser, unsigned propertyID) const;
    unsigned short hasHint() const {
        return a;
    }
    bool hasKeyword() const {
        return a == Index;
    }
    unsigned short getKeyword() const {
        assert(a == Index);
        return b;
    }
    operator std::u16string() const;
};

class CSSValueParser
{
    friend class CSSValueRule;

    static CSSValueRule angle;
    static CSSValueRule percent_length;
    static CSSValueRule percent_non_negative_length;
    static CSSValueRule auto_length;
    static CSSValueRule auto_non_negative_length;
    static CSSValueRule none_length;
    static CSSValueRule none_non_negative_length;
    static CSSValueRule normal_length;
    static CSSValueRule auto_numbering;
    static CSSValueRule comma;
    static CSSValueRule ident;
    static CSSValueRule inherit;
    static CSSValueRule integer;
    static CSSValueRule length;
    static CSSValueRule non_negative_length;
    static CSSValueRule number;
    static CSSValueRule percentage;
    static CSSValueRule non_negative_percentage;
    static CSSValueRule slash;
    static CSSValueRule string;
    static CSSValueRule uri;
    static CSSValueRule lineHeight;
    static CSSValueRule list_style_type;
    static CSSValueRule listStyleType;
    static CSSValueRule absolute_size;
    static CSSValueRule border_width;
    static CSSValueRule border_style;
    static CSSValueRule counter;
    static CSSValueRule family_name;
    static CSSValueRule font_style;
    static CSSValueRule font_variant;
    static CSSValueRule font_weight;
    static CSSValueRule generic_family;
    static CSSValueRule relative_size;
    static CSSValueRule shape;
    static CSSValueRule azimuth;
    static CSSValueRule backgroundAttachment;
    static CSSValueRule backgroundImage;
    static CSSValueRule backgroundPosition;
    static CSSValueRule backgroundRepeat;
    static CSSValueRule background;
    static CSSValueRule borderCollapse;
    static CSSValueRule borderColor;
    static CSSValueRule borderSpacing;
    static CSSValueRule borderStyle;
    static CSSValueRule borderTopStyle; // = borderRightStyle, borderBottomStyle, borderLeftStyle
    static CSSValueRule borderTopWidth; // = borderRightWidth, borderBottomWidth, borderLeftWidth
    static CSSValueRule borderWidth;
    static CSSValueRule border;  // = borderTop, borderRight, borderBottom, borderLeft
    static CSSValueRule captionSide;
    static CSSValueRule clear;
    static CSSValueRule clip;
    static CSSValueRule color;
    static CSSValueRule content;
    static CSSValueRule direction;
    static CSSValueRule display;
    static CSSValueRule float_;
    static CSSValueRule fontFamily;
    static CSSValueRule fontSize;
    static CSSValueRule fontStyle;
    static CSSValueRule fontVariant;
    static CSSValueRule fontWeight;
    static CSSValueRule font;
    static CSSValueRule letterSpacing;
    static CSSValueRule listStyleImage;
    static CSSValueRule listStylePosition;
    static CSSValueRule listStyle;
    static CSSValueRule margin;
    static CSSValueRule outlineColor;
    static CSSValueRule outlineStyle;
    static CSSValueRule outlineWidth;
    static CSSValueRule outline;
    static CSSValueRule overflow;
    static CSSValueRule padding;
    static CSSValueRule pageBreak;  // = pageBreakAfter, pageBreakBefore
    static CSSValueRule pageBreakInside;
    static CSSValueRule position;
    static CSSValueRule tableLayout;
    static CSSValueRule textAlign;
    static CSSValueRule textDecoration;
    static CSSValueRule textTransform;
    static CSSValueRule unicodeBidi;
    static CSSValueRule verticalAlign;
    static CSSValueRule visibility;
    static CSSValueRule whiteSpace;
    static CSSValueRule zIndex;

    static CSSValueRule binding;

    int propertyID;
    CSSValueRule* rule;
    CSSParserExpr* expr;
    CSSParserTerm op;

    std::deque<CSSParserTerm>::iterator iter;

    std::deque<CSSParserTerm*> stack;

    static void initializeRules();

    struct Initializer
    {
        Initializer() {
            initializeRules();
        }
    };

public:
    struct Pos
    {
        short op;
        std::deque<CSSParserTerm>::iterator iter;
        Pos() {
        }
        Pos(short op, std::deque<CSSParserTerm>::iterator iter) :
            op(op),
            iter(iter) {
        }
    };

    CSSValueParser(int propertyID);
    bool isValid(CSSParserExpr* expr);
    std::deque<CSSParserTerm*>& getStack() {
        return stack;
    }

    CSSParserTerm& getToken();
    bool acceptToken();
    Pos getPos() const;
    void setPos(const Pos& pos);

    void push(CSSParserTerm* term, unsigned propertyID) {
        term->propertyID = propertyID;
        stack.push_back(term);
    }

    CSSParserExpr* switchExpression(CSSParserExpr* next)
    {
        CSSParserExpr* prev = expr;
        expr = next;
        Pos pos(0, expr->list.begin());
        setPos(pos);
        return prev;
    }

    bool colorKeyword(const CSSValueRule& rule);
    bool rgb(const CSSValueRule& rule);
};

unsigned CSSGetKeywordColor(const std::u16string& keyword);

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSVALUEPARSER_H
