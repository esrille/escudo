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

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void HTMLBodyElementImp::eval()
{
    HTMLElementImp::eval();
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

// Node
Node HTMLBodyElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLBodyElementImp(this, deep);
}

// HTMLBodyElement

events::EventHandlerNonNull HTMLBodyElementImp::getOnafterprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnafterprint(events::EventHandlerNonNull onafterprint)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnbeforeprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnbeforeprint(events::EventHandlerNonNull onbeforeprint)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnbeforeunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnbeforeunload(events::EventHandlerNonNull onbeforeunload)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnblur(events::EventHandlerNonNull onblur)
{
    // TODO: implement me!
}

events::OnErrorEventHandlerNonNull HTMLBodyElementImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnhashchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnhashchange(events::EventHandlerNonNull onhashchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnload(events::EventHandlerNonNull onload)
{
    getOwnerDocument().getDefaultView().setOnload(onload);
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnmessage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnmessage(events::EventHandlerNonNull onmessage)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnoffline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnoffline(events::EventHandlerNonNull onoffline)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnonline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnonline(events::EventHandlerNonNull ononline)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpopstate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpopstate(events::EventHandlerNonNull onpopstate)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpagehide()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpagehide(events::EventHandlerNonNull onpagehide)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnpageshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpageshow(events::EventHandlerNonNull onpageshow)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnresize()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnresize(events::EventHandlerNonNull onresize)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnscroll(events::EventHandlerNonNull onscroll)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnstorage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnstorage(events::EventHandlerNonNull onstorage)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLBodyElementImp::getOnunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnunload(events::EventHandlerNonNull onunload)
{
    // TODO: implement me!
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
