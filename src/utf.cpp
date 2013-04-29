/*
 * Copyright 2010-2013 Esrille Inc.
 * Copyright 2008 Google Inc.
 * Copyright 2006 Nintendo Co., Ltd.
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

/*
 * These coded instructions, statements, and computer programs
 * contain data derived from the following file:
 *
 * http://www.unicode.org/Public/4.1.0/ucd/CaseFolding.txt
 */

#include "utf.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

//
// UTF-8 <==> UTF-32
//

char* utf8to32(const char* utf8, char32_t* utf32)
{
    char32_t u = 0;
    uint8_t  c;
    unsigned len;
    unsigned i;

    c = (uint8_t) *utf8;
    if (c != '\0')
    {
        ++utf8;
    }
    if ((c & 0x80u) == 0x00u)
    {
        u = c;
        len = 0;
    }
    else if ((c & 0xe0u) == 0xc0u)
    {
        u = c & 0x1fu;
        len = 1;
    }
    else if ((c & 0xf0u) == 0xe0u)
    {
        u = c & 0x0fu;
        len = 2;
    }
    else if ((c & 0xf8u) == 0xf0u)
    {
        u = c & 0x07u;
        len = 3;
    }
    else
    {
        return NULL;
    }

    for (i = 0; i < len; ++i)
    {
        u <<= 6;
        c = (uint8_t) *utf8++;
        if ((c & 0xc0u) != 0x80u)
        {
            return NULL;
        }
        u |= (c & 0x3fu);
    }

    if (u <= 0x7f)
    {
        if (len != 0)
        {
            return NULL;
        }
    }
    else if (u <= 0x7ff)
    {
        if (len != 1)
        {
            return NULL;
        }
    }
    else if (u <= 0xffff)
    {
        if (len != 2)
        {
            return NULL;
        }
    }

    if (0xD800 <= u && u <= 0xDFFF) // Surrogate Area
    {
        return NULL;
    }

    *utf32 = u;
    return (char*) utf8;
}

char* utf32to8(char32_t utf32, char* utf8)
{
    int len;

    if (0xD800 <= utf32 && utf32 <= 0xDFFF) // Surrogate Area
    {
        return NULL;
    }

    if (utf32 < 0x80)
    {
        *utf8++ = (char) utf32;
        len = 0;
    }
    else if (utf32 < 0x800)
    {
        *utf8++ = (char) (0xc0 | (utf32 >> 6));
        len = 1;
    }
    else if (utf32 < 0x10000)
    {
        *utf8++ = (char) (0xe0 | (utf32 >> 12));
        len = 2;
    }
    else if (utf32 < 0x110000)
    {
        *utf8++ = (char) (0xf0 | (utf32 >> 18));
        len = 3;
    }
    else
    {
        return NULL;
    }

    while (0 < len--)
    {
        *utf8++ = (char) (0x80 | ((utf32 >> (6 * len)) & 0x3f));
    }
    return utf8;
}

size_t utf32to8len(char32_t utf32)
{
    if (0xD800 <= utf32 && utf32 <= 0xDFFF) // Surrogate Area
    {
        return 0;
    }
    if (utf32 < 0x80)
    {
        return 1;
    }
    if (utf32 < 0x800)
    {
        return 2;
    }
    if (utf32 < 0x10000)
    {
        return 3;
    }
    if (utf32 < 0x110000)
    {
        return  4;
    }
    return 0;
}

//
// UTF-16 <==> UTF-32
//

