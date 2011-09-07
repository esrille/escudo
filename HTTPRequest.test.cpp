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

#include "http/HTTPConnection.h"

#include <unistd.h>

#include <iostream>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "utf.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

int test(std::u16string urlString)
{
    URL url(urlString);
    if (url.isEmpty())
        return 1;

    HttpRequest request;
    request.open(u"get", url);

    std::cerr << request.getRequestMessage().toString();

    request.send();

    while (request.getReadyState() != HttpRequest::DONE)
        HttpConnectionManager::getIOService().run_one();
    if (request.getErrorFlag())
        return 1;

    std::cerr << request.getResponseMessage().toString() << "----\n";
    std::cerr << request.getResponseMessage().getContentCharset() << "----\n";
#if 104400 <= BOOST_VERSION
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request.getContentDescriptor(), boost::iostreams::never_close_handle);
#else
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request.getContentDescriptor(), false);
#endif
    stream.seekg(0, std::ios::beg);
    while (stream) {
        char c = stream.get();
        if (stream.good())
            std::cout << c;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int result = 0;
    if (2 <= argc)
        return test(utfconv(argv[1]));

    result += test(u"http://www.esrille.com/index.html");
    sleep(3);
    result += test(u"http://www.esrille.com/index.html");

    return result;
}
