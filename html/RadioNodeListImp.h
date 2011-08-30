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

#ifndef ORG_W3C_DOM_BOOTSTRAP_RADIONODELISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_RADIONODELISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/RadioNodeList.h>
#include "NodeListImp.h"

#include <org/w3c/dom/NodeList.h>

#include <deque>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

// Note even though RadioNodeList inherits NodeList, there doesn't seem to
// be a practical inheritance relationship between the two interfaces.
// Thus, we don't expand RadioNodeListImp from NodeListImp.
class RadioNodeListImp : public ObjectMixin<RadioNodeListImp>
{
    std::deque<Node> list;

public:
    RadioNodeListImp();
    ~RadioNodeListImp();

    void addItem(Node item);

    // NodeList
    Node item(unsigned int index);
    unsigned int getLength();
    // RadioNodeList
    std::u16string getValue();
    void setValue(std::u16string value);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::RadioNodeList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::RadioNodeList::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_RADIONODELISTIMP_H_INCLUDED
