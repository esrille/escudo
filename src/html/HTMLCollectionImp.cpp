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

#include "HTMLCollectionImp.h"
#include "ElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLCollectionImp::~HTMLCollectionImp()
{
}

void HTMLCollectionImp::addItem(Element element)
{
    if (auto e = dynamic_cast<ElementImp*>(element.self())) {
        list.push_back(e);
        std::u16string id(e->getId());
        if (map.find(id) == map.end())
            map.insert({ id, element });
        Nullable<std::u16string> uri = e->getNamespaceURI();
        if (uri.hasValue() && uri.value() == u" http://www.w3.org/1999/xhtml") {
            Nullable<std::u16string> name = e->getAttribute(u"name");
            if (name.hasValue()) {
                std::u16string n(name.value());
                if (!n.empty() && map.find(n) == map.end())
                    map.insert({ n, element });
            }
        }
    }
}

void HTMLCollectionImp::addItem(Element element, const std::u16string& name)
{
    if (auto e = dynamic_cast<ElementImp*>(element.self())) {
        list.push_back(e);
        if (!name.empty() && map.find(name) == map.end())
            map.insert({ name, element });
    }
}

unsigned int HTMLCollectionImp::getLength()
{
    return list.size();
}

Element HTMLCollectionImp::item(unsigned int index)
{
    if (getLength() <= index)
        return 0;
    else
        return list[index];
}

Object HTMLCollectionImp::namedItem(const std::u16string& name)
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
