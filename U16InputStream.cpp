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

#include "U16InputStream.h"

#include <assert.h>
#include <string.h>

#include <unicode/ucnv.h>

#include <algorithm>

const char* U16InputStream::DefaultEncoding = "utf-8";

namespace {

struct Override
{
    const char* input;
    const char* replacement;
};

// cf. HTML Living Standard 13.2.2.2 Character encodings
// cf. http://bugs.icu-project.org/trac/browser/icu/trunk/source/data/mappings/convrtrs.txt
Override overrides[] = {
    { "euc-kr", "windows-949" },
    { "euc-jp", "windows-51932" },    // ucnv_open() may not understand 'CP51932'.
    { "gb2312", "GBK" },
    { "gb_2312-80", "GBK" },
    // { "iso-2022-jp", "CP50220" },  // ucnv_open() may not understand 'CP50220'; iso-2022-jp appears to be the same as CP50220 in ICU.
    { "iso-8859-1", "windows-1252" },
    { "iso-8859-9", "windows-1254" },
    { "iso-8859-11", "windows-874" },
    { "ks_c_5601-1987", "windows-949" },
    { "shift_jis", "Windows-31j" },
    { "shift-jis", "" },              // ucnv_open() may accept shift-jis as shift_jis; cf. at-charset-056.
    { "tis-620", "windows-874" },
    { "us-ascii", "windows-1252" }
};

}  // namespace

U16InputStream::U16InputStream(std::istream& stream, const std::string& optionalEncoding) :
    confidence(Certain),
    encoding(optionalEncoding),
    stream(stream)
{
    initializeConverter();
    encoding = checkEncoding(encoding);
    if (encoding.empty())
        confidence = Tentative;
}

U16InputStream::~U16InputStream()
{
    if (converter)
        ucnv_close(converter);
}

const char* U16InputStream::skipSpace(const char* p)
{
    while (*p) {
        if (strchr(" \t\r\n\f", *p) == 0)
            return p;
        ++p;
    }
    return p;
};


const char* U16InputStream::skipOver(const char* p, const char* target, size_t length)
{
    while (*p) {
        if (strncmp(p, target, length) == 0)
            return p + length;
        ++p;
    }
    return p;
};

bool U16InputStream::detect(const char* p)
{
    if (confidence != Tentative)
        return true;
    if (strncmp(p, "\xfe\xff", 2) == 0) {
        encoding = "utf-16be";
        confidence = Irrelevant;
        return true;
    }
    if (strncmp(p, "\xff\xfe", 2) == 0) {
        encoding = "utf-16le";
        confidence = Irrelevant;
        return true;
    }
    if (strncmp(p, "\xef\xbb\xbf", 3) == 0) {
        encoding = "utf-8";
        confidence = Irrelevant;
        return true;
    }
    encoding = "";
    return true;
}

std::string U16InputStream::checkEncoding(std::string value)
{
    // Remove any leading or trailing space characters
    std::string::iterator i = value.begin();
    while (i < value.end() && strchr("\t\n\f\r ", *i))
        ++i;
    value.erase(value.begin(), i);
    int j;
    for (j = value.length(); 0 < j && strchr("\t\n\f\r ", value[j - 1]); --j)
        ;
    value.erase(j, value.length());

    if (value.empty())
        return "";

    // Override character encoding
    for (Override* override = overrides;
         override < &overrides[sizeof overrides / sizeof overrides[0]];
         ++override) {
        if (!strcasecmp(value.c_str(), override->input)) {
            value = override->replacement;
            break;
        }
    }
    return value;
}

void U16InputStream::setEncoding(std::string value, bool useDefault)
{
    value = checkEncoding(value);
    if (value.empty()) {
        if (!useDefault) {
            eof = true;
            return;
        }
        value = DefaultEncoding;
    }

    // Re-check encoding with ICU for conversion
    UErrorCode error = U_ZERO_ERROR;
    converter = ucnv_open(value.c_str(), &error);
    if (!converter) {
        if (useDefault) {
            value = DefaultEncoding;
            converter = ucnv_open(value.c_str(), &error);
        }
        if (!converter)
            eof = true;
    }
    encoding = value;
}

void U16InputStream::initializeConverter()
{
    flush = false;
    eof = !stream;
    converter = 0;
    source = sourceLimit = sourceBuffer;
    target = targetBuffer;
    nextChar = target;
    lastChar = 0;
}

void U16InputStream::updateSource()
{
    assert(!flush);
    size_t count = sourceLimit - source;
    if (0 < count && sourceBuffer != source)
        memmove(sourceBuffer, source, count);
    source = sourceBuffer;
    sourceLimit = sourceBuffer + count;
    count = ChunkSize - count;
    if (0 < count) {
        stream.read(sourceLimit, count);
        count = stream.gcount();
        if (!converter) {
            bool useDefault = true;
            if (encoding.empty()) {
                sourceLimit[count] = '\0';
                useDefault = detect(sourceLimit);
            }
            setEncoding(encoding, useDefault);
        }
        sourceLimit += count;
        if (count == 0)
            flush = true;
    }
}

void U16InputStream::readChunk()
{
    nextChar = target = targetBuffer;
    updateSource();
    UErrorCode err = U_ZERO_ERROR;
    ucnv_toUnicode(converter,
                   reinterpret_cast<UChar**>(&target),
                   reinterpret_cast<UChar*>(targetBuffer) + ChunkSize,
                   const_cast<const char**>(&source),
                   sourceLimit, 0, flush, &err);
}

U16InputStream::operator std::u16string()
{
    std::u16string text;
    char16_t c;
    while (getChar(c))
        text += c;
    return text;
}