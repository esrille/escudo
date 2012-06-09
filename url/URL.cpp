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

#include "URL.h"

#include <assert.h>
#include <string.h>

#include <cstddef>

#include "utf.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

// ALPHA / DIGIT / "+" / "-" / "."
bool isSchemeChar(int c)
{
    return isAlnum(c) || c == '+' || c == '-' || c == '.';
}

// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
bool isSubDelims(int c)
{
    switch (c) {
    case '!':
    case '$':
    case '&':
    case '\'':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case ';':
    case '=':
        return true;
    default:
        return false;
    }
}

std::u16string removeDotSegments(std::u16string input)
{
    std::u16string output;
    size_t pos = 0;
    while (pos < input.length()) {
        if (input.compare(pos, 3, u"../") == 0)
            pos += 3;
        else if (input.compare(pos, 2, u"./") == 0)
            pos += 2;
        else if (input.compare(pos, 3, u"/./") == 0 || input.compare(pos, std::u16string::npos, u"/.") == 0) {
            pos += 2;
            if (pos == input.length())
                output += u"/";
        } else if (input.compare(pos, 4, u"/../") == 0 || input.compare(pos, std::u16string::npos, u"/..") == 0) {
            pos += 3;
            size_t slash = output.rfind('/');
            if (slash != std::u16string::npos)
                output.erase(slash);
            else
                output.clear();
            if (pos == input.length())
                output += u"/";
        } else if (input.compare(pos, std::u16string::npos, u"..") == 0)
            pos += 2;
        else if (input.compare(pos, std::u16string::npos, u".") == 0)
            pos += 1;
        else {
            if (input[pos] == '/') {
                ++pos;
                output += u"/";
            }
            size_t slash = input.find('/', pos);
            if (slash != std::u16string::npos)
                output += input.substr(pos, slash - pos);
            else
                output += input.substr(pos);
            pos = slash;
        }
    }
    return (0 < output.length()) ? output : u"/";
}

}  // namespace

bool URL::parseUCSChar(size_t& pos)
{
    char32_t c = url[pos];
    if (0xA0 <= c && c <= 0xD7FF || 0xF900 <= c && c < 0xFDCF || 0xFDF0 <= c && c < 0xFFEF) {
        ++pos;
        return true;
    }

    if (0xD800 <= c && c <= 0xDBFF) {
        ++pos;
        if (url.length() <= pos)
            return false;
        char32_t d = url[pos];
        if (0xDC00 <= d && d <= 0xDFFF) {
            c -= 0xD800;
            c <<= 10;
            d -= 0xDC00;
            c += d + 0x10000;
        } else
            return false;
    } else
        return false;
    if (c < 0xF0000 && (c & 0xFFFF) <= 0xFFFD)
        return true;
    return false;
}

// %xE000-F8FF / %xF0000-FFFFD / %x100000-10FFFD
bool URL::parsePrivate(size_t& pos)
{
    char32_t c = url[pos];
    if (0xE000 <= c && c <= 0xF8FF) {
        ++pos;
        return true;
    }

    if (0xD800 <= c && c <= 0xDBFF) {
        ++pos;
        if (url.length() <= pos)
            return false;
        char32_t d = url[pos];
        if (0xDC00 <= d && d <= 0xDFFF) {
            c -= 0xD800;
            c <<= 10;
            d -= 0xDC00;
            c += d + 0x10000;
        } else
            return false;
    } else
        return false;
    if (0xF0000 <= c && c <= 0x10FFFD && (c & 0xFFFF) <= 0xFFFD)
        return true;
    return false;
}

bool URL::parseUnreservedChar(size_t& pos)
{
    char16_t c = url[pos];
    if (isAlnum(c) || c == '-' || c == '.' || c == '_' || c == '~' ||
        c <= 0x0020 || 0x007f <= c ||
        c == '"' || c == '<' || c == '>' || c == '[' || c == '\\' ||
        c == ']' || c == '^' || c == '`' || c == '{' || c == '|' || c == '}')
    {
        ++pos;
        return true;
    }

    return parseUCSChar(pos);
}

// ipchar = iunreserved / pct-encoded / sub-delims / ":" / "@"
bool URL::parsePchar(size_t& pos)
{
    size_t first = pos;
    if (parseUnreservedChar(pos))
        return true;
    pos = first;
    if (parsePercentEncoded(pos))
        return true;
    pos = first;
    char16_t c = url[pos];
    if (isSubDelims(c) || c == ':' || c == '@') {
        ++pos;
        return true;
    }
    pos = first;
    return false;
}

