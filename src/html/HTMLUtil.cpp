/*
 * Copyright 2013 Esrille Inc.
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

#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

bool toInteger(const std::u16string& value, int& output)
{
    const char16_t* input = value.c_str();
    const char16_t* end = input + value.length();
    return parseInt(input, end, output);    // Do not care what follows after digits.
}

bool toUnsigned(const std::u16string& value, unsigned int& output)
{
    int s;
    if (!toInteger(value, s))
        return false;
    return 0 <= s;
}

bool mapToInteger(std::u16string& value)
{
    if (stripLeadingWhitespace(value).empty())
        return false;
    const char16_t* input = value.c_str();
    const char16_t* end = input + value.length();
    int u;
    end = parseInt(input, end, u);
    if (!end)
        return false;
    value.erase(end - input);
    return true;
}

bool mapToPixelLength(std::u16string& value)
{
    if (stripLeadingWhitespace(value).empty())
        return false;
    const char16_t* input = value.c_str();
    const char16_t* end = input + value.length();
    int u;
    end = parseInt(input, end, u);
    if (!end || u < 0)
        return false;
    if (0 < u)
        value.replace(end - input, std::u16string::npos, u"px");
    else
        value.erase(end - input);
    return true;
}

bool mapToDimension(std::u16string& value)
{
    if (stripLeadingWhitespace(value).empty())
        return false;
    if (value[0] == '+') {
        if (value.erase(0, 1).empty())
            return false;
    }
    size_t pos = 0;
    while (value[pos] == '0')
        ++pos;
    if (0 < pos) {
        if (value.erase(0, pos).empty())
            return false;
    }
    pos = 0;
    while (isDigit(value[pos]))
        ++pos;
    if (value.length() <= pos) {
        value += u"px";
        return true;
    }
    size_t end = pos;
    if (value[pos] == '.') {
        ++pos;
        if (value.length() <= pos || !isDigit(value[pos])) {
            value.replace(end, std::u16string::npos, u"px");
            return true;
        }
        ++pos;
        while (isDigit(value[pos]))
            ++pos;
    }
    if (pos < value.length() && value[pos] == '%') {
        value.erase(++pos);
        return true;
    }
    value.replace(pos, std::u16string::npos, u"px");    // TODO: Check whether floating point length should be allowed
    return true;
}

}
}
}
}
