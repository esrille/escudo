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

#include "HTMLSelectElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

bool HTMLSelectElementImp::getAutofocus()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setAutofocus(bool autofocus)
{
    // TODO: implement me!
}

bool HTMLSelectElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLSelectElementImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

bool HTMLSelectElementImp::getMultiple()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setMultiple(bool multiple)
{
    // TODO: implement me!
}

std::u16string HTMLSelectElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLSelectElementImp::setName(std::u16string name)
{
    setAttribute(u"name", name);
}

bool HTMLSelectElementImp::getRequired()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setRequired(bool required)
{
    // TODO: implement me!
}

unsigned int HTMLSelectElementImp::getSize()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setSize(unsigned int size)
{
    // TODO: implement me!
}

std::u16string HTMLSelectElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

html::HTMLOptionsCollection HTMLSelectElementImp::getOptions()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

unsigned int HTMLSelectElementImp::getLength()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setLength(unsigned int length)
{
    // TODO: implement me!
}

Any HTMLSelectElementImp::item(unsigned int index)
{
    // TODO: implement me!
    return 0;
}

Any HTMLSelectElementImp::namedItem(std::u16string name)
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::add(html::HTMLElement element)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::add(html::HTMLElement element, html::HTMLElement before)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::add(html::HTMLElement element, int before)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::remove(int index)
{
    // TODO: implement me!
}

html::HTMLCollection HTMLSelectElementImp::getSelectedOptions()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

int HTMLSelectElementImp::getSelectedIndex()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setSelectedIndex(int selectedIndex)
{
    // TODO: implement me!
}

std::u16string HTMLSelectElementImp::getValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLSelectElementImp::setValue(std::u16string value)
{
    // TODO: implement me!
}

bool HTMLSelectElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLSelectElementImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLSelectElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLSelectElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setCustomValidity(std::u16string error)
{
    // TODO: implement me!
}

NodeList HTMLSelectElementImp::getLabels()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

}
}
}
}
