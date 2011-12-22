/*
 * Copyright 2011 Esrille Inc.
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

#include "CounterImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

namespace {

struct AdditiveGlyph
{
    int weight;
    char16_t* glyph;
};

// 1 4999
AdditiveGlyph upperRoman[] =
{
    1000, u"M",
    900, u"CM",
    500, u"D",
    400, u"CD",
    100, u"C",
    90, u"XC",
    50, u"L",
    40, u"XL",
    10, u"X",
    9, u"IX",
    5, u"V",
    4, u"IV",
    1, u"I",
    0, 0
};

// 1 4999
AdditiveGlyph lowerRoman[] =
{
    1000, u"m",
    900, u"cm",
    500, u"d",
    400, u"cd",
    100, u"c",
    90, u"xc",
    50, u"l",
    40, u"xl",
    10, u"x",
    9, u"ix",
    5, u"v",
    4, u"iv",
    1, u"i",
    0, 0
};

std::u16string convertToRoman(int n, AdditiveGlyph* glyphList)
{
    if (n < 0 || 5000 <= n)
        return toString(n);
    std::u16string value;
    for (AdditiveGlyph* i = glyphList; i->glyph; ++i) {
        int t = n / i->weight;
        while (0 < t--)
            value += i->glyph;
    }
    return value;
}

}

void CounterImp::reset(int number)
{
    counters.push_back(number);
}

void CounterImp::increment(int number)
{
    if (counters.empty())
        counters.push_back(0);
    counters.back() += number;
}

bool CounterImp::restore()
{
    counters.pop_back();
    return counters.empty();
}

std::u16string CounterImp::eval(const std::u16string& separator, unsigned type)
{
    this->separator = separator;
    this->listStyle.setValue(type);
    if (counters.empty())
        return u"";
    std::u16string value;
    if (separator.empty()) {
        switch (type) {
        case CSSListStyleTypeValueImp::Decimal:
            value = toString(counters.back());
            break;
        case CSSListStyleTypeValueImp::DecimalLeadingZero:
            value = toString(counters.back() % 100);
            if (value.length() == 1)
                value = u"0" + value;
            break;
        case CSSListStyleTypeValueImp::LowerAlpha:
        case CSSListStyleTypeValueImp::LowerLatin:
            value = std::u16string(1, u'a' + static_cast<unsigned>(counters.back()) % 26 - 1);
            break;
        case CSSListStyleTypeValueImp::UpperAlpha:
        case CSSListStyleTypeValueImp::UpperLatin:
            value = std::u16string(1, u'A' + static_cast<unsigned>(counters.back()) % 26 - 1);
            break;
        case CSSListStyleTypeValueImp::LowerGreek:
            // This style is only defined because CSS2.1 has it.
            // It doesn't appear to actually be used in Greek texts.
            value = std::u16string(1, u"αβγδεζηθικλμνξοπρστυφχψω"[static_cast<unsigned>(counters.back() - 1) % 24]);
            break;
        case CSSListStyleTypeValueImp::LowerRoman:
            value = convertToRoman(counters.back(), lowerRoman);
            break;
        case CSSListStyleTypeValueImp::UpperRoman:
            value = convertToRoman(counters.back(), upperRoman);
            break;
        case CSSListStyleTypeValueImp::Armenian:
        case CSSListStyleTypeValueImp::Georgian:
        default:
            value = toString(counters.back());
            break;
        }
        return value;
    }
    for (auto i = counters.begin(); i != counters.end(); ++i) {
        if (i != counters.begin())
            value += separator;
        value += toString(*i);
    }
    return value;
}

std::u16string CounterImp::getIdentifier()
{
    return identifier;
}

std::u16string CounterImp::getListStyle()
{
    return listStyle.getCssText();
}

std::u16string CounterImp::getSeparator()
{
    return separator;
}

}
}
}
}
