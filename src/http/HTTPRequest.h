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

#ifndef ES_HTTP_REQUEST_H
#define ES_HTTP_REQUEST_H

#include <atomic>
#include <cstdio>
#include <deque>
#include <memory>
#include <fstream>
#include <boost/function.hpp>

#include "http/HTTPRequestMessage.h"
#include "http/HTTPResponseMessage.h"

namespace org { namespace w3c { namespace dom {

namespace bootstrap {

class BoxImage;
class HttpCache;
class HttpRequest;

typedef std::shared_ptr<HttpRequest> HttpRequestPtr;

class HttpRequest : public std::enable_shared_from_this<HttpRequest>
{
    friend class HttpCacheManager;
    friend class HttpConnectionManager;

public:
    // states
    static const unsigned short UNSENT = 0;
    static const unsigned short OPENED = 1;
    static const unsigned short HEADERS_RECEIVED = 2;
    static const unsigned short LOADING = 3;
    static const unsigned short COMPLETE = 4;
    static const unsigned short DONE = 5;

    // flags
    static const unsigned short DONT_REMOVE = 1;    // Do not remove filePath upon destruction
    static const unsigned short CANCELED = 2;

private:
    static std::string aboutPath;
    static std::string cachePath;

    std::u16string base;
    unsigned short readyState;
    std::atomic_ushort flags;
    bool errorFlag;
    HttpRequestMessage request;
    HttpResponseMessage response;

    std::string filePath;
    std::fstream content;

    HttpCache* cache;
    boost::function<void (void)> handler;
    long long lastModified;

    std::deque<boost::function<void (void)>> callbackList;

    BoxImage* boxImage;

public:
    HttpRequest(const std::u16string& base = u"");
    ~HttpRequest();

    HttpRequestPtr self() {
        return shared_from_this();
    }

    void setBase(const std::u16string& base) {
        this->base = base;
    }

    const URL& getURL() const {
        return request.getURL();
    }

    bool constructResponseFromCache(bool sync = false);
    bool constructResponseFromData();

    HttpRequestMessage& getRequestMessage() {
        return request;
    }
    HttpResponseMessage& getResponseMessage() {
        return response;
    }

    const std::string& getFilePath() const {
        return filePath;
    }
    void removeFile() {
        if (!filePath.empty()) {
            remove(filePath.c_str());
            filePath.clear();
        }
    }

    int getContentDescriptor();
    std::fstream& getContent();
    std::FILE* openFile();

    void setHandler(boost::function<void (void)> f);
    void clearHandler();
    unsigned addCallback(boost::function<void (void)> f, unsigned id = static_cast<unsigned>(-1));
    void clearCallback(unsigned id);

    bool redirect(const HttpResponseMessage& res);
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
    void abort(bool dtor = false);

    // cancel() does not abort the currently running request, but when notified
    // the caller can test if the request has been canceled. If the request
    // has been completed or not used at all, it is simply deleted.
    void cancel();
    bool isCanceled() const {
        return flags & CANCELED;
    }

    unsigned short getStatus() const;
    const std::string& getStatusText() const;
    const std::string getResponseHeader(std::u16string header) const;
    std::string getAllResponseHeaders() const;
    // void overrideMimeType(std::u16string mime);

    bool getError() const {
        return errorFlag;
    }
    void setError(bool error) {
        errorFlag = error;
    }

    long long getLastModified() const {
        return lastModified;
    }

    BoxImage* getBoxImage(unsigned repeat);

    static void setAboutPath(const std::string& path) {
        aboutPath = path;
    }
    static void setCachePath(const std::string& path) {
        cachePath = path;
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_REQUEST_H
