/*
 * Copyright 2010-2012 Esrille Inc.
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

#include "CSSInputStream.h"

#include <assert.h>
#include <string.h>

#include <unicode/ucnv.h>

#include <algorithm>

namespace {

const char* const CSSDefaultEncoding = "utf-8";

}

CSSInputStream::CSSInputStream(std::istream& stream, const std::string& optionalEncoding) :
    U16InputStream(stream, optionalEncoding)
{
}

bool CSSInputStream::detect(const char* p)
{
    static char be[] = { 0x00, 0x40, 0x00, 0x63, 0x00, 0x68, 0x00, 0x61, 0x00, 0x72, 0x00, 0x73, 0x00, 0x65, 0x00, 0x74 };
    static char le[] = { 0x40, 0x00, 0x63, 0x00, 0x68, 0x00, 0x61, 0x00, 0x72, 0x00, 0x73, 0x00, 0x65, 0x00, 0x74, 0x00 };

    std::string u16 = "";
    if (confidence == Certain)
        return false;
    if (strncmp(p, "\xfe\xff", 2) == 0 || strncmp(p, be, sizeof(be)) == 0) {
        encoding = "utf-16be";
        confidence = Irrelevant;
        u16 = beToAscii((*p == '\xfe') ? p + 2 : p);
        p = u16.c_str();
    } else if (strncmp(p, "\xff\xfe", 2) == 0 || strncmp(p, le, sizeof(le)) == 0) {
        encoding = "utf-16le";
        confidence = Irrelevant;
        u16 = leToAscii((*p == '\xff') ? p + 2 : p);
        p = u16.c_str();
    } else if (strncmp(p, "\xef\xbb\xbf", 3) == 0) {
        encoding = "utf-8";
        confidence = Irrelevant;
        p += 3;
    }

    if (strncmp(p, "@charset", 8) != 0) {
        if (confidence == Tentative)
            encoding = CSSDefaultEncoding;
        return false;
    }
    std::string tentative;
    p += 8;
    if (*p++ != ' ' || *p++ != '"') {
        encoding = "";
        return false;
    }
    for (;;) {
        if (!*p) {
            encoding = "";
            return false;
        }
        if (*p == '"') {
            if (*++p != ';') {
                encoding = "";
                return false;
            }
            break;
        }
        tentative += *p++;
    }
    if (confidence == Tentative) {
        encoding = tentative;
        return false;
    }
    if (strcasecmp(tentative.c_str(), "utf-16") == 0 && strncmp(encoding.c_str(), "utf-16", 6) == 0 ||
        strcasecmp(encoding.c_str(), tentative.c_str()) == 0)
        return false;
    encoding = "";
    return false;
}
