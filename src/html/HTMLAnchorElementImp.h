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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLANCHORELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLANCHORELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLAnchorElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/DOMTokenList.h>

#include "EventListenerImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLAnchorElementImp : public ObjectMixin<HTMLAnchorElementImp, HTMLElementImp>
{
    Retained<EventListenerImp> clickListener;
    void handleClick(EventListenerImp* listener, events::Event event);

public:
    HTMLAnchorElementImp(DocumentImp* ownerDocument);
    HTMLAnchorElementImp(HTMLAnchorElementImp* org, bool deep);
    ~HTMLAnchorElementImp();

    virtual void handleMutation(events::MutationEvent mutation);

    // HTMLAnchorElement
    std::u16string getHref();
    void setHref(const std::u16string& href);
    std::u16string getTarget();
    void setTarget(const std::u16string& target);
    std::u16string getRel();
    void setRel(const std::u16string& rel);
    DOMTokenList getRelList();
    std::u16string getMedia();
    void setMedia(const std::u16string& media);
    std::u16string getHreflang();
    void setHreflang(const std::u16string& hreflang);
    std::u16string getType();
    void setType(const std::u16string& type);
    std::u16string getText();
    void setText(const std::u16string& text);
    std::u16string getProtocol();
    void setProtocol(const std::u16string& protocol);
    std::u16string getHost();
    void setHost(const std::u16string& host);
    std::u16string getHostname();
    void setHostname(const std::u16string& hostname);
    std::u16string getPort();
    void setPort(const std::u16string& port);
    std::u16string getPathname();
    void setPathname(const std::u16string& pathname);
    std::u16string getSearch();
    void setSearch(const std::u16string& search);
    std::u16string getHash();
    void setHash(const std::u16string& hash);
    // HTMLAnchorElement-7
    std::u16string getCoords();
    void setCoords(const std::u16string& coords);
    std::u16string getCharset();
    void setCharset(const std::u16string& charset);
    std::u16string getName();
    void setName(const std::u16string& name);
    std::u16string getRev();
    void setRev(const std::u16string& rev);
    std::u16string getShape();
    void setShape(const std::u16string& shape);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLAnchorElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLAnchorElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLANCHORELEMENTIMP_H_INCLUDED
