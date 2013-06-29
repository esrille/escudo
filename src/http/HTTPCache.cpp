/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "utf.h"

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
        unsigned short status = request->getResponseMessage().getStatus();
        if (status == 304) {  // Not Modified?
            request->removeFile();
            request->constructResponseFromCache(false);
        } else {
            if (!filePath.empty() && filePath != request->getFilePath())
                remove(filePath.c_str());
            response.updateStatus(request->getResponseMessage());
            filePath = request->getFilePath();
        }
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
    HttpCacheManager& manager(HttpCacheManager::getInstance());
    manager.remove(this);
    delete this;
}

void HttpCache::invalidate()
{
    response.clear();
    contentLength = 0;
    if (!filePath.empty()) {
        remove(filePath.c_str());
        filePath.clear();
    }
    requestTime = 0;
}

HttpCache* HttpCache::send(HttpRequest* request)
{
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
        HttpRequestMessage& requestMessage(request->getRequestMessage());
        std::string lastModified = response.getResponseHeader("Last-Modified");
        if (!lastModified.empty()) {
            // Use only strong validator here
            long long date = response.getDateValue();
            if (date == 0)
                date = requestTime;
            long long lastModifiedValue;
            if (response.getLastModifiedValue(lastModifiedValue) && 60 <= date - lastModifiedValue)
                requestMessage.setHeader("If-Modified-Since", lastModified);
        }
        // by If-None-Match
        std::string value = response.getResponseHeader("ETag");
        if (!value.empty())
            requestMessage.setHeader("If-None-Match", value);
    }
    HttpConnectionManager& manager(HttpConnectionManager::getInstance());
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
    for (;;) {
        // TODO: check protocol is http.
        HttpRequestMessage& message(request->getRequestMessage());
        HttpCache* cache = getCache(message.getURL());
        if (!cache)
            return 0;

        int code = message.getMethodCode();
        if (code == HttpRequestMessage::GET || code == HttpRequestMessage::HEAD) {
            if (cache->response.isCacheable() && cache->response.isFresh(cache->requestTime)) {
                if (request->redirect(cache->response))
                    continue;
                if (code == HttpRequestMessage::HEAD || !cache->filePath.empty())
                    return cache;
            }
            return cache->send(request);
        }
        break;
    }

    // No need for going through the cache
    // TODO: call cache->invlidate();
    request->flags &= ~HttpRequest::DONT_REMOVE;
    HttpConnectionManager& manager(HttpConnectionManager::getInstance());
    manager.send(request);
    return 0;
}

void HttpCacheManager::remove(HttpCache* cache)
{
    lru.remove(cache);
}

void HttpCacheManager::dump() {
    for (auto i = lru.begin(); i != lru.end(); ++i) {
        HttpCache* cache = *i;
        std::cout << static_cast<std::u16string>(cache->url) << ' ' << cache->response.getStatus() << ' ' << cache->filePath << '\n';
    }
}

HttpCacheManager::~HttpCacheManager()
{
    while (!lru.empty()) {
        HttpCache* cache = lru.front();
        remove(cache);
        delete cache;
    }
}

}}}}  // org::w3c::dom::bootstrap
