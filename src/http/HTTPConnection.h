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
#include <mutex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "http/HTTPCache.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpConnection;

class HttpConnectionManager
{
    std::recursive_mutex mutex;
    std::list<HttpConnection*> connections;
    std::list<HttpRequest*> completed;

    boost::asio::io_service ioService;
    boost::asio::ip::tcp::resolver resolver;
    boost::asio::io_service::work work;

    HttpRequest* getCompleted();

public:
    HttpConnectionManager() :
        resolver(ioService),
        work(ioService)
    {
    }

    HttpConnection* getConnection(const std::string& protocol, const std::string& hostname, const std::string& port);
    void send(HttpRequest* request);
    void abort(HttpRequest* request);
    void done(HttpConnection* conn, bool error);
    void complete(HttpRequest* request, bool error);
    void poll();

    template <typename ResolveHandler>
    void resolve(const boost::asio::ip::tcp::resolver::query& q, ResolveHandler handler) {
        resolver.async_resolve(q,handler);
    }

    void operator()();
    void stop() {
        ioService.stop();
    }

    static HttpConnectionManager& getInstance() {
        static HttpConnectionManager manager;
        return manager;
    }

    static boost::asio::io_service& getIOService() {
        return getInstance().ioService;
    }
};

class HttpConnection
{
    friend class HttpConnectionManager;

    // State
    enum {
        Closed,
        Resolving,
        Resolved,
        Connected,
        Handshaking,
        // RequestSent,
        ReadStatusLine,
        ReadHead,
        ReadContent,
        ReadChunk,
        ReadTrailer,
        CloseWait
    };

    static const int MaxRetryCount = 3;

    int state;
    int retryCount;
    std::string line;  // line buffer

    std::string protocol;
    std::string hostname;
    std::string port;

    // Boost
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf request;
    boost::asio::streambuf response;

    boost::asio::ssl::context context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> secureSocket;

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
    void handleHandshake(const boost::system::error_code& err);
    void handleWriteRequest(const boost::system::error_code& err);
    void handleRead(const boost::system::error_code& err);

    void readStatusLine(const boost::system::error_code& err);
    void readHead(const boost::system::error_code& err);
    void readContent(const boost::system::error_code& err);
    void readChunk(const boost::system::error_code& err);
    void readTrailer(const boost::system::error_code& err);

    void close();
    void retry();

    void send(HttpRequest* request);
    void abort(HttpRequest* request);
    void done(HttpConnectionManager* manager, bool error);

    template<typename CompletionCondition, typename ReadHandler>
    void asyncRead(boost::asio::streambuf& buffers, CompletionCondition completionCondition, ReadHandler handler) {
        if (protocol == "https:")
            boost::asio::async_read(secureSocket, buffers, completionCondition, handler);
        else
            boost::asio::async_read(socket, buffers, completionCondition, handler);
    }

    template<typename WriteHandler>
    void asyncWrite(boost::asio::streambuf& buffers, WriteHandler handler) {
        if (protocol == "https:")
            boost::asio::async_write(secureSocket, buffers, handler);
        else
            boost::asio::async_write(socket, buffers, handler);
    }

public:
    HttpConnection(const std::string& protocol, const std::string& hostname, const std::string& port);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_HTTP_CONNECTION_H