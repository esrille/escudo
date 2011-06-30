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

#include "http/HTTPUtil.h"

#include <limits.h>
#include <string.h>
#include <time.h>

#include <algorithm>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

const bool tokenCharMap[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, //   0
    0, 0, 0, 0, 0, 0, 0, 0, //   8
    0, 0, 0, 0, 0, 0, 0, 0, //  16
    0, 0, 0, 0, 0, 0, 0, 0, //  24
    0, 1, 0, 1, 1, 1, 1, 1, //  32
    0, 0, 1, 1, 0, 1, 1, 0, //  40
    1, 1, 1, 1, 1, 1, 1, 1, //  48
    1, 1, 0, 0, 0, 0, 0, 0, //  56
    0, 1, 1, 1, 1, 1, 1, 1, //  64
    1, 1, 1, 1, 1, 1, 1, 1, //  72
    1, 1, 1, 1, 1, 1, 1, 1, //  80
    1, 1, 1, 0, 0, 0, 1, 1, //  88
    1, 1, 1, 1, 1, 1, 1, 1, //  96
    1, 1, 1, 1, 1, 1, 1, 1, // 104
    1, 1, 1, 1, 1, 1, 1, 1, // 112
    1, 1, 1, 0, 1, 0, 1, 0  // 120
};

const char* const months[12] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

}

namespace http {

void toLowerCase(std::string& v)
{
    for (auto it = v.begin(); it != v.end(); ++it)
        *it = static_cast<char>(tolower(*it));
}

void toUpperCase(std::string& v)
{
    for (auto it = v.begin(); it != v.end(); ++it)
        *it = static_cast<char>(toupper(*it));
}

void trimLWS(std::string& v)
{
    std::string::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        if (!isLWS(*it))
            break;
    }
    it = v.erase(v.begin(), it);

    if (!v.empty()) {
        it = v.end();
        for (;;) {
            --it;
            if (!isLWS(*it))
                break;
            it = v.erase(it);
        }
    }
}

void trimQuoted(std::string& v)
{
    std::string::iterator it;
    for (it = v.begin(); it != v.end(); ++it) {
        if (!isLWS(*it))
            break;
    }
    it = v.erase(v.begin(), it);

    if (*it == '\'' || *it == '"') {
        char quote = *it;
        it = v.erase(it);
        while (it != v.end()) {
            if (*it == quote) {
                it = v.erase(it);
                v.erase(it, v.end());
                break;
            } else if (*it == '\\') {
                it = v.erase(it);
                if (it == v.end())
                    return;
            }
            ++it;
        }
        return;
    }

    if (!v.empty()) {
        it = v.end();
        for (;;) {
            --it;
            if (!isLWS(*it))
                break;
            it = v.erase(it);
        }
    }
}

bool isTokenChar(int c)
{
    return c < 128 && tokenCharMap[c];
}

bool isValidToken(const char* start, const char* end)
{
    if (end <= start)
        return false;
    for (; start < end; ++start) {
        if (!isTokenChar(*start))
            return false;
    }
    return true;
}

const char* skipSpace(const char* start, const char* const end)
{
    while (start < end) {
        int c = *start;
        if (c != ' ' && c != '\t')
            break;
        ++start;
    }
    return start;
}

const char* skipTo(const char* start, const char* const end, const char delim)
{
    char quoted = 0;
    while (start < end) {
        char c = *start;
        if (!quoted) {
            if (c == '"' || c == '\'')
                quoted = c;
            else if (c == delim) {
                break;
            }
        } else {
            if (c == '\\' && start + 1 < end)
                ++start;
            else if (c == quoted)
                quoted = 0;
        }
        ++start;
    }
    return start;
}

const char* parseHexDigits(const char* start, const char* const end, unsigned long long& hex)
{
    unsigned long long value = 0;
    start = skipSpace(start, end);
    while (start < end && isxdigit(*start)) {
        int x = (*start) - '0';
        if ('a' - '0' <= x)
            x += '0' - 'a' + 10;
        else if ('A' - '0' <= x)
            x += '0' - 'A' + 10;
        if ((ULLONG_MAX - x) / 16 < value)
            value = ULLONG_MAX;
        else
            value = 16 * value + x;
        ++start;
    }
    hex = value;
    return start;
}

