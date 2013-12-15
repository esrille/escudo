/*
 * Copyright 2010-2013 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLOptionElement.h>

#include <org/w3c/dom/html/HTMLSelectElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/html/HTMLOptionsCollection.h>
#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLFormElement.h>
#include <org/w3c/dom/html/HTMLOptGroupElement.h>
#include <org/w3c/dom/html/ValidityState.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class HTMLFormElementImp;
class HTMLOptionElementImp;
class HTMLOptionsCollectionImp;

typedef std::shared_ptr<HTMLOptionElementImp> HTMLOptionElementPtr;

class HTMLSelectElementImp : public ObjectMixin<HTMLSelectElementImp, HTMLElementImp>
{
    std::weak_ptr<HTMLFormElementImp> form;
    std::shared_ptr<HTMLOptionsCollectionImp> options;
    Retained<EventListenerImp> mutationListener;
    bool mute;

    void handleMutation(EventListenerImp* listener, events::Event event);

public:
    HTMLSelectElementImp(DocumentImp* ownerDocument);
    HTMLSelectElementImp(const HTMLSelectElementImp& org);

    int getIndex(const HTMLOptionElementPtr& option);

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLSelectElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // HTMLSelectElement
    bool getAutofocus();
    void setAutofocus(bool autofocus);
    bool getDisabled();
    void setDisabled(bool disabled);
    html::HTMLFormElement getForm();
    bool getMultiple();
    void setMultiple(bool multiple);
    std::u16string getName();
    void setName(const std::u16string& name);
    bool getRequired();
    void setRequired(bool required);
    unsigned int getSize();
    void setSize(unsigned int size);
    std::u16string getType();
    html::HTMLOptionsCollection getOptions();
    unsigned int getLength();
    void setLength(unsigned int length);
    Element item(unsigned int index);
    Object namedItem(const std::u16string& name);
    void add(Any element);
    void add(Any element, Any before);
    void remove(int index);
    void setElement(unsigned int index, html::HTMLOptionElement option);
    html::HTMLCollection getSelectedOptions();
    int getSelectedIndex();
    void setSelectedIndex(int selectedIndex);
    std::u16string getValue();
    void setValue(const std::u16string& value);
    bool getWillValidate();
    html::ValidityState getValidity();
    std::u16string getValidationMessage();
    bool checkValidity();
    void setCustomValidity(const std::u16string& error);
    NodeList getLabels();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLSelectElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLSelectElement::getMetaData();
    }
};

typedef std::shared_ptr<HTMLSelectElementImp> HTMLSelectElementPtr;

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED
