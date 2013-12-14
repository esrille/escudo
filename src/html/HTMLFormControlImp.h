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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLFORMCONTROLIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLFORMCONTROLIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

class HTMLFormControlImp : public HTMLElementImp
{
public:
    HTMLFormControlImp(DocumentImp* ownerDocument, const std::u16string& localName) :
        HTMLElementImp(ownerDocument, localName)
    {
        tabIndex = 0;
    }

    virtual void handleMutation(events::MutationEvent mutation);

    // Node
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLFormControlImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    bool getAutofocus();
    void setAutofocus(bool autofocus);
    bool getDisabled();
    void setDisabled(bool disabled);
    html::HTMLFormElement getForm();
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
    std::u16string getName();
    void setName(const std::u16string& name);
    std::u16string getType();
    void setType(const std::u16string& type);
    std::u16string getValue();
    void setValue(const std::u16string& value);
    bool getWillValidate();
    html::ValidityState getValidity();
    std::u16string getValidationMessage();
    bool checkValidity();
    void setCustomValidity(const std::u16string& error);
    NodeList getLabels();
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLFORMCONTROLIMP_H_INCLUDED