char16_t* utf16to32(const char16_t* utf16, char32_t* utf32)
{
    char16_t  w1;
    char16_t  w2;
    char32_t  u = 0;

    w1 = *utf16;
    if (w1 != 0)
    {
        ++utf16;
    }
    if (w1 < 0xD800 || 0xDFFF < w1)
    {
        u = w1;
    }
    else if (w1 <= 0xDBFF)
    {
        w2 = *utf16++;
        if (0xDC00 <= w2 && w2 <= 0xDFFF)
        {
            u = ((w1 & 0x03ffu) << 10) | (w2 & 0x03ffu);
            u += 0x10000;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }

    *utf32 = u;
    return (char16_t*) utf16;
}

char16_t* utf32to16(char32_t utf32, char16_t* utf16)
{
    if (0xD800 <= utf32 && utf32 <= 0xDFFF) // Surrogate Area
    {
        return NULL;
    }

    if (utf32 < 0x10000)
    {
        *utf16++ = (char16_t) utf32;
    }
    else if (utf32 <= 0x10FFFF)
    {
        char16_t w1 = 0xD800;
        char16_t w2 = 0xDC00;

        utf32 -= 0x10000;
        assert(utf32 <= 0xFFFFF);
        w1 |= (utf32 >> 10u);
        w2 |= (utf32 & 0x3ffu);
        *utf16++ = w1;
        *utf16++ = w2;
    }
    else
    {
        return NULL;
    }
    return utf16;
}

int utf16cmp(const char16_t* a, const char16_t* b)
{
    for (; *a == *b; ++a, ++b)
    {
        if (*a == 0)
        {
            return 0;
        }
    }
    return (*a < *b) ? -1 : 1;
}

int utf16ncmp(const char16_t* a, const char16_t* b, size_t len)
{
    while (0 < len--)
    {
        if (*a != *b)
        {
            return (*a < *b) ? -1 : 1;
        }
        if (*a == 0)
        {
            return 0;
        }
        ++a;
        ++b;
    }
    return 0;
}

char16_t* utf16cpy(char16_t* a, const char16_t* b)
{
    char16_t* s = a;
    while ((*s++ = *b++) != 0)
    {
        ;
    }
    return a;
}

char16_t* utf16ncpy(char16_t* a, const char16_t* b, size_t len)
{
    char16_t* s = a;
    while (0 < len && *b != 0)
    {
        *s++ = *b++;
        --len;
    }
    while (0 < len--)
    {
        *s++ = 0;
    }
    return a;
}

size_t utf16len(const char16_t* s)
{
    const char16_t* t;

    for (t = s; *t != 0; ++t)
        ;
    return t - s;
}

char* utf16cpy8(char* a, const char16_t* b)
{
    char* s = a;
    do {
        char32_t utf32;
        b = utf16to32(b, &utf32);
        s = utf32to8(utf32, s);
    } while (b && *b);
    *s = 0;
    return a;
}

char* utf16ncpy8(char* a, const char16_t* b, size_t len)
{
    char* s = a;
    size_t n;

    while (0 < len && b && *b != 0)
    {
        char32_t utf32;
        b = utf16to32(b, &utf32);
        n = utf32to8len(utf32);
        if (len < n)
        {
            break;
        }
        s = utf32to8(utf32, s);
        len -= n;
    }
    while (0 < len--)
    {
        *s++ = 0;
    }
    return a;
}

std::string utfconv(const std::u16string& s)
{
    std::string u;
    char buffer[8];

    const char16_t* b = s.c_str();
    while (*b) {
        char32_t utf32;
        b = utf16to32(b, &utf32);
        if (!b)
            break;
        if (char* p = utf32to8(utf32, buffer)) {
            *p = 0;
            u += buffer;
        }
    }
    return u;
}

std::u16string utfconv(const std::string& s)
{
    std::u16string u;
    char16_t buffer[3];

    const char* b = s.c_str();
    while (*b) {
        char32_t utf32;
        b = utf8to32(b, &utf32);
        if (!b)
            break;
        if (char16_t* p = utf32to16(utf32, buffer)) {
            *p = 0;
            u += buffer;
        }
    }
    return u;
}

void hexdump(const void* ptr, size_t len)
{
    size_t j;
    size_t i;
    size_t n;
    for (j = 0; j < len; j += 16) {
        n = len - j;
        if (16 < n)
            n = 16;
        printf("%p: ", (const uint8_t*) ptr + j);
        for (i = 0; i < n; i++)
            printf("%02x ", ((const uint8_t*) ptr)[j + i]);
        for (; i < 16; i++)
            printf("   ");
        printf("  ");
        for (i = 0; i < n; i++)
            printf("%c", (isprint)(((const uint8_t*) ptr)[j + i]) ? ((const uint8_t*) ptr)[j + i] : '.');
        printf("\n");
    }
}
