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

#ifndef ES_CSSPARSER_H
#define ES_CSSPARSER_H

#include <cstring>
#include <deque>
#include <iostream>
#include <string>

#include <Object.h>
#include <org/w3c/dom/css/CSSPrimitiveValue.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/css/CSSStyleSheet.h>

#include "CSSTokenizer.h"
#include "CSSSerialize.h"
#include "MediaListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentImp;
class CSSStyleRuleImp;
class CSSStyleSheetImp;
class CSSStyleDeclarationImp;
class CSSMediaRuleImp;
class CSSRuleImp;

struct CSSParserNumber
{
    double number;
    bool integer;   // true if number was expressed in an integer format.

    operator double() const {
        return number;
    }
    bool isInteger() const {
        return integer;
    }
    int toInteger() const {
        return static_cast<int>(number);
    }
};

struct CSSParserString
{
    const char16_t* text;
    ssize_t length;

    std::u16string toString(bool caseSensitive = true) const {
        const char16_t* end = text + length;
        std::u16string string;
        for (auto i = text; i < end; ++i) {
            char16_t c = *i;
            if (c != '\\') {
                if (!caseSensitive)
                    c = toLower(c);
                string += c;
                continue;
            }
            if (end <= ++i) {
                string += c;
                break;
            }
            c = *i;
            int x = isHexDigit(c);
            if (!x) {
                if (c != '\n') {  // TODO: check '\r' as well?
                    if (!caseSensitive)
                        c = toLower(c);
                    string += c;
                }
                continue;
            }
            // unescape
            char32_t code = c - x;
            int count;
            for (count = 1; count < 6; ++count) {
                if (end <= ++i)
                    break;
                c = *i;
                x = isHexDigit(c);
                if (x)
                    code = code * 16 + (c - x);
                else if (isSpace(c))
                    break;
                else {
                    --i;
                    break;
                }
            }
            if (count == 6 && ++i < end) {
                c = *i;
                if (!isSpace(c))
                    --i;
            }
            if (code) {
                char16_t s[2];
                if (char16_t* t = utf32to16(code, s)) {
                    for (char16_t* p = s; p < t; ++p)
                        string += caseSensitive ? *p : toLower(*p);
                }
            } else {
                while (0 < count--)
                    string += u'0';
                if (isSpace(c))
                    string += c;
            }
        }
        return string;
    }

    operator std::u16string() const {
        return toString(false);
    }

    // returns 0x00FFFFFF upon an invalid #hex color
    unsigned toRGB() const {
        unsigned rgb = 0;
        unsigned h;
        if (length == 3) {
            for (int i = 0; i < 3; ++i) {
                if (!(h = isHexDigit(text[i])))
                    return 0x00FFFFFF;
                h = text[i] - h;
                rgb <<= 4;
                rgb += h;
                rgb <<= 4;
                rgb += h;
            }
        } else if (length == 6) {
            for (int i = 0; i < 6; ++i) {
                if (!(h = isHexDigit(text[i])))
                    return 0x00FFFFFF;
                rgb <<= 4;
                rgb += text[i] - h;
            }
        } else
            return 0x00FFFFFF;
        return rgb | 0xFF000000u;
    }
    bool operator==(const char16_t* s) const {
        return std::memcmp(text, s, length * sizeof(char16_t)) == 0 && s[length] == 0;
    }

    void clear() {
        length = 0;
    }
};

struct CSSParserExpr;

struct CSSParserTerm
{
    static const unsigned short CSS_TERM_FUNCTION = 201;
    static const unsigned short CSS_TERM_OPERATOR = 202;
    static const unsigned short CSS_TERM_BAR_NTH = 203;
    static const unsigned short CSS_TERM_INDEX = 204;
    static const unsigned short CSS_TERM_END = 205;

    short op;  // '/', ',', or '\0'
    unsigned short unit;
    CSSParserNumber number;  // TODO: number should be float
    unsigned rgb;   // also used as the keyword index
    CSSParserString text;
    CSSParserExpr* expr;  // for function

