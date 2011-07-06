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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLINPUTELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLINPUTELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLInputElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/file/FileList.h>
#include <org/w3c/dom/html/HTMLFormElement.h>
#include <org/w3c/dom/html/HTMLOptionElement.h>
#include <org/w3c/dom/html/ValidityState.h>

#include "EventListenerImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLInputElementImp : public ObjectMixin<HTMLInputElementImp, HTMLElementImp>
{
    Retained<EventListenerImp> keydownListener;
    void handleKeydown(events::Event event);

    size_t cursor;

public:
    HTMLInputElementImp(DocumentImp* ownerDocument);
    HTMLInputElementImp(HTMLInputElementImp* org, bool deep);

    // HTMLInputElement
    std::u16string getAccept() __attribute__((weak));
    void setAccept(std::u16string accept) __attribute__((weak));
    std::u16string getAlt() __attribute__((weak));
    void setAlt(std::u16string alt) __attribute__((weak));
    std::u16string getAutocomplete() __attribute__((weak));
    void setAutocomplete(std::u16string autocomplete) __attribute__((weak));
    bool getAutofocus() __attribute__((weak));
    void setAutofocus(bool autofocus) __attribute__((weak));
    bool getDefaultChecked() __attribute__((weak));
    void setDefaultChecked(bool defaultChecked) __attribute__((weak));
    bool getChecked() __attribute__((weak));
    void setChecked(bool checked) __attribute__((weak));
    std::u16string getDirName() __attribute__((weak));
    void setDirName(std::u16string dirName) __attribute__((weak));
    bool getDisabled() __attribute__((weak));
    void setDisabled(bool disabled) __attribute__((weak));
    html::HTMLFormElement getForm() __attribute__((weak));
    file::FileList getFiles() __attribute__((weak));
    std::u16string getFormAction() __attribute__((weak));
    void setFormAction(std::u16string formAction) __attribute__((weak));
    std::u16string getFormEnctype() __attribute__((weak));
    void setFormEnctype(std::u16string formEnctype) __attribute__((weak));
    std::u16string getFormMethod() __attribute__((weak));
    void setFormMethod(std::u16string formMethod) __attribute__((weak));
    bool getFormNoValidate() __attribute__((weak));
    void setFormNoValidate(bool formNoValidate) __attribute__((weak));
    std::u16string getFormTarget() __attribute__((weak));
    void setFormTarget(std::u16string formTarget) __attribute__((weak));
    std::u16string getHeight() __attribute__((weak));
    void setHeight(std::u16string height) __attribute__((weak));
    bool getIndeterminate() __attribute__((weak));
    void setIndeterminate(bool indeterminate) __attribute__((weak));
    html::HTMLElement getList() __attribute__((weak));
    std::u16string getMax() __attribute__((weak));
    void setMax(std::u16string max) __attribute__((weak));
    int getMaxLength() __attribute__((weak));
    void setMaxLength(int maxLength) __attribute__((weak));
    std::u16string getMin() __attribute__((weak));
    void setMin(std::u16string min) __attribute__((weak));
    bool getMultiple() __attribute__((weak));
    void setMultiple(bool multiple) __attribute__((weak));
    std::u16string getName() __attribute__((weak));
    void setName(std::u16string name) __attribute__((weak));
    std::u16string getPattern() __attribute__((weak));
    void setPattern(std::u16string pattern) __attribute__((weak));
    std::u16string getPlaceholder() __attribute__((weak));
    void setPlaceholder(std::u16string placeholder) __attribute__((weak));
    bool getReadOnly() __attribute__((weak));
    void setReadOnly(bool readOnly) __attribute__((weak));
    bool getRequired() __attribute__((weak));
    void setRequired(bool required) __attribute__((weak));
    unsigned int getSize() __attribute__((weak));
    void setSize(unsigned int size) __attribute__((weak));
    std::u16string getSrc() __attribute__((weak));
    void setSrc(std::u16string src) __attribute__((weak));
    std::u16string getStep() __attribute__((weak));
    void setStep(std::u16string step) __attribute__((weak));
    std::u16string getType() __attribute__((weak));
    void setType(std::u16string type) __attribute__((weak));
    std::u16string getDefaultValue() __attribute__((weak));
    void setDefaultValue(std::u16string defaultValue) __attribute__((weak));
    std::u16string getValue() __attribute__((weak));
    void setValue(std::u16string value) __attribute__((weak));
    unsigned long long getValueAsDate() __attribute__((weak));
    void setValueAsDate(unsigned long long valueAsDate) __attribute__((weak));
    double getValueAsNumber() __attribute__((weak));
    void setValueAsNumber(double valueAsNumber) __attribute__((weak));
    html::HTMLOptionElement getSelectedOption() __attribute__((weak));
    std::u16string getWidth() __attribute__((weak));
    void setWidth(std::u16string width) __attribute__((weak));
    void stepUp() __attribute__((weak));
    void stepUp(int n) __attribute__((weak));
    void stepDown() __attribute__((weak));
    void stepDown(int n) __attribute__((weak));
    bool getWillValidate() __attribute__((weak));
    html::ValidityState getValidity() __attribute__((weak));
    std::u16string getValidationMessage() __attribute__((weak));
    bool checkValidity() __attribute__((weak));
    void setCustomValidity(std::u16string error) __attribute__((weak));
    NodeList getLabels() __attribute__((weak));
    void select() __attribute__((weak));
    unsigned int getSelectionStart() __attribute__((weak));
    void setSelectionStart(unsigned int selectionStart) __attribute__((weak));
    unsigned int getSelectionEnd() __attribute__((weak));
    void setSelectionEnd(unsigned int selectionEnd) __attribute__((weak));
    std::u16string getSelectionDirection() __attribute__((weak));
    void setSelectionDirection(std::u16string selectionDirection) __attribute__((weak));
    void setSelectionRange(unsigned int start, unsigned int end) __attribute__((weak));
    void setSelectionRange(unsigned int start, unsigned int end, std::u16string direction) __attribute__((weak));
    // HTMLInputElement-21
    std::u16string getAlign() __attribute__((weak));
    void setAlign(std::u16string align) __attribute__((weak));
    std::u16string getUseMap() __attribute__((weak));
    void setUseMap(std::u16string useMap) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLInputElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLInputElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLINPUTELEMENTIMP_H_INCLUDED
