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

#ifndef ES_CSSTOKENIZER_H
#define ES_CSSTOKENIZER_H

#include <string>

#include "CSSSerialize.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

struct CSSParserString;

class CSSTokenizer
{
public:
    enum
    {
        StartStyleSheet,
        StartDeclarationList,
        StartExpression,
        Normal,
        Nth
    };

    static unsigned long parseInt(const char16_t* text, ssize_t length) {
        const char16_t* end = text + length;
        unsigned long value = 0;
        for (const char16_t* p = text; p < end && isDigit(*p); ++p) {
            value *= 10;
            value += *p - '0';
        }
        return value;
    }

    static double parseNumber(const char16_t* text, ssize_t length, const char16_t** endptr = 0) {
        const char16_t* end = text + length;
        double value = 0.0;
        double frac = 0.0;
        double e = 0.1;
        bool hasE = false;
        if (endptr)
            *endptr = 0;
        for (const char16_t* p = text; p < end; ++p) {
            if (endptr && !isDigit(*p) && (*p != '.' || hasE)) {
                *endptr = p;
                break;
            }
            if (*p == '.') {
                hasE = true;
                continue;
            }
            if (!hasE) {
                value *= 10;
                value += *p - '0';
                continue;
            }
            frac += (*p - '0') * e;
            e *= 0.1;
        }
        return value + frac;
    }

private:
    int mode;
    std::u16string cssText;
    const char16_t* yyin;
    const char16_t* yylimit;
    const char16_t* yymarker;

    static void parseURL(const char16_t* text, ssize_t length, CSSParserString* string);

public:
    CSSTokenizer() :
        mode(StartStyleSheet),
        yyin(0),
        yylimit(0),
        yymarker(0) {
    }

    void reset(const std::u16string cssText, int mode = StartStyleSheet) {
        this->mode = mode;
        this->cssText = cssText;
        yyin = this->cssText.c_str();
        yylimit = yyin + this->cssText.length();
        yymarker = 0;
    }

    int getToken();
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSTOKENIZER_H
