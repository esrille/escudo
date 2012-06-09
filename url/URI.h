/*
 * Copyright 2011, 2012 Esrille Inc.
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

#ifndef ES_URI_H
#define ES_URI_H

#include "URL.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class URI
{
    std::string uri;

    size_t protocolEnd;
    size_t hostStart;
    size_t hostEnd;
    size_t hostnameStart;
    size_t hostnameEnd;
    size_t portStart;
    size_t portEnd;
    size_t pathnameStart;
    size_t pathnameEnd;
    size_t searchStart;
    size_t searchEnd;
    size_t hashStart;
    size_t hashEnd;

    void clear();

public:
    bool isEmpty() const {
        return uri.empty();
    }

    operator const std::string&() const {
        return uri;
    }

    std::string getProtocol() const {
        return uri.substr(0, protocolEnd);
    }
    std::string getHost() const {
        return uri.substr(hostStart, hostEnd - hostStart);
    }
    std::string getHostname() const {
        return uri.substr(hostnameStart, hostnameEnd - hostnameStart);
    }
    std::string getPort() const;
    std::string getPathname() const {
        return uri.substr(pathnameStart, pathnameEnd - pathnameStart);
    }
    std::string getSearch() const {
        return uri.substr(searchStart, searchEnd - searchStart);
    }
    std::string getHash() const {
        return uri.substr(hashStart, hashEnd - hashStart);
    }

    URI(const URL& url);

    static std::string percentEncode(const std::u16string& string, size_t pos = 0, size_t n = std::string::npos);
    static std::string percentDecode(const std::string& string, size_t pos = 0, size_t n = std::string::npos);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_URI_H
