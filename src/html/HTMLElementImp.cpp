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

#include "HTMLElementImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "DocumentImp.h"
#include "EventImp.h"
#include "ECMAScript.h"
#include "WindowImp.h"
#include "css/Box.h"
#include "css/CSSParser.h"
#include "css/CSSStyleDeclarationImp.h"
#include "HTMLBindingElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLElementImp::HTMLElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
    ObjectMixin(ownerDocument, localName, u"http://www.w3.org/1999/xhtml"),
    style(0),
    tabIndex(-1),
    scrollTop(0),
    scrollLeft(0),
    clickListener(boost::bind(&HTMLElementImp::handleClick, this, _1)),
    mouseMoveListener(boost::bind(&HTMLElementImp::handleMouseMove, this, _1)),
    bindingImplementation(0),
    shadowTree(0),
    shadowTarget(0),
    shadowImplementation(0)
{
    addEventListener(u"click", &clickListener);
    addEventListener(u"mousemove", &mouseMoveListener);
}

HTMLElementImp::HTMLElementImp(HTMLElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    style(0),
    tabIndex(org->tabIndex),
    scrollTop(0),
    scrollLeft(0),
    clickListener(boost::bind(&HTMLElementImp::handleClick, this, _1)),
    mouseMoveListener(boost::bind(&HTMLElementImp::handleMouseMove, this, _1)),
    bindingImplementation(0), // TODO: clone
    shadowTree(0),            // TODO: clone
    shadowTarget(0),          // TODO: clone
    shadowImplementation(0)   // TODO: clone
{
    if (auto orgStyle = dynamic_cast<CSSStyleDeclarationImp*>(org->style.self())) {
        CSSStyleDeclarationImp* imp = new(std::nothrow) CSSStyleDeclarationImp(orgStyle);
        if (imp) {
            imp->setOwner(this);
            style = imp;
        }
    }
    addEventListener(u"click", &clickListener);
    addEventListener(u"mousemove", &mouseMoveListener);
}

HTMLElementImp::~HTMLElementImp()
{
}

void HTMLElementImp::handleClick(events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    switch (mouse.getButton()) {
    case 0:
        moveX = mouse.getScreenX();
        moveY = mouse.getScreenY();
        focus();
        break;
    default:
        break;
    }
}

void HTMLElementImp::handleMouseMove(events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();
    if (buttons & 1) {
        setScrollTop(getScrollTop() + moveY - mouse.getScreenY());
        setScrollLeft(getScrollLeft() + moveX - mouse.getScreenX());
    }
    moveX = mouse.getScreenX();
    moveY = mouse.getScreenY();
}

void HTMLElementImp::eval()
{
    DocumentWindowPtr window = getOwnerDocumentImp()->activate();
    Nullable<std::u16string> attr = getAttribute(u"style");
    if (attr.hasValue()) {
        CSSParser parser;
        style = parser.parseDeclarations(attr.value());
        parser.getStyleDeclaration()->setOwner(this);
    }
    attr = getAttribute(u"onclick");
    if (attr.hasValue())
        setOnclick(window->getContext()->compileFunction(attr.value()));
    attr = getAttribute(u"onload");
    if (attr.hasValue())
        setOnload(window->getContext()->compileFunction(attr.value()));
}

Box* HTMLElementImp::getBox()
{
    html::Window window = getOwnerDocument().getDefaultView();
    if (!window)
        return 0;
    css::CSSStyleDeclaration style = window.getComputedStyle(this);
    if (!style)
        return 0;
    // TODO: Fix MVC violation
    CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(style.self());
    if (!imp)
        return 0;
    return imp->getBox();
}

// XBL 2.0 internal
void HTMLElementImp::generateShadowContent(CSSStyleDeclarationImp* style)
{
    if (style->binding.getValue() != CSSBindingValueImp::Uri ||
        style->display.getValue() == CSSDisplayValueImp::None)
        return;
    if (getShadowTree())  // already attached?
        return;
    DocumentImp* document = getOwnerDocumentImp();
    assert(document);
    URL base(document->getDocumentURI());
    URL url(base, style->binding.getURL());
    if (!base.isSameExceptFragments(url)) {
        document = dynamic_cast<DocumentImp*>(document->loadBindingDocument(url).self());
        if (!document || document->getReadyState() != u"complete")
            return;
    }

    std::u16string hash = url.getHash();
    if (hash[0] == '#')
        hash.erase(0, 1);
    Element element = document->getElementById(hash);
    if (!element)
        return;
    auto binding = dynamic_cast<HTMLBindingElementImp*>(element.self());
    if (!binding)
        return;
    bindingImplementation = binding->getImplementation();
    if (!bindingImplementation)
        return;
    if (html::HTMLTemplateElement shadowTree = binding->cloneTemplate()) {
        setShadowTree(shadowTree);
        shadowTarget = new(std::nothrow) EventTargetImp;
        // TODO: if (not called from the background thread) {
#if 0
            DocumentWindowPtr window = document->activate();
            ECMAScriptContext* context = window->getContext();
            shadowImplementation = context->xblCreateImplementation(shadowTarget, bindingImplementation, this, shadowTree);
            shadowImplementation .xblEnteredDocument();
        }
#endif
    }
}

