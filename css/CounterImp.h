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

#ifndef ORG_W3C_DOM_BOOTSTRAP_COUNTERIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_COUNTERIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/Counter.h>

#include <deque>
#include <boost/intrusive_ptr.hpp>

#include "css/CSSPropertyValueImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class CounterImp : public ObjectMixin<CounterImp>
{
    std::u16string identifier;
    std::u16string separator;
    CSSListStyleTypeValueImp listStyle;

    std::deque<int> counters;

public:
    CounterImp(const std::u16string& id) :
        identifier(id)
    {
    }

    void nest(int number);
    void reset(int number);
    void increment(int number);
    bool restore();

    std::u16string eval(unsigned type);
    std::u16string eval(const std::u16string& separator, unsigned type);

    // Counter
    std::u16string getIdentifier();
    std::u16string getListStyle();
    std::u16string getSeparator();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::Counter::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::Counter::getMetaData();
    }
};

typedef boost::intrusive_ptr<CounterImp> CounterImpPtr;

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_COUNTERIMP_H_INCLUDED