bool URL::parsePercentEncoded(size_t& pos)
{
    if (url[pos] != '%')
        return false;
    if (url.length() <= ++pos)
        return false;
    if (!isHexDigit(url[pos]))
        return false;
    if (url.length() <= ++pos)
        return false;
    if (!isHexDigit(url[pos]))
        return false;
    ++pos;
    return true;
}

// *( iunreserved / pct-encoded / sub-delims )
void URL::parseIregName(size_t& pos)
{
    while (pos < url.length()) {
        size_t first = pos;
        if (parsePercentEncoded(pos))
            continue;
        pos = first;
        url[pos] = toLower(url[pos]);
        if (parseUnreservedChar(pos))
            continue;
        pos = first;
        if (isSubDelims(url[pos])) {
            ++pos;
            continue;
        }
        break;
    }
}

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
bool URL::parseIPv4Address(size_t& pos)
{
    int c;
    for (c = 0 ; c < 4; ++c) {
        int a = 0;
        for (size_t first = pos; pos < url.length(); ++pos) {
            if (int n = isDigit(url[pos])) {
                a *= 10;
                a += url[pos] - n;
                if (255 < a)
                    return false;
            } else if (pos == first)
                return false;
            else
                break;
        }
        if (c < 3) {
            if (url[pos] != '.')
                return false;
            ++pos;
        }
    }
    return c == 4;
}

/*
  IPv6address = hexpart [ ":" IPv4address ]
  IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT

  IPv6prefix  = hexpart "/" 1*2DIGIT

  hexpart = hexseq | hexseq "::" [ hexseq ] | "::" [ hexseq ]
  hexseq  = hex4 *( ":" hex4)
  hex4    = 1*4HEXDIG
 */
bool URL::parseIPv6Address(size_t& pos)
{
    bool useDoubleColon = false;
    size_t colon = pos;
    int c;
    for (c = 0 ; c < 8; ++c) {
        if (c <= 6 && !useDoubleColon && url.compare(colon, 2, u"::") == 0) {
            useDoubleColon = true;
            pos = colon + 2;
            c += 2;
            if (8 <= c)
                break;
        }
        unsigned a = 0;
        size_t first;
        for (first = pos; pos < url.length(); ++pos) {
            if (int n = isHexDigit(url[pos])) {
                a *= 16;
                a += url[pos] - n;
                if (0xffff < a)
                    return false;
            } else if (pos == first && !useDoubleColon)
                return false;
            else
                break;
        }
        if (c < 7) {
            if (url[pos] != ':')
                break;
            colon = pos;
            ++pos;
        }
        if (c == 5 || c < 5 && useDoubleColon) {
            first = pos;
            if (parseIPv4Address(pos)) {
                c += 3;  // including one hex4
                break;
            }
            pos = first;
        }
    }
    return (c == 8) || (0 < c && c <= 6 && useDoubleColon);
}

bool URL::parseIPLiteral(size_t& pos)
{
    if (url[pos] != '[')
        return false;
    ++pos;
    if (url.length() <= pos)
        return false;

    // TODO: IPvFuture

    if (!parseIPv6Address(pos))
        return false;

    if (url.length() <= pos)
        return false;
    if (url[pos] != ']')
        return false;
    ++pos;
    return true;
}

// IP-literal / IPv4address / ireg-name
bool URL::parseHost(size_t& pos)
{
    size_t first = pos;
    if (parseIPLiteral(pos))
        return true;
    pos = first;
    if (parseIPv4Address(pos))
        return true;
    pos = first;
    parseIregName(pos);
    return true;
}

// ipath-abempty  = *( "/" isegment )
// isegment       = *ipchar
// ipchar         = iunreserved / pct-encoded / sub-delims / ":" / "@"
void URL::parsePath(size_t& pos)
{
    while (pos < url.length()) {
        if (url[pos] != '/')
            return;
        ++pos;
        while (pos < url.length() && parsePchar(pos))
            ;
    }
}

// iquery = *( ipchar / iprivate / "/" / "?" )
bool URL::parseQuery(size_t& pos)
{
    while (pos < url.length()) {
        if (parsePchar(pos))
            continue;
        size_t first = pos;
        if (parsePrivate(pos))
            continue;
        pos = first;
        char16_t c = url[pos];
        if (c == '/' || c == '?') {
            ++pos;
            continue;
        }
        break;
    }
}

