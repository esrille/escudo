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

#include "HTMLInputElementImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/events/KeyboardEvent.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLInputElementImp::HTMLInputElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"input"),
    keydownListener(boost::bind(&HTMLInputElementImp::handleKeydown, this, _1)),
    cursor(0)
{
    addEventListener(u"keydown", &keydownListener);
}

HTMLInputElementImp::HTMLInputElementImp(HTMLInputElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    keydownListener(boost::bind(&HTMLInputElementImp::handleKeydown, this, _1)),
    cursor(0)
{
    addEventListener(u"click", &keydownListener);
}

void HTMLInputElementImp::handleKeydown(events::Event event)
{
    bool modified = false;
    std::u16string value = getValue();
    events::KeyboardEvent key = interface_cast<events::KeyboardEvent>(event);
    char16_t c = key.getCharCode();
    if (32 <= c && c < 127) {
        value.insert(cursor, 1, c);
        ++cursor;
        modified = true;
    } else if (c == 8) {
        if (0 < cursor) {
            --cursor;
            value.erase(cursor, 1);
            modified = true;
        }
    }
    unsigned k = key.getKeyCode();
    switch (k) {
    case 35:  // End
        cursor = value.length();
        break;
    case 36:  // Home
        cursor = 0;
        break;
    case 37:  // <-
        if (0 < cursor)
            --cursor;
        break;
    case 39:  // ->
        if (cursor < value.length())
            ++cursor;
        break;
    case 46:  // Del
        if (cursor < value.length()) {
            value.erase(cursor, 1);
            modified = true;
        }
        break;
    default:
        break;
    }
    if (modified)
        setValue(value);
}

std::u16string HTMLInputElementImp::getAccept()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAccept(std::u16string accept)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAlt()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAlt(std::u16string alt)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAutocomplete()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAutocomplete(std::u16string autocomplete)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getAutofocus()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setAutofocus(bool autofocus)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getDefaultChecked()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setDefaultChecked(bool defaultChecked)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getChecked()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setChecked(bool checked)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getDirName()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setDirName(std::u16string dirName)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLInputElementImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

file::FileList HTMLInputElementImp::getFiles()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLInputElementImp::getFormAction()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormAction(std::u16string formAction)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getFormEnctype()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormEnctype(std::u16string formEnctype)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getFormMethod()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormMethod(std::u16string formMethod)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getFormNoValidate()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setFormNoValidate(bool formNoValidate)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getFormTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormTarget(std::u16string formTarget)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setHeight(std::u16string height)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getIndeterminate()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setIndeterminate(bool indeterminate)
{
    // TODO: implement me!
}

html::HTMLElement HTMLInputElementImp::getList()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLInputElementImp::getMax()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setMax(std::u16string max)
{
    // TODO: implement me!
}

int HTMLInputElementImp::getMaxLength()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setMaxLength(int maxLength)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getMin()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setMin(std::u16string min)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getMultiple()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setMultiple(bool multiple)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setName(std::u16string name)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getPattern()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setPattern(std::u16string pattern)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getPlaceholder()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setPlaceholder(std::u16string placeholder)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getReadOnly()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setReadOnly(bool readOnly)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getRequired()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setRequired(bool required)
{
    // TODO: implement me!
}

unsigned int HTMLInputElementImp::getSize()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setSize(unsigned int size)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getSrc()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setSrc(std::u16string src)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getStep()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setStep(std::u16string step)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getDefaultValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setDefaultValue(std::u16string defaultValue)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getValue()
{
    Nullable<std::u16string> src = getAttribute(u"value");
    if (src.hasValue())
        return src.value();
    return u"";
}

void HTMLInputElementImp::setValue(std::u16string value)
{
    setAttribute(u"value", value);
}

unsigned long long HTMLInputElementImp::getValueAsDate()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setValueAsDate(unsigned long long valueAsDate)
{
    // TODO: implement me!
}

double HTMLInputElementImp::getValueAsNumber()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setValueAsNumber(double valueAsNumber)
{
    // TODO: implement me!
}

html::HTMLOptionElement HTMLInputElementImp::getSelectedOption()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLInputElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

void HTMLInputElementImp::stepUp()
{
    // TODO: implement me!
}

void HTMLInputElementImp::stepUp(int n)
{
    // TODO: implement me!
}

void HTMLInputElementImp::stepDown()
{
    // TODO: implement me!
}

void HTMLInputElementImp::stepDown(int n)
{
    // TODO: implement me!
}

bool HTMLInputElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLInputElementImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLInputElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLInputElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setCustomValidity(std::u16string error)
{
    // TODO: implement me!
}

NodeList HTMLInputElementImp::getLabels()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLInputElementImp::select()
{
    // TODO: implement me!
}

unsigned int HTMLInputElementImp::getSelectionStart()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setSelectionStart(unsigned int selectionStart)
{
    // TODO: implement me!
}

unsigned int HTMLInputElementImp::getSelectionEnd()
{
    // TODO: implement me!
    return 0;
}

void HTMLInputElementImp::setSelectionEnd(unsigned int selectionEnd)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getSelectionDirection()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setSelectionDirection(std::u16string selectionDirection)
{
    // TODO: implement me!
}

void HTMLInputElementImp::setSelectionRange(unsigned int start, unsigned int end)
{
    // TODO: implement me!
}

void HTMLInputElementImp::setSelectionRange(unsigned int start, unsigned int end, std::u16string direction)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getUseMap()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setUseMap(std::u16string useMap)
{
    // TODO: implement me!
}

}
}
}
}
