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
#include "ECMAScript.h"
#include "HTMLUtil.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void HTMLBodyElementImp::handleMutationMarginHeight()
{
    // TODO: Check container frame element's marginheight
    std::u16string value;
    css::CSSStyleDeclaration style(getStyle());
    if (mapToPixelLength(value = getAttribute(u"marginheight"))) {
        style.setProperty(u"margin-top", value, u"non-css");
        style.setProperty(u"margin-bottom", value, u"non-css");
    } else {
        if (mapToPixelLength(value = getAttribute(u"topmargin")))
            style.setProperty(u"margin-top", value, u"non-css");
        if (mapToPixelLength(value = getAttribute(u"bottommargin")))
            style.setProperty(u"margin-bottom", value, u"non-css");
    }
}

void HTMLBodyElementImp::handleMutationMarginWidth()
{
    // TODO: Check container frame element's marginwidth
    std::u16string value;
    css::CSSStyleDeclaration style(getStyle());
    if (mapToPixelLength(value = getAttribute(u"marginwidth"))) {
        style.setProperty(u"margin-left", value, u"non-css");
        style.setProperty(u"margin-right", value, u"non-css");
    } else {
        if (mapToPixelLength(value = getAttribute(u"leftmargin")))
            style.setProperty(u"margin-left", value, u"non-css");
        if (mapToPixelLength(value = getAttribute(u"rightmargin")))
            style.setProperty(u"margin-right", value, u"non-css");
    }
}

void HTMLBodyElementImp::handleMutation(events::MutationEvent mutation)
{
    ECMAScriptContext* context = getOwnerDocumentImp()->getContext();
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
    // Styles
    case Intern(u"background"):
        handleMutationBackground(mutation);
        break;
    case Intern(u"bgcolor"):
        handleMutationColor(mutation, u"background-color");
        break;
    case Intern(u"marginheight"):
    case Intern(u"topmargin"):
    case Intern(u"bottommargin"):
        handleMutationMarginHeight();
        break;
    case Intern(u"marginwidth"):
    case Intern(u"leftmargin"):
    case Intern(u"rightmargin"):
        handleMutationMarginWidth();
        break;
    case Intern(u"text"):
        handleMutationColor(mutation, u"color");
        break;
    // Event handlers
    case Intern(u"onafterprint"):
        setOnafterprint(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onbeforeprint"):
        setOnbeforeprint(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onbeforeunload"):
        setOnbeforeunload(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onblur"):
        setOnblur(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onerror"):
        setOnerror(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onfocus"):
        setOnfocus(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onhashchange"):
        setOnhashchange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onload"):
        setOnload(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmessage"):
        setOnmessage(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onoffline"):
        setOnoffline(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ononline"):
        setOnonline(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onpopstate"):
        setOnpopstate(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onpagehide"):
        setOnpagehide(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onresize"):
        setOnresize(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onscroll"):
        setOnscroll(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onstorage"):
        setOnstorage(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onunload"):
        setOnunload(compile ? context->compileFunction(value).self() : nullptr);
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
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