void HTMLElementImp::xblEnteredDocument(Node node)
{
    while (node) {
        if (auto element = dynamic_cast<HTMLElementImp*>(node.self())) {
            if (element->shadowTarget && !element->shadowImplementation) {
                DocumentImp* document = element->getOwnerDocumentImp();
                DocumentWindowPtr window = document->activate();
                ECMAScriptContext* context = window->getContext();
                element->shadowImplementation = context->xblCreateImplementation(element->shadowTarget, element->bindingImplementation, element, element->shadowTree);
                element->shadowImplementation.xblEnteredDocument();
            }
        }
        if (node.hasChildNodes())
            xblEnteredDocument(node.getFirstChild());
        node = node.getNextSibling();
    }
}

void HTMLElementImp::invokeShadowTarget(EventImp* event)
{
    auto currentWindow = static_cast<WindowImp*>(ECMAScriptContext::getCurrent());
    if (auto shadowDocument = dynamic_cast<DocumentImp*>(shadowTree.getOwnerDocument().self()))
        shadowDocument->activate();
    dynamic_cast<EventTargetImp*>(shadowTarget.self())->invoke(event);
    if (currentWindow)
        currentWindow->activate();
}

void HTMLElementImp::invoke(EventImp* event)
{
    if (!shadowTarget) {
        EventTargetImp::invoke(event);
        return;
    }
    event->setCurrentTarget(this);
    switch (event->getEventPhase()) {
    case events::Event::CAPTURING_PHASE:
        EventTargetImp::invoke(event);
        invokeShadowTarget(event);
        break;
    case events::Event::BUBBLING_PHASE:
    case events::Event::AT_TARGET:
        invokeShadowTarget(event);
        EventTargetImp::invoke(event);
        break;
    default:
        break;
    }

}

// Node
Node HTMLElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLElementImp(this, deep);
}

// Element

views::ClientRectList HTMLElementImp::getClientRects()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

views::ClientRect HTMLElementImp::getBoundingClientRect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

// cf. http://www.w3.org/TR/cssom-view/#scroll-an-element-into-view
void HTMLElementImp::scrollIntoView(bool top)
{
    if (html::Window window = getOwnerDocument().getDefaultView()) {
        if (Box* box = getBox()) {
            int x = box->getX() + box->getMarginLeft();
            int y = box->getY() + box->getMarginTop();
            if (!top)
                y += box->getBorderHeight();
            window.scroll(x, y);
        }
    }
}

int HTMLElementImp::getScrollTop()
{
    return scrollTop;
}

void HTMLElementImp::setScrollTop(int y)
{
    BlockLevelBox* box = dynamic_cast<BlockLevelBox*>(getBox());
    if (!box)
        return;
    float overflow = 0.0f;
    for (Box* child = box->getFirstChild(); child; child = child->getNextSibling())
        overflow += child->getTotalHeight();
    overflow -= box->height;
    scrollTop = std::max(0, std::min(y, static_cast<int>(overflow)));
    box->setFlags(2);
}