    unsigned propertyID;

    std::u16string getCssText();
    int getIndex() const {
        if (unit == CSS_TERM_INDEX)
            return rgb;
        else
            return -1;
    }
    double getNumber() const {
        return number;
    }
    std::u16string getString(bool caseSensitive = true) const {
        switch (unit) {
        case css::CSSPrimitiveValue::CSS_STRING:
        case css::CSSPrimitiveValue::CSS_URI:
        case css::CSSPrimitiveValue::CSS_IDENT:
        case css::CSSPrimitiveValue::CSS_UNICODE_RANGE:
        case CSS_TERM_FUNCTION:
            return text.toString(caseSensitive);
        default:
            return u"";
        }
    }
};

struct CSSParserExpr
{
    std::deque<CSSParserTerm> list;
    std::u16string priority;
    void push_front(const CSSParserTerm& term) {
        list.push_front(term);
    }
    void push_back(const CSSParserTerm& term) {
        list.push_back(term);
    }
    void setPriority(const std::u16string& priority) {
        this->priority = priority;
    }
    const std::u16string& getPriority() const {
        return priority;
    }
    std::u16string getCssText();
    bool isInherit() const {
        if (list.size() != 1)
            return false;
        const CSSParserTerm& term = list.front();
        if (term.unit == css::CSSPrimitiveValue::CSS_IDENT && term.text == u"inherit")
            return true;
        return false;
    }
};

class CSSParser
{
    DocumentImp* document;
    CSSTokenizer tokenizer;
    CSSStyleSheetImp* styleSheet;
    CSSStyleDeclarationImp* styleDeclaration;
    CSSParserExpr* styleExpression;
    CSSMediaRuleImp* mediaRule;
    bool caseSensitive;  // for element names and attribute names.
    bool importable;

    Retained<MediaListImp> mediaList;

    void reset(const std::u16string cssText) {
        tokenizer.reset(cssText);
    }
public:
    CSSParser() :
        document(0),
        styleSheet(0),
        styleDeclaration(0),
        styleExpression(0),
        mediaRule(0),
        caseSensitive(false),
        importable(true)
    {
    }

    css::CSSStyleSheet parse(DocumentImp* document, const std::u16string& cssText);
    css::CSSStyleDeclaration parseDeclarations(const std::u16string& cssDecl);
    CSSParserExpr* parseExpression(const std::u16string& cssExpr);

    DocumentImp* getDocument() const {
        return document;
    }

    CSSTokenizer* getTokenizer() {
        return &tokenizer;
    }
    CSSStyleSheetImp* getStyleSheet() {
        return styleSheet;
    }
    void setStyleDeclaration(CSSStyleDeclarationImp* styleDeclaration) {
        this->styleDeclaration = styleDeclaration;
    }
    CSSStyleDeclarationImp* getStyleDeclaration() {
        return styleDeclaration;
    }

    void setExpression(CSSParserExpr* styleExpression) {
        this->styleExpression = styleExpression;
    }
    CSSParserExpr* getExpression() {
        return styleExpression;
    }

    void setMediaRule(CSSMediaRuleImp* mediaRule) {
        this->mediaRule = mediaRule;
    }
    CSSMediaRuleImp* getMediaRule() {
        return mediaRule;
    }

    MediaListImp* getMediaList() {
        return &mediaList;
    }

    bool getCaseSensitivity() const {
        return caseSensitive;
    }
    void setCaseSensitivity(bool value) {
        caseSensitive = value;
    }

    void disableImport() {
        importable = false;
    }
    bool isImportable() {
        return importable;
    }
};

inline void CSSerror(CSSParser* parser, const char* message, ...)
{
    std::cerr << message << '\n';
}

inline int CSSlex(CSSParser* parser)
{
    return parser->getTokenizer()->getToken();
}

}}}}  // org::w3c::dom::bootstrap

extern "C" int CSSparse(org::w3c::dom::bootstrap::CSSParser* parser);

#endif  // ES_CSSPARSER_H