// ifragment = *( ipchar / "/" / "?" )
bool URL::parseFragment(size_t& pos)
{
    while (pos < url.length()) {
        if (parsePchar(pos))
            continue;
        char16_t c = url[pos];
        if (c == '/' || c == '?' || c == '#') {  // Note '#' is allowed by HTML 5.
            ++pos;
            continue;
        }
        break;
    }
}

// An empty abs_path is equivalent to an abs_path of "/"
bool URL::parseHTTP(size_t& pos)
{
    // "//"
    if (url.compare(pos, 2, u"//") != 0)
        return false;
    pos += 2;

    // ihost
    hostStart = hostnameStart = pos;
    size_t first = pos;
    if (!parseHost(pos))
        return false;
    if (pos == first)
        return false;  // The "http" scheme considers a missing authority or empty host invalid.
    hostnameEnd = pos;

    //  [ ":" port ]
    if (pos < url.length() && url[pos] == ':') {
        ++pos;
        portStart = pos;
        while (pos < url.length() && isDigit(url[pos]))
            ++pos;
        portEnd = pos;
    } else
        portStart = portEnd = pos;
    hostEnd = pos;

    // [ path-absolute [ "?" iquery ]]
    if (url.length() <= pos)
        url += u'/';
    else if (url[pos] != '/')
        url.insert(pos, 1, '/');
    pathnameStart = pos;
    parsePath(pos);
    pathnameEnd = pos;
    if (url.length() <= pos)
        return true;

    if (url[pos] == '?') {
        searchStart = pos;
        ++pos;
        parseQuery(pos);
        searchEnd = pos;
    }
    if (url.length() <= pos)
        return true;

    // [ "#" ifragment ]
    if (url[pos] == '#') {
        hashStart = pos;
        ++pos;
        parseFragment(pos);
        hashEnd = pos;
    }

    return url.length() <= pos;
}

bool URL::parseHTTPRelative()
{
    size_t pos = protocolEnd;
    do {
        // "//" authority
        if (url.compare(pos, 2, u"//") == 0) {
            pos += 2;

            // ihost
            hostStart = hostnameStart = pos;
            size_t first = pos;
            if (!parseHost(pos))
                return false;
            if (pos == first)
                return false;  // The "http" scheme considers a missing authority or empty host invalid.
            hostnameEnd = pos;

            //  [ ":" port ]
            if (pos < url.length() && url[pos] == ':') {
                ++pos;
                portStart = pos;
                while (pos < url.length() && isDigit(url[pos]))
                    ++pos;
                portEnd = pos;
            } else
                portStart = portEnd = pos;
            hostEnd = pos;

            // An empty abs_path is equivalent to an abs_path of "/"
            if (url.length() <= pos)
                url += u'/';
            else if (url[pos] != '/')
                url.insert(pos, 1, '/');
        }
        if (url.length() <= pos)
            break;

        if (url[pos] == '/') {
            // [ path-absolute [ "?" iquery ]]
            pathnameStart = pos;
            parsePath(pos);
            pathnameEnd = pos;
        } else {
            // a URI reference
            pathnameStart = pos;
            for (;;) {
                while (pos < url.length() && parsePchar(pos))
                    ;
                if (url.length() <= pos || url[pos] != '/')
                    break;
                ++pos;
            }
            pathnameEnd = pos;
        }
        if (url.length() <= pos)
            break;

        if (url[pos] == '?') {
            searchStart = pos;
            ++pos;
            parseQuery(pos);
            searchEnd = pos;
        }
        if (url.length() <= pos)
            break;

        // [ "#" ifragment ]
        if (url[pos] == '#') {
            hashStart = pos;
            ++pos;
            parseFragment(pos);
            hashEnd = pos;
        }

        if (pos < url.length())
            return false;
    } while (0);
    return true;
}

void URL::clear()
{
    protocolEnd = 0;
    hostStart = hostEnd = 0;
    hostnameStart = hostnameEnd = 0;
    portStart = portEnd = 0;
    pathnameStart = pathnameEnd = 0;
    searchStart = searchEnd = 0;
    hashStart = hashEnd = 0;
    url.clear();
}

bool URL::hasScheme()
{
    // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    size_t pos = 0;
    if (url.length() <= pos || !isAlpha(url[pos]))
        return false;
    ++pos;
    while (pos < url.length() && isSchemeChar(url[pos]))
        ++pos;

    // ":"
    if (url.length() <= pos || url[pos] != ':')
        return false;
    ++pos;
    protocolEnd = pos;
    for (size_t i = 0; i < protocolEnd; ++i)
        url[i] = toLower(url[i]);
    return true;
}

//
// File URL - file://<host>/<path>
//

