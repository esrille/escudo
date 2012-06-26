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

#ifndef ES_HTTP_REQUEST_MESSAGE_H
#define ES_HTTP_REQUEST_MESSAGE_H

#include <string>

#include "url/URL.h"
#include "http/HTTPHeader.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpRequestMessage
{
    unsigned short version;  // 9, 10, or 11
    std::string method;
    URL url;
    HttpHeaderList headers;

public:
    // method codes
    enum {
        OPTIONS,
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        CONNECT,
        UNKNOWN
    };

    HttpRequestMessage() :
        version(11)
    {
    }

    unsigned short getVersion() const {
        return version;
    }
    void setVersion(unsigned short version) {
        if (version < 8 || 11 < version)
            version = 10;
        this->version = version;
    }

    const std::string& getMethod() const {
        return method;
    }
    const URL& getURL() const {
        return url;
    }

    int getMethodCode() const;

    void open(const std::string& method, const std::u16string& url);
    void setHeader(const std::string& header, const std::string& value);

    void clear();

    std::string toString();
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_REQUEST_MESSAGE_H