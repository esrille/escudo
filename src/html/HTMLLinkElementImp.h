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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLLINKELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLLINKELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <org/w3c/dom/html/HTMLLinkElement.h>

#include <org/w3c/dom/stylesheets/StyleSheet.h>
#include <org/w3c/dom/DOMTokenList.h>
#include <org/w3c/dom/DOMSettableTokenList.h>

#include "HTMLElementImp.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLLinkElementImp : public ObjectMixin<HTMLLinkElementImp, HTMLElementImp>
{
    bool dirty;
    HttpRequest* current;
    stylesheets::StyleSheet styleSheet;

    void resetStyleSheet();

public:
    HTMLLinkElementImp(DocumentImp* ownerDocument);
    HTMLLinkElementImp(HTMLLinkElementImp* org, bool deep);
    ~HTMLLinkElementImp();

    virtual void notify(NotificationType type);
    virtual void handleMutation(events::MutationEvent mutation);

    void requestRefresh();
    void refresh();

    void linkStyleSheet(HttpRequest* request);
    void linkIcon(HttpRequest* request);
    bool setFavicon(DocumentImp* document);

    // Node
    virtual Node cloneNode(bool deep = true);

    // HTMLLinkElement
    virtual bool getDisabled();
    virtual void setDisabled(bool disabled);
    virtual std::u16string getHref();
    virtual void setHref(const std::u16string& href);
    virtual std::u16string getRel();
    virtual void setRel(const std::u16string& rel);
    virtual DOMTokenList getRelList();
    virtual std::u16string getMedia();
    virtual void setMedia(const std::u16string& media);
    virtual std::u16string getHreflang();
    virtual void setHreflang(const std::u16string& hreflang);
    virtual std::u16string getType();
    virtual void setType(const std::u16string& type);
    virtual DOMSettableTokenList getSizes();
    virtual void setSizes(const std::u16string& sizes);
    // HTMLLinkElement-21
    virtual std::u16string getCharset();
    virtual void setCharset(const std::u16string& charset);
    virtual std::u16string getRev();
    virtual void setRev(const std::u16string& rev);
    virtual std::u16string getTarget();
    virtual void setTarget(const std::u16string& target);

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

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLLINKELEMENTIMP_H_INCLUDED
