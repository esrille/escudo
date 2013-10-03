/*
 * Copyright 2011-2013 Esrille Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

namespace {

bool isDigit(char16_t c)
{
    return '0' <= c && c <= '9';
}

int isHexDigit(char16_t c)
{
    if ('0' <= c && c <= '9')
        return '0';
    if ('A' <= c && c <= 'F')
        return 'A' - 10;
    if ('a' <= c && c <= 'f')
        return 'a' - 10;
    return 0;
}

bool isSpace(char16_t c)
{
    switch (c) {
    // White Space
    case u'\u0009': // Tab <TAB>
    case u'\u000B': // Vertical Tab <VT>
    case u'\u000C': // Form Feed <FF>
    case u'\u0020': // Space <SP>
    case u'\u00A0': // No-break space <NBSP>
    case u'\uFEFF': // Byte Order Mark <BOM>
    // Other category “Zs” Any other Unicode “space separator” <USP>
    case u'\u1680':
    case u'\u180e':
    case u'\u2000':
    case u'\u2001':
    case u'\u2002':
    case u'\u2003':
    case u'\u2004':
    case u'\u2005':
    case u'\u2006':
    case u'\u2007':
    case u'\u2008':
    case u'\u2009':
    case u'\u200a':
    case u'\u202f':
    case u'\u205f':
    case u'\u3000':
    // Line Terminators
    case u'\u000A':  // Line Feed <LF>
    case u'\u000D':  // Carriage Return <CR>
    case u'\u2028':  // Line separator <LS>
    case u'\u2029':  // Paragraph separator <PS>
        return true;
    default:
        return false;
    }
}

std::u16string toString(const char* format, ...)
{
    va_list ap;
    char buffer[24];
    std::u16string text;

    va_start(ap, format);
    vsnprintf(buffer, sizeof buffer, format, ap);
    va_end(ap);
    for (char* p = buffer; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

std::u16string toString(double value)
{
    if (isnan(value))
        return u"NaN";
    if (isinf(value))
        return (isinf(value) == 1) ? u"Infinity" : u"-Infinity";
    std::u16string text;
    char buffer[24];
    std::sprintf(buffer, "%g", value);
    for (char* p = buffer; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

std::u16string toString(const std::shared_ptr<Imp>& pimpl)
{
    if (!pimpl)
        return u"null";
    Any result{ pimpl->message_(0, 0, Object::STRINGIFY_, 0) };
    if (result.isString())
        return result.toString();
    return std::u16string();
}

double toNumber(const std::u16string& value)
{
    char16_t c = 0;
    std::u16string::const_iterator i = value.begin();
    while (i != value.end()) {
        c = *i;
        if (!isSpace(c))
            break;
        ++i;
    }
    double number = 0.0;
    if (value.compare(i - value.begin(), 2, u"0x") == 0 || value.compare(i - value.begin(), 2, u"0X") == 0) {
        for (i += 2; i != value.end(); ++i) {
            c = *i;
            int hex = isHexDigit(c);
            if (!hex)
                break;
            number *= 16;
            number += (c - hex);
        }
    } else {
        bool negative = (c == '-');
        if (negative || c == '+')
            ++i;
        int exp = 0;
        if (value.compare(i - value.begin(), 8, u"Infinity") == 0) {
            i += 8;
            number = negative ? -INFINITY : INFINITY;
        } else {
            while (i != value.end()) {
                c = *i;
                if (!isDigit(c))
                    break;
                number *= 10.0;
                number += (c - '0');
                ++i;
            }
            if (c == '.') {
                while (++i != value.end()) {
                    c = *i;
                    if (!isDigit(c))
                        break;
                    number *= 10.0;
                    number += (c - '0');
                    --exp;
                }
            }
            if (negative)
                number = -number;
            if (c == 'e' || c == 'E') {
                negative = false;
                if (++i != value.end()) {
                    c = *i;
                    negative = (c == '-');
                    if (negative || c == '+')
                        ++i;
                }
                int n = 0;
                while (i != value.end()) {
                    c = *i;
                    if (!isDigit(c))
                        break;
                    n *= 10;
                    n += (c - '0');
                    ++i;
                }
                if (negative)
                    exp -= n;
                else
                    exp += n;
            }
            if (DBL_MAX_EXP < exp)
                number = negative ? -INFINITY : INFINITY;
            else if (exp < DBL_MIN_EXP)
                number = negative ? -0.0 : 0.0;
            else {
                double p10 = 10.0;
                int n = exp;
                if (n < 0)
                    n = -n;
                while (n) {
                    if (n & 1) {
                        if (exp < 0)
                            number /= p10;
                        else
                            number *= p10;
                    }
                    n >>= 1;
                    p10 *= p10;
                }
            }
        }
    }
    while (i != value.end()) {
        if (!isSpace(*i))
            return NAN;
        ++i;
    }
    return number;
};

}  // namespace

std::u16string Any::toString() const
{
    switch (type) {
    case Bool:
        return i32 ? u"true" : u"false";
    case Int32:
        return ::toString("%d", i32);
    case Uint32:
        return ::toString("%u", u32);
    case Int64:
        return ::toString("%lld", i64);
    case Uint64:
        return ::toString("%llu", u64);
    case Float32:
        return ::toString(f32);
    case Float64:
        return ::toString(f64);
    case String:
        return string;
    case Shared:
        return ::toString(pimpl);
        break;
    default:
        break;
    }
    return std::u16string();
}

// toNumber
// for Object, call valueOf and then apply toNumber
// null returns 0, undefined returns NAN
double Any::toNumber() const
{
    switch (type) {
    case Undefined:
        return NAN;
    case Bool:
    case Int32:
        return i32;
    case Uint32:
        return u32;
    case Int64:
        return i64;
    case Uint64:
        return u64;
    case Float32:
        return f32;
    case Float64:
        return f64;
    case String:
        return ::toNumber(string);
        break;
    default:
        break;
    }
    return 0.0;
}

unsigned int Any::toArrayIndex() const
{
    switch (type) {
    case Undefined:
        break;
    case Bool:
        break;
    case Int32:
        if (0 <= i32)
            return i32;
        break;
    case Uint32:
        return u32;
    case Int64:
        if (0 <= i64 && i64 < UINT_MAX)
            return i64;
    case Uint64:
        if (u64 < UINT_MAX)
            return u64;
        break;
    case Float32:
        if (0.0f <= f32) {
            float f = floorf(f32);
            if (f == f32 && f < UINT_MAX)
                return static_cast<unsigned int>(f32);
        }
        break;
    case Float64:
        if (0.0 <= f64) {
            double f = floor(f64);
            if (f == f64 && f < UINT_MAX)
                return static_cast<unsigned int>(f64);
        }
        break;
        break;
    case String: {
        double v = ::toNumber(string);
        if (0.0 <= v) {
            double f = floor(v);
            if (f == v && f < UINT_MAX && ::toString(f) == string)
                return static_cast<unsigned int>(f);
        }
        break;
    }
    default:
        break;
    }
    return UINT_MAX;
}

// toBoolean
// Object returns true. Otherwise, false.
// NAN returns false.
// empty string returns false. Otherwise, true.
bool Any::toBoolean() const
{
    switch (type) {
    case Bool:
    case Int32:
        return i32;
    case Uint32:
        return u32;
    case Int64:
        return i64;
    case Uint64:
        return u64;
    case Float32:
        return f32 && !isnan(f32);
    case Float64:
        return f64 && !isnan(f64);
    case String:
        return string.length();
        break;
    default:
        break;
    }
    return false;
}
