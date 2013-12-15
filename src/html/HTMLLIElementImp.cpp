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

#include "HTMLLIElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

int HTMLLIElementImp::getValue()
{
    return getAttributeAsInteger(u"value", 0);
}

void HTMLLIElementImp::setValue(int value)
{
    setAttributeAsInteger(u"value", value);
}

std::u16string HTMLLIElementImp::getType()
{
    return getAttribute(u"type");
}

void HTMLLIElementImp::setType(const std::u16string& type)
{
    setAttribute(u"type", type);
}

}
}
}
}
