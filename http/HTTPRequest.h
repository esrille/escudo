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

#ifndef ES_HTTP_REQUEST_H
#define ES_HTTP_REQUEST_H

#include <fstream>
#include <cstdio>
#include <boost/function.hpp>

#include "http/HTTPRequestMessage.h"
#include "http/HTTPResponseMessage.h"

namespace org { namespace w3c { namespace dom {

namespace bootstrap {

class HttpCache;

class HttpRequest
{
    friend class HttpConnectionManager;

public:
    // states
    static const unsigned short UNSENT = 0;
    static const unsigned short OPENED = 1;
    static const unsigned short HEADERS_RECEIVED = 2;
    static const unsigned short LOADING = 3;
    static const unsigned short COMPLETE = 4;
    static const unsigned short DONE = 5;

private:
    std::u16string base;
    unsigned short readyState;
    bool errorFlag;
    HttpRequestMessage request;
    HttpResponseMessage response;
    std::fstream content;
    int fdContent;
    HttpCache* cache;
    boost::function<void (void)> handler;

public:
    HttpRequest(const std::u16string& base = u"");
    ~HttpRequest();

    bool constructResponseFromCache(bool sync = false);

    HttpRequestMessage& getRequestMessage() {
        return request;
    }
    HttpResponseMessage& getResponseMessage() {
        return response;
    }
    int getContentDescriptor();
    std::fstream& getContent();
    std::FILE* openFile();

    void setHanndler(boost::function<void (void)> f) {
        handler = f;
    }
    void clearHanndler() {
        handler.clear();
    }

    bool complete(bool error);
    void notify();
    bool notify(bool error);

    unsigned short getReadyState() const {
        return readyState;
    }
    void open(const std::u16string& method, const std::u16string& url);
    void setRequestHeader(const std::u16string& header, const std::u16string& value);
    unsigned int getTimeout();
    void setTimeout(unsigned int timeout);
    bool send();
    void abort();
    unsigned short getStatus() const;
    const std::string& getStatusText() const;
    const std::string getResponseHeader(std::u16string header) const;
    const std::string& getAllResponseHeaders() const;
    // void overrideMimeType(std::u16string mime);

    bool getError() const {
        return errorFlag;
    }
    void setError(bool error) {
        errorFlag = error;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_REQUEST_H
