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

#ifndef ES_HTML_UTIL_H_INCLUDED
#define ES_HTML_UTIL_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>

#include "utf.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

// Find an match for one of the given keyword list
inline int findKeyword(const std::u16string& keyword, std::initializer_list<const char16_t*> list)
{
    for (auto i = list.begin(); i != list.end(); ++i) {
        if (keyword == *i)
            return i - list.begin();
    }
    return -1;
}

inline int findKeyword(const std::u16string& keyword, const char16_t** const list, size_t size)
{
    for (auto i = list; i < list + size; ++i) {
        if (keyword == *i)
            return i - list;
    }
    return -1;
}

template<class Ch>
inline const Ch* skipSpace(const Ch* input, const Ch* end)
{
    while (input < end) {
        if (!isSpace(*input))
            break;
        ++input;
    }
    return input;
}

template<class Ch>
inline const Ch* parseInt(const Ch* input, const Ch* end, int& value)
{
    int sign = 1;
    input = skipSpace(input, end);
    if (end <= input)
        return 0;
    Ch c = *input;
    if (c == '+')
        ++input;
    else if (c == '-') {
        sign = -1;
        ++input;
    }
    if (end <= input)
        return 0;
    c = *input;
    if (!isDigit(c))
        return 0;
    value = 0;
    do {
        value *= 10;
        value += c - '0';
        if (end <= ++input)
            break;
        c = *input;
    } while (isDigit(c));
    value *= sign;
    return input;
}

bool toInteger(const std::u16string& value, int& output);
bool toUnsigned(const std::u16string& value, unsigned& output);

bool mapToInteger(std::u16string& value);
bool mapToPixelLength(std::u16string& value);
bool mapToDimension(std::u16string& value);

}
}
}
}

#endif  // ES_HTML_UTIL_H_INCLUDED
