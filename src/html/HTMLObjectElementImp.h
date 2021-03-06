/*
 * Copyright 2012-2013 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLOBJECTELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLOBJECTELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLObjectElement.h>
#include "HTMLReplacedElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/html/HTMLFormElement.h>
#include <org/w3c/dom/html/ValidityState.h>
#include <org/w3c/dom/html/Window.h>

#include "http/HTTPRequest.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class BoxImage;

class HTMLObjectElementImp : public ObjectMixin<HTMLObjectElementImp, HTMLReplacedElementImp>
{
    bool dirty;

public:
    HTMLObjectElementImp(DocumentImp* ownerDocument);
    HTMLObjectElementImp(const HTMLObjectElementImp& org);

    virtual void notify(NotificationType type);
    virtual void handleMutation(events::MutationEvent mutation);

    void requestRefresh();
    void refresh();
    void handleRefresh(const HttpRequestPtr& request);

    // TODO: Refine this interface as this is only for CSS
    bool getIntrinsicSize(float& w, float& h);

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLObjectElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // HTMLObjectElement
    std::u16string getData();
    void setData(const std::u16string& data);
    std::u16string getType();
    void setType(const std::u16string& type);
    bool getTypeMustMatch();
    void setTypeMustMatch(bool typeMustMatch);
    std::u16string getName();
    void setName(const std::u16string& name);
    std::u16string getUseMap();
    void setUseMap(const std::u16string& useMap);
    html::HTMLFormElement getForm();
    std::u16string getWidth();
    void setWidth(const std::u16string& width);
    std::u16string getHeight();
    void setHeight(const std::u16string& height);
    Document getContentDocument();
    html::Window getContentWindow();
    bool getWillValidate();
    html::ValidityState getValidity();
    std::u16string getValidationMessage();
    bool checkValidity();
    void setCustomValidity(const std::u16string& error);
    Any operator() (Variadic<Any> arguments = Variadic<Any>());
    // HTMLObjectElement-24
    std::u16string getAlign();
    void setAlign(const std::u16string& align);
    std::u16string getArchive();
    void setArchive(const std::u16string& archive);
    std::u16string getCode();
    void setCode(const std::u16string& code);
    bool getDeclare();
    void setDeclare(bool declare);
    unsigned int getHspace();
    void setHspace(unsigned int hspace);
    std::u16string getStandby();
    void setStandby(const std::u16string& standby);
    unsigned int getVspace();
    void setVspace(unsigned int vspace);
    std::u16string getCodeBase();
    void setCodeBase(const std::u16string& codeBase);
    std::u16string getCodeType();
    void setCodeType(const std::u16string& codeType);
    std::u16string getBorder();
    void setBorder(const std::u16string& border);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLObjectElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLObjectElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLOBJECTELEMENTIMP_H_INCLUDED
