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

    void handleClick(events::Event event);

public:
    HTMLAnchorElementImp(DocumentImp* ownerDocument);
    HTMLAnchorElementImp(HTMLAnchorElementImp* org, bool deep);
    ~HTMLAnchorElementImp();

    // HTMLAnchorElement
    std::u16string getHref() __attribute__((weak));
    void setHref(std::u16string href) __attribute__((weak));
    std::u16string getTarget() __attribute__((weak));
    void setTarget(std::u16string target) __attribute__((weak));
    std::u16string getPing() __attribute__((weak));
    void setPing(std::u16string ping) __attribute__((weak));
    std::u16string getRel() __attribute__((weak));
    void setRel(std::u16string rel) __attribute__((weak));
    DOMTokenList getRelList() __attribute__((weak));
    std::u16string getMedia() __attribute__((weak));
    void setMedia(std::u16string media) __attribute__((weak));
    std::u16string getHreflang() __attribute__((weak));
    void setHreflang(std::u16string hreflang) __attribute__((weak));
    std::u16string getType() __attribute__((weak));
    void setType(std::u16string type) __attribute__((weak));
    std::u16string getText() __attribute__((weak));
    void setText(std::u16string text) __attribute__((weak));
    std::u16string getProtocol() __attribute__((weak));
    void setProtocol(std::u16string protocol) __attribute__((weak));
    std::u16string getHost() __attribute__((weak));
    void setHost(std::u16string host) __attribute__((weak));
    std::u16string getHostname() __attribute__((weak));
    void setHostname(std::u16string hostname) __attribute__((weak));
    std::u16string getPort() __attribute__((weak));
    void setPort(std::u16string port) __attribute__((weak));
    std::u16string getPathname() __attribute__((weak));
    void setPathname(std::u16string pathname) __attribute__((weak));
    std::u16string getSearch() __attribute__((weak));
    void setSearch(std::u16string search) __attribute__((weak));
    std::u16string getHash() __attribute__((weak));
    void setHash(std::u16string hash) __attribute__((weak));
    // HTMLAnchorElement-7
    std::u16string getCoords() __attribute__((weak));
    void setCoords(std::u16string coords) __attribute__((weak));
    std::u16string getCharset() __attribute__((weak));
    void setCharset(std::u16string charset) __attribute__((weak));
    std::u16string getName() __attribute__((weak));
    void setName(std::u16string name) __attribute__((weak));
    std::u16string getRev() __attribute__((weak));
    void setRev(std::u16string rev) __attribute__((weak));
    std::u16string getShape() __attribute__((weak));
    void setShape(std::u16string shape) __attribute__((weak));
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
