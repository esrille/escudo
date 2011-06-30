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

#ifndef ES_HTTP_RESPONSE_MESSAGE_H
#define ES_HTTP_RESPONSE_MESSAGE_H

#include <string>

#include "http/HTTPHeader.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpResponseMessage
{
    unsigned short version;  // 9, 10, or 11

    unsigned short status;
    std::string    statusText;
    HttpHeaderList headers;

    bool noCache;
    bool noStore;
    std::string contentCharset;
    unsigned long long contentLength;
    std::string contentType;

    const char* parseVersion(const char* start, const char* const end);
    bool parseContentLength(const std::string& value);
    bool parseContentType(const std::string& value);
    bool parseCacheControl(const std::string& value);
    bool parsePragma(const std::string& value);
    const char* parseMediaType(const char* start, const char* const end);

    bool parseHeader(const HttpHeader& hdr);

public:
    HttpResponseMessage();

    unsigned short getVersion() const {
        return version;
    }

    const std::string& getContentCharset() {
        return contentCharset;
    }
    unsigned long long getContentLength() const {
        return contentLength;
    }
    const std::string& getContentType() {
        return contentType;
    }

    unsigned short getStatus() const {
        return status;
    }
    std::string getStatusText() const {
        return statusText;
    }
    std::string getResponseHeader(const std::string& header) const {
        std::string value;
        if (headers.get(header, value))
            return value;
        return "";  // TODO: or null?
    }
    std::string getAllResponseHeaders() const {
        return headers.toString();  // TODO: remove unwanted headers
    }
    const char* parse(const char* start, const char* const end);

    const char* parseStatusLine(const char* start, const char* const end);
    const char* parseHeader(const char* start, const char* const end);

    std::string toString() const;

    unsigned getAgeValue() const;
    unsigned getDateValue() const;

    bool getExpiresValue(unsigned& expiresValue) const;
    bool getMaxAgeValue(unsigned& maxAge) const;
    bool getLastModifiedValue(unsigned& lastModifiedValue) const;

    unsigned getCurrentAge(unsigned now, unsigned requestTime) const;
    unsigned getFreshnessLifetime(unsigned now) const;

    bool isNoCache() const {
        return noCache;
    }
    bool isNoStore() const {
        return noStore;
    }

    bool isChunked() const;

    void clear();
    bool update(const HttpResponseMessage& response);
};


}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_RESPONSE_MESSAGE_H