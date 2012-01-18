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
html::Function HTMLBodyElementImp::getOnafterprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnafterprint(html::Function onafterprint)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnbeforeprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnbeforeprint(html::Function onbeforeprint)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnbeforeunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnbeforeunload(html::Function onbeforeunload)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnblur(html::Function onblur)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnerror(html::Function onerror)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnfocus(html::Function onfocus)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnhashchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnhashchange(html::Function onhashchange)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnload(html::Function onload)
{
    getOwnerDocument().getDefaultView().setOnload(onload);
}

html::Function HTMLBodyElementImp::getOnmessage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnmessage(html::Function onmessage)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnoffline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnoffline(html::Function onoffline)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnonline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnonline(html::Function ononline)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnpopstate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpopstate(html::Function onpopstate)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnpagehide()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpagehide(html::Function onpagehide)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnpageshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnpageshow(html::Function onpageshow)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnredo()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnredo(html::Function onredo)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnresize()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnresize(html::Function onresize)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnstorage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnstorage(html::Function onstorage)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnundo()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnundo(html::Function onundo)
{
    // TODO: implement me!
}

html::Function HTMLBodyElementImp::getOnunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLBodyElementImp::setOnunload(html::Function onunload)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getText()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setText(std::u16string text)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setBgColor(std::u16string bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getBackground()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setBackground(std::u16string background)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getLink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setLink(std::u16string link)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getVLink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setVLink(std::u16string vLink)
{
    // TODO: implement me!
}

std::u16string HTMLBodyElementImp::getALink()
{
    // TODO: implement me!
    return u"";
}

void HTMLBodyElementImp::setALink(std::u16string aLink)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
