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

#include "HTMLInputElementImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/events/KeyboardEvent.h>
#include <org/w3c/dom/html/HTMLTemplateElement.h>

#include "utf.h"
#include "DocumentImp.h"
#include "HTMLTemplateElementImp.h"
#include "css/CSSStyleDeclarationImp.h"     // TODO: only for XBL; isolate this later.

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

namespace {

const char16_t* typeKeywords[] = {
    u"hidden",
    u"text",
    u"search",
    u"tel",
    u"url",
    u"email",
    u"password",
    u"datetime",
    u"date",
    u"month",
    u"week",
    u"time",
    u"datetime-local",
    u"number",
    u"range",
    u"color",
    u"checkbox",
    u"radio",
    u"file",
    u"submit",
    u"image",
    u"reset",
    u"button",
};

}  // namespace

HTMLInputElementImp::HTMLInputElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"input"),
    type(Text),
    form(0),
    clickListener(boost::bind(&HTMLInputElementImp::handleClick, this, _1)),
    keydownListener(boost::bind(&HTMLInputElementImp::handleKeydown, this, _1)),
    cursor(0),
    checked(false)
{
}

HTMLInputElementImp::HTMLInputElementImp(HTMLInputElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    type(org->type),
    form(0),
    clickListener(boost::bind(&HTMLInputElementImp::handleClick, this, _1)),
    keydownListener(boost::bind(&HTMLInputElementImp::handleKeydown, this, _1)),
    cursor(0),
    checked(org->checked)
{
    // TODO: check event listeners.
}

void HTMLInputElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalHspace(this);
    HTMLElementImp::evalVspace(this);

    setType(getAttribute(u"type"));

    if (!getDisabled() && type != Hidden)
        setTabIndex(0);

    if (hasAttribute(u"checked"))
        checked = true;

    switch (type) {
    case Text:
    case Search:
    case Telephone:
    case URL:
    case Email:
    case Password: {
        Nullable<std::u16string> size = getAttribute(u"size");
        std::u16string length(u"20em");
        if (size.hasValue()) {
            std::u16string length(size.value());
            if (toUnsigned(length))
                length += u"em";
            else
                length = u"20em";
        }
        css::CSSStyleDeclaration style = getStyle();
        style.setProperty(u"min-width", length, u"non-css");
        break;
    }
    default:
        break;
    }
}

void HTMLInputElementImp::handleClick(events::Event event)
{
    if (type == SubmitButton) {
        html::HTMLFormElement form = getForm();
        if (HTMLFormElementImp* imp = dynamic_cast<HTMLFormElementImp*>(form.self()))
            imp->submit(this);
    }
}

