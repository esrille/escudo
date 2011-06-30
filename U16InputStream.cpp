/*
 * Copyright 2010 Esrille Inc.
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

const char* U16InputStream::DefaultEncoding = "windows-1252";

namespace {

struct Override
{
    const char* input;
    const char* replacement;
};

Override overrides[] = {
    { "euc-kr", "windows-949" },
    { "gb2312", "gbk" },
    { "gb_2312-80", "gbk" },
    { "iso-8859-1", "windows-1252" },
    { "iso-8859-9", "windows-1254" },
    { "iso-8859-11", "windows-874" },
    { "ks_c_5601-1987", "windows-949" },
    { "shift_jis", "windows-31j" },
    { "tis-620", "windows-874" },
    { "us-ascii", "windows-1252" }
};

const char* validEncodings[] = {
    "437",
    "850",
    "852",
    "855",
    "857",
    "860",
    "861",
    "862",
    "863",
    "865",
    "866",
    "869",
    "ansi_x3.4-1968",
    "ansi_x3.4-1986",
    "arabic",
    "ascii",
    "asmo-708",
    "big5",
    "big5-hkscs",
    "chinese",
    "cp037",
    "cp1026",
    "cp154",
    "cp367",
    "cp424",
    "cp437",
    "cp500",
    "cp775",
    "cp819",
    "cp850",
    "cp852",
    "cp855",
    "cp857",
    "cp860",
    "cp861",
    "cp862",
    "cp863",
    "cp864",
    "cp865",
    "cp866",
    "cp869",
    "cp936",
    "cp-gr",
    "cp-is",
    "csascii",
    "csbig5",
    "csibm037",
    "csibm1026",
    "csibm424",
    "csibm500",
    "csibm855",
    "csibm857",
    "csibm860",
    "csibm861",
    "csibm863",
    "csibm864",
    "csibm865",
    "csibm866",
    "csibm869",
    "csiso2022jp",
    "csiso2022kr",
    "csiso58gb231280",
    "csisolatin1",
    "csisolatin2",
    "csisolatin3",
    "csisolatin4",
    "csisolatin5",
    "csisolatin6",
    "csisolatinarabic",
    "csisolatincyrillic",
    "csisolatingreek",
    "csisolatinhebrew",
    "cskoi8r",
    "cspc775baltic",
    "cspc850multilingual",
    "cspc862latinhebrew",
    "cspc8codepage437",
    "cspcp852",
    "csptcp154",
    "csshiftjis",
    "cyrillic",
    "ecma-114",
    "ecma-118",
    "elot_928",
    "euc-jp",
    "euc-kr",
    "gb18030",
    "gb2312",
    "gbk",
    "greek",
    "greek8",
    "hebrew",
    "hp-roman8",
    "hz-gb-2312",
    "ibm037",
    "ibm1026",
    "ibm367",
    "ibm424",
    "ibm437",
    "ibm500",
    "ibm775",
    "ibm819",
    "ibm850",
    "ibm852",
    "ibm855",
    "ibm857",
    "ibm860",
    "ibm861",
    "ibm862",
    "ibm863",
    "ibm864",
    "ibm865",
    "ibm866",
    "ibm869",
    "iso-2022-jp",
    "iso-2022-jp-2",
    "iso-2022-kr",
    "iso_646.irv:1991",
    "iso646-us",
    "iso_8859-1",
    "iso-8859-1",
    "iso-8859-10",
    "iso_8859-10:1992",
    "iso_8859-1:1987",
    "iso-8859-13",
    "iso_8859-14",
    "iso-8859-14",
    "iso_8859-14:1998",
    "iso_8859-15",
    "iso-8859-15",
    "iso_8859-16",
    "iso-8859-16",
    "iso_8859-16:2001",
    "iso_8859-2",
    "iso-8859-2",
    "iso_8859-2:1987",
    "iso_8859-3",
    "iso-8859-3",
    "iso_8859-3:1988",
    "iso_8859-4",
    "iso-8859-4",
    "iso_8859-4:1988",
    "iso_8859-5",
    "iso-8859-5",
    "iso_8859-5:1988",
    "iso_8859-6",
    "iso-8859-6",
    "iso_8859-6:1987",
    "iso_8859-7",
    "iso-8859-7",
    "iso_8859-7:1987",
    "iso_8859-8",
    "iso-8859-8",
    "iso_8859-8:1988",
    "iso_8859-9",
    "iso-8859-9",
    "iso_8859-9:1989",
    "iso-celtic",
    "iso-ir-100",
    "iso-ir-101",
    "iso-ir-109",
    "iso-ir-110",
    "iso-ir-126",
    "iso-ir-127",
    "iso-ir-138",
    "iso-ir-144",
    "iso-ir-148",
    "iso-ir-157",
    "iso-ir-199",
    "iso-ir-226",
    "iso-ir-58",
    "iso-ir-6",
    "koi8-r",
    "koi8-u",
    "korean",
    "ks_c_5601-1987",
    "l1",
    "l10",
    "l2",
    "l3",
    "l4",
    "l5",
    "l6",
    "l8",
    "latin1",
    "latin10",
    "latin2",
    "latin3",
    "latin4",
    "latin5",
    "latin6",
    "latin8",
    "ms936",
    "ms_kanji",
    "pt154",
    "ptcp154",
    "r8",
    "roman8",
    "shift_jis",
    "tis-620",
    "us",
    "us-ascii",
    "utf-16",
    "utf-16be",
    "utf-16le",
    "utf-7",
    "utf-8",
    "windows-1250",
    "windows-1251",
    "windows-1252",
    "windows-1253",
    "windows-1254",
    "windows-1255",
    "windows-1256",
    "windows-1257",
    "windows-1258",
};

bool compareEncodings(const char* s, const char* t)
{
    return strcmp(s, t) < 0;
}

}  // namespace

U16InputStream::U16InputStream(std::istream& stream, const std::string optionalEncoding) :
    confidence(Certain),
    stream(stream)
{
    initializeConverter();
    encoding = checkEncoding(optionalEncoding);
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

void U16InputStream::detect(const char* p)
{
    if (strncmp(p, "\xfe\xff", 2) == 0) {
        encoding = "utf-16be";
        return;
    }
    if (strncmp(p, "\xff\xfe", 2) == 0) {
        encoding = "utf-16le";
        return;
    }
    if (strncmp(p, "\xef\xbb\xbf", 3) == 0) {
        encoding = "utf-8";
        return;
    }
    encoding = "";
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

    if (value.length() == 0)
        return "";

    // Override character encoding
    for (Override* override = overrides;
         override < &overrides[sizeof overrides / sizeof overrides[0]];
         ++override) {
        if (value == override->input) {
            value = override->replacement;
            break;
        }
    }

    // Check valid encodings
    if (!std::binary_search(validEncodings,
                            &validEncodings[sizeof validEncodings / sizeof validEncodings[0]],
                            value.c_str(),
                            compareEncodings))
        return "";

    return value;
}

void U16InputStream::setEncoding(std::string value)
{
    value = checkEncoding(value);
    if (value.length() == 0)
        value = DefaultEncoding;

    // Re-check encoding with ICU for conversion
    UErrorCode error = U_ZERO_ERROR;
    converter = ucnv_open(value.c_str(), &error);
    if (!converter) {
        value = DefaultEncoding;
        converter = ucnv_open(value.c_str(), &error);
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
            if (encoding.length() == 0) {
                sourceLimit[count] = '\0';
                detect(sourceLimit);
            }
            setEncoding(encoding);
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