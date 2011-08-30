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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLFORMELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLFORMELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLFormElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLFormControlsCollection.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLFormElementImp : public ObjectMixin<HTMLFormElementImp, HTMLElementImp>
{
    html::HTMLFormControlsCollection elements;
    std::map<const std::u16string, Element> pastNamesMap;

public:
    HTMLFormElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"form"),
        elements(0) {
    }
    HTMLFormElementImp(HTMLFormElementImp* org, bool deep) :
        ObjectMixin(org, deep),
        elements(0) {
    }
    ~HTMLFormElementImp();

    // HTMLFormElement
    std::u16string getAcceptCharset();
    void setAcceptCharset(std::u16string acceptCharset);
    std::u16string getAction();
    void setAction(std::u16string action);
    std::u16string getAutocomplete();
    void setAutocomplete(std::u16string autocomplete);
    std::u16string getEnctype();
    void setEnctype(std::u16string enctype);
    std::u16string getEncoding();
    void setEncoding(std::u16string encoding);
    std::u16string getMethod();
    void setMethod(std::u16string method);
    std::u16string getName();
    void setName(std::u16string name);
    bool getNoValidate();
    void setNoValidate(bool noValidate);
    std::u16string getTarget();
    void setTarget(std::u16string target);
    html::HTMLFormControlsCollection getElements();
    int getLength();
    Any getElement(unsigned int index);
    Any getElement(std::u16string name);
    void submit();
    void reset();
    bool checkValidity();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLFormElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLFormElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLFORMELEMENTIMP_H_INCLUDED