void HTMLInputElementImp::handleKeydown(events::Event event)
{
    if (type == Text) {
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
}

void HTMLInputElementImp::generateShadowContent(CSSStyleDeclarationImp* style)
{
    DocumentImp* document = getOwnerDocumentImp();
    assert(document);
    switch (style->binding.getValue()) {
    case CSSBindingValueImp::InputTextfield: {
        HTMLTemplateElementImp* element = new(std::nothrow) HTMLTemplateElementImp(document);
        if (element) {
            dom::Text text = document->createTextNode(getValue());
            element->appendChild(text, true);
            style->setCssText(u"display: inline-block; white-space: pre; background-color: white; border: 2px inset; text-align: left; padding: 1px; min-height: 1em;");
            setShadowTree(element);
            addEventListener(u"keydown", &keydownListener, false, true);
        }
        break;
    }
    case CSSBindingValueImp::InputButton: {
        HTMLTemplateElementImp* element = new(std::nothrow) HTMLTemplateElementImp(document);
        if (element) {
            dom::Text text = document->createTextNode(getValue());
            element->appendChild(text, true);
            style->setCssText(u"display: inline-block; border: 2px outset; padding: 1px; text-align: center; min-height: 1em;");
            setShadowTree(element);
            addEventListener(u"click", &clickListener, false, true);
        }
        break;
    }
    case CSSBindingValueImp::InputRadio: {
        HTMLTemplateElementImp* element = new(std::nothrow) HTMLTemplateElementImp(document);
        if (element) {
            dom::Text text = document->createTextNode(getChecked() ? u"\u25c9" : u"\u25cb");
            element->appendChild(text, true);
            style->setCssText(u"display: inline-block; border-style: none; padding: 2px;");
            setShadowTree(element);
        }
        break;
    }
    default:
        HTMLElementImp::generateShadowContent(style);
        switch (type) {
        case SubmitButton:
            addEventListener(u"click", &clickListener, false, true);
            break;
        default:
            break;
        }
        break;
    }
}

std::u16string HTMLInputElementImp::getAccept()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAccept(const std::u16string& accept)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAlt()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAlt(const std::u16string& alt)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAutocomplete()
{
    return getAttribute(u"autocomplete");
}

void HTMLInputElementImp::setAutocomplete(const std::u16string& autocomplete)
{
    setAttribute(u"autocomplete", autocomplete);
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
    return checked;
}

void HTMLInputElementImp::setChecked(bool checked)
{
    this->checked = checked;
    // TODO: dispatch an event
}

std::u16string HTMLInputElementImp::getDirName()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setDirName(const std::u16string& dirName)
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
    if (form)
        return form;
    for (Element parent = getParentElement(); parent; parent = parent.getParentElement()) {
        if (html::HTMLFormElement::hasInstance(parent))
            return interface_cast<html::HTMLFormElement>(parent);
    }
    return 0;
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

void HTMLInputElementImp::setFormAction(const std::u16string& formAction)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getFormEnctype()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormEnctype(const std::u16string& formEnctype)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getFormMethod()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setFormMethod(const std::u16string& formMethod)
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

void HTMLInputElementImp::setFormTarget(const std::u16string& formTarget)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setHeight(const std::u16string& height)
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

void HTMLInputElementImp::setMax(const std::u16string& max)
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

void HTMLInputElementImp::setMin(const std::u16string& min)
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
    return getAttribute(u"name");
}

void HTMLInputElementImp::setName(const std::u16string& name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLInputElementImp::getPattern()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setPattern(const std::u16string& pattern)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getPlaceholder()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setPlaceholder(const std::u16string& placeholder)
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

void HTMLInputElementImp::setSrc(const std::u16string& src)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getStep()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setStep(const std::u16string& step)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getType()
{
    return typeKeywords[type];
}

void HTMLInputElementImp::setType(const std::u16string& type)
{
    std::u16string t(type);
    toLower(t);
    for (const char16_t** i = typeKeywords; i < typeKeywords + TypeMax; ++i) {
        if (t.compare(*i) == 0) {
            this->type = i - typeKeywords;
            return;
        }
    }
    this->type = Text;
}

std::u16string HTMLInputElementImp::getDefaultValue()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setDefaultValue(const std::u16string& defaultValue)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getValue()
{
    return getAttribute(u"value");
}

void HTMLInputElementImp::setValue(const std::u16string& value)
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

void HTMLInputElementImp::setWidth(const std::u16string& width)
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

void HTMLInputElementImp::setCustomValidity(const std::u16string& error)
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

void HTMLInputElementImp::setSelectionDirection(const std::u16string& selectionDirection)
{
    // TODO: implement me!
}

void HTMLInputElementImp::setSelectionRange(unsigned int start, unsigned int end)
{
    // TODO: implement me!
}

void HTMLInputElementImp::setSelectionRange(unsigned int start, unsigned int end, const std::u16string& direction)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLInputElementImp::getUseMap()
{
    // TODO: implement me!
    return u"";
}

void HTMLInputElementImp::setUseMap(const std::u16string& useMap)
{
    // TODO: implement me!
}

}
}
}
}
