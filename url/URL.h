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

#ifndef ES_URL_H
#define ES_URL_H

#include <assert.h>
#include <string>
#include <cstring>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class URL
{
    friend class URI;

    std::u16string url;

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

    bool parseUCSChar(size_t& pos);
    bool parsePrivate(size_t& pos);
    bool parseUnreservedChar(size_t& pos);
    bool parsePchar(size_t& pos);
    bool parsePercentEncoded(size_t& pos);
    bool parseIPv4Address(size_t& pos);
    bool parseIPv6Address(size_t& pos);
    bool parseIPLiteral(size_t& pos);
    void parseIregName(size_t& pos);
    void parsePath(size_t& pos);
    bool parseHost(size_t& pos);
    bool parseQuery(size_t& pos);
    bool parseFragment(size_t& pos);
    bool parseHTTP(size_t& pos);
    bool parseHTTPRelative(const URL& base);

    bool parseFile(size_t& pos);
    bool parseFileRelative(const URL& base);

    void clear();
    bool hasScheme();
    bool parse();
    bool parseRelative(const URL& base);

    bool isPort80() const {
        return portStart + 2 == portEnd && url[portStart] == '8' && url[portStart + 1] == '0';
    }

public:
    bool isEmpty() const {
        return url.empty();
    }

    operator std::u16string() const {
        return url;
    }

    std::u16string getProtocol() const {
        return url.substr(0, protocolEnd);
    }
    void setProtocol(std::u16string protocol);
    std::u16string getHost() const {
        if (isPort80())
            return getHostname();
        return url.substr(hostStart, hostEnd - hostStart);
    }
    void setHost(std::u16string host);
    std::u16string getHostname() const {
        return url.substr(hostnameStart, hostnameEnd - hostnameStart);
    }
    void setHostname(std::u16string hostname);
    std::u16string getPort() const {
        return url.substr(portStart, portEnd - portStart);
    }
    void setPort(std::u16string port);
    std::u16string getPathname() const {
        return url.substr(pathnameStart, pathnameEnd - pathnameStart);
    }
    void setPathname(std::u16string pathname);
    std::u16string getSearch() const {
        return url.substr(searchStart, searchEnd - searchStart);
    }
    void setSearch(std::u16string search) {
        // TODO: update fields
        if (search[0] != u'?')
            search = u"?" + search;
        if (searchStart < searchEnd)
            url.replace(searchStart, searchEnd - searchStart, search);
        else {
            assert(pathnameStart < pathnameEnd);
            url.insert(pathnameEnd, search);
        }
    }
    std::u16string getHash() const {
        return url.substr(hashStart, hashEnd - hashStart);
    }
    void setHash(std::u16string hash) {
        // TODO: update fields
        if (hash[0] != u'#')
            hash = u"#" + hash;
        if (hashStart < hashEnd)
            url.replace(hashStart, hashEnd - hashStart, hash);
        else
            url += hash;
    }

    bool testProtocol(const char16_t* protocol) const {
        return protocolEnd && url.compare(0, protocolEnd - 1, protocol) == 0;
    }

    bool hasFragment() const {
        return hashStart < hashEnd;
    }

    bool isSameExceptFragments(const URL& target) const {
        size_t a = hasFragment() ? hashStart : url.length();
        size_t b = target.hasFragment() ? target.hashStart : target.url.length();
        return (a == b) && !std::memcmp(url.data(), target.url.data(), a);
    }

    URL() {}
    URL(const std::u16string& url);
    URL(const URL& base, const std::u16string& relative);
    URL(const URL& other);

    bool operator ==(const URL& other) const {
        return url == other.url;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_URL_H
