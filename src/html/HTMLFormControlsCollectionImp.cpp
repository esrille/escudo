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

#include "HTMLFormControlsCollectionImp.h"

#include <org/w3c/dom/html/RadioNodeList.h>

#include "ElementImp.h"
#include "HTMLFormElementImp.h"
#include "RadioNodeListImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

namespace {

bool isListedElement(const std::u16string& tag)
{
    if (tag == u"button" ||
        tag == u"fieldset" ||
        tag == u"input" ||
        tag == u"keygen" ||
        tag == u"label" ||
        tag == u"object" ||
        tag == u"output" ||
        tag == u"select" ||
        tag == u"textarea")
        return true;
    return false;
}

}

HTMLFormControlsCollectionImp::HTMLFormControlsCollectionImp(HTMLFormElementImp* form) :
    length(0)
{
    ElementImp* i = form;
    while (i = i->getNextElement(form)) {
        std::u16string tag = i->getTagName();
        if (isListedElement(tag)) {
            std::u16string name;
            Nullable<std::u16string> a = i->getAttribute(u"name");
            if (a.hasValue())
                name = a.value();
            if (name.empty()) {
                a = i->getAttribute(u"id");
                if (a.hasValue())
                    name = a.value();
            }
            Object found = namedItem(name);
            if (!found) {
                map.insert(std::pair<const std::u16string, Object>(name, i));
                ++length;
                continue;
            }
            if (html::RadioNodeList::hasInstance(found)) {
                html::RadioNodeList r = interface_cast<html::RadioNodeList>(found);
                if (RadioNodeListImp* list = dynamic_cast<RadioNodeListImp*>(r.self())) {
                    list->addItem(i);
                    ++length;
                }
                continue;
            }
            RadioNodeListImp* list = new(std::nothrow) RadioNodeListImp;
            if (list) {
                list->addItem(interface_cast<Element>(found));
                list->addItem(i);
                ++length;
                map.erase(name);
                map.insert(std::pair<const std::u16string, Object>(name, list));
            }
        }
    }
}

HTMLFormControlsCollectionImp::~HTMLFormControlsCollectionImp()
{
    map.clear();
}

unsigned int HTMLFormControlsCollectionImp::getLength()
{
    return length;
}

Element HTMLFormControlsCollectionImp::item(unsigned int index)
{
    if (getLength() <= index)
        return 0;
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (Element::hasInstance(it->second)) {
            if (index == 0)
                return interface_cast<Element>(it->second);
            --index;
        } else {
            html::RadioNodeList r = interface_cast<html::RadioNodeList>(it->second);
            unsigned int length = r.getLength();
            if (index < length)
                return interface_cast<Element>(r.item(index));
            index -= length;
        }
    }
    return 0;
}

Object HTMLFormControlsCollectionImp::namedItem(std::u16string name)
{
    auto it = map.find(name);
    if (it == map.end())
        return 0;
    else
        return it->second;
}

}
}
}
}
