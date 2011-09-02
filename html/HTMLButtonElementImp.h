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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLBUTTONELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLBUTTONELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLButtonElement.h>
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
class HTMLButtonElementImp : public ObjectMixin<HTMLButtonElementImp, HTMLElementImp>
{
public:
    // HTMLButtonElement
    bool getAutofocus();
    void setAutofocus(bool autofocus);
    bool getDisabled();
    void setDisabled(bool disabled);
    html::HTMLFormElement getForm();
    std::u16string getFormAction();
    void setFormAction(std::u16string formAction);
    std::u16string getFormEnctype();
    void setFormEnctype(std::u16string formEnctype);
    std::u16string getFormMethod();
    void setFormMethod(std::u16string formMethod);
    std::u16string getFormNoValidate();
    void setFormNoValidate(std::u16string formNoValidate);
    std::u16string getFormTarget();
    void setFormTarget(std::u16string formTarget);
    std::u16string getName();
    void setName(std::u16string name);
    std::u16string getType();
    void setType(std::u16string type);
    std::u16string getValue();
    void setValue(std::u16string value);
    bool getWillValidate();
    html::ValidityState getValidity();
    std::u16string getValidationMessage();
    bool checkValidity();
    void setCustomValidity(std::u16string error);
    NodeList getLabels();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLButtonElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLButtonElement::getMetaData();
    }
    HTMLButtonElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"button") {
    }
    HTMLButtonElementImp(HTMLButtonElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLBUTTONELEMENTIMP_H_INCLUDED
