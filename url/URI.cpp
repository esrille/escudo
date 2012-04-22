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

#include "URI.h"

#include <assert.h>
#include <unicode/uidna.h>

#include "utf.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

std::string percentEncode(const std::u16string& string, size_t pos, size_t n)
{
    std::string encoding;
    const char16_t* p = string.c_str() + pos;
    for (const char16_t* s = p; s < p + n; ) {
        char32_t utf32;
        s = utf16to32(s, &utf32);
        assert(s);
        char utf8[5];
        char* end = utf32to8(utf32, utf8);
        assert(end);
        for (const char* p = utf8; p < end; ++p) {
            if (*p <= 0x20 || (*p < 127 && strchr("\"#%<>[\\]^{|}", *p))) {
                encoding += '%';
                char hex[3];
                sprintf(hex, "%02X", *p & 0xff);
                encoding += hex;
            } else
                encoding += *p;
        }
    }
    return encoding;
}

}  // namespace

void URI::clear()
{
    protocolEnd = 0;
    hostStart = hostEnd = 0;
    hostnameStart = hostnameEnd = 0;
    portStart = portEnd = 0;
    pathnameStart = pathnameEnd = 0;
    searchStart = searchEnd = 0;
    hashStart = hashEnd = 0;
    uri.clear();
}

URI::URI(const URL& url)
{
    if (url.isEmpty()) {
        protocolEnd = 0;
        hostStart = hostEnd = 0;
        hostnameStart = hostnameEnd = 0;
        portStart = portEnd = 0;
        pathnameStart = pathnameEnd = 0;
        searchStart = searchEnd = 0;
        hashStart = hashEnd = 0;
        return;
    }

    uri += percentEncode(url.url, 0, url.protocolEnd);
    protocolEnd = uri.length();

    // TODO: the following code is HTTP specific.
    uri += "//";
    hostStart = hostnameStart = uri.length();
    if (url.hostnameStart < url.hostnameEnd) {
        UChar idn[256];
        int32_t len;
        UErrorCode status = U_ZERO_ERROR;
        len = uidna_IDNToASCII(reinterpret_cast<const UChar*>(url.url.c_str()) + url.hostnameStart, url.hostnameEnd - url.hostnameStart,
                               idn, 256, UIDNA_DEFAULT, 0, &status);
        if (status != U_ZERO_ERROR) {
            // TODO: error
            clear();
            return;
        }
        for (int32_t i = 0; i < len; ++i)
            uri += static_cast<char>(idn[i]);
    }
    hostnameEnd = uri.length();
    if (url.portStart < url.portEnd) {
        uri += ':';
        portStart = uri.length();
        uri += percentEncode(url.url, url.portStart, url.portEnd - url.portStart);
        portEnd = uri.length();
    } else
        portStart = portEnd = 0;
    pathnameStart = hostEnd = uri.length();
    uri += percentEncode(url.url, url.pathnameStart, url.pathnameEnd - url.pathnameStart);
    pathnameEnd = uri.length();
    if (url.searchStart < url.searchEnd) {
        searchStart = uri.length();
        uri += percentEncode(url.url, url.searchStart, url.searchEnd - url.searchStart);
        searchEnd = uri.length();
    } else
        searchStart = searchEnd = 0;
    if (url.hashStart < url.hashEnd) {
        hashStart = uri.length();
        uri += percentEncode(url.url, url.hashStart, url.hashEnd - url.hashStart);
        hashEnd = uri.length();
    } else
        hashStart = hashEnd = 0;
}

std::string URI::getPort() const
{
    if (portStart < portEnd)
        return uri.substr(portStart, portEnd - portStart);
    if (!uri.compare(0, 5, "http:"))
        return "80";
    if (!uri.compare(0, 6, "https:"))
        return "443";
    return "";
}

}}}}  // org::w3c::dom::bootstrap
