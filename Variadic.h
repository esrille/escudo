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

#ifndef ES_VARIADIC_H
#define ES_VARIADIC_H

#include <initializer_list>

class Any;

template <typename T>
class Variadic
{
    std::initializer_list<T> list;
    const Any* variadic;
    size_t length;

public:
    Variadic() :
        variadic(0),
        length(0) {
    }

    Variadic(const Any* variadic, size_t length) :
        variadic(length ? variadic : 0),
        length(length) {
    }

    Variadic(std::initializer_list<T> list) :
        list(list),
        variadic(0),
        length(list.size())
    {
    }

    T operator[](int index) const
    {
        if (!variadic)
            return *(list.begin() + index);
        return variadic[index];
    }

    size_t size() const
    {
        return length;
    }
};

#endif  // ES_VARIADIC_H
