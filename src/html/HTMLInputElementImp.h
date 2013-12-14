/*
 * Copyright 2011-2013 Esrille Inc.
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
#include "HTMLFormElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class HTMLFormElementImp;

class HTMLInputElementImp : public ObjectMixin<HTMLInputElementImp, HTMLElementImp>
{
    enum {
        Hidden,
        Text,
        Search,
        Telephone,
        URL,
        Email,
        Password,
        DateAndTime,
        Date,
        Month,
        Week,
        Time,
        LocalDateAndTime,
        Number,
        Range,
        Color,
        Checkbox,
        RadioButton,
        FileUpload,
        SubmitButton,
        ImageButton,
        ResetButton,
        Button,

        TypeMax
    };

    int type;
    std::weak_ptr<HTMLFormElementImp> form;

    Retained<EventListenerImp> clickListener;
    Retained<EventListenerImp> keydownListener;

    size_t cursor;
    bool checked;

    void handleKeydown(EventListenerImp* listener, events::Event event);
    void handleClick(EventListenerImp* listener, events::Event event);

public:
    HTMLInputElementImp(DocumentImp* ownerDocument);
    HTMLInputElementImp(const HTMLInputElementImp& org);

    virtual void handleMutation(events::MutationEvent mutation);

    bool isButton() const {
        switch (type) {
        case SubmitButton:
        case ImageButton:
        case ResetButton:
        case Button:
            return true;
        default:
            return false;
        }
    }

    void updateStyle();

    // XBL 2.0 internal
    virtual bool generateShadowContent(const CSSStyleDeclarationPtr& style);

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLInputElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // HTMLElement
    css::CSSStyleDeclaration getStyle();

    // HTMLInputElement
    std::u16string getAccept();
    void setAccept(const std::u16string& accept);
    std::u16string getAlt();
    void setAlt(const std::u16string& alt);
    std::u16string getAutocomplete();
    void setAutocomplete(const std::u16string& autocomplete);
    bool getAutofocus();
    void setAutofocus(bool autofocus);
    bool getDefaultChecked();
    void setDefaultChecked(bool defaultChecked);
    bool getChecked();
    void setChecked(bool checked);
    std::u16string getDirName();
    void setDirName(const std::u16string& dirName);
    bool getDisabled();
    void setDisabled(bool disabled);
    html::HTMLFormElement getForm();
    file::FileList getFiles();
    std::u16string getFormAction();
    void setFormAction(const std::u16string& formAction);
    std::u16string getFormEnctype();
    void setFormEnctype(const std::u16string& formEnctype);
    std::u16string getFormMethod();
    void setFormMethod(const std::u16string& formMethod);
    bool getFormNoValidate();
    void setFormNoValidate(bool formNoValidate);
    std::u16string getFormTarget();
    void setFormTarget(const std::u16string& formTarget);
    unsigned int getHeight();
    void setHeight(unsigned int height);
    bool getIndeterminate();
    void setIndeterminate(bool indeterminate);
    html::HTMLElement getList();
    std::u16string getMax();
    void setMax(const std::u16string& max);
    int getMaxLength();
    void setMaxLength(int maxLength);
    std::u16string getMin();
    void setMin(const std::u16string& min);
    bool getMultiple();
    void setMultiple(bool multiple);
    std::u16string getName();
    void setName(const std::u16string& name);
    std::u16string getPattern();
    void setPattern(const std::u16string& pattern);
    std::u16string getPlaceholder();
    void setPlaceholder(const std::u16string& placeholder);
    bool getReadOnly();
    void setReadOnly(bool readOnly);
    bool getRequired();
    void setRequired(bool required);
    unsigned int getSize();
    void setSize(unsigned int size);
    std::u16string getSrc();
    void setSrc(const std::u16string& src);
    std::u16string getStep();
    void setStep(const std::u16string& step);
    std::u16string getType();
    void setType(const std::u16string& type);
    std::u16string getDefaultValue();
    void setDefaultValue(const std::u16string& defaultValue);
    std::u16string getValue();
    void setValue(const std::u16string& value);
    Nullable<unsigned long long> getValueAsDate();
    void setValueAsDate(Nullable<unsigned long long> valueAsDate);
    double getValueAsNumber();
    void setValueAsNumber(double valueAsNumber);
    unsigned int getWidth();
    void setWidth(unsigned int width);
    void stepUp();
    void stepUp(int n);
    void stepDown();
    void stepDown(int n);
    bool getWillValidate();
    html::ValidityState getValidity();
    std::u16string getValidationMessage();
    bool checkValidity();
    void setCustomValidity(const std::u16string& error);
    NodeList getLabels();
    void select();
    unsigned int getSelectionStart();
    void setSelectionStart(unsigned int selectionStart);
    unsigned int getSelectionEnd();
    void setSelectionEnd(unsigned int selectionEnd);
    std::u16string getSelectionDirection();
    void setSelectionDirection(const std::u16string& selectionDirection);
    void setSelectionRange(unsigned int start, unsigned int end);
    void setSelectionRange(unsigned int start, unsigned int end, const std::u16string& direction);
    // HTMLInputElement-18
    std::u16string getAlign();
    void setAlign(const std::u16string& align);
    std::u16string getUseMap();
    void setUseMap(const std::u16string& useMap);
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
