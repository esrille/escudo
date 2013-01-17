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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLBODYELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLBODYELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLBodyElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/events/EventHandlerNonNull.h>
#include <org/w3c/dom/events/OnErrorEventHandlerNonNull.h>
#include <org/w3c/dom/html/HTMLElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLBodyElementImp : public ObjectMixin<HTMLBodyElementImp, HTMLElementImp>
{
public:
    HTMLBodyElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"body")
    {}
    HTMLBodyElementImp(HTMLBodyElementImp* org, bool deep) :
        ObjectMixin(org, deep)
    {}

    virtual void eval();
    virtual void handleMutation(events::MutationEvent mutation);

    // Node
    virtual Node cloneNode(bool deep = true);
    // HTMLBodyElement
    virtual events::EventHandlerNonNull getOnafterprint();
    virtual void setOnafterprint(events::EventHandlerNonNull onafterprint);
    virtual events::EventHandlerNonNull getOnbeforeprint();
    virtual void setOnbeforeprint(events::EventHandlerNonNull onbeforeprint);
    virtual events::EventHandlerNonNull getOnbeforeunload();
    virtual void setOnbeforeunload(events::EventHandlerNonNull onbeforeunload);
    virtual events::EventHandlerNonNull getOnblur();
    virtual void setOnblur(events::EventHandlerNonNull onblur);
    virtual events::OnErrorEventHandlerNonNull getOnerror();
    virtual void setOnerror(events::OnErrorEventHandlerNonNull onerror);
    virtual events::EventHandlerNonNull getOnfocus();
    virtual void setOnfocus(events::EventHandlerNonNull onfocus);
    virtual events::EventHandlerNonNull getOnhashchange();
    virtual void setOnhashchange(events::EventHandlerNonNull onhashchange);
    virtual events::EventHandlerNonNull getOnload();
    virtual void setOnload(events::EventHandlerNonNull onload);
    virtual events::EventHandlerNonNull getOnmessage();
    virtual void setOnmessage(events::EventHandlerNonNull onmessage);
    virtual events::EventHandlerNonNull getOnoffline();
    virtual void setOnoffline(events::EventHandlerNonNull onoffline);
    virtual events::EventHandlerNonNull getOnonline();
    virtual void setOnonline(events::EventHandlerNonNull ononline);
    virtual events::EventHandlerNonNull getOnpopstate();
    virtual void setOnpopstate(events::EventHandlerNonNull onpopstate);
    virtual events::EventHandlerNonNull getOnpagehide();
    virtual void setOnpagehide(events::EventHandlerNonNull onpagehide);
    virtual events::EventHandlerNonNull getOnpageshow();
    virtual void setOnpageshow(events::EventHandlerNonNull onpageshow);
    virtual events::EventHandlerNonNull getOnresize();
    virtual void setOnresize(events::EventHandlerNonNull onresize);
    virtual events::EventHandlerNonNull getOnscroll();
    virtual void setOnscroll(events::EventHandlerNonNull onscroll);
    virtual events::EventHandlerNonNull getOnstorage();
    virtual void setOnstorage(events::EventHandlerNonNull onstorage);
    virtual events::EventHandlerNonNull getOnunload();
    virtual void setOnunload(events::EventHandlerNonNull onunload);
    // HTMLBodyElement-6
    virtual std::u16string getText();
    virtual void setText(const std::u16string& text);
    virtual std::u16string getBgColor();
    virtual void setBgColor(const std::u16string& bgColor);
    virtual std::u16string getBackground();
    virtual void setBackground(const std::u16string& background);
    virtual std::u16string getLink();
    virtual void setLink(const std::u16string& link);
    virtual std::u16string getVLink();
    virtual void setVLink(const std::u16string& vLink);
    virtual std::u16string getALink();
    virtual void setALink(const std::u16string& aLink);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLBodyElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLBodyElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLBODYELEMENTIMP_H_INCLUDED