const char* parseDigits(const char* start, const char* const end, unsigned long long& digits)
{
    unsigned long long value = 0;
    start = skipSpace(start, end);
    while (start < end && isdigit(*start)) {
        int d = (*start) - '0';
        if ((ULLONG_MAX - d) / 10 < value)
            value = ULLONG_MAX;
        else
            value = 10 * value + d;
        ++start;
    }
    digits = value;
    return start;
}

const char* parseDigits(const char* start, const char* const end, unsigned& digits)
{
    unsigned long long value = 0;
    start = skipSpace(start, end);
    while (start < end && isdigit(*start)) {
        value = 10 * value + (*start) - '0';
        if (2147483648LL < value)
            value = 2147483648LL;
        ++start;
    }
    digits = static_cast<unsigned>(value);
    return start;
}

const char* parseDigits(const char* start, const char* const end, int& digits)
{
    long long value = 0;
    start = skipSpace(start, end);
    while (start < end && isdigit(*start)) {
        value = 10 * value + (*start) - '0';
        // TODO: range check
        ++start;
    }
    digits = static_cast<int>(value);
    return start;
}

const char* parseMonth(const char* start, const char* const end, int& m)
{
    start = skipSpace(start, end);
    if (end - start < 3) {
        m = 12;
        return start;
    }
    for (int i = 0; i < 12; ++i) {
        if (strncasecmp(months[i], start, 3) == 0) {
            m = i;
            return start + 3;
        }
    }
    return start;
}

const char* parseTime(const char* start, const char* const end, unsigned& t)
{
    t = 0;
    tm tm;
    auto comma = std::find(start, end, ',');
    if (comma != end) {
        start = comma + 1;
        auto hyphen = std::find(start, end, '-');
        if (hyphen != end) {
            // RFC 850:  Weekday, 00-Mon-00 00:00:00 GMT
            start = parseDigits(start, end, tm.tm_mday);
            start = parseMonth(++start, end, tm.tm_mon);
            start = parseDigits(++start, end, tm.tm_year);
            start = parseDigits(start, end, tm.tm_hour);
            start = parseDigits(++start, end, tm.tm_min);
            start = parseDigits(++start, end, tm.tm_sec);
            if (tm.tm_year < 70)
                tm.tm_year += 100;
        } else {
            // RFC 1123: Wkd, 00 Mon 0000 00:00:00 GMT
            start = parseDigits(start, end, tm.tm_mday);
            start = parseMonth(start, end, tm.tm_mon);
            start = parseDigits(start, end, tm.tm_year);
            tm.tm_year -= 1900;
            start = parseDigits(start, end, tm.tm_hour);
            start = parseDigits(++start, end, tm.tm_min);
            start = parseDigits(++start, end, tm.tm_sec);
        }
    } else {
        // asctime: Wkd Mon 00 00:00:00 0000 GMT
        start = skipSpace(start, end);
        start = std::find(start, end, ' ');
        if (start == end)
            return end;
        start = parseMonth(start, end, tm.tm_mon);
        start = parseDigits(start, end, tm.tm_mday);
        start = parseDigits(start, end, tm.tm_hour);
        start = parseDigits(++start, end, tm.tm_min);
        start = parseDigits(++start, end, tm.tm_sec);
        start = parseDigits(start, end, tm.tm_year);
        tm.tm_year -= 1900;
    }
    if (tm.tm_sec < 0 || 59 < tm.tm_sec ||
        tm.tm_min < 0 || 59 < tm.tm_min ||
        tm.tm_hour < 0 || 23 < tm.tm_hour ||
        tm.tm_mday < 1 || 31 < tm.tm_mday  ||
        tm.tm_mon < 0 || 11 < tm.tm_mon ||
        tm.tm_year < 70 || 120 < tm.tm_year) {
        return start;
    }
    t = timegm(&tm);
    return start;
}

const char* parseCRLF(const char* start, const char* const end)
{
    while (start < end) {
        if (*start == '\n')
            break;
        if (*start == '\r')
            ++start;
        else
            break;
    }
    return start;
}

}  // http

}}}}  // org::w3c::dom::bootstrap
