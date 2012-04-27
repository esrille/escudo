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

#ifndef ES_HTTP_CACHE_H
#define ES_HTTP_CACHE_H

#include <fstream>
#include <list>

#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpCache
{
    friend class HttpCacheManager;

    URL url;
    HttpResponseMessage response;
    unsigned long long contentLength;
    int fdContent;  // for content

    unsigned requestTime;

    std::string etag;
    bool range;
    bool mustRevalidate;
    int hitCount;
    unsigned lm;  // Last modified
    unsigned expires;
    unsigned freshnessLifetime;
    unsigned responseTime;
    unsigned correctedInitialAge;

    std::list<HttpRequest*> requests;
    HttpRequest* current;
public:

    bool isBusy() const {
        return current;
    }

    int getContentDescriptor() {
        return fdContent;
    }

    void notify(HttpRequest* request, bool error);

    // via PUT, DELETE, POST, and unknown
    void invalidate();
    bool isFresh();
    HttpCache* send(HttpRequest* request);
    bool abort(HttpRequest* request);

    const HttpResponseMessage& getResponseMessage() const {
        return response;
    }

    HttpCache(const URL& url) :
        url(url),
        contentLength(0),
        fdContent(-1),
        requestTime(0),
        range(false),
        mustRevalidate(false),
        hitCount(0),
        lm(0),
        expires(0),
        freshnessLifetime(0),
        responseTime(0),
        correctedInitialAge(0),
        current(0)
    {
    }

    ~HttpCache()
    {
        if (0 <= fdContent)
            close(fdContent);
    }
};

class HttpCacheManager
{
    std::list<HttpCache*> list;
public:
    HttpCache* getCache(const URL& url);
    HttpCache* send(HttpRequest* request);

    static HttpCacheManager& getInstance()
    {
        static HttpCacheManager manager;
        return manager;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_CACHE_H