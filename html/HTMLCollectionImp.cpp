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
    map.clear();
}

void HTMLCollectionImp::addItem(const std::u16string& name, Element element)
{
    map.insert(std::pair<const std::u16string, Element>(name, element));
}

unsigned int HTMLCollectionImp::getLength()
{
    return map.size();
}

Element HTMLCollectionImp::item(unsigned int index)
{
    if (getLength() <= index)
        return 0;
    auto it = map.begin();
    while (0 < index--)
        ++it;
    return it->second;
}

Object HTMLCollectionImp::namedItem(std::u16string name)
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