bool URL::parseFile(size_t& pos)
{
    // "//"
    if (url.compare(pos, 2, u"//") != 0)
        return false;
    pos += 2;

    // host
    hostStart = hostnameStart = pos;
    size_t first = pos;
    if (!parseHost(pos))
        return false;
    hostEnd = hostnameEnd = pos;

    // "/" path
    if (url.length() <= pos)
        url += u'/';
    else if (url[pos] != '/')
        url.insert(pos, 1, '/');
    pathnameStart = pos;
    parsePath(pos);
    pathnameEnd = pos;
    if (url.length() <= pos)
        return true;

    // [ "#" ifragment ]
    if (url[pos] == '#') {
        hashStart = pos;
        ++pos;
        parseFragment(pos);
        hashEnd = pos;
    }

    return url.length() <= pos;
}

bool URL::parseFileRelative()
{
    size_t pos = protocolEnd;
    do {
        // "//" host
        if (url.compare(pos, 2, u"//") == 0) {
            pos += 2;

            // host
            hostStart = hostnameStart = pos;
            size_t first = pos;
            if (!parseHost(pos))
                return false;
            hostEnd = hostnameEnd = pos;

            // An empty abs_path is equivalent to an abs_path of "/"
            if (url.length() <= pos)
                url += u'/';
            else if (url[pos] != '/')
                url.insert(pos, 1, '/');
        }
        if (url.length() <= pos)
            break;

        if (url[pos] == '/') {
            // [ path-absolute [ "?" iquery ]]
            pathnameStart = pos;
            parsePath(pos);
            pathnameEnd = pos;
        } else {
            // a URI reference
            pathnameStart = pos;
            for (;;) {
                while (pos < url.length() && parsePchar(pos))
                    ;
                if (url.length() <= pos || url[pos] != '/')
                    break;
                ++pos;
            }
            pathnameEnd = pos;
        }
        if (url.length() <= pos)
            break;
        // [ "#" ifragment ]
        if (url[pos] == '#') {
            hashStart = pos;
            ++pos;
            parseFragment(pos);
            hashEnd = pos;
        }
        if (pos < url.length())
            return false;
        break;
    } while (0);
    return true;
}

//
// about URI
//   "about:" about-token [ about-query ]
//     about-token = *pchar
//     about-query = "?" query
//

bool URL::parseAbout(size_t& pos)
{
    if (url.length() <= pos)
        return true;

    // *pchar
    pathnameStart = pos;
    while (pos < url.length() && parsePchar(pos))
        ;
    pathnameEnd = pos;
    if (url.length() <= pos)
        return true;

    if (url[pos] == '?') {
        searchStart = pos;
        ++pos;
        parseQuery(pos);
        searchEnd = pos;
    }
    if (url.length() <= pos)
        return true;

    // [ "#" ifragment ]
    if (url[pos] == '#') {
        hashStart = pos;
        ++pos;
        parseFragment(pos);
        hashEnd = pos;
    }

    return url.length() <= pos;
}

bool URL::parseData(size_t& pos)
{
    if (url.length() <= pos)
        return false;
    size_t data = url.find(u',', pos);
    if (data == std::u16string::npos)
        return false;
    pathnameStart = pos;
    pos = data + 1;
    while (pos < url.length() && parsePchar(pos))
        ;
    pathnameEnd = pos;
    return url.length() <= pos;
}

bool URL::parseAboutRelative()
{
    size_t pos = protocolEnd;
    do {
        pathnameStart = pos;
        while (pos < url.length() && parsePchar(pos))
            ;
        pathnameEnd = pos;
        if (url.length() <= pos)
            break;

        // [ "#" ifragment ]
        if (url[pos] == '#') {
            hashStart = pos;
            ++pos;
            parseFragment(pos);
            hashEnd = pos;
        }
        if (pos < url.length())
            return false;
        break;
    } while (0);
    return true;
}

//
// Base
//

bool URL::parse()
{
    size_t pos = protocolEnd;
    if (url.compare(0, pos - 1, u"http") == 0 || url.compare(0, pos - 1, u"https") == 0)
        return parseHTTP(pos);
    if (url.compare(0, pos - 1, u"file") == 0)
        return parseFile(pos);
    if (url.compare(0, pos - 1, u"about") == 0)
        return parseAbout(pos);
    if (url.compare(0, pos - 1, u"data") == 0)
        return parseData(pos);
    // TODO: support other schemes
    return false;
}

