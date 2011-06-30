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

#include "HTTPResponseMessage.h"

#include <assert.h>

#include <algorithm>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include "http/HTTPUtil.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace http;

namespace {

bool isToken(const char* start, const char* token, size_t length)
{
    assert(strlen(token) == length);
    if (strncasecmp(start, token, length) != 0)
        return false;
    int c = start[length];
    if (c == ' ' || c == '\0' || c == '\t' || c == '=' || c == ',')
        return true;
    return false;
}

bool hasToken(const std::string& value, const char* token, size_t length, char separator = ',')
{
    const char* start = value.c_str();
    const char* end = start + value.length();
    do {
        start = skipSpace(start, end);
        if (isToken(start, token, length))
            return true;
        start = skipTo(start, end, separator);
        if (*start == separator)
            ++start;
    } while (start < end);
    return false;
}

bool isHopByHopHeader(const std::string& header)
{
    const char* names[] = {
        "Connection",
        "Keep-Alive",
        "Proxy-Authenticate",
        "Proxy-Authorization",
        "TE",
        "Trailers",
        "Transfer-Encoding",
        "Upgrade",
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

const char* HttpResponseMessage::parseVersion(const char* start, const char* const end)
{
    if (strncasecmp(start, "HTTP", 4) != 0)
        version = 9;
    else {
        version = 10;
        start += 4;
        if (*start == '/') {
            ++start;
            unsigned major = 0;
            unsigned minor = 0;
            start = parseDigits(start, end, major);
            if (start < end && *start == '.') {
                ++start;
                start = parseDigits(start, end, minor);
            }
            if (1 < major || (major == 1 && 1 <= minor))
                version = 11;
        }
    }
    return start;
}

const char* HttpResponseMessage::parseStatusLine(const char* start, const char* const end)
{
    noStore = noCache = false;

    start = parseVersion(start, end);
    if (version == 9 || *start != ' ') {
        status = 200;
        statusText = "OK";
    } else {
        unsigned digits = 0;
        ++start;
        start = parseDigits(start, end, digits);
        if (digits < 100 || 999 < digits)
            status = 200;
        else
            status = static_cast<unsigned short>(digits);
        const char* text = 0;
        if (start < end && *start == ' ') {
            ++start;
            text = start;
        }
        const char* textEnd = 0;
        while (start < end) {
            if (*start == '\n') {
                if (!textEnd)
                    textEnd = start;
                break;
            }
            if (*start == '\r') {
                if (!textEnd)
                    textEnd = start;
            }
            ++start;
        }
        if (!text)
            statusText = "OK";
        else if (textEnd)
            statusText = std::string(text, textEnd - text);
    }
    return start;
}

bool HttpResponseMessage::parseContentLength(const std::string& value)
{
    parseDigits(value.c_str(), value.c_str() + value.length(), contentLength);
    return true;
}

const char* HttpResponseMessage::parseMediaType(const char* start, const char* const end)
{
    // type "/" subtype
    const char* type = skipSpace(start, end);
    start = type;
    while (start < end && isTokenChar(*start))
        ++start;
    contentType = std::string(type, start - type) + '/';
    start = skipSpace(start, end);
    if (*start != '/') {
        contentType.clear();
        return start;
    }
    const char* subtype = skipSpace(++start, end);
    start = subtype;
    while (start < end && isTokenChar(*start))
        ++start;
    contentType += std::string(subtype, start - subtype);
    toLowerCase(contentType);

    start = skipSpace(start, end);
    if (*start != ';')
        return start;
    ++start;

    // parameter
    while (start < end) {
        start = skipSpace(start, end);
        const char* next = skipTo(start, end, ';');
        if (isToken(start, "charset", 7)) {
            start = skipSpace(start + 7, end);
            if (*start == '=') {
                ++start;
                contentCharset = std::string(start, next - start);
                trimQuoted(contentCharset);
            }
        }
        start = next;
        if (*start == ';')
            ++start;
    }
    return start;
}

bool HttpResponseMessage::parseContentType(const std::string& value)
{
    const char* start = value.c_str();
    const char* end = start + value.length();
    do {
        start = skipSpace(start, end);
        const char* next = skipTo(start, end, ',');
        parseMediaType(start, next);
        start = next;
        if (*start == ',')
            ++start;
    } while (start < end);
    return true;
}

bool HttpResponseMessage::parseCacheControl(const std::string& value)
{
    const char* start = value.c_str();
    const char* end = start + value.length();
    do {
        start = skipSpace(start, end);
        if (isToken(start, "no-cache", 8) && start[8] != '=')
            noCache = true;
        else if (isToken(start, "no-store", 8))
            noStore = true;
        start = skipTo(start, end, ',');
        if (*start == ',')
            ++start;
    } while (start < end && (!noCache || !noStore));
    return true;
}

bool HttpResponseMessage::parsePragma(const std::string& value)
{
    const char* start = value.c_str();
    const char* end = start + value.length();
    do {
        start = skipSpace(start, end);
        if (isToken(start, "no-cache", 8))
            noCache = true;
        start = skipTo(start, end, ',');
        if (*start == ',')
            ++start;
    } while (start < end && !noCache);
    return true;
}

unsigned HttpResponseMessage::getAgeValue() const
{
    unsigned age = 0;
    std::string value;
    if (headers.get("Age", value))
        parseDigits(value.c_str(), value.c_str() + value.length(), age);
    return age;
}

unsigned HttpResponseMessage::getDateValue() const
{
    unsigned date = 0;
    std::string value;
    if (headers.get("Date", value))
        parseTime(value.c_str(), value.c_str() + value.length(), date);
    return date;
}

bool HttpResponseMessage::isChunked() const
{
    std::string value;
    if (!headers.get("Transfer-Encoding", value))
        return false;
    return hasToken(value, "chunked", 7);
}

bool HttpResponseMessage::getExpiresValue(unsigned& expiresValue) const
{
    std::string value;
    if (headers.get("Expires", value)) {
        parseTime(value.c_str(), value.c_str() + value.length(), expiresValue);
        return true;
    }
    return false;
}

bool HttpResponseMessage::getMaxAgeValue(unsigned& maxAge) const
{
    std::string value;
    if (!headers.get("Cache-Control", value))
        return false;
    const char* s = value.c_str();
    s = strcasestr(s, "max-age=");
    if (!s)
        return false;
    parseDigits(s + 8, value.c_str() + value.length(), maxAge);
    return true;
}

bool HttpResponseMessage::getLastModifiedValue(unsigned& lastModifiedValue) const
{
    std::string value;
    if (headers.get("Last-Modified", value)) {
        parseTime(value.c_str(), value.c_str() + value.length(), lastModifiedValue);
        return true;
    }
    return false;
}

bool HttpResponseMessage::parseHeader(const HttpHeader& hdr)
{
    if (hdr == "Content-Length")
        return parseContentLength(hdr.value);
    if (hdr == "Content-Type")
        return parseContentType(hdr.value);
    if (hdr == "Cache-Control")
        return parseCacheControl(hdr.value);
    if (hdr == "Pragma")
        return parsePragma(hdr.value);
    return true;
}

const char* HttpResponseMessage::parseHeader(const char* start, const char* const end)
{
    HttpHeader hdr;
    start = headers.parseLine(start, end, &hdr);
    if (!start)
        return 0;
    parseHeader(hdr);
    return start;
}

const char* HttpResponseMessage::parse(const char* start, const char* const end)
{
    const char* header = parseStatusLine(start, end);
    if (version < 10)
        return 0;
    if (*header != '\n')
        return 0;
    ++header;
    while (header < end) {
        header = parseHeader(header, end);
        if (!header)
            return 0;
        header = parseCRLF(header, end);
        if (*header == '\n')
            break;
    }
    return header;
}

std::string HttpResponseMessage::toString() const
{
    std::string s;
    if (version <= 9)
        return s;
    s = "HTTP/";
    if (version == 11)
        s += "1.1 ";
    else
        s += "1.0 ";
    s += boost::lexical_cast<std::string>(status) + " " + statusText + "\r\n";
    s += getAllResponseHeaders();
    return s;
}

// current_age = max(max(0, now - date_value), age_value) + now - request_time
unsigned HttpResponseMessage::getCurrentAge(unsigned now, unsigned requestTime) const
{
    unsigned currentAge = 0;
    unsigned dateValue = getDateValue();
    if (dateValue && dateValue < now)
        currentAge = now - dateValue;
    currentAge = std::max(currentAge, getAgeValue());
    currentAge += now;
    currentAge -= requestTime;
    return currentAge;
}

unsigned HttpResponseMessage::getFreshnessLifetime(unsigned now) const
{
    unsigned lifetime = 0;
    if (getMaxAgeValue(lifetime))
        return lifetime;

    unsigned dateValue = getDateValue();
    if (dateValue == 0)
        dateValue = now;

    unsigned expiresValue = 0;
    if (getExpiresValue(expiresValue)) {
        if (dateValue < expiresValue)
            return expiresValue - dateValue;
        return 0;
    }

    unsigned lastModifiedValue = 0;
    if (getLastModifiedValue(lastModifiedValue)) {
        if (lastModifiedValue <= dateValue)
            return (dateValue - lastModifiedValue) / 10;
    }

    if (status == 300 || status == 301)
        return UINT_MAX;

    return 0;
}

void HttpResponseMessage::clear()
{
    version = 11;
    status = 200;
    statusText = "OK";
    headers.clear();
    noCache = false;
    noStore = false;
    contentCharset.clear();
    contentLength = 0;
    contentType.clear();
}

bool HttpResponseMessage::update(const HttpResponseMessage& response)
{
    unsigned short code = response.getStatus();
    switch (code) {
    case 200:
    case 203:
    case 206:
    case 300:
    case 301:
    case 410:
        // TODO: if a cache-control directive prohibits caching.
        //    return false;
        break;
    default:
        // TODO: if there are cache-control directives or another header(s) that explicitly allow it
            return false;
        break;
    }

    for (auto i = response.headers.begin(); i !=response.headers.end(); ++i) {
        if (isHopByHopHeader(i->header))
            continue;
        headers.set(i->header, i->value, false);
        parseHeader(*i);
    }

    // TODO:
    // 13.5.3
    // If the status code is 206 (Partial Content) and the ETag or
    // Last-Modified headers match exactly, the cache MAY combine the
    // contents stored in the cache entry with the new contents received in
    // the response and use the result as the entity-body of this outgoing
    // response

    return true;
}

HttpResponseMessage::HttpResponseMessage()
{
    clear();
}

}}}}  // org::w3c::dom::bootstrap
