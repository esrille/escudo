/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "HTMLElementImp.h"

#include "DocumentImp.h"
#include "css/CSSParser.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLElementImp::HTMLElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
    ObjectMixin(ownerDocument, localName, u"http://www.w3.org/1999/xhtml"),
    style(0)
{
}

HTMLElementImp::HTMLElementImp(HTMLElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    style(org->style)  // TODO: clone
{
}

HTMLElementImp::~HTMLElementImp()
{
}

void HTMLElementImp::eval()
{
    Nullable<std::u16string> attr = getAttribute(u"style");
    if (!attr.hasValue())
        return;
    CSSParser parser;
    style = parser.parseDeclarations(attr.value());
}

// Node
Node HTMLElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLElementImp(this, deep);
}

// HTMLElement
NodeList HTMLElementImp::getElementsByClassName(std::u16string classNames)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLElementImp::getInnerHTML()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setInnerHTML(std::u16string innerHTML)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getOuterHTML()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setOuterHTML(std::u16string outerHTML)
{
    // TODO: implement me!
}

void HTMLElementImp::insertAdjacentHTML(std::u16string position, std::u16string text)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getId()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setId(std::u16string id)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getTitle()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setTitle(std::u16string title)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getLang()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setLang(std::u16string lang)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getDir()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setDir(std::u16string dir)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getClassName()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setClassName(std::u16string className)
{
    // TODO: implement me!
}

DOMTokenList HTMLElementImp::getClassList()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

DOMStringMap HTMLElementImp::getDataset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

bool HTMLElementImp::getHidden()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setHidden(bool hidden)
{
    // TODO: implement me!
}

void HTMLElementImp::click()
{
    // TODO: implement me!
}

int HTMLElementImp::getTabIndex()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setTabIndex(int tabIndex)
{
    // TODO: implement me!
}

void HTMLElementImp::focus()
{
    // TODO: check if this element is focusable.
    if (!ownerDocument || !ownerDocument->getDefaultView())
        return;
    ownerDocument->setFocus(this);
}

void HTMLElementImp::blur()
{
    if (!ownerDocument || ownerDocument->getFocus() != this)
        return;
    ownerDocument->setFocus(0);
}

std::u16string HTMLElementImp::getAccessKey()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setAccessKey(std::u16string accessKey)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getAccessKeyLabel()
{
    // TODO: implement me!
    return u"";
}

bool HTMLElementImp::getDraggable()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setDraggable(bool draggable)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getContentEditable()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setContentEditable(std::u16string contentEditable)
{
    // TODO: implement me!
}

bool HTMLElementImp::getIsContentEditable()
{
    // TODO: implement me!
    return 0;
}

html::HTMLMenuElement HTMLElementImp::getContextMenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setContextMenu(html::HTMLMenuElement contextMenu)
{
    // TODO: implement me!
}

bool HTMLElementImp::getSpellcheck()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setSpellcheck(bool spellcheck)
{
    // TODO: implement me!
}

Nullable<std::u16string> HTMLElementImp::getCommandType()
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> HTMLElementImp::getLabel()
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> HTMLElementImp::getIcon()
{
    // TODO: implement me!
    return u"";
}

bool HTMLElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

bool HTMLElementImp::getChecked()
{
    // TODO: implement me!
    return 0;
}

css::CSSStyleDeclaration HTMLElementImp::getStyle()
{
    return style;
}

html::Function HTMLElementImp::getOnabort()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnabort(html::Function onabort)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnblur(html::Function onblur)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOncanplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncanplay(html::Function oncanplay)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOncanplaythrough()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncanplaythrough(html::Function oncanplaythrough)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnchange(html::Function onchange)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnclick(html::Function onclick)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOncontextmenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncontextmenu(html::Function oncontextmenu)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndblclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndblclick(html::Function ondblclick)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndrag()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndrag(html::Function ondrag)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndragend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragend(html::Function ondragend)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndragenter()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragenter(html::Function ondragenter)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndragleave()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragleave(html::Function ondragleave)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndragover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragover(html::Function ondragover)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndragstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragstart(html::Function ondragstart)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndrop()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndrop(html::Function ondrop)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOndurationchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndurationchange(html::Function ondurationchange)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnemptied()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnemptied(html::Function onemptied)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnended()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnended(html::Function onended)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnerror(html::Function onerror)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnfocus(html::Function onfocus)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOninput()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOninput(html::Function oninput)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOninvalid()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOninvalid(html::Function oninvalid)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnkeydown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeydown(html::Function onkeydown)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnkeypress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeypress(html::Function onkeypress)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnkeyup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeyup(html::Function onkeyup)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnload(html::Function onload)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnloadeddata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadeddata(html::Function onloadeddata)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnloadedmetadata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadedmetadata(html::Function onloadedmetadata)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnloadstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadstart(html::Function onloadstart)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmousedown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousedown(html::Function onmousedown)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmousemove()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousemove(html::Function onmousemove)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmouseout()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseout(html::Function onmouseout)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmouseover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseover(html::Function onmouseover)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmouseup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseup(html::Function onmouseup)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnmousewheel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousewheel(html::Function onmousewheel)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnpause()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnpause(html::Function onpause)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnplay(html::Function onplay)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnplaying()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnplaying(html::Function onplaying)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnprogress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnprogress(html::Function onprogress)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnratechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnratechange(html::Function onratechange)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnreadystatechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnreadystatechange(html::Function onreadystatechange)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnscroll(html::Function onscroll)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnseeked()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnseeked(html::Function onseeked)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnseeking()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnseeking(html::Function onseeking)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnselect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnselect(html::Function onselect)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnshow(html::Function onshow)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnstalled()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnstalled(html::Function onstalled)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnsubmit()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnsubmit(html::Function onsubmit)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnsuspend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnsuspend(html::Function onsuspend)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOntimeupdate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOntimeupdate(html::Function ontimeupdate)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnvolumechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnvolumechange(html::Function onvolumechange)
{
    // TODO: implement me!
}

html::Function HTMLElementImp::getOnwaiting()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnwaiting(html::Function onwaiting)
{
    // TODO: implement me!
}

Element HTMLElementImp::getOffsetParent()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

int HTMLElementImp::getOffsetTop()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getOffsetLeft()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getOffsetWidth()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getOffsetHeight()
{
    // TODO: implement me!
    return 0;
}

bool HTMLElementImp::getItemScope()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setItemScope(bool itemScope)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getItemType()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setItemType(std::u16string itemType)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getItemId()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setItemId(std::u16string itemId)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLElementImp::getItemRef()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setItemRef(std::u16string itemRef)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLElementImp::getItemProp()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setItemProp(std::u16string itemProp)
{
    // TODO: implement me!
}

html::HTMLPropertiesCollection HTMLElementImp::getProperties()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Any HTMLElementImp::getItemValue()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setItemValue(Any itemValue)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