bool URL::parseRelative(const URL& base)
{
    size_t pos = base.protocolEnd - 1;
    bool result = false;
    bool hier = true;
    if (base.url.compare(0, pos, u"http") == 0 || base.url.compare(0, pos, u"https") == 0)
        result = parseHTTPRelative();
    else if (base.url.compare(0, pos, u"file") == 0)
        result = parseFileRelative();
    else if (base.url.compare(0, pos, u"about") == 0) {
        result = parseAboutRelative();
        hier = false;
    }
    // TODO: support other schemes
    if (!result)
        return false;

    std::u16string targetURL = base.getProtocol();
    if (hier)
        targetURL += u"//";
    protocolEnd = base.protocolEnd;
    ptrdiff_t offset = 0;
    if (hostStart == hostEnd) {
        targetURL += base.getHost();
        hostStart = base.hostStart;
        hostEnd = base.hostEnd;
        hostnameStart = base.hostnameStart;
        hostnameEnd = base.hostnameEnd;
        portStart = base.portStart;
        portEnd = base.portEnd;
    } else {
        targetURL += getHost();
        ptrdiff_t d = base.hostStart - hostStart;
        hostStart += d;
        hostEnd += d;
        hostnameStart += d;
        hostnameEnd += d;
        portStart += d;
        portEnd += d;
    }

    pos = targetURL.length();
    if (!hier)
        targetURL += getPathname();
    else if (pathnameStart == pathnameEnd) {
        targetURL += base.getPathname();
        pathnameStart = pos;
        pathnameEnd = targetURL.length();
        if (searchStart != searchEnd)
            targetURL += getSearch();
        else
            targetURL += base.getSearch();
    } else {
        if (url[pathnameStart] == '/')
            targetURL += removeDotSegments(getPathname());
        else {
            // merge paths
            size_t slash = base.url.rfind('/', base.pathnameEnd - 1);
            assert(slash != std::u16string::npos);
            targetURL += removeDotSegments(base.url.substr(base.pathnameStart, slash + 1 - base.pathnameStart) + getPathname());
        }
        pathnameStart = pos;
        pathnameEnd = targetURL.length();
        targetURL += getSearch();
    }
    searchStart = pathnameEnd;
    searchEnd = targetURL.length();

    targetURL += getHash();
    hashStart = searchEnd;
    hashEnd = targetURL.length();

    url = targetURL;
    return true;
}

URL::URL(const std::u16string& string) :
    url(string),
    protocolEnd(0),
    hostStart(0),
    hostEnd(0),
    hostnameStart(0),
    hostnameEnd(0),
    portStart(0),
    portEnd(0),
    pathnameStart(0),
    pathnameEnd(0),
    searchStart(0),
    searchEnd(0),
    hashStart(0),
    hashEnd(0)
{
    stripLeadingAndTrailingWhitespace(url);
    if (!hasScheme()) {
        clear();
        return;
    }
    if (!parse()) {
        clear();
        return;
    }
}

URL::URL(const URL& base, const std::u16string& relative) :
    url(relative),
    protocolEnd(0),
    hostStart(0),
    hostEnd(0),
    hostnameStart(0),
    hostnameEnd(0),
    portStart(0),
    portEnd(0),
    pathnameStart(0),
    pathnameEnd(0),
    searchStart(0),
    searchEnd(0),
    hashStart(0),
    hashEnd(0)
{
    stripLeadingAndTrailingWhitespace(url);
    if (hasScheme() && (testProtocol(u"data") || base.isEmpty() || base.getProtocol() != getProtocol())) {
        if (!parse())
            clear();
        return;
    }
    if (!parseRelative(base)) {
        clear();
        return;
    }
}

URL::URL(const URL&other) :
    url(other.url),
    protocolEnd(other.protocolEnd),
    hostStart(other.hostStart),
    hostEnd(other.hostEnd),
    hostnameStart(other.hostnameStart),
    hostnameEnd(other.hostnameEnd),
    portStart(other.portStart),
    portEnd(other.portEnd),
    pathnameStart(other.pathnameStart),
    pathnameEnd(other.pathnameEnd),
    searchStart(other.searchStart),
    searchEnd(other.searchEnd),
    hashStart(other.hashStart),
    hashEnd(other.hashEnd)
{
}

std::u16string URL::getPort() const {
    if (portStart < portEnd)
        return url.substr(portStart, portEnd - portStart);
    if (!url.compare(0, 5, u"http:"))
        return u"80";
    if (!url.compare(0, 6, u"https:"))
        return u"443";
    return u"";
}

}}}}  // org::w3c::dom::bootstrap
