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

#ifndef HTMLLINKELEMENT_IMP_H
#define HTMLLINKELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLLinkElement.h>

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/DOMTokenList.h>
#include <org/w3c/dom/DOMSettableTokenList.h>

#include "HTMLElementImp.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLLinkElementImp : public ObjectMixin<HTMLLinkElementImp, HTMLElementImp>
{
    HttpRequest* request;
    stylesheets::StyleSheet styleSheet;

public:
    HTMLLinkElementImp(DocumentImp* ownerDocument);
    HTMLLinkElementImp(HTMLLinkElementImp* org, bool deep);
    ~HTMLLinkElementImp();

    virtual void eval();
    void linkStyleSheet();
    void linkIcon();
    bool setFavicon(DocumentImp* document);

    // Node
    virtual Node cloneNode(bool deep);

    // HTMLLinkElement
    virtual bool getDisabled();
    virtual void setDisabled(bool disabled);
    virtual std::u16string getHref();
    virtual void setHref(std::u16string href);
    virtual std::u16string getRel();
    virtual void setRel(std::u16string rel);
    virtual DOMTokenList getRelList();
    virtual std::u16string getMedia();
    virtual void setMedia(std::u16string media);
    virtual std::u16string getHreflang();
    virtual void setHreflang(std::u16string hreflang);
    virtual std::u16string getType();
    virtual void setType(std::u16string type);
    virtual DOMSettableTokenList getSizes();
    virtual void setSizes(std::u16string sizes);
    // HTMLLinkElement-21
    virtual std::u16string getCharset();
    virtual void setCharset(std::u16string charset);
    virtual std::u16string getRev();
    virtual void setRev(std::u16string rev);
    virtual std::u16string getTarget();
    virtual void setTarget(std::u16string target);

    // LinkStyle
    virtual stylesheets::StyleSheet getSheet();

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLLinkElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLLinkElement::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLLINKELEMENT_IMP_H
