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

#include "HTMLButtonElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLButtonElementImp::eval()
{
    HTMLElementImp::eval();

    if (!getDisabled())
        setTabIndex(0);
}

bool HTMLButtonElementImp::getAutofocus()
{
    // TODO: implement me!
    return 0;
}

void HTMLButtonElementImp::setAutofocus(bool autofocus)
{
    // TODO: implement me!
}

bool HTMLButtonElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLButtonElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLButtonElementImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLButtonElementImp::getFormAction()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setFormAction(std::u16string formAction)
{
    // TODO: implement me!
}

std::u16string HTMLButtonElementImp::getFormEnctype()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setFormEnctype(std::u16string formEnctype)
{
    // TODO: implement me!
}

std::u16string HTMLButtonElementImp::getFormMethod()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setFormMethod(std::u16string formMethod)
{
    // TODO: implement me!
}

std::u16string HTMLButtonElementImp::getFormNoValidate()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setFormNoValidate(std::u16string formNoValidate)
{
    // TODO: implement me!
}

std::u16string HTMLButtonElementImp::getFormTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setFormTarget(std::u16string formTarget)
{
    // TODO: implement me!
}

std::u16string HTMLButtonElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLButtonElementImp::setName(std::u16string name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLButtonElementImp::getType()
{
    // TODO: see http://www.whatwg.org/specs/web-apps/current-work/multipage/common-microsyntaxes.html#enumerated-attribute
    return getAttribute(u"type");
}

void HTMLButtonElementImp::setType(std::u16string type)
{
    setAttribute(u"type", type);
}

std::u16string HTMLButtonElementImp::getValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLButtonElementImp::setValue(std::u16string value)
{
    // TODO: implement me!
}

bool HTMLButtonElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLButtonElementImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLButtonElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLButtonElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLButtonElementImp::setCustomValidity(std::u16string error)
{
    // TODO: implement me!
}

NodeList HTMLButtonElementImp::getLabels()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}
}
}
}
