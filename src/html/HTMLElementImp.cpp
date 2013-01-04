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
#include "HTMLTemplateElementImp.h"

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
    addEventListener(u"click", &clickListener, false, true);
    addEventListener(u"mousemove", &mouseMoveListener, false, true);
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
    addEventListener(u"click", &clickListener, false, true);
    addEventListener(u"mousemove", &mouseMoveListener, false, true);
}

HTMLElementImp::~HTMLElementImp()
{
}

void HTMLElementImp::handleClick(events::Event event)
{
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
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();
    if (buttons & 1) {
        Block* block = dynamic_cast<Block*>(getBox());
        if (block && block->canScroll()) {
            setScrollTop(getScrollTop() + moveY - mouse.getScreenY());
            setScrollLeft(getScrollLeft() + moveX - mouse.getScreenX());
            event.preventDefault();
        }
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

void HTMLElementImp::setShadowTree(HTMLTemplateElementImp* e)
{
    shadowTree = e;
}

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
    case EventImp::DEFAULT_PHASE:
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
    Block* block = dynamic_cast<Block*>(getBox());
    if (!block)
        return;
    float overflow = block->getScrollHeight() - block->height;
    scrollTop = std::max(0, std::min(y, static_cast<int>(overflow)));
    block->setFlags(Box::NEED_REPAINT);
}

int HTMLElementImp::getScrollLeft()
{
    return scrollLeft;
}

void HTMLElementImp::setScrollLeft(int x)
{
    Block* block = dynamic_cast<Block*>(getBox());
    if (!block)
        return;
    float overflow = block->getScrollWidth() - block->width;
    scrollLeft = std::max(0, std::min(x, static_cast<int>(overflow)));
    block->setFlags(Box::NEED_REPAINT);
}

int HTMLElementImp::getScrollWidth()
{
    if (Block* block = dynamic_cast<Block*>(getBox()))
        return block->getScrollWidth();
    return 0;
}

int HTMLElementImp::getScrollHeight()
{
    if (Block* block = dynamic_cast<Block*>(getBox()))
        return block->getScrollHeight();
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

std::u16string HTMLElementImp::getTitle()
{
    return getAttribute(u"title");
}

void HTMLElementImp::setTitle(const std::u16string& title)
{
    setAttribute(u"title", title);
}

std::u16string HTMLElementImp::getLang()
{
    return getAttribute(u"lang");
}

void HTMLElementImp::setLang(const std::u16string& lang)
{
    setAttribute(u"lang", lang);
}

bool HTMLElementImp::getTranslate()
{
    // TODO: implement me!
    return 0;
}

void HTMLElementImp::setTranslate(bool translate)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getDir()
{
    return getAttribute(u"dir");
}

void HTMLElementImp::setDir(const std::u16string& dir)
{
    setAttribute(u"dir", dir);
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
    return tabIndex;
}

void HTMLElementImp::setTabIndex(int tabIndex)
{
    this->tabIndex = tabIndex;
}

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

std::u16string HTMLElementImp::getAccessKey()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setAccessKey(const std::u16string& accessKey)
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

void HTMLElementImp::setDropzone(const std::u16string& dropzone)
{
    // TODO: implement me!
}

std::u16string HTMLElementImp::getContentEditable()
{
    // TODO: implement me!
    return u"";
}

void HTMLElementImp::setContentEditable(const std::u16string& contentEditable)
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
    return Nullable<std::u16string>();
}

Nullable<std::u16string> HTMLElementImp::getCommandLabel()
{
    return Nullable<std::u16string>();
}

Nullable<std::u16string> HTMLElementImp::getCommandIcon()
{
    return Nullable<std::u16string>();
}

Nullable<bool> HTMLElementImp::getCommandHidden()
{
    return Nullable<bool>();
}

Nullable<bool> HTMLElementImp::getCommandDisabled()
{
    return Nullable<bool>();
}

Nullable<bool> HTMLElementImp::getCommandChecked()
{
    return Nullable<bool>();
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

events::EventHandlerNonNull HTMLElementImp::getOnabort()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnabort(events::EventHandlerNonNull onabort)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnblur(events::EventHandlerNonNull onblur)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOncancel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncancel(events::EventHandlerNonNull oncancel)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOncanplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncanplay(events::EventHandlerNonNull oncanplay)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOncanplaythrough()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnchange(events::EventHandlerNonNull onchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnclick(events::EventHandlerNonNull onclick)
{
    DocumentImp* document = getOwnerDocumentImp();
    DocumentWindowPtr window = document->activate();
    addEventListener(u"click",
                     new(std::nothrow) EventListenerImp(boost::bind(&ECMAScriptContext::dispatchEvent, window->getContext(), onclick, _1)));
}

events::EventHandlerNonNull HTMLElementImp::getOnclose()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnclose(events::EventHandlerNonNull onclose)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOncontextmenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncontextmenu(events::EventHandlerNonNull oncontextmenu)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOncuechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOncuechange(events::EventHandlerNonNull oncuechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndblclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndblclick(events::EventHandlerNonNull ondblclick)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndrag()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndrag(events::EventHandlerNonNull ondrag)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndragend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragend(events::EventHandlerNonNull ondragend)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndragenter()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragenter(events::EventHandlerNonNull ondragenter)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndragleave()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragleave(events::EventHandlerNonNull ondragleave)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndragover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragover(events::EventHandlerNonNull ondragover)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndragstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndragstart(events::EventHandlerNonNull ondragstart)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndrop()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndrop(events::EventHandlerNonNull ondrop)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOndurationchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOndurationchange(events::EventHandlerNonNull ondurationchange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnemptied()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnemptied(events::EventHandlerNonNull onemptied)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnended()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnended(events::EventHandlerNonNull onended)
{
    // TODO: implement me!
}

events::OnErrorEventHandlerNonNull HTMLElementImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOninput()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOninput(events::EventHandlerNonNull oninput)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOninvalid()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOninvalid(events::EventHandlerNonNull oninvalid)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnkeydown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeydown(events::EventHandlerNonNull onkeydown)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnkeypress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeypress(events::EventHandlerNonNull onkeypress)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnkeyup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnkeyup(events::EventHandlerNonNull onkeyup)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnload(events::EventHandlerNonNull onload)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnloadeddata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadeddata(events::EventHandlerNonNull onloadeddata)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnloadedmetadata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnloadstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnloadstart(events::EventHandlerNonNull onloadstart)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmousedown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousedown(events::EventHandlerNonNull onmousedown)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmousemove()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousemove(events::EventHandlerNonNull onmousemove)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseout()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseout(events::EventHandlerNonNull onmouseout)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseover(events::EventHandlerNonNull onmouseover)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmouseup(events::EventHandlerNonNull onmouseup)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnmousewheel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnmousewheel(events::EventHandlerNonNull onmousewheel)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnpause()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnpause(events::EventHandlerNonNull onpause)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnplay(events::EventHandlerNonNull onplay)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnplaying()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnplaying(events::EventHandlerNonNull onplaying)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnprogress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnprogress(events::EventHandlerNonNull onprogress)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnratechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnratechange(events::EventHandlerNonNull onratechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnreset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnreset(events::EventHandlerNonNull onreset)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnscroll(events::EventHandlerNonNull onscroll)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnseeked()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnseeked(events::EventHandlerNonNull onseeked)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnseeking()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnseeking(events::EventHandlerNonNull onseeking)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnselect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnselect(events::EventHandlerNonNull onselect)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnshow(events::EventHandlerNonNull onshow)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnstalled()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnstalled(events::EventHandlerNonNull onstalled)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnsubmit()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnsubmit(events::EventHandlerNonNull onsubmit)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnsuspend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnsuspend(events::EventHandlerNonNull onsuspend)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOntimeupdate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOntimeupdate(events::EventHandlerNonNull ontimeupdate)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnvolumechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnvolumechange(events::EventHandlerNonNull onvolumechange)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLElementImp::getOnwaiting()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLElementImp::setOnwaiting(events::EventHandlerNonNull onwaiting)
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

bool HTMLElementImp::toUnsigned(std::u16string& value)
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
    return true;
}

bool HTMLElementImp::toPx(std::u16string& value)
{
    if (toUnsigned(value)) {
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
