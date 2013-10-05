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

#include "http/HTTPConnection.h"

#include <unistd.h>

#include <iostream>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "Test.util.h"
#include "utf.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

int test(std::u16string urlString)
{
    URL url(urlString);
    if (url.isEmpty())
        return 1;

    HttpRequestPtr request(std::make_shared<HttpRequest>());
    request->open(u"get", url);

    std::cerr << request->getRequestMessage().toString();

    request->send();

    while (request->getReadyState() != HttpRequest::DONE) {
        HttpConnectionManager::getIOService().run_one();
        HttpConnectionManager::getInstance().poll();
    }
    if (request->getError())
        return 1;

    std::cerr << request->getResponseMessage().toString() << "----\n";
    std::cerr << request->getResponseMessage().getContentCharset() << "----\n";
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::close_handle);
    while (stream) {
        char c = stream.get();
        if (stream.good())
            std::cout << c;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    initLogLevel(&argc, argv, 3);

    int result = 0;
    if (2 <= argc) {
        for (int i = 1; i < argc; ++i)
            result += test(utfconv(argv[1]));
    } else {
        result += test(u"http://www.esrille.com/index.html");
        sleep(3);
        result += test(u"http://www.esrille.com/index.html");
    }
    return result;
}
