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

#ifndef ES_HTTP_HEADER_H
#define ES_HTTP_HEADER_H

#include <deque>
#include <string>
#include <cstring>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpHeader
{
public:
    std::string header;
    std::string value;
    HttpHeader() {}
    HttpHeader(const std::string& header, const std::string& value) :
        header(header),
        value(value)
    {
    }
    bool operator==(const std::string& header) const {
        return strcasecmp(this->header.c_str(), header.c_str()) == 0;
    }
};

class HttpHeaderList
{
    std::deque<HttpHeader> headers;
public:
    bool get(const std::string& header, std::string& value) const;
    void set(const std::string& header, const std::string& value, bool merge = false);
    void erase(const std::string& header);

    void clear() {
        headers.clear();
    }

    size_t size() const {
        return headers.size();
    }

    const char* parseLine(const char* start, const char* const end, HttpHeader* p = 0);
    bool parse(const char* start, const char* const end);
    std::string toString() const;

    std::deque<HttpHeader>::const_iterator begin() const {
        return headers.begin();
    }

    std::deque<HttpHeader>::const_iterator end() const {
        return headers.end();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_REQUEST_H