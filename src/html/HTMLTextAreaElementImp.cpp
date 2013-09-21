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

#include "HTMLTextAreaElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

bool HTMLTextAreaElementImp::getAutofocus()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setAutofocus(bool autofocus)
{
    // TODO: implement me!
}

unsigned int HTMLTextAreaElementImp::getCols()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setCols(unsigned int cols)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getDirName()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setDirName(const std::u16string& dirName)
{
    // TODO: implement me!
}

bool HTMLTextAreaElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLTextAreaElementImp::getForm()
{
    // TODO: implement me!
    return nullptr;
}

int HTMLTextAreaElementImp::getMaxLength()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setMaxLength(int maxLength)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLTextAreaElementImp::setName(const std::u16string& name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLTextAreaElementImp::getPlaceholder()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setPlaceholder(const std::u16string& placeholder)
{
    // TODO: implement me!
}

bool HTMLTextAreaElementImp::getReadOnly()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setReadOnly(bool readOnly)
{
    // TODO: implement me!
}

bool HTMLTextAreaElementImp::getRequired()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setRequired(bool required)
{
    // TODO: implement me!
}

unsigned int HTMLTextAreaElementImp::getRows()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setRows(unsigned int rows)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getWrap()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setWrap(const std::u16string& wrap)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getType()
{
    return u"textarea";
}

std::u16string HTMLTextAreaElementImp::getDefaultValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setDefaultValue(const std::u16string& defaultValue)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setValue(const std::u16string& value)
{
    // TODO: implement me!
}

unsigned int HTMLTextAreaElementImp::getTextLength()
{
    // TODO: implement me!
    return 0;
}

bool HTMLTextAreaElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLTextAreaElementImp::getValidity()
{
    // TODO: implement me!
    return nullptr;
}

std::u16string HTMLTextAreaElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLTextAreaElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setCustomValidity(const std::u16string& error)
{
    // TODO: implement me!
}

NodeList HTMLTextAreaElementImp::getLabels()
{
    // TODO: implement me!
    return nullptr;
}

void HTMLTextAreaElementImp::select()
{
    // TODO: implement me!
}

unsigned int HTMLTextAreaElementImp::getSelectionStart()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setSelectionStart(unsigned int selectionStart)
{
    // TODO: implement me!
}

unsigned int HTMLTextAreaElementImp::getSelectionEnd()
{
    // TODO: implement me!
    return 0;
}

void HTMLTextAreaElementImp::setSelectionEnd(unsigned int selectionEnd)
{
    // TODO: implement me!
}

std::u16string HTMLTextAreaElementImp::getSelectionDirection()
{
    // TODO: implement me!
    return u"";
}

void HTMLTextAreaElementImp::setSelectionDirection(const std::u16string& selectionDirection)
{
    // TODO: implement me!
}

void HTMLTextAreaElementImp::setSelectionRange(unsigned int start, unsigned int end)
{
    // TODO: implement me!
}

void HTMLTextAreaElementImp::setSelectionRange(unsigned int start, unsigned int end, const std::u16string& direction)
{
    // TODO: implement me!
}

}
}
}
}
