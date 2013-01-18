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

#include "HTMLBodyElementImp.h"

#include <org/w3c/dom/html/Window.h>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "DocumentImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void HTMLBodyElementImp::eval()
{
    HTMLElementImp::evalBackground(this);
    HTMLElementImp::evalBgcolor(this);
    HTMLElementImp::evalColor(this, u"text", u"color");
    HTMLElementImp::evalPx(this, u"margintop", u"margin-top");
    HTMLElementImp::evalPx(this, u"marginright", u"margin-right");
    HTMLElementImp::evalPx(this, u"marginbottom", u"margin-bottom");
    HTMLElementImp::evalPx(this, u"marginleft", u"margin-left");
    HTMLElementImp::evalMarginHeight(this);
    HTMLElementImp::evalMarginWidth(this);
}

void HTMLBodyElementImp::handleMutation(events::MutationEvent mutation)
{
    DocumentWindowPtr window = getOwnerDocumentImp()->activate();
    std::u16string value = mutation.getNewValue();
    bool compile = false;
    if (!value.empty()) {
        switch (mutation.getAttrChange()) {
        case events::MutationEvent::MODIFICATION:
        case events::MutationEvent::ADDITION:
            compile = true;
            break;
        default:
            break;
        }
    }
    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"onafterprint"):
        setOnafterprint(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onbeforeprint"):
        setOnbeforeprint(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onbeforeunload"):
        setOnbeforeunload(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onblur"):
        setOnblur(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onerror"):
        setOnerror(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onfocus"):
        setOnfocus(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onhashchange"):
        setOnhashchange(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onload"):
        setOnload(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onmessage"):
        setOnmessage(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onoffline"):
        setOnoffline(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"ononline"):
        setOnonline(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onpopstate"):
        setOnpopstate(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onpagehide"):
        setOnpagehide(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onresize"):
        setOnresize(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onscroll"):
        setOnscroll(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onstorage"):
        setOnstorage(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    case Intern(u"onunload"):
        setOnunload(compile ? window->getContext()->compileFunction(value) : 0);
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

// Node
Node HTMLBodyElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLBodyElementImp(this, deep);
}

// HTMLBodyElement

events::EventHandlerNonNull HTMLBodyElementImp::getOnafterprint()
{
    return getOwnerDocument().getDefaultView().getOnafterprint();
}

void HTMLBodyElementImp::setOnafterprint(events::EventHandlerNonNull onafterprint)
{
    getOwnerDocument().getDefaultView().setOnafterprint(onafterprint);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnbeforeprint()
{
    return getOwnerDocument().getDefaultView().getOnbeforeprint();
}

void HTMLBodyElementImp::setOnbeforeprint(events::EventHandlerNonNull onbeforeprint)
{
    getOwnerDocument().getDefaultView().setOnbeforeprint(onbeforeprint);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnbeforeunload()
{
    return getOwnerDocument().getDefaultView().getOnbeforeunload();
}

void HTMLBodyElementImp::setOnbeforeunload(events::EventHandlerNonNull onbeforeunload)
{
    getOwnerDocument().getDefaultView().setOnbeforeunload(onbeforeunload);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnblur()
{
    return getOwnerDocument().getDefaultView().getOnblur();
}

void HTMLBodyElementImp::setOnblur(events::EventHandlerNonNull onblur)
{
    getOwnerDocument().getDefaultView().setOnblur(onblur);
}

events::OnErrorEventHandlerNonNull HTMLBodyElementImp::getOnerror()
{
    return getOwnerDocument().getDefaultView().getOnerror();
}

void HTMLBodyElementImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    getOwnerDocument().getDefaultView().setOnerror(onerror);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnfocus()
{
    return getOwnerDocument().getDefaultView().getOnfocus();
}

void HTMLBodyElementImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    getOwnerDocument().getDefaultView().setOnfocus(onfocus);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnhashchange()
{
    return getOwnerDocument().getDefaultView().getOnhashchange();
}

void HTMLBodyElementImp::setOnhashchange(events::EventHandlerNonNull onhashchange)
{
    getOwnerDocument().getDefaultView().setOnhashchange(onhashchange);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnload()
{
    return getOwnerDocument().getDefaultView().getOnload();
}

void HTMLBodyElementImp::setOnload(events::EventHandlerNonNull onload)
{
    getOwnerDocument().getDefaultView().setOnload(onload);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnmessage()
{
    return getOwnerDocument().getDefaultView().getOnmessage();
}

void HTMLBodyElementImp::setOnmessage(events::EventHandlerNonNull onmessage)
{
    getOwnerDocument().getDefaultView().setOnmessage(onmessage);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnoffline()
{
    return getOwnerDocument().getDefaultView().getOnoffline();
}

void HTMLBodyElementImp::setOnoffline(events::EventHandlerNonNull onoffline)
{
    getOwnerDocument().getDefaultView().setOnoffline(onoffline);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnonline()
{
    return getOwnerDocument().getDefaultView().getOnonline();
}

void HTMLBodyElementImp::setOnonline(events::EventHandlerNonNull ononline)
{
    getOwnerDocument().getDefaultView().setOnonline(ononline);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpopstate()
{
    return getOwnerDocument().getDefaultView().getOnpopstate();
}

void HTMLBodyElementImp::setOnpopstate(events::EventHandlerNonNull onpopstate)
{
    getOwnerDocument().getDefaultView().setOnpopstate(onpopstate);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpagehide()
{
    return getOwnerDocument().getDefaultView().getOnpagehide();
}

void HTMLBodyElementImp::setOnpagehide(events::EventHandlerNonNull onpagehide)
{
    getOwnerDocument().getDefaultView().setOnpagehide(onpagehide);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpageshow()
{
    return getOwnerDocument().getDefaultView().getOnpageshow();
}

void HTMLBodyElementImp::setOnpageshow(events::EventHandlerNonNull onpageshow)
{
    getOwnerDocument().getDefaultView().setOnpageshow(onpageshow);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnresize()
{
    return getOwnerDocument().getDefaultView().getOnresize();
}

void HTMLBodyElementImp::setOnresize(events::EventHandlerNonNull onresize)
{
    getOwnerDocument().getDefaultView().setOnresize(onresize);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnscroll()
{
    return getOwnerDocument().getDefaultView().getOnscroll();
}

void HTMLBodyElementImp::setOnscroll(events::EventHandlerNonNull setOnscroll)
{
    getOwnerDocument().getDefaultView().setOnscroll(setOnscroll);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnstorage()
{
    return getOwnerDocument().getDefaultView().getOnstorage();
}

void HTMLBodyElementImp::setOnstorage(events::EventHandlerNonNull onstorage)
{
    getOwnerDocument().getDefaultView().setOnstorage(onstorage);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnunload()
{
    return getOwnerDocument().getDefaultView().getOnunload();
}

void HTMLBodyElementImp::setOnunload(events::EventHandlerNonNull onunload)
{
    getOwnerDocument().getDefaultView().setOnunload(onunload);
}

std::u16string HTMLBodyElementImp::getText()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setText(const std::u16string& text)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getBackground()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setBackground(const std::u16string& background)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getLink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setLink(const std::u16string& link)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getVLink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setVLink(const std::u16string& vLink)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getALink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setALink(const std::u16string& aLink)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
