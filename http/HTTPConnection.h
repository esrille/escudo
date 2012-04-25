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

#ifndef ES_HTTP_CONNECTION_H
#define ES_HTTP_CONNECTION_H

#include <list>

#include <boost/asio.hpp>

#include "http/HTTPCache.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpConnection;

class HttpConnectionManager
{
    std::list<HttpConnection*> connections;

    // Boost
    boost::asio::io_service ioService;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::io_service::work work;

public:
    HttpConnectionManager() :
        resolver(ioService),
        work(ioService)
    {
    }

    HttpConnection* getConnection(const std::string& hostname, const std::string& port);

    void send(HttpRequest* request);
    void abort(HttpRequest* request);

    template <typename ResolveHandler>
    void resolve(const boost::asio::ip::tcp::resolver::query& q, ResolveHandler handler) {
        resolver.async_resolve(q,handler);
    }

    static HttpConnectionManager& getInstance()
    {
        static HttpConnectionManager manager;
        return manager;
    }

    static boost::asio::io_service& getIOService()
    {
        return getInstance().ioService;
    }
};

class HttpConnection
{
    // State
    enum {
        Closed,
        Resolving,
        Resolved,
        Connected,
        // RequestSent,
        ReadStatusLine,
        ReadHead,
        ReadContent,
        ReadChunk,
        ReadTrailer,
        CloseWait
    };

    static const int MaxRetryCount = 3;

    friend class HttpConnectionManager;

    int state;
    int retryCount;
    std::string line;  // line buffer

    std::string hostname;
    std::string port;

    // Boost
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf request;
    boost::asio::streambuf response;

    unsigned long long octetCount;
    unsigned long long contentLength;

    // chunked
    unsigned long long chunkLength;
    int chunkCRLF;

    std::list<HttpRequest*> requests;
    HttpRequest* current;

    void sendRequest();

    void handleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpointIterator);
    void handleConnect(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpointIterator);
    void handleWriteRequest(const boost::system::error_code& err);
    void handleRead(const boost::system::error_code& err);

    void readStatusLine(const boost::system::error_code& err);
    void readHead(const boost::system::error_code& err);
    void readContent(const boost::system::error_code& err);
    void readChunk(const boost::system::error_code& err);
    void readTrailer(const boost::system::error_code& err);

    void close();
    void retry();

    void abort(HttpRequest* request);

public:
    HttpConnection(const std::string& hostname, const std::string& port);

    void send(HttpRequest* request);
    void done(bool error);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_CONNECTION_H