/*
 * Copyright 2013 Esrille Inc.
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

#include "HTMLOptionsCollectionImp.h"

#include "HTMLSelectElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLOptionsCollectionImp::HTMLOptionsCollectionImp(const HTMLSelectElementPtr& select) :
    select(select)
{
}

Element HTMLOptionsCollectionImp::item(unsigned int index)
{
    if (auto imp = select.lock())
        return imp->item(index);
    return nullptr;
}

unsigned int HTMLOptionsCollectionImp::getLength()
{
    if (auto imp = select.lock())
        return imp->getLength();
    return 0;
}

void HTMLOptionsCollectionImp::setLength(unsigned int length)
{
    if (auto imp = select.lock())
        return imp->setLength(length);
}

Object HTMLOptionsCollectionImp::namedItem(const std::u16string& name)
{
    if (auto imp = select.lock())
        return imp->namedItem(name);
    return nullptr;
}

void HTMLOptionsCollectionImp::setElement(unsigned int index, html::HTMLOptionElement option)
{
    if (auto imp = select.lock())
        imp->setElement(index, option);
}

void HTMLOptionsCollectionImp::add(Any element)
{
    if (auto imp = select.lock())
        imp->add(element);
}

void HTMLOptionsCollectionImp::add(Any element, Any before)
{
    if (auto imp = select.lock())
        imp->add(element, before);
}

void HTMLOptionsCollectionImp::remove(int index)
{
    if (auto imp = select.lock())
        imp->remove(index);
}

int HTMLOptionsCollectionImp::getSelectedIndex()
{
    if (auto imp = select.lock())
        return imp->getSelectedIndex();
    return -1;
}

void HTMLOptionsCollectionImp::setSelectedIndex(int selectedIndex)
{
    if (auto imp = select.lock())
        imp->setSelectedIndex(selectedIndex);
}

}
}
}
}
