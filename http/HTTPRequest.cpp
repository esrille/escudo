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

#include "HTTPRequest.h"

#include <iostream>
#include <string>

#include "utf.h"

#include "url/URI.h"
#include "http/HTTPCache.h"
#include "http/HTTPConnection.h"

#include "css/Box.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

const unsigned short HttpRequest::UNSENT;
const unsigned short HttpRequest::OPENED;
const unsigned short HttpRequest::HEADERS_RECEIVED;
const unsigned short HttpRequest::LOADING;
const unsigned short HttpRequest::COMPLETE;
const unsigned short HttpRequest::DONE;

std::string HttpRequest::aboutPath;
std::string HttpRequest::cachePath("/tmp");

int HttpRequest::getContentDescriptor()
{
    return fdContent;  // TODO: call dup internally?
}

std::FILE* HttpRequest::openFile()
{
    if (fdContent == -1)
        return 0;
    std::FILE* file = fdopen(dup(fdContent), "rb");
    if (!file)
        return 0;
    rewind(file);
    return file;
}

std::fstream& HttpRequest::getContent()
{
    if (content.is_open())
        return content;

    char filename[PATH_MAX];
    if (PATH_MAX <= cachePath.length() + 9)
        return content;
    strcpy(filename, cachePath.c_str());
    strcat(filename, "/esXXXXXX");
    fdContent = mkstemp(filename);
    if (fdContent == -1)
        return content;
    content.open(filename, std::ios_base::trunc | std::ios_base::in | std::ios_base::out | std::ios::binary);
    remove(filename);

    return content;
}

bool HttpRequest::complete(bool error)
{
    readyState = handler ? COMPLETE : DONE;
    errorFlag = error;
    if (cache)
        cache->notify(this, error);
    return handler;
}

void HttpRequest::notify()
{
    readyState = DONE;
    if (handler) {
        handler();
        handler.clear();
    }
}

bool HttpRequest::notify(bool error)
{
    if (complete(error))
        notify();
    return errorFlag;
}

void HttpRequest::open(const std::u16string& method, const std::u16string& urlString)
{
    URL url(base, urlString);
    request.open(utfconv(method), url);
    readyState = OPENED;
}

void HttpRequest::setRequestHeader(const std::u16string& header, const std::u16string& value)
{
    request.setHeader(utfconv(header), utfconv(value));
}

bool HttpRequest::constructResponseFromCache(bool sync)
{
    readyState = DONE;
    errorFlag = false;
    response.update(cache->getResponseMessage());

    // TODO: deal with partial...
    int fd = cache->getContentDescriptor();
    if (0 <= fd)
        fdContent = dup(fd);

    cache = 0;
    if (sync)
        notify();
    else
        HttpConnectionManager::getInstance().complete(this, errorFlag);
    return errorFlag;
}

bool HttpRequest::send()
{
    if (request.getURL().isEmpty())
        return notify(false);

    if (request.getURL().testProtocol(u"file")) {
        if (request.getMethodCode() != HttpRequestMessage::GET)
            return notify(true);
        std::u16string host = request.getURL().getHostname();
        if (!host.empty() && host != u"localhost")  // TODO: maybe allow local host IP addresses?
            return notify(true);
        std::string path = utfconv(request.getURL().getPathname());
        fdContent = ::open(path.c_str(), O_RDONLY);
        return notify(fdContent == -1);
    }

    if (request.getURL().testProtocol(u"about")) {
        if (aboutPath.empty() || request.getMethodCode() != HttpRequestMessage::GET)
            return notify(true);
        std::string path = utfconv(request.getURL().getPathname());
        if (path.empty())
            path = aboutPath + "/about.html";
        else
            path = aboutPath + "/about/" + path;
        fdContent = ::open(path.c_str(), O_RDONLY);
        return notify(fdContent == -1);
    }

    cache = HttpCacheManager::getInstance().send(this);
    if (!cache || cache->isBusy())
        return false;
    return constructResponseFromCache(true);
}

void HttpRequest::abort()
{
    if (readyState == UNSENT)
        return;

    // TODO: implement more details.
    clearHanndler();
    HttpConnectionManager& manager = HttpConnectionManager::getInstance();
    manager.abort(this);
    readyState = UNSENT;
    errorFlag = false;
    request.clear();
    response.clear();
    if (content.is_open())
        content.close();
    if (0 <= fdContent) {
        close(fdContent);
        fdContent = -1;
    }
}

unsigned short HttpRequest::getStatus() const
{
    return response.getStatus();
}

const std::string& HttpRequest::getStatusText() const
{
    return response.getStatusText();
}

const std::string HttpRequest::getResponseHeader(std::u16string header) const
{
    return response.getResponseHeader(utfconv(header));
}

const std::string& HttpRequest::getAllResponseHeaders() const
{
    return response.getAllResponseHeaders();
}

HttpRequest::HttpRequest(const std::u16string& base) :
    base(base),
    readyState(UNSENT),
    errorFlag(false),
    fdContent(-1),
    cache(0),
    handler(0),
    boxImage(0)
{
}

HttpRequest::~HttpRequest()
{
    abort();
    delete boxImage;
}

}}}}  // org::w3c::dom::bootstrap