int HTMLElementImp::getScrollLeft()
{
    return scrollLeft;
}

void HTMLElementImp::setScrollLeft(int x)
{
    BlockLevelBox* box = dynamic_cast<BlockLevelBox*>(getBox());
    if (!box)
        return;
    float overflow = 0.0f;
    for (Box* child = box->getFirstChild(); child; child = child->getNextSibling())
        overflow = std::max(overflow, child->getTotalWidth());
    overflow -= box->width;
    scrollLeft = std::max(0, std::min(x, static_cast<int>(overflow)));
    box->setFlags(2);
}

int HTMLElementImp::getScrollWidth()
{
    if (Box* box = getBox())
        return box->getPaddingWidth();
    return 0;
}

int HTMLElementImp::getScrollHeight()
{
    if (Box* box = getBox())
        return box->getPaddingHeight();
    return 0;
}

int HTMLElementImp::getClientTop()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getClientLeft()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getClientWidth()
{
    // TODO: implement me!
    return 0;
}

int HTMLElementImp::getClientHeight()
{
    // TODO: implement me!
    return 0;
}

// HTMLElement - implemented

void HTMLElementImp::focus()
{
    if (!ownerDocument || !ownerDocument->getDefaultView() || getTabIndex() < 0)
        return;
    ownerDocument->setFocus(this);
}

void HTMLElementImp::blur()
{
    if (!ownerDocument || ownerDocument->getFocus() != this)
        return;
    ownerDocument->setFocus(0);
}

css::CSSStyleDeclaration HTMLElementImp::getStyle()
{
    if (!style) {
        CSSStyleDeclarationImp* imp = new(std::nothrow) CSSStyleDeclarationImp;
        if (imp)
            imp->setOwner(this);
        style = imp;
    }
    return style;
}

// HTMLElement - just genereated

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
    return getAttribute(u"id");
}

void HTMLElementImp::setId(std::u16string id)
{
    setAttribute(u"id", id);
}

std::u16string HTMLElementImp::getTitle()
{
    return getAttribute(u"title");
}

void HTMLElementImp::setTitle(std::u16string title)
{
    setAttribute(u"title", title);
}

std::u16string HTMLElementImp::getLang()
{
    return getAttribute(u"lang");
}

void HTMLElementImp::setLang(std::u16string lang)
{
    setAttribute(u"lang", lang);
}

std::u16string HTMLElementImp::getDir()
{
    return getAttribute(u"dir");
}

void HTMLElementImp::setDir(std::u16string dir)
{
    setAttribute(u"dir", dir);
}

std::u16string HTMLElementImp::getClassName()
{
    return getAttribute(u"class");
}

void HTMLElementImp::setClassName(std::u16string className)
{
    setAttribute(u"class", className);
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
    return tabIndex;
}

