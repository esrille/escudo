/*
 * Copyright 2011, 2012 Esrille Inc.
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

// 1 9999
AdditiveGlyph armenian[] =
{
    9000, u"\u0554",
    8000, u"\u0553",
    7000, u"\u0552",
    6000, u"\u0551",
    5000, u"\u0550",
    4000, u"\u054F",
    3000, u"\u054E",
    2000, u"\u054D",
    1000, u"\u054C",
    900, u"\u054B",
    800, u"\u054A",
    700, u"\u0549",
    600, u"\u0548",
    500, u"\u0547",
    400, u"\u0546",
    300, u"\u0545",
    200, u"\u0544",
    100, u"\u0543",
    90, u"\u0542",
    80, u"\u0541",
    70, u"\u0540",
    60, u"\u053F",
    50, u"\u053E",
    40, u"\u053D",
    30, u"\u053C",
    20, u"\u053B",
    10, u"\u053A",
    9, u"\u0539",
    8, u"\u0538",
    7, u"\u0537",
    6, u"\u0536",
    5, u"\u0535",
    4, u"\u0534",
    3, u"\u0533",
    2, u"\u0532",
    1, u"\u0531"
};

// 1 19999
AdditiveGlyph georgian[] =
{
    10000, u"\u10F5",
    9000, u"\u10F0",
    8000, u"\u10EF",
    7000, u"\u10F4",
    6000, u"\u10EE",
    5000, u"\u10ED",
    4000, u"\u10EC",
    3000, u"\u10EB",
    2000, u"\u10EA",
    1000, u"\u10E9",
    900, u"\u10E8",
    800, u"\u10E7",
    700, u"\u10E6",
    600, u"\u10E5",
    500, u"\u10E4",
    400, u"\u10F3",
    300, u"\u10E2",
    200, u"\u10E1",
    100, u"\u10E0",
    90, u"\u10DF",
    80, u"\u10DE",
    70, u"\u10DD",
    60, u"\u10F2",
    50, u"\u10DC",
    40, u"\u10DB",
    30, u"\u10DA",
    20, u"\u10D9",
    10, u"\u10D8",
    9, u"\u10D7",
    8, u"\u10F1",
    7, u"\u10D6",
    6, u"\u10D5",
    5, u"\u10D4",
    4, u"\u10D3",
    3, u"\u10D2",
    2, u"\u10D1",
    1, u"\u10D0"
};

std::u16string convertToRoman(int n, AdditiveGlyph* glyphList, int min = 1, int max = 4999)
{
    if (n < min || max < n)
        return toString(n);
    std::u16string value;
    for (AdditiveGlyph* i = glyphList; i->glyph; ++i) {
        int t = n / i->weight;
        if (0 < t) {
            n -= t * i->weight;
            while (0 < t--)
                value += i->glyph;
        }
    }
    return value;
}

std::u16string convertAlphabetic(int n, char16_t* alphabet, size_t glyphCount)
{
    if (n < 0)
        return toString(n);
    std::u16string value;
    while (n) {
        --n;
        value.insert(0, 1, alphabet[n % glyphCount]);
        n /= glyphCount;
    }
    return value;
}

std::u16string emit(int i, unsigned type)
{
    std::u16string value;
    switch (type) {
    case CSSListStyleTypeValueImp::None:
        break;
    case CSSListStyleTypeValueImp::Disc:
        value = u"\u2022";    // •
        break;
    case CSSListStyleTypeValueImp::Circle:
        value = u"\u25E6";    // ◦
        break;
    case CSSListStyleTypeValueImp::Square:
        // Use u25A0 instead of u25FE for the IPA font for now
        value = u"\u25A0";    // ◾ "\u25FE"
        break;
    case CSSListStyleTypeValueImp::Decimal:
        value = toString(i);
        break;
    case CSSListStyleTypeValueImp::DecimalLeadingZero:
        value = toString(i < 0 ? -i : i);
        if (-9 <= i && i <= 9)
            value = u"0" + value;
        if (i < 0)
            value = u"-" + value;
        break;
    case CSSListStyleTypeValueImp::LowerAlpha:
    case CSSListStyleTypeValueImp::LowerLatin:
        value = convertAlphabetic(i, u"abcdefghijklmnopqrstuvwxyz", 26);
        break;
    case CSSListStyleTypeValueImp::UpperAlpha:
    case CSSListStyleTypeValueImp::UpperLatin:
        value = convertAlphabetic(i, u"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
        break;
    case CSSListStyleTypeValueImp::LowerGreek:
        // This style is only defined because CSS2.1 has it.
        // It doesn't appear to actually be used in Greek texts.
        value = convertAlphabetic(i, u"αβγδεζηθικλμνξοπρστυφχψω", 24);
        break;
    case CSSListStyleTypeValueImp::LowerRoman:
        value = convertToRoman(i, lowerRoman);
        break;
    case CSSListStyleTypeValueImp::UpperRoman:
        value = convertToRoman(i, upperRoman);
        break;
    case CSSListStyleTypeValueImp::Armenian:
        value = convertToRoman(i, armenian, 1, 9999);
        break;
    case CSSListStyleTypeValueImp::Georgian:
        value = convertToRoman(i, georgian, 1, 19999);
        break;
    default:
        value = toString(i);
        break;
    }
    return value;
}

}

void CounterImp::nest(int number)
{
    counters.push_back(number);
}

void CounterImp::reset(int number)
{
    if (counters.empty())
        nest(number);
    else
        counters.back() = number;
}

void CounterImp::increment(int number)
{
    if (counters.empty())
        nest(0);
    counters.back() += number;
}

bool CounterImp::restore()
{
    counters.pop_back();
    return counters.empty();
}

std::u16string CounterImp::eval(unsigned type, CSSAutoNumberingValueImp::CounterContext* context)
{
    this->separator = u"";
    this->listStyle.setValue(type);
    if (counters.empty()) {
        nest(0);
        context->addCounter(this);
    }
    return emit(counters.back(), type);
}

std::u16string CounterImp::eval(const std::u16string& separator, unsigned type, CSSAutoNumberingValueImp::CounterContext* context)
{
    this->separator = separator;
    this->listStyle.setValue(type);
    if (counters.empty()) {
        nest(0);
        context->addCounter(this);
    }
    std::u16string value;
    for (auto i = counters.begin(); i != counters.end(); ++i) {
        if (i != counters.begin())
            value += separator;
        value += emit(*i, type);
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
