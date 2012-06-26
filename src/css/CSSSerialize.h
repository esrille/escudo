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

#ifndef ES_CSSSERIALIZE_H
#define ES_CSSSERIALIZE_H

#include <cstdio>
#include <cstring>
#include <string>
#include <ostream>

#include "utf.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

struct CSSSerializeControl_t
{
    bool serializeSpecificity;
};

extern CSSSerializeControl_t CSSSerializeControl;

inline std::u16string CSSSerializeInteger(long n)
{
    std::u16string text;
    char buf[24];
    std::sprintf(buf, "%ld", n);
    for (char* p = buf; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

inline std::u16string CSSSerializeNumber(double n)
{
    std::u16string text;
    char buf[24];
    std::sprintf(buf, "%g", n);
    for (char* p = buf; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

inline std::u16string CSSSerializeHex(unsigned long n)
{
    std::u16string text;
    char buf[24];
    std::sprintf(buf, "%lx", n);
    for (char* p = buf; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

inline std::u16string CSSSerializeRGB(unsigned argb)
{
    std::u16string text;
    char buf[32];
    if ((argb & 0xFF000000) == 0xFF000000)
        std::sprintf(buf, "#%06x", argb & 0x00FFFFFF);
    else if (argb == 0)
        return u"transparent";
    else
        std::sprintf(buf, "rgba(%u,%u,%u,%.3g)",
                     (argb >> 16) & 0xff,
                     (argb >> 8) & 0xff,
                     argb & 0xff,
                     (argb >> 24) / 255.0);
    for (char* p = buf; *p; ++p)
        text += static_cast<char16_t>(*p);
    return text;
}

inline std::u16string CSSSerializeIdentifier(const std::u16string& ident)
{
    std::u16string cssText;
    int n = 0;
    for (auto i = ident.begin(); i != ident.end(); ++i, ++n) {
        if (isControl(*i))
            cssText += u"\\" + CSSSerializeHex(*i) + u" ";
        else if (n == 0 && isDigit(*i))
            cssText += u"\\" + CSSSerializeHex(*i) + u" ";
        else if (n == 1 &&  isDigit(*i) && ident[0] == u'-')
            cssText += u"\\" + CSSSerializeHex(*i) + u" ";
        else if (n == 1 && *i == u'-' && ident[0] == u'-') {
            cssText += u'\\';
            cssText += *i;
        } else if (0x0080 <= *i || *i == u'-' || *i == u'_' || isAlnum(*i))
            cssText += *i;
        else {
            cssText += u'\\';
            cssText += *i;
        }
    }
    return cssText;
}

inline std::u16string CSSSerializeString(const std::u16string& value)
{
    std::u16string cssText = u"\"";
    for (auto i = value.begin(); i != value.end(); ++i) {
        if (isControl(*i))
            cssText += u'\\' + CSSSerializeHex(*i) + u' ';
        else if (*i == u'"' || *i == u'\\')
            cssText += u'\\' + *i;
        else
            cssText += *i;
    }
    cssText += u'"';
    return cssText;
}

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSSERIALIZE_H
