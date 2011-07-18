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

#include "HTTPCache.h"

#include <assert.h>
#include <time.h>

#include <iostream>

#include "url/URI.h"
#include "http/HTTPConnection.h"

// "expiration" mechanism
// "validation" mechanism

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void HttpCache::notify(HttpRequest* request, bool error)
{
    current = 0;

    if (error || !response.update(request->getResponseMessage()))
        invalidate();
    else {
        // TODO: deal with partial...
        int fd = request->getContentDescriptor();
        if (0 <= fd)
            fdContent = dup(fd);
    }

    while (!requests.empty()) {
        request = requests.front();
        requests.pop_front();
        request->send();
        if (isBusy())
            break;
    }
}

void HttpCache::invalidate()
{
    response.clear();
    contentLength = 0;
    close(fdContent);
    fdContent = -1;
    requestTime = 0;
}

bool HttpCache::isFresh()
{
    unsigned now = time(0);
    // TODO: if (url.hasSearch()) ...
    unsigned freshnessLifetime = response.getFreshnessLifetime(now);
    unsigned currentAge = response.getCurrentAge(now, requestTime);
    return freshnessLifetime > currentAge;
}

HttpCache* HttpCache::send(HttpRequest* request)
{
    HttpRequestMessage& requestMessage = request->getRequestMessage();

    // TODO: check cache-control

    if (isFresh()) {
        if (requestMessage.getMethodCode() == HttpRequestMessage::HEAD ||
            0 <= fdContent)
            return this;
    }

    if (current) {
        requests.push_back(request);
        return this;
    }
    current = request;

    if (!requestTime)
        requestTime = time(0);
    else {
        // Validate
        // by If-Modified-Since
        std::string lastModified = response.getResponseHeader("Last-Modified");
        if (!lastModified.empty()) {
            // Use only strong validator here
            unsigned date = response.getDateValue();
            if (date == 0)
                date = requestTime;
            unsigned lastModifiedValue;
            if (response.getLastModifiedValue(lastModifiedValue) && 60 <= date - lastModifiedValue)
                requestMessage.setHeader("If-Modified-Since", lastModified);
        }
        // by If-None-Match
        std::string value = response.getResponseHeader("ETag");
        if (!value.empty())
            requestMessage.setHeader("If-None-Match", value);
    }
    HttpConnectionManager& manager = HttpConnectionManager::getInstance();
    manager.send(request);
    return this;
}

void HttpCache::abort(HttpRequest* request)
{
    if (current != request) 
        requests.remove(request);
    else {
        HttpConnectionManager& manager = HttpConnectionManager::getInstance();
        manager.abort(request);
    }
}

HttpCache* HttpCacheManager::getCache(const URL& url)
{
    for (auto i = list.begin(); i != list.end(); ++i) {
        if ((*i)->url == url)
            return *i;
    }
    HttpCache* c = new(std::nothrow) HttpCache(url);
    if (c)
        list.push_back(c);
    return c;
}

HttpCache* HttpCacheManager::send(HttpRequest* request)
{
    HttpRequestMessage& message = request->getRequestMessage();
    HttpCache* cache = getCache(message.getURL());
    if (!cache)
        return 0;

    int methodCode = message.getMethodCode();
    if (methodCode == HttpRequestMessage::GET || methodCode == HttpRequestMessage::HEAD)
        return cache->send(request);

    // No need for going through the cache
    // TODO: call cache->invlidate();
    HttpConnectionManager& manager = HttpConnectionManager::getInstance();
    manager.send(request);
    return 0;
}

}}}}  // org::w3c::dom::bootstrap
