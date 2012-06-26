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

#include "HTTPRequestMessage.h"

#include <algorithm>

#include "url/URI.h"
#include "http/HTTPUtil.h"

namespace {

// methodNames must be in sync with method codes in HttpRequestMessage
const char* methodNames[] = {
    "OPTIONS",
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "DELETE",
    "TRACE",
    "CONNECT"
};
const char** methodNamesEnd = methodNames + (sizeof methodNames / sizeof methodNames[0]);

}

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace http;

int HttpRequestMessage::getMethodCode() const
{
    return std::find(methodNames, methodNamesEnd, method) - methodNames;
}

void HttpRequestMessage::open(const std::string& method, const std::u16string& url)
{
    this->method = method;
    toUpperCase(this->method);
    this->url = URL(url);
    setHeader("User-Agent", "Escort/" PACKAGE_VERSION);
}

void HttpRequestMessage::setHeader(const std::string& header, const std::string& value)
{
    headers.set(header, value);
}

void HttpRequestMessage::clear()
{
    version = 11;
    headers.clear();
}

std::string HttpRequestMessage::toString()
{
    URI uri(url);
    std::string s;
    if (version == 9) {
        s += "GET " + uri.getPathname() + uri.getSearch() + " \r\n";
        return s;
    }
    s += method + ' ' + uri.getPathname() + uri.getSearch() + " HTTP/";
    switch (version) {
    case 11:
        s += "1.1";
        headers.set("Host", uri.getHost());
        break;
    default:
        s += "1.0";
        break;
    }
    s += "\r\n";
    s += headers.toString();
    return s;
}

}}}}  // org::w3c::dom::bootstrap
