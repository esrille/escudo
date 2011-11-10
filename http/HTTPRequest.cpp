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

#include "HTTPRequest.h"

#include <iostream>
#include <string>

#include "utf.h"

#include "url/URI.h"
#include "http/HTTPCache.h"
#include "http/HTTPConnection.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

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

    char filename[] = "/tmp/esXXXXXX";
    fdContent = mkstemp(filename);
    if (fdContent == -1)
        return content;
    content.open(filename, std::ios_base::trunc | std::ios_base::in | std::ios_base::out | std::ios::binary);
    remove(filename);

    return content;
}

void HttpRequest::notify(bool error)
{
    readyState = DONE;
    errorFlag = error;
    if (!cache)
        return;
    cache->notify(this, error);

    if (handler)
        handler();
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

void HttpRequest::constructResponseFromCache()
{
    readyState = DONE;
    errorFlag = false;
    response.update(cache->getResponseMessage());

    // TODO: deal with partial...
    int fd = cache->getContentDescriptor();
    if (0 <= fd)
        fdContent = dup(fd);

    if (handler)
        handler();
}

void HttpRequest::send()
{
    cache = HttpCacheManager::getInstance().send(this);
    if (!cache || cache->isBusy())
        return;
    constructResponseFromCache();
}

void HttpRequest::abort()
{
    // TODO: implement more details.
    clearHanndler();
    if (cache)
        cache->abort(this);
    else {
        HttpConnectionManager& manager = HttpConnectionManager::getInstance();
        manager.abort(this);
    }
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
    cache(0)
{
}

HttpRequest::~HttpRequest()
{
    abort();
}

}}}}  // org::w3c::dom::bootstrap
