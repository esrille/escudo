/*
 * Copyright 2011, 2013 Esrille Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <math.h>

#include "utf.h"

int check(bool cond)
{
    return cond ? EXIT_SUCCESS : EXIT_FAILURE;
}

class X : public Imp
{
    const char* msg;
public:
    X (const char* msg) : msg(msg) {}
    ~X() {
        std::cout << msg << '\n';
    }
};

std::weak_ptr<Imp> w;

Any testRet()
{
    Object x(std::make_shared<X>("hello"));
    w = x.self();
    Any y(x);
    return y;
}

int testAssign()
{
    Any z;
    z = testRet();
    std::cout << "object: " << z.isObject() << '\n';
    return 0;
}

int main()
{
    int result = 0;

    Any x;
    std::cout << "undefined: " << x.isUndefined() << '\n';
    result += check(x.isUndefined());
    x = nullptr;
    std::cout << "null: " << x.isNull() << '\n';
    result += check(x.isNull());

    Any e(nullptr);
    std::cout << "null: " << e.isNull() << '\n';
    result += check(e.isNull());

    Object o;
    x = o;
    std::cout << "!object: " << !x.isObject() << '\n';
    std::cout << "null: " << x.isNull() << '\n';
    result += check(!x.isObject());
    result += check(x.isNull());

    Any g(0);
    std::cout << "g: " << (g.getType() == Any::Int32) << '\n';

    x = 3.14;
    std::cout << static_cast<int>(x) << '\n';
    std::cout << static_cast<double>(x) << '\n';

    x = (char) 2;
    std::cout << static_cast<int>(x) << '\n';
    std::cout << static_cast<double>(x) << '\n';

    x = -1;
    std::cout << static_cast<int>(x) << '\n';
    std::cout << static_cast<unsigned int>(x) << '\n';
    std::cout << static_cast<double>(x) << '\n';

    x = u"abc";
    std::cout << static_cast<const std::u16string&>(x) << '\n';

    Any y(u"xyz");
    std::cout << static_cast<const std::u16string&>(y) << '\n';

    Any n(u"3.14");
    std::cout << n.toNumber() << ' ' << static_cast<int>(n) << '\n';
    n = u"0xff";
    std::cout << n.toNumber() << ' ' << static_cast<int>(n) << '\n';
    n = 0x7f;
    std::cout << n.toString() << '\n';
    n = true;
    std::cout << n.toString() << '\n';
    n = u"";
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = u"a";
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = +0.0;
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = -0.0;
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = 3.0;
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = NAN;
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';
    n = (double) NAN;
    std::cout << n.toBoolean() << ' ' << static_cast<bool>(n) << '\n';

    result += testAssign();

    return result;
}
