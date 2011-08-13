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

#include "HTTPConnection.h"

#include <algorithm>
#include <iostream>
#include <boost/bind.hpp>

#include "url/URI.h"
#include "http/HTTPUtil.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace http;

void HttpConnection::sendRequest()
{
    std::ostream stream(&request);
    stream << current->getRequestMessage().toString();
    stream << "\r\n";
    boost::asio::async_write(socket, request, boost::bind(&HttpConnection::handleWriteRequest, this, boost::asio::placeholders::error));
}

void HttpConnection::done(bool error)
{
    line.clear();
    if (current) {
        current->notify(error);
        current = 0;
    }
    if (!error) {
        if (!requests.empty()) {
            current = requests.front();
            requests.pop_front();
            sendRequest();
        }
    } else {
        while (!requests.empty()) {
            current = requests.front();
            requests.pop_front();
            current->notify(error);
        }
        current = 0;
    }
}

void HttpConnection::close()
{
    state = Closed;
    line.clear();
    retryCount = 0;
    response.consume(response.size());
    socket.close();
}

void HttpConnection::retry()
{
    close();
    ++retryCount;
    if (retryCount < MaxRetryCount)
        send(current);
    else
        done(true);
}

void HttpConnection::handleResolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpointIterator)
{
    std::cerr << __func__ << ' ' << err << '\n';

    if (!err) {
        state = Resolved;
        boost::asio::ip::tcp::endpoint endpoint = *endpointIterator;
        socket.async_connect(endpoint, boost::bind(&HttpConnection::handleConnect, this, boost::asio::placeholders::error, ++endpointIterator));
        return;
    }
    done(true);
}

void HttpConnection::handleConnect(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpointIterator)
{
    std::cerr << __func__ << ' ' << err << '\n';

    if (!err) {
        state = Connected;
        if (current) {
            sendRequest();
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
        }
        return;
    }
    if (endpointIterator != boost::asio::ip::tcp::resolver::iterator()) {
        close();
        boost::asio::ip::tcp::endpoint endpoint = *endpointIterator;
        socket.async_connect(endpoint, boost::bind(&HttpConnection::handleConnect, this, boost::asio::placeholders::error, ++endpointIterator));
        return;
    }
    done(true);
}

void HttpConnection::handleRead(const boost::system::error_code& err)
{
    std::cerr << __func__ << ' ' << state << ' ' << err << '\n';
    switch (state) {
    case Closed:
        break;
    case ReadStatusLine:
        readStatusLine(err);
        break;
    case ReadHead:
        readHead(err);
        break;
    case ReadContent:
        readContent(err);
        break;
    case ReadChunk:
        readChunk(err);
        break;
    case ReadTrailer:
        readTrailer(err);
        break;
    case CloseWait:
        close();
        break;
    default:
        close();
        done(true);
        break;
    }
}

void HttpConnection::handleWriteRequest(const boost::system::error_code& err)
{
    std::cerr << __func__ << ' ' << err <<  '\n';

    if (!err && current) {
        if (current->getRequestMessage().getVersion() < 10)
            state = ReadContent;
        else
            state = ReadStatusLine;
        return;
    }
    close();
    done(true);
}

void HttpConnection::readStatusLine(const boost::system::error_code& err)
{
    if (err && err != boost::asio::error::eof) {
        close();
        done(true);
        return;
    }
    const char* start = boost::asio::buffer_cast<const char*>(response.data());
    const char* end = start + response.size();
    const char* eol = std::find(start, end, '\n');
    if (eol == end) {
        if (err == boost::asio::error::eof) {
            retry();
            return;
        }
        line += std::string(start, response.size());
        response.consume(response.size());
        boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
        return;
    }
    ++eol;
    if (!line.empty()) {
        line += std::string(start, eol - start);
        response.consume(eol - start);
        start = line.c_str();
        eol = start + line.length();
    }
    // std::cerr << __func__ << ": " <<  std::string(start, eol - start) << '\n';
    const char* statusLine = current->getResponseMessage().parseStatusLine(start, eol);
    if (!statusLine) {
        close();
        done(true);
        return;
    }
    if (line.empty())
        response.consume(eol - start);
    line.clear();
    state = ReadHead;
    readHead(err);
}

void HttpConnection::readHead(const boost::system::error_code& err)
{
    if (err && err != boost::asio::error::eof) {
        close();
        done(true);
        return;
    }
    HttpResponseMessage& responseMessage = current->getResponseMessage();
    for (;;) {
        const char* start = boost::asio::buffer_cast<const char*>(response.data());
        const char* end = start + response.size();
        const char* eol = std::find(start, end, '\n');
        if (eol == end) {
            if (err == boost::asio::error::eof) {
                close();
                done(true);
                return;
            }
            line += std::string(start, response.size());
            response.consume(response.size());
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
            return;
        }
        ++eol;
        if (!line.empty()) {
            line += std::string(start, eol - start);
            response.consume(eol - start);
            start = line.c_str();
            eol = start + line.length();
        }
        // std::cerr << __func__ << ": " <<  std::string(start, eol - start) << '\n';
        const char* header = parseCRLF(start, eol);
        if (*header == '\n') {
            if (line.empty())
                response.consume(eol - start);
            else
                line.clear();
            break;
        }
        header = responseMessage.parseHeader(start, eol);
        if (!header) {
            close();
            done(true);
            return;
        }
        if (line.empty())
            response.consume(eol - start);
        line.clear();
    }

    // TODO: handle every status code
    switch (responseMessage.getStatus()) {
    case 304:   // Not Modified
        current->constructResponseFromCache();
        done(false);
        if (!err)
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
        return;
    default:
        break;
    }
    if (responseMessage.isChunked()) {
        chunkCRLF = 0;
        octetCount = contentLength = 0;
        state = ReadChunk;
        readChunk(err);
    } else {
        octetCount = 0;
        contentLength = responseMessage.getContentLength();
        state = ReadContent;
        readContent(err);
    }
}

