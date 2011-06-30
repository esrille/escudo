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

#include "HTTPHeader.h"

#include <string.h>
#include <algorithm>

#include "http/HTTPUtil.h"

namespace {

inline bool isLWS(int c)
{
    return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}

bool canCommaSeparated(const std::string& header)
{
    const char* names[] = {
        "Accept",
        "Accept-Charset",
        "Accept-Encoding",
        "Accept-Language",
        "Accept-Ranges",
        "Allow",
        "Cache-Control",
        "Connection",
        "Content-Encoding",
        "Content-Language",
        "Expect",
        "If-Match",
        "If-None-Match",
        "Pragma",
        "TE",
        "Trailer",
        "Transfer-Encoding",
        "Upgrade",
        "Vary",
        "Via",
        "Warning",
        0
    };
    const char* h = header.c_str();
    for (const char** i = names; *i; ++i) {
        if (strcasecmp(*i, h) == 0)
            return true;
    }
    return false;
}

}

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace http;

bool HttpHeaderList::get(const std::string& header, std::string& value) const
{
    auto it = std::find(headers.begin(), headers.end(), header);
    if (it != headers.end()) {
        value = it->value;
        return true;
    }
    return false;
}

void HttpHeaderList::erase(const std::string& header)
{
    auto it = std::find(headers.begin(), headers.end(), header);
    if (it != headers.end())
        headers.erase(it);
}

void HttpHeaderList::set(const std::string& header, const std::string& value, bool merge)
{
    std::string v = value;
    trimLWS(v);
    auto it = std::find(headers.begin(), headers.end(), header);
    if (v.empty()) {
        if (!merge && it != headers.end())
            headers.erase(it);
        return;
    }
    if (it == headers.end()) {
        headers.push_back(HttpHeader{header, v});
        return;
    }
    if (merge && canCommaSeparated(header)) {
        it->value += ", " + v;
        return;
    }
    it->value = v;  // replace the existing value
}

const char* HttpHeaderList::parseLine(const char* start, const char* const end, HttpHeader* p)
{
    const char* name = start;
    const char* colon = 0;
    while (start < end) {
        if (*start == ':') {
            colon = start++;
            break;
        }
        ++start;
    }
    if (!colon || !isValidToken(name, colon))
        return 0;
    const char* field = start;
    while (field < end) {
        if (!isLWS(*field))
            break;
        ++field;
    }
    const char* eol = field;
    while (eol < end) {
        if (*eol == '\n')  // LF?
            break;
        ++eol;
    }
    if (*eol != '\n')
        return 0;
    start = eol + 1;
    while (field < eol) {
        if (!isLWS(*eol)) {
            ++eol;
            break;
        }
        --eol;
    }
    HttpHeader header(std::string(name, colon - name), std::string(field, eol - field));
    set(header.header, header.value);
    if (p)
        *p = header;
    return start;
}

bool HttpHeaderList::parse(const char* start, const char* const end)
{
    while (start < end) {
        start = parseLine(start, end);
        if (!start)
            return false;
        start = parseCRLF(start, end);
        if (*start == '\n')
            return true;
    }
    return false;
}

std::string HttpHeaderList::toString() const
{
    std::string s;
    for (auto it = headers.begin(); it != headers.end(); ++it)
        s += it->header + ": " + it->value + "\r\n";
    return s;
}

}}}}  // org::w3c::dom::bootstrap