void HTMLElementImp::setTabIndex(int tabIndex)
{
    this->tabIndex = tabIndex;
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

DOMSettableTokenList HTMLElementImp::getDropzone()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setDropzone(std::u16string dropzone)
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
    return 0;
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
    DocumentImp* document = getOwnerDocumentImp();
    DocumentWindowPtr window = document->activate();
    addEventListener(u"click",
                     new(std::nothrow) EventListenerImp(boost::bind(&ECMAScriptContext::dispatchEvent, window->getContext(), onclick, _1)),
                     false);
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

html::Function HTMLElementImp::getOncuechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncuechange(html::Function oncuechange)
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

html::Function HTMLElementImp::getOnreset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnreset(html::Function onreset)
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

bool HTMLElementImp::toPx(std::u16string& value)
{
    stripLeadingAndTrailingWhitespace(value);
    if (value.empty())
        return false;
    const char16_t* s = value.c_str();
    while (*s) {
        if (!isDigit(*s))
            return false;
        ++s;
    }
    if (!*s) {
        value += u"px";
        return true;
    }
    return false;
}

bool HTMLElementImp::toPxOrPercentage(std::u16string& value)
{
    stripLeadingAndTrailingWhitespace(value);
    if (value.empty())
        return false;
    const char16_t* s = value.c_str();
    while (*s) {
        if (*s == '%')
            break;
        if (!isDigit(*s))
            return false;
        ++s;
    }
    if (!*s) {
        value += u"px";
        return true;
    }
    assert(*s == '%');
    if (!s[1] && 1 < value.length())
        return true;
    return false;
}

bool HTMLElementImp::evalBackground(HTMLElementImp* element)
{
    Nullable<std::u16string> attr = element->getAttribute(u"background");
    if (attr.hasValue()) {
        css::CSSStyleDeclaration style = element->getStyle();
        style.setProperty(u"background-image", u"url(" + attr.value() + u")", u"non-css");
        return true;
    }
    return false;
}

bool HTMLElementImp::evalColor(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop)
{
    Nullable<std::u16string> value = element->getAttribute(attr);
    if (value.hasValue()) {
        css::CSSStyleDeclaration style = element->getStyle();
        style.setProperty(prop, value.value(), u"non-css");
        return true;
    }
    return false;
}

bool HTMLElementImp::evalPx(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop)
{
    Nullable<std::u16string> value = element->getAttribute(attr);
    if (value.hasValue()) {
        std::u16string length = value.value();
        if (toPx(length)) {
            css::CSSStyleDeclaration style = element->getStyle();
            style.setProperty(prop, length, u"non-css");
            return true;
        }
    }
    return false;
}

bool HTMLElementImp::evalPxOrPercentage(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop)
{
    Nullable<std::u16string> value = element->getAttribute(attr);
    if (value.hasValue()) {
        std::u16string length = value.value();
        if (toPxOrPercentage(length)) {
            css::CSSStyleDeclaration style = element->getStyle();
            style.setProperty(prop, length, u"non-css");
            return true;
        }
    }
    return false;
}

bool HTMLElementImp::evalBorder(HTMLElementImp* element)
{
    Nullable<std::u16string> value = element->getAttribute(u"border");
    if (value.hasValue()) {
        std::u16string px = value.value();
        if (toPx(px)) {
            css::CSSStyleDeclaration style = element->getStyle();
            style.setProperty(u"border-width", px, u"non-css");
            style.setProperty(u"border-style", u"solid", u"non-css");
            return true;
        }
    }
    return false;
}

bool HTMLElementImp::evalHspace(HTMLElementImp* element, const std::u16string& prop)
{
    Nullable<std::u16string> value = element->getAttribute(prop);
    if (value.hasValue()) {
        std::u16string px = value.value();
        if (toPx(px)) {
            css::CSSStyleDeclaration style = element->getStyle();
            style.setProperty(u"margin-left", px, u"non-css");
            style.setProperty(u"margin-right", px, u"non-css");
            return true;
        }
    }
    return false;
}

bool HTMLElementImp::evalVspace(HTMLElementImp* element, const std::u16string& prop)
{
    Nullable<std::u16string> value = element->getAttribute(prop);
    if (value.hasValue()) {
        std::u16string px = value.value();
        if (toPx(px)) {
            css::CSSStyleDeclaration style = element->getStyle();
            style.setProperty(u"margin-top", px, u"non-css");
            style.setProperty(u"margin-bottom", px, u"non-css");
            return true;
        }
    }
    return false;
}

bool HTMLElementImp::evalNoWrap(HTMLElementImp* element)
{
    Nullable<std::u16string> value = element->getAttribute(u"nowrap");
    if (value.hasValue()) {
        element->getStyle().setProperty(u"white-space", u"nowrap", u"non-css");
        return true;
    }
    return false;
}

bool HTMLElementImp::evalValign(HTMLElementImp* element)
{
    Nullable<std::u16string> value = element->getAttribute(u"valign");
    if (value.hasValue()) {
        element->getStyle().setProperty(u"vertical-align", value.value(), u"non-css");
        return true;
    }
    return false;
}

}}}}  // org::w3c::dom::bootstrap
