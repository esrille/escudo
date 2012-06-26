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

#include "HTMLKeygenElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

bool HTMLKeygenElementImp::getAutofocus()
{
    // TODO: implement me!
    return 0;
}

void HTMLKeygenElementImp::setAutofocus(bool autofocus)
{
    // TODO: implement me!
}

std::u16string HTMLKeygenElementImp::getChallenge()
{
    // TODO: implement me!
    return u"";
}

void HTMLKeygenElementImp::setChallenge(std::u16string challenge)
{
    // TODO: implement me!
}

bool HTMLKeygenElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLKeygenElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLKeygenElementImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLKeygenElementImp::getKeytype()
{
    // TODO: implement me!
    return u"";
}

void HTMLKeygenElementImp::setKeytype(std::u16string keytype)
{
    // TODO: implement me!
}

std::u16string HTMLKeygenElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLKeygenElementImp::setName(std::u16string name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLKeygenElementImp::getType()
{
    return u"keygen";
}

bool HTMLKeygenElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLKeygenElementImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLKeygenElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLKeygenElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLKeygenElementImp::setCustomValidity(std::u16string error)
{
    // TODO: implement me!
}

NodeList HTMLKeygenElementImp::getLabels()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}
}
}
}
