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

#include "HTMLFormControlImp.h"

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLFormControlImp::handleMutation(events::MutationEvent mutation)
{
    switch (Intern(mutation.getAttrName().c_str())) {
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

bool HTMLFormControlImp::getAutofocus()
{
    return getAttributeAsBoolean(u"autofocus");
}

void HTMLFormControlImp::setAutofocus(bool autofocus)
{
    setAttributeAsBoolean(u"autofocus", autofocus);
}

bool HTMLFormControlImp::getDisabled()
{
    return getAttributeAsBoolean(u"disabled");
}

void HTMLFormControlImp::setDisabled(bool disabled)
{
    setAttributeAsBoolean(u"disabled", disabled);
}

html::HTMLFormElement HTMLFormControlImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLFormControlImp::getFormAction()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormControlImp::setFormAction(const std::u16string& formAction)
{
    // TODO: implement me!
}

std::u16string HTMLFormControlImp::getFormEnctype()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormControlImp::setFormEnctype(const std::u16string& formEnctype)
{
    // TODO: implement me!
}

std::u16string HTMLFormControlImp::getFormMethod()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormControlImp::setFormMethod(const std::u16string& formMethod)
{
    // TODO: implement me!
}

bool HTMLFormControlImp::getFormNoValidate()
{
    // TODO: implement me!
    return 0;
}

void HTMLFormControlImp::setFormNoValidate(bool formNoValidate)
{
    // TODO: implement me!
}

std::u16string HTMLFormControlImp::getFormTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormControlImp::setFormTarget(const std::u16string& formTarget)
{
    // TODO: implement me!
}

std::u16string HTMLFormControlImp::getName()
{
    return getAttribute(u"name");
}

void HTMLFormControlImp::setName(const std::u16string& name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLFormControlImp::getType()
{
    // TODO: see http://www.whatwg.org/specs/web-apps/current-work/multipage/common-microsyntaxes.html#enumerated-attribute
    return getAttribute(u"type");
}

void HTMLFormControlImp::setType(const std::u16string& type)
{
    setAttribute(u"type", type);
}

std::u16string HTMLFormControlImp::getValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormControlImp::setValue(const std::u16string& value)
{
    // TODO: implement me!
}

bool HTMLFormControlImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLFormControlImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLFormControlImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLFormControlImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLFormControlImp::setCustomValidity(const std::u16string& error)
{
    // TODO: implement me!
}

NodeList HTMLFormControlImp::getLabels()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}
}
}
}