void HttpConnection::readContent(const boost::system::error_code& err)
{
    if (!err || err == boost::asio::error::eof) {
        std::fstream& content = current->getContent();
        if (!content.is_open()) {
            done(true);
            return;
        }
        bool completed = false;
        if (0 < response.size()) {
            if (!contentLength) {
                octetCount += response.size();
                content << &response;
            } else {
                unsigned long long length = std::min(static_cast<unsigned long long>(response.size()), contentLength - octetCount);
                content.write(boost::asio::buffer_cast<const char*>(response.data()), length);
                response.consume(length);
                octetCount += length;
                if (contentLength <= octetCount)
                    completed = true;
            }
        }
        if (!err && !completed) {
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
            return;
        }
        content.flush();
    }
    if (err == boost::asio::error::eof) {
        close();
        if (contentLength < octetCount) {
            contentLength = octetCount;
            // TODO: set Content-length:
        }
        done(octetCount < contentLength);
        return;
    }
    done(err);
    state = CloseWait;
    boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
}

void HttpConnection::readChunk(const boost::system::error_code& err)
{
    if (!err || err == boost::asio::error::eof) {
        std::fstream& content = current->getContent();
        if (!content.is_open()) {
            done(true);
            return;
        }
        bool completed = false;
        while (0 < response.size()) {
            if (line[line.length() - 1] != '\n') {
                const char* chunk = boost::asio::buffer_cast<const char*>(response.data());
                const char* chunkEnd = chunk + response.size();
                auto p = std::find(chunk, chunkEnd, '\n');
                if (p == chunkEnd) {
                    line += std::string(chunk, response.size());
                    response.consume(response.size());
                    // TODO: check error and length
                } else {
                    line += std::string(chunk, p - chunk + 1);
                    response.consume(p - chunk + 1);
                    parseHexDigits(line.c_str(), line.c_str() + line.length(), chunkLength);
                    contentLength += chunkLength;
                    if (chunkLength == 0) {
                        if (line[0] != '0') {
                            done(true);
                            close();
                            return;
                        }
                        completed = true;
                        content.flush();
                        chunkCRLF = 1;
                    }
                }
            }
            if (!completed) {
                if (0 < response.size() && octetCount < contentLength) {
                    unsigned long long length = std::min(static_cast<unsigned long long>(response.size()), contentLength - octetCount);
                    content.write(boost::asio::buffer_cast<const char*>(response.data()), length);
                    response.consume(length);
                    octetCount += length;
                }
                while (0 < response.size() && contentLength <= octetCount) {
                    int c = response.sbumpc();
                    if (c == '\n') {
                        chunkCRLF = 0;
                        line.clear();
                        break;
                    } else if (c == '\r' && chunkCRLF == 0)
                        ++chunkCRLF;
                    else {
                        done(true);
                        close();
                        return;
                    }
                }
            } else {
                // TODO: process trailer
                state = ReadTrailer;
                readTrailer(err);
                return;
            }
        }
        if (!err) {
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
            return;
        }
    }
    assert(err);
    done(err);
    close();
}

void HttpConnection::readTrailer(const boost::system::error_code& err)
{
    if (!err || err == boost::asio::error::eof) {
        while (0 < response.size()) {
            int c = response.sbumpc();
            if (c == '\n') {
                if (++chunkCRLF == 2) {
                    // TODO: set Content-length:
                    done(false);
                    if (err)
                        close();
                    else {
                        state = CloseWait;
                        boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
                    }
                    return;
                }
            } else if (c != '\r')
                chunkCRLF = 0;
        }
        if (!err) {
            boost::asio::async_read(socket, response, boost::asio::transfer_at_least(1), boost::bind(&HttpConnection::handleRead, this, boost::asio::placeholders::error));
            return;
        }
    }
    assert(err);
    done(err);
    close();
}

void HttpConnection::send(HttpRequest* request)
{
    if (current && current != request) {
        requests.push_back(request);
        return;
    }
    current = request;

    if (socket.is_open()) {
        sendRequest();
        return;
    }

    state = Resolving;
    boost::asio::ip::tcp::resolver::query query(hostname, port);
    HttpConnectionManager::getInstance().resolve(query,
                                                 boost::bind(&HttpConnection::handleResolve, this,
                                                             boost::asio::placeholders::error,
                                                             boost::asio::placeholders::iterator));
}

void HttpConnection::abort(HttpRequest* request)
{
    if (current != request) {
        requests.remove(request);
        return;
    }
    close();
    done(true);
}

HttpConnection* HttpConnectionManager::getConnection(const std::string& hostname, const std::string& port)
{
    for (auto i = connections.begin(); i != connections.end(); ++i) {
        if ((*i)->hostname == hostname && (*i)->port == port)
            return *i;
    }
    HttpConnection* c = new(std::nothrow) HttpConnection(hostname, port);
    if (c)
        connections.push_back(c);
    return c;
}

void HttpConnectionManager::send(HttpRequest* request)
{
    URI uri(request->getRequestMessage().getURL());
    std::string hostname = uri.getHostname();
    std::string port = uri.getPort();
    HttpConnection* conn = getConnection(hostname, port);
    if (!conn)
        return;
    conn->send(request);
}

void HttpConnectionManager::abort(HttpRequest* request)
{
    URI uri(request->getRequestMessage().getURL());
    std::string hostname = uri.getHostname();
    std::string port = uri.getPort();
    HttpConnection* conn = getConnection(hostname, port);
    if (!conn)
        return;
    conn->abort(request);
}

}}}}  // org::w3c::dom::bootstrap
