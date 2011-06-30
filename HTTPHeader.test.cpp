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

#include "http/HTTPHeader.h"
#include "http/HTTPResponseMessage.h"
#include "http/HTTPRequestMessage.h"

#include <string.h>
#include <iostream>

using namespace org::w3c::dom::bootstrap;

const char* message =
    "Host: www.w3.org\r\n"
    "\r\n";

const char* response1 =
    "HTTP/1.1 206 Partial content\r\n"
    "Date: Wed, 15 Nov 1995 06:25:24 GMT\r\n"
    "Last-Modified: Wed, 15 Nov 1995 04:58:08 GMT\r\n"  // RFC 1123: Wkd, 00 Mon 0000 00:00:00 GMT
    "Content-Range: bytes 21010-47021/47022\r\n"
    "Content-Length: 26012\r\n"
    "Content-Type: text/html\r\n"
    "\r\n";

const char* response2 =
    "HTTP/1.1 206 Partial content\r\n"
    "Date: Wednesday, 15-Nov-95 06:25:24 GMT\r\n"
    "Last-Modified: Wednesday, 15-Nov-95 04:58:08 GMT\r\n"  // RFC 850:  Weekday, 00-Mon-00 00:00:00 GMT
    "Content-Range: bytes 21010-47021/47022\r\n"
    "Content-Length: 26012\r\n"
    "Content-Type: text/html; charset=ISO-8859-1\r\n"
    "\r\n";

const char* response3 =
    "HTTP/1.1 206 Partial content\r\n"
    "Date: Wed Nov 15 06:25:24 1995 GMT\r\n"
    "Last-Modified: Wed Nov 15 04:58:08 1995 GMT\r\n"  // asctime: Wkd Mon 00 00:00:00 0000 GMT
    "Content-Range: bytes 21010-47021/47022\r\n"
    "Content-Length: 26012\r\n"
    "Content-Type: text/html; charset=   \"ISO-8859-1\"  \r\n"
    "Cache-Control: no-store\r\n"
    "Pragma: no-cache\r\n"
    "\r\n";

const char* response4 =
    "HTTP/1.1 206 Partial content\r\n"
    "Date: Wednesday, 15-Nov-11   06:25:24 GMT\r\n"
    "Last-Modified: Wednesday, 15-Nov-11    04:58:08 GMT\r\n"  // RFC 850:  Weekday, 00-Mon-00 00:00:00 GMT
    "Content-Range: bytes 21010-47021/47022\r\n"
    "Content-Length: 26012\r\n"
    "Content-Type: text/html;charset='ISO-8859-1'\r\n"
    "Cache-Control: no-cache\r\n"
    "\r\n";

const char* response5 =
    "HTTP/1.1 206 Partial content\r\n"
    "Date: Wed, 15 Nov 2011    06:25:24 GMT\r\n"
    "Last-Modified: Wed, 15 Nov 2011    04:58:08 GMT\r\n"  // RFC 1123: Wkd, 00 Mon 0000 00:00:00 GMT
    "Content-Range: bytes 21010-47021/47022\r\n"
    "Content-Length: 18446744073709551615\r\n"  // ULLONG_MAX
    "Content-Type: text/html;charset=ISO-8859-1, text/html\r\n"
    "Cache-Control: no-store, no-cache\r\n"
    "\r\n";

int testHttpHeaderList()
{
    HttpHeaderList list;
    list.parse(message, message + strlen(message));
    list.set("Connection", "close");
    std::cout << list.toString().c_str() << '\n';

    list.erase("host");
    std::cout << list.toString().c_str() << '\n';

    std::string value;
    if (list.get("Connection", value))
        std::cout << "'" << value << "'\n";

    list.set("Allow", "GET");
    std::cout << list.toString().c_str() << '\n';
    list.set("Allow", "HEAD");
    std::cout << list.toString().c_str() << '\n';
    list.set("allow", "SET", true);
    std::cout << list.toString().c_str() << '\n';

    return 0;
}

int testHttpResponseMessage(const char* response)
{
    HttpResponseMessage res;
    res.parse(response, response + strlen(response));
    std::cout << res.toString() << ' ' << res.getContentLength() << ' ' << res.getDateValue() << ' ' <<
                 res.isNoCache() << ' ' << res.isNoStore() << ' ' <<
                 res.getContentType() << ' ' << res.getContentCharset() << "\n\n";
    return 0;
}

int testHttpRequestMessage()
{
    HttpRequestMessage req9;
    req9.setVersion(9);
    req9.open("GET", u"http://www.esrille.com/");
    std::cout << req9.toString() << '\n';

    HttpRequestMessage req10;
    req10.setVersion(10);
    req10.open("GET", u"http://www.esrille.com/");
    std::cout << req10.toString() << '\n';

    HttpRequestMessage req11;
    req11.setVersion(11);
    req11.open("GET", u"http://www.esrille.com/");
    std::cout << req11.toString() << '\n';
    return 0;
}

int main(int argc, char* argv[])
{
    testHttpHeaderList();
    testHttpRequestMessage();
    testHttpResponseMessage(response1);
    testHttpResponseMessage(response2);
    testHttpResponseMessage(response3);
    testHttpResponseMessage(response4);
    testHttpResponseMessage(response5);
}