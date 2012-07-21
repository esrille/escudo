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

    if (error)
        invalidate();
    else {
        response.update(request->getResponseMessage());
        // TODO: deal with partial...
        int fd = request->getContentDescriptor();
        if (0 <= fd)
            fdContent = dup(fd);
    }

    while (!requests.empty()) {
        // TODO: Do this only when cacheable
        assert(!isBusy());
        request = requests.front();
        requests.pop_front();
        request->constructResponseFromCache(false);
    }

    if (!error)
        return;
    assert(requests.empty());
    HttpCacheManager& manager = HttpCacheManager::getInstance();
    manager.remove(this);
    delete this;
}

void HttpCache::invalidate()
{
    response.clear();
    contentLength = 0;
    close(fdContent);
    fdContent = -1;
    requestTime = 0;
}

HttpCache* HttpCache::send(HttpRequest* request)
{
    HttpRequestMessage& requestMessage = request->getRequestMessage();

    if (response.isCacheable() && response.isFresh(requestTime)) {
        if (requestMessage.getMethodCode() == HttpRequestMessage::HEAD || 0 <= fdContent)
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

bool HttpCache::abort(HttpRequest* request)
{
    if (current != request) {
        requests.remove(request);
        return true;
    }
    return false;
}

HttpCache* HttpCacheManager::getCache(const URL& url)
{
    for (auto i = lru.begin(); i != lru.end(); ++i) {
        HttpCache* cache = *i;
        if (cache->url == url) {
            lru.erase(i);
            lru.push_front(cache);
            return cache;
        }
    }
    HttpCache* cache = new(std::nothrow) HttpCache(url);
    if (cache)
        lru.push_front(cache);
    return cache;
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

void HttpCacheManager::remove(HttpCache* cache)
{
    lru.remove(cache);
}

}}}}  // org::w3c::dom::bootstrap
