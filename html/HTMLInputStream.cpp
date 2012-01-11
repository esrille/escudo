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

#include "HTMLInputStream.h"

#include <assert.h>
#include <string.h>

#include <unicode/ucnv.h>

#include <algorithm>

HTMLInputStream::HTMLInputStream(std::istream& stream, const std::string& optionalEncoding) :
    U16InputStream(stream, optionalEncoding)
{
}

const char* HTMLInputStream::getAttr(const char* p, std::string& name, std::string& value)
{
    name = value = "";

    p += strspn(p, "\t\n\f\r /");
    if (!*p || *p == '>')
        return p;

    // Attribute name:
    for (;;) {
        if (!*p)
            return p;
        if ((*p == '=' && 0 < name.length()) || strchr("\t\n\f\r ", *p))
            break;
        if (*p == '/' || *p == '>')
            return p;
        name += tolower(*p);
        ++p;
    }

    p += strspn(p, "\t\n\f\r ");
    if (!*p || *p != '=')
        return p;
    ++p;

    // Attribute value:
    p += strspn(p, "\t\n\f\r ");
    if (*p == '\'' || *p == '"') {
        char b = *p;
        ++p;
        while (*p && *p != b) {
            value += tolower(*p);
            ++p;
        }
        if (*p == b)
            ++p;
        return p;
    }
    if (!*p || *p == '>')
        return p;
    value += tolower(*p);
    while (*++p) {
        if (strchr("\t\n\f\r >", *p))
            return p;
        value += tolower(*p);
    }
    return p;
};

const char* HTMLInputStream::handleTag(const char* p)
{
    p += strcspn(p, "\t\n\f\r >");
    std::string name;
    std::string value;
    do {
        p = getAttr(p, name, value);
    } while (*p && 0 < name.length());
    return p;
};

std::string HTMLInputStream::handleContentType(const char* p)
{
    std::string encoding;

    p = skipOver(p, ";", 1);
    if (!*p)
        return encoding;
    p += strspn(p, "\t\n\f\r ");
    if (strncasecmp(p, "charset", 7) != 0)
        return encoding;
    p += 7;
    p += strspn(p, "\t\n\f\r ");
    if (*p != '=')
        return encoding;
    ++p;
    p += strspn(p, "\t\n\f\r ");
    if (*p == '\'' || *p == '"') {
        char b = *p;
        ++p;
        while (*p && *p != b) {
            encoding += tolower(*p);
            ++p;
        }
        if (*p == b)
            ++p;
        return encoding;
    }
    while (*p && !strchr("\t\n\f\r ", *p)) {
        encoding += tolower(*p);
        ++p;
    }
    return encoding;
};

const char* HTMLInputStream::handleMeta(const char* p)
{
    std::string name;
    std::string value;
    p = getAttr(p, name, value);
    while (0 < name.length()) {
        if (name == "charset") {
            encoding = value;
            break;
        } else if (name == "content") {
            encoding = handleContentType(value.c_str());
            break;
        }
        p = getAttr(p, name, value);
    }
    return p;
};

void HTMLInputStream::detect(const char* p)
{
    U16InputStream::detect(p);
    if (confidence != Tentative)
        return;

    confidence = Tentative;
    encoding.clear();
    while (*p) {
        if (strncmp(p, "<!--", 4) == 0)
            p = skipOver(p, "-->", 3);
        else if (strncasecmp(p, "<meta", 4) == 0 && strchr("\t\n\f\r /", p[5])) {
            p = handleMeta(p + 6);
            if (0 < encoding.length())
                break;
        } else if (*p == '<' && isalpha(p[1]))
            p = handleTag(p + 2);
        else if (*p == '<' && p[1] == '/' && isalpha(p[2]))
            p = handleTag(p + 3);
        else if (strncmp(p, "<!", 2) == 0 || strncmp(p, "</", 2) == 0 || strncmp(p, "<?", 2) == 0)
            p = skipOver(p, ">", 1);
        else
            ++p;
    }
    if (encoding.empty())
        encoding = DefaultEncoding;
    else
        confidence = Certain;
}
