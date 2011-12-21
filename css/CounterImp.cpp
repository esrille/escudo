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
            value = toString(counters.back()) + u".\u00A0";
            break;
        case CSSListStyleTypeValueImp::DecimalLeadingZero:
        case CSSListStyleTypeValueImp::LowerRoman:
        case CSSListStyleTypeValueImp::UpperRoman:
        case CSSListStyleTypeValueImp::LowerGreek:
        case CSSListStyleTypeValueImp::LowerLatin:
        case CSSListStyleTypeValueImp::UpperLatin:
        case CSSListStyleTypeValueImp::Armenian:
        case CSSListStyleTypeValueImp::Georgian:
        case CSSListStyleTypeValueImp::LowerAlpha:
        case CSSListStyleTypeValueImp::UpperAlpha:
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
