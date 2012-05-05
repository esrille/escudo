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

#include "url/URI.h"
#include "utf.h"

#include <assert.h>

#include <iostream>

using namespace org::w3c::dom::bootstrap;

void test(const URL& url, const std::u16string& reference = u"", const std::u16string& result = u"")
{
    if (reference.length()) {
        URL r(url, reference);
        test(r);
        if (result.length())
            assert(static_cast<std::u16string>(r) == result);
        return;
    }

    URI uri(url);
    std::cout << "uri: '" << static_cast<std::string>(uri) << "'\n";
    std::cout << "protocol: " << url.getProtocol() << '\n';
    std::cout << "host: " << url.getHost() << '\n';
    std::cout << "hostname: " << url.getHostname() << '\n';
    std::cout << "port: " << url.getPort() << '\n';
    std::cout << "pathname: " << url.getPathname() << '\n';
    std::cout << "search: " << url.getSearch() << '\n';
    std::cout << "hash: " << url.getHash() << '\n';
    std::cout << '\n';
    std::cout << "protocol: " << uri.getProtocol() << '\n';
    std::cout << "host: " << uri.getHost() << '\n';
    std::cout << "hostname: " << uri.getHostname() << '\n';
    std::cout << "port: " << uri.getPort() << '\n';
    std::cout << "pathname: " << uri.getPathname() << '\n';
    std::cout << "search: " << uri.getSearch() << '\n';
    std::cout << "hash: " << uri.getHash() << '\n';
    std::cout << "\n\n";
}

int main()
{
    URL a(u"http://www.esrille.com");
    URL b(u"http://192.168.0.1");
    URL c(u"http://192.168.0.1:8080");
    URL d(u"http://192.168.0.1:8080/index.html");
    URL e(u"http://192.168.0.1:8080/esrille/index.html");
    URL f(u"http://192.168.0.1:8080/esrille/index.html?lang=ja");
    URL g(u"http://192.168.0.1:8080/esrille/index.html?lang=ja#html5");
    URL h(u"   http://192.168.0.1:8080/esrille/index.html?lang=ja#html5   ");
    URL i(u"http://こんにちは.jp/esrille/index.html?lang=ja#世界world");
    URL j(u"HTTP://総務省.JP/");
    URL k(u"HTTP://総務省.JP?lang=ja");
    URL l(u"http://[2001:0db8:bd05:01d2:288a:1fc0:0001:10ee]");
    URL m(u"http://[2001:db8::9abc]");
    URL n(u"http://[::ffff]");
    URL o(u"http://[ffff::]");
    URL p(u"http://[::ffff:192.0.2.1]");

    test(a);
    test(b);
    test(c);
    test(d);
    test(e);
    test(f);
    test(g);
    test(h);
    test(i);
    test(j);
    test(k);
    test(l);
    test(m);
    test(n);
    test(o);
    test(p);

    URL r1(e, u"en.html");
    test(r1);
    URL r2(e, u"?lang=ja");
    test(r2);
    URL r3(e, u"#html5");
    test(r3);
    URL r4(e, u"/en.html");
    test(r4);
    URL r5(e, u"//総務省.JP/");
    test(r5);
    URL r6(e, u"HTTP://総務省.JP/");
    test(r6);
    URL r7(h, u"?lang=en#html4");
    test(r7);
    URL r8(h, u"en.html?lang=en#html4");
    test(r8);

    // cf. RFC 3986
    URL base(u"http://a/b/c/d;p?q");
    test(base);
    test(base, u"g", u"http://a/b/c/g");
    test(base, u"./g", u"http://a/b/c/g");
    test(base, u"g/", u"http://a/b/c/g/");
    test(base, u"/g", u"http://a/g");
    test(base, u"//g", u"http://g/");
    test(base, u"?y", u"http://a/b/c/d;p?y");
    test(base, u"g?y", u"http://a/b/c/g?y");
    test(base, u"#s", u"http://a/b/c/d;p?q#s");
    test(base, u"g#s", u"http://a/b/c/g#s");
    test(base, u"g?y#s", u"http://a/b/c/g?y#s");
    test(base, u";x", u"http://a/b/c/;x");
    test(base, u"g;x", u"http://a/b/c/g;x");
    test(base, u"g;x?y#s", u"http://a/b/c/g;x?y#s");
    test(base, u"", u"http://a/b/c/d;p?q");
    test(base, u".", u"http://a/b/c/");
    test(base, u"./", u"http://a/b/c/");
    test(base, u"..", u"http://a/b/");
    test(base, u"../", u"http://a/b/");
    test(base, u"../g", u"http://a/b/g");
    test(base, u"../..", u"http://a/");
    test(base, u"../../", u"http://a/");
    test(base, u"../../g", u"http://a/g");

    test(base, u"../../../g", u"http://a/g");
    test(base, u"../../../../g", u"http://a/g");

    test(base, u"/./g", u"http://a/g");
    test(base, u"/../g", u"http://a/g");
    test(base, u"g.", u"http://a/b/c/g.");
    test(base, u".g", u"http://a/b/c/.g");
    test(base, u"g..", u"http://a/b/c/g..");
    test(base, u"..g", u"http://a/b/c/..g");

    test(base, u"./../g", u"http://a/b/g");
    test(base, u"./g/.", u"http://a/b/c/g/");
    test(base, u"g/./h", u"http://a/b/c/g/h");
    test(base, u"g/../h", u"http://a/b/c/h");
    test(base, u"g;x=1/./y", u"http://a/b/c/g;x=1/y");
    test(base, u"g;x=1/../y", u"http://a/b/c/y");

    test(base, u"g?y/./x", u"http://a/b/c/g?y/./x");
    test(base, u"g?y/../x", u"http://a/b/c/g?y/../x");
    test(base, u"g#s/./x", u"http://a/b/c/g#s/./x");
    test(base, u"g#s/../x", u"http://a/b/c/g#s/../x");

    test(base, u"http:g", u"http://a/b/c/g");  // for backward compatibility

    // cf. RFC 1738; File URL - file://<host>/<path>
    URL f1(u"file://localhost/etc/hosts");
    URL f2(u"file:///etc/hosts");
    URL f3(u"file:///home/user/dir/file");
    test(f1);
    test(f2);
    test(f3);
    test(f3, u"another", u"file:///home/user/dir/another");
    test(f3, u"../another", u"file:///home/user/another");

    // About URI
    URL a1(u"about:");
    URL a2(u"about:blank");
    test(a1);
    test(a2);
    test(a2, u"plugins", u"about:plugins");
}
