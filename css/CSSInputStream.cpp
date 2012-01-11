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

void CSSInputStream::detect(const char* p)
{
    U16InputStream::detect(p);
    if (confidence == Certain)
        return;

    confidence = Tentative;
    encoding = "";

    if (strncmp(p, "@charset", 8) != 0) {
        encoding = CSSDefaultEncoding;
        return;
    }
    p = skipSpace(p + 8);
    char quote = *p++;
    if (quote == '\'' || quote == '"') {
        for (;;) {
            if (!*p) {
                encoding = DefaultEncoding;
                return;
            }
            if (*p == quote) {
                p = skipSpace(++p);
                if (*p != ';') {
                    encoding = DefaultEncoding;
                    return;
                }
                break;
            }
            encoding += *p++;
        }
    }
    if (encoding.length() == 0)
        encoding = CSSDefaultEncoding;
}
