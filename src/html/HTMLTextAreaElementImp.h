/*
 * Copyright 2010-2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLTEXTAREAELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLTEXTAREAELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLTextAreaElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLFormElement.h>
#include <org/w3c/dom/html/ValidityState.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLTextAreaElementImp : public ObjectMixin<HTMLTextAreaElementImp, HTMLElementImp>
{
public:
    HTMLTextAreaElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"textarea") {
    }
    HTMLTextAreaElementImp(HTMLTextAreaElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }

    // HTMLTextAreaElement
    bool getAutofocus();
    void setAutofocus(bool autofocus);
    unsigned int getCols();
    void setCols(unsigned int cols);
    std::u16string getDirName();
    void setDirName(const std::u16string& dirName);
    bool getDisabled();
    void setDisabled(bool disabled);
    html::HTMLFormElement getForm();
    int getMaxLength();
    void setMaxLength(int maxLength);
    std::u16string getName();
    void setName(const std::u16string& name);
    std::u16string getPlaceholder();
    void setPlaceholder(const std::u16string& placeholder);
    bool getReadOnly();
    void setReadOnly(bool readOnly);
    bool getRequired();
    void setRequired(bool required);
    unsigned int getRows();
    void setRows(unsigned int rows);
    std::u16string getWrap();
    void setWrap(const std::u16string& wrap);
    std::u16string getType();
    std::u16string getDefaultValue();
    void setDefaultValue(const std::u16string& defaultValue);
    std::u16string getValue();
    void setValue(const std::u16string& value);
    unsigned int getTextLength();
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
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLTextAreaElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLTextAreaElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLTEXTAREAELEMENTIMP_H_INCLUDED
