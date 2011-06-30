/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "AttrImp.h"

#include <Object.h>

#include <new>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Attr
Nullable<std::u16string> AttrImp::getNamespaceURI()
{
    return namespaceURI;
}

Nullable<std::u16string> AttrImp::getPrefix()
{
    return prefix;
}

std::u16string AttrImp::getLocalName()
{
    return localName;
}

std::u16string AttrImp::getName()
{
    if (prefix.hasValue())
        return prefix.value() + u":" + localName;
    return localName;
}

std::u16string AttrImp::getValue()
{
    return value;
}

void AttrImp::setValue(std::u16string value)
{
    this->value = value;
}

AttrImp::AttrImp(Nullable<std::u16string> namespaceURI, Nullable<std::u16string> prefix, std::u16string localName, std::u16string value) :
    namespaceURI(namespaceURI),
    prefix(prefix),
    localName(localName),
    value(value)
{
}

}}}}  // org::w3c::dom::bootstrap
