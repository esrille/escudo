/*
 * Copyright 2010-2014 Esrille Inc.
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
#include <boost/lexical_cast.hpp>

#include <org/w3c/dom/events/MouseEvent.h>
#include <org/w3c/dom/events/MutationEvent.h>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "utf.h"

#include "DocumentImp.h"
#include "EventImp.h"
#include "ECMAScript.h"
#include "WindowProxy.h"
#include "css/Box.h"
#include "css/CSSParser.h"
#include "css/CSSStyleDeclarationImp.h"
#include "css/ViewCSSImp.h"
#include "HTMLBindingElementImp.h"
#include "HTMLTemplateElementImp.h"
#include "HTMLUtil.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLElementImp::HTMLElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
    ObjectMixin(ownerDocument, localName, u"http://www.w3.org/1999/xhtml"),
    scrollTop(0),
    scrollLeft(0),
    clickListener(boost::bind(&HTMLElementImp::handleClick, this, _1, _2)),
    mouseMoveListener(boost::bind(&HTMLElementImp::handleMouseMove, this, _1, _2)),
    mutationListener(boost::bind(&HTMLElementImp::handleMutation, this, _1, _2)),
    tabIndex(-1)
{
    addEventListener(u"click", clickListener, false, EventTargetImp::UseDefault);
    addEventListener(u"mousemove", mouseMoveListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
}

HTMLElementImp::HTMLElementImp(const HTMLElementImp& org) :
    ObjectMixin(org),
    scrollTop(0),
    scrollLeft(0),
    clickListener(boost::bind(&HTMLElementImp::handleClick, this, _1, _2)),
    mouseMoveListener(boost::bind(&HTMLElementImp::handleMouseMove, this, _1, _2)),
    mutationListener(boost::bind(&HTMLElementImp::handleMutation, this, _1, _2)),
    tabIndex(org.tabIndex)
{
    addEventListener(u"click", clickListener, false, EventTargetImp::UseDefault);
    addEventListener(u"mousemove", mouseMoveListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
}

HTMLElementImp::~HTMLElementImp()
{
}

void HTMLElementImp::handleClick(EventListenerImp* listener, events::Event event)
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

void HTMLElementImp::handleMouseMove(EventListenerImp* listener, events::Event event)
{
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();
    if (buttons & 1) {
        BlockPtr block = std::dynamic_pointer_cast<Block>(getBox());
        if (block && block->canScroll()) {
            setScrollTop(getScrollTop() + moveY - mouse.getScreenY());
            setScrollLeft(getScrollLeft() + moveX - mouse.getScreenX());
            event.preventDefault();
        }
    }
    moveX = mouse.getScreenX();
    moveY = mouse.getScreenY();
}

void HTMLElementImp::handleMutation(EventListenerImp* listener, events::Event event)
{
    if (event.getCurrentTarget() != event.getTarget())
        return;
    handleMutation(interface_cast<events::MutationEvent>(event));
}

void HTMLElementImp::handleMutation(events::MutationEvent mutation)
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
    case Intern(u"style"):
        if (auto imp = std::dynamic_pointer_cast<CSSStyleDeclarationImp>(getStyle().self())) {
            if (!imp->isMutated()) {
                imp->clearProperties();
                if (!value.empty()) {
                    CSSParser parser;
                    parser.setStyleDeclaration(imp);
                    parser.parseDeclarations(value);
                }
            }
        }
        break;
    case Intern(u"tabindex"):
        if (!toInteger(value, tabIndex))
            tabIndex = -1;
        break;
    // Event handlers
    case Intern(u"onabort"):
        setOnabort(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onblur"):
        setOnblur(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oncancel"):
        setOncancel(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oncanplay"):
        setOncanplay(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oncanplaythrough"):
        setOncanplaythrough(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onchange"):
        setOnchange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onclick"):
        setOnclick(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onclose"):
        setOnclose(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oncontextmenu"):
        setOncontextmenu(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oncuechange"):
        setOncuechange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondblclick"):
        setOndblclick(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondrag"):
        setOndrag(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondragend"):
        setOndragend(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondragenter"):
        setOndragenter(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondragleave"):
        setOndragleave(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondragover"):
        setOndragover(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondragstart"):
        setOndragstart(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondrop"):
        setOndrop(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ondurationchange"):
        setOndurationchange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onemptied"):
        setOnemptied(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onended"):
        setOnended(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onerror"):
        setOnerror(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onfocus"):
        setOnfocus(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oninput"):
        setOninput(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"oninvalid"):
        setOninvalid(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onkeydown"):
        setOnkeydown(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onkeypress"):
        setOnkeypress(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onkeyup"):
        setOnkeyup(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onload"):
        setOnload(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onloadeddata"):
        setOnloadeddata(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onloadedmetadata"):
        setOnloadedmetadata(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onloadstart"):
        setOnloadstart(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmousedown"):
        setOnmousedown(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmousemove"):
        setOnmousemove(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmouseout"):
        setOnmouseout(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmouseover"):
        setOnmouseover(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmouseup"):
        setOnmouseup(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onmousewheel"):
        setOnmousewheel(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onpause"):
        setOnpause(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onplay"):
        setOnplay(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onplaying"):
        setOnplaying(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onprogress"):
        setOnprogress(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onratechange"):
        setOnratechange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onreset"):
        setOnreset(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onscroll"):
        setOnscroll(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onseeked"):
        setOnseeked(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onseeking"):
        setOnseeking(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onselect"):
        setOnselect(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onshow"):
        setOnshow(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onstalled"):
        setOnstalled(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onsubmit"):
        setOnsubmit(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onsuspend"):
        setOnsuspend(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"ontimeupdate"):
        setOntimeupdate(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onvolumechange"):
        setOnvolumechange(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"onwaiting"):
        setOnwaiting(compile ? context->compileFunction(value).self() : nullptr);
        break;
    case Intern(u"title"):
    case Intern(u"lang"):
    case Intern(u"translate"):
    case Intern(u"dir"):
    case Intern(u"hidden"):
    default:
        break;
    }
}

BoxPtr HTMLElementImp::getBox()
{
    DocumentPtr owner = getOwnerDocumentImp();
    if (!owner)
        return 0;
    WindowProxyPtr window = owner->getDefaultWindow();
    if (!window || !window->getView())
        return 0;
    // TODO: Fix MVC violation
    CSSStyleDeclarationPtr style = window->getView()->getStyle(self());
    if (!style)
        return 0;
    return style->getBox();
}

void HTMLElementImp::setEventHandler(const std::u16string& type, Object handler)
{
    EventListenerPtr listener = getEventHandlerListener(type);
    if (listener) {
        listener->setEventHandler(handler);
        return;
    }
    if (!handler)
        return;
    listener = std::make_shared<EventListenerImp>(boost::bind(&ECMAScriptContext::dispatchEvent, getOwnerDocumentImp()->getContext(), _1, _2));
    if (listener) {
        listener->setEventHandler(handler);
        addEventListener(type, listener, false, EventTargetImp::UseEventHandler);
    }
}

// XBL 2.0 internal

void HTMLElementImp::setShadowTree(const HTMLTemplateElementPtr& e)
{
    shadowTree = e;
}

bool HTMLElementImp::generateShadowContent(const CSSStyleDeclarationPtr& style)
{
    if (style->binding.getValue() != CSSBindingValueImp::Uri ||
        style->display.getValue() == CSSDisplayValueImp::None)
        return false;
    if (getShadowTree())  // already attached?
        return false;
    DocumentPtr document = getOwnerDocumentImp();
    assert(document);
    URL base(document->getDocumentURI());
    URL url(base, style->binding.getURL());
    if (!base.isSameExceptFragments(url)) {
        document = std::dynamic_pointer_cast<DocumentImp>(document->loadBindingDocument(url).self());
        if (!document || document->getReadyState() != u"complete")
            return false;
    }

    std::u16string hash = url.getHash();
    if (hash[0] == '#')
        hash.erase(0, 1);
    Element element = document->getElementById(hash);
    if (!element)
        return false;
    auto binding = std::dynamic_pointer_cast<HTMLBindingElementImp>(element.self());
    if (!binding)
        return false;
    bindingImplementation = binding->getImplementation();
    if (!bindingImplementation)
        return false;
    if (html::HTMLTemplateElement shadowTree = binding->cloneTemplate()) {
        setShadowTree(std::static_pointer_cast<HTMLTemplateElementImp>(shadowTree.self()));
        shadowTarget = std::make_shared<EventTargetImp>();
        // TODO: if (not called from the background thread) {
#if 0
            ECMAScriptContext* context = document->getContext();
            shadowImplementation = context->xblCreateImplementation(shadowTarget, bindingImplementation, this, shadowTree);
            shadowImplementation.xblEnteredDocument();
        }
#endif
        return true;
    }
    return false;
}

bool HTMLElementImp::xblEnteredDocument(Node node)
{
    bool result = false;
    while (node) {
        if (auto element = std::dynamic_pointer_cast<HTMLElementImp>(node.self())) {
            if (element->shadowTarget && !element->shadowImplementation) {
                DocumentPtr document = element->getShadowTree()->getOwnerDocumentImp();
                assert(document);
                document->enter();
                element->shadowImplementation = document->getContext()->xblCreateImplementation(element->shadowTarget, element->bindingImplementation, element, element->shadowTree).self();
                element->shadowImplementation.xblEnteredDocument();
                document->exit();
                result = true;
            }
        }
        if (node.hasChildNodes())
            result |= xblEnteredDocument(node.getFirstChild());
        node = node.getNextSibling();
    }
    return result;
}

void HTMLElementImp::invokeShadowTarget(const EventPtr& event)
{
    auto shadowDocument = getShadowTree()->getOwnerDocumentImp();
    shadowDocument->enter();
    std::dynamic_pointer_cast<EventTargetImp>(shadowTarget.self())->invoke(event);
    shadowDocument->exit();
}

void HTMLElementImp::invoke(const EventPtr& event)
{
    if (!shadowTarget) {
        EventTargetImp::invoke(event);
        return;
    }
    event->setCurrentTarget(self());
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

// Element

DOMRectList HTMLElementImp::getClientRects()
{
    // TODO: implement me!
    return nullptr;
}

DOMRect HTMLElementImp::getBoundingClientRect()
{
    // TODO: implement me!
    return nullptr;
}

// cf. http://www.w3.org/TR/cssom-view/#scroll-an-element-into-view
void HTMLElementImp::scrollIntoView(bool top)
{
    if (html::Window window = getOwnerDocument().getDefaultView()) {
        if (BoxPtr box = getBox()) {
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
    auto block = std::dynamic_pointer_cast<Block>(getBox());
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
    auto block = std::dynamic_pointer_cast<Block>(getBox());
    if (!block)
        return;
    float overflow = block->getScrollWidth() - block->width;
    scrollLeft = std::max(0, std::min(x, static_cast<int>(overflow)));
    block->setFlags(Box::NEED_REPAINT);
}

int HTMLElementImp::getScrollWidth()
{
    if (auto block = std::dynamic_pointer_cast<Block>(getBox()))
        return block->getScrollWidth();
    return 0;
}

int HTMLElementImp::getScrollHeight()
{
    if (auto block = std::dynamic_pointer_cast<Block>(getBox()))
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
    Nullable<std::u16string> value = getAttribute(u"translate");
    if (value.hasValue()) {
        std::u16string translate = value.value();
        toLower(translate);
        switch (findKeyword(translate, { u"", u"yes", u"no" })) {
        case 0: // ""
        case 1: // "yes"
            return true;
        case 2: // "no"
            return false;
        default:
            break;
        }
    }
    if (html::HTMLElement parent = interface_cast<html::HTMLElement>(getParentElement()))
        return parent.getTranslate();
    return true;
}

void HTMLElementImp::setTranslate(bool translate)
{
    setAttribute(u"translate", translate ? u"yes" : u"no");
}

std::u16string HTMLElementImp::getDir()
{
    std::u16string dir = getAttribute(u"dir");
    toLower(dir);
    if (0 <= findKeyword(dir, {u"ltr", u"rtl", u"auto"}))
        return dir;
    return u"";
}

void HTMLElementImp::setDir(const std::u16string& dir)
{
    setAttribute(u"dir", dir);
}

DOMStringMap HTMLElementImp::getDataset()
{
    // TODO: implement me!
    return nullptr;
}

bool HTMLElementImp::getHidden()
{
    return getAttributeAsBoolean(u"hidden");
}

void HTMLElementImp::setHidden(bool hidden)
{
    setAttributeAsBoolean(u"hidden", hidden);
}

void HTMLElementImp::click()
{
    // TODO: implement me!
}

int HTMLElementImp::getTabIndex()
{
    return tabIndex;
}

void HTMLElementImp::setTabIndex(int index)
{
    setAttribute(u"tabindex", toString(index));
}

void HTMLElementImp::focus()
{
    auto ownerDocument = getOwnerDocumentImp();
    if (!ownerDocument || !ownerDocument->getDefaultView() || getTabIndex() < 0)
        return;
    ownerDocument->setFocus(std::static_pointer_cast<HTMLElementImp>(self()));
}

void HTMLElementImp::blur()
{
    auto ownerDocument = getOwnerDocumentImp();
    if (!ownerDocument || ownerDocument->getFocus().get() != this)
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
    return nullptr;
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
    return nullptr;
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
        if (auto imp = std::make_shared<CSSStyleDeclarationImp>()) {
            imp->setOwner(self());
            style = imp;
        }
    }
    return style;
}

events::EventHandlerNonNull HTMLElementImp::getOnabort()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"abort"));
}

void HTMLElementImp::setOnabort(events::EventHandlerNonNull onabort)
{
    setEventHandler(u"abort", onabort);
}

events::EventHandlerNonNull HTMLElementImp::getOnblur()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"blur"));
}

void HTMLElementImp::setOnblur(events::EventHandlerNonNull onblur)
{
    setEventHandler(u"blur", onblur);
}

events::EventHandlerNonNull HTMLElementImp::getOncancel()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"cancel"));
}

void HTMLElementImp::setOncancel(events::EventHandlerNonNull oncancel)
{
    setEventHandler(u"cancel", oncancel);
}

events::EventHandlerNonNull HTMLElementImp::getOncanplay()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"canplay"));
}

void HTMLElementImp::setOncanplay(events::EventHandlerNonNull oncanplay)
{
    setEventHandler(u"canplay", oncanplay);
}

events::EventHandlerNonNull HTMLElementImp::getOncanplaythrough()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"canplaythrough"));
}

void HTMLElementImp::setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough)
{
    setEventHandler(u"canplaythrough", oncanplaythrough);
}

events::EventHandlerNonNull HTMLElementImp::getOnchange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"change"));
}

void HTMLElementImp::setOnchange(events::EventHandlerNonNull onchange)
{
    setEventHandler(u"change", onchange);
}

events::EventHandlerNonNull HTMLElementImp::getOnclick()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"click"));
}

void HTMLElementImp::setOnclick(events::EventHandlerNonNull onclick)
{
    setEventHandler(u"click", onclick);
}

events::EventHandlerNonNull HTMLElementImp::getOnclose()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"close"));
}

void HTMLElementImp::setOnclose(events::EventHandlerNonNull onclose)
{
    setEventHandler(u"close", onclose);
}

events::EventHandlerNonNull HTMLElementImp::getOncontextmenu()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"contextmenu"));
}

void HTMLElementImp::setOncontextmenu(events::EventHandlerNonNull oncontextmenu)
{
    setEventHandler(u"contextmenu", oncontextmenu);
}

events::EventHandlerNonNull HTMLElementImp::getOncuechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"cuechange"));
}

void HTMLElementImp::setOncuechange(events::EventHandlerNonNull oncuechange)
{
    setEventHandler(u"cuechange", oncuechange);
}

events::EventHandlerNonNull HTMLElementImp::getOndblclick()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dblclick"));
}

void HTMLElementImp::setOndblclick(events::EventHandlerNonNull ondblclick)
{
    setEventHandler(u"dblclick", ondblclick);
}

events::EventHandlerNonNull HTMLElementImp::getOndrag()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"drag"));
}

void HTMLElementImp::setOndrag(events::EventHandlerNonNull ondrag)
{
    setEventHandler(u"drag", ondrag);
}

events::EventHandlerNonNull HTMLElementImp::getOndragend()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragend"));
}

void HTMLElementImp::setOndragend(events::EventHandlerNonNull ondragend)
{
    setEventHandler(u"dragend", ondragend);
}

events::EventHandlerNonNull HTMLElementImp::getOndragenter()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragenter"));
}

void HTMLElementImp::setOndragenter(events::EventHandlerNonNull ondragenter)
{
    setEventHandler(u"dragenter", ondragenter);
}

events::EventHandlerNonNull HTMLElementImp::getOndragleave()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragleave"));
}

void HTMLElementImp::setOndragleave(events::EventHandlerNonNull ondragleave)
{
    setEventHandler(u"dragleave", ondragleave);
}

events::EventHandlerNonNull HTMLElementImp::getOndragover()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragover"));
}

void HTMLElementImp::setOndragover(events::EventHandlerNonNull ondragover)
{
    setEventHandler(u"dragover", ondragover);
}

events::EventHandlerNonNull HTMLElementImp::getOndragstart()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"dragstart"));
}

void HTMLElementImp::setOndragstart(events::EventHandlerNonNull ondragstart)
{
    setEventHandler(u"dragstart", ondragstart);
}

events::EventHandlerNonNull HTMLElementImp::getOndrop()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"drop"));
}

void HTMLElementImp::setOndrop(events::EventHandlerNonNull ondrop)
{
    setEventHandler(u"drop", ondrop);
}

events::EventHandlerNonNull HTMLElementImp::getOndurationchange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"durationchange"));
}

void HTMLElementImp::setOndurationchange(events::EventHandlerNonNull ondurationchange)
{
    setEventHandler(u"durationchange", ondurationchange);
}

events::EventHandlerNonNull HTMLElementImp::getOnemptied()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"emptied"));
}

void HTMLElementImp::setOnemptied(events::EventHandlerNonNull onemptied)
{
    setEventHandler(u"emptied", onemptied);
}

events::EventHandlerNonNull HTMLElementImp::getOnended()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"ended"));
}

void HTMLElementImp::setOnended(events::EventHandlerNonNull onended)
{
    setEventHandler(u"ended", onended);
}

events::OnErrorEventHandlerNonNull HTMLElementImp::getOnerror()
{
    return interface_cast<events::OnErrorEventHandlerNonNull>(getEventHandler(u"error"));
}

void HTMLElementImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    setEventHandler(u"error", onerror);
}

events::EventHandlerNonNull HTMLElementImp::getOnfocus()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"focus"));
}

void HTMLElementImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    setEventHandler(u"focus", onfocus);
}

events::EventHandlerNonNull HTMLElementImp::getOninput()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"input"));
}

void HTMLElementImp::setOninput(events::EventHandlerNonNull oninput)
{
    setEventHandler(u"input", oninput);
}

events::EventHandlerNonNull HTMLElementImp::getOninvalid()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"invalid"));
}

void HTMLElementImp::setOninvalid(events::EventHandlerNonNull oninvalid)
{
    setEventHandler(u"invalid", oninvalid);
}

events::EventHandlerNonNull HTMLElementImp::getOnkeydown()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keydown"));
}

void HTMLElementImp::setOnkeydown(events::EventHandlerNonNull onkeydown)
{
    setEventHandler(u"keydown", onkeydown);
}

events::EventHandlerNonNull HTMLElementImp::getOnkeypress()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keypress"));
}

void HTMLElementImp::setOnkeypress(events::EventHandlerNonNull onkeypress)
{
    setEventHandler(u"keypress", onkeypress);
}

events::EventHandlerNonNull HTMLElementImp::getOnkeyup()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"keyup"));
}

void HTMLElementImp::setOnkeyup(events::EventHandlerNonNull onkeyup)
{
    setEventHandler(u"keyup", onkeyup);
}

events::EventHandlerNonNull HTMLElementImp::getOnload()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"load"));
}

void HTMLElementImp::setOnload(events::EventHandlerNonNull onload)
{
    setEventHandler(u"load", onload);
}

events::EventHandlerNonNull HTMLElementImp::getOnloadeddata()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadeddata"));
}

void HTMLElementImp::setOnloadeddata(events::EventHandlerNonNull onloadeddata)
{
    setEventHandler(u"loadeddata", onloadeddata);
}

events::EventHandlerNonNull HTMLElementImp::getOnloadedmetadata()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadedmetadata"));
}

void HTMLElementImp::setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata)
{
    setEventHandler(u"loadedmetadata", onloadedmetadata);
}

events::EventHandlerNonNull HTMLElementImp::getOnloadstart()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"loadstart"));
}

void HTMLElementImp::setOnloadstart(events::EventHandlerNonNull onloadstart)
{
    setEventHandler(u"loadstart", onloadstart);
}

events::EventHandlerNonNull HTMLElementImp::getOnmousedown()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousedown"));
}

void HTMLElementImp::setOnmousedown(events::EventHandlerNonNull onmousedown)
{
    setEventHandler(u"mousedown", onmousedown);
}

events::EventHandlerNonNull HTMLElementImp::getOnmousemove()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousemove"));
}

void HTMLElementImp::setOnmousemove(events::EventHandlerNonNull onmousemove)
{
    setEventHandler(u"mousemove", onmousemove);
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseout()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseout"));
}

void HTMLElementImp::setOnmouseout(events::EventHandlerNonNull onmouseout)
{
    setEventHandler(u"mouseout", onmouseout);
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseover()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseover"));
}

void HTMLElementImp::setOnmouseover(events::EventHandlerNonNull onmouseover)
{
    setEventHandler(u"mouseover", onmouseover);
}

events::EventHandlerNonNull HTMLElementImp::getOnmouseup()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mouseup"));
}

void HTMLElementImp::setOnmouseup(events::EventHandlerNonNull onmouseup)
{
    setEventHandler(u"mouseup", onmouseup);
}

events::EventHandlerNonNull HTMLElementImp::getOnmousewheel()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"mousewheel"));
}

void HTMLElementImp::setOnmousewheel(events::EventHandlerNonNull onmousewheel)
{
    setEventHandler(u"mousewheel", onmousewheel);
}

events::EventHandlerNonNull HTMLElementImp::getOnpause()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"pause"));
}

void HTMLElementImp::setOnpause(events::EventHandlerNonNull onpause)
{
    setEventHandler(u"pause", onpause);
}

events::EventHandlerNonNull HTMLElementImp::getOnplay()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"play"));
}

void HTMLElementImp::setOnplay(events::EventHandlerNonNull onplay)
{
    setEventHandler(u"play", onplay);
}

events::EventHandlerNonNull HTMLElementImp::getOnplaying()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"playing"));
}

void HTMLElementImp::setOnplaying(events::EventHandlerNonNull onplaying)
{
    setEventHandler(u"playing", onplaying);
}

events::EventHandlerNonNull HTMLElementImp::getOnprogress()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"progress"));
}

void HTMLElementImp::setOnprogress(events::EventHandlerNonNull onprogress)
{
    setEventHandler(u"progress", onprogress);
}

events::EventHandlerNonNull HTMLElementImp::getOnratechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"ratechange"));
}

void HTMLElementImp::setOnratechange(events::EventHandlerNonNull onratechange)
{
    setEventHandler(u"ratechange", onratechange);
}

events::EventHandlerNonNull HTMLElementImp::getOnreset()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"reset"));
}

void HTMLElementImp::setOnreset(events::EventHandlerNonNull onreset)
{
    setEventHandler(u"reset", onreset);
}

events::EventHandlerNonNull HTMLElementImp::getOnscroll()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"scroll"));
}

void HTMLElementImp::setOnscroll(events::EventHandlerNonNull onscroll)
{
    setEventHandler(u"scroll", onscroll);
}

events::EventHandlerNonNull HTMLElementImp::getOnseeked()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"seeked"));
}

void HTMLElementImp::setOnseeked(events::EventHandlerNonNull onseeked)
{
    setEventHandler(u"seeked", onseeked);
}

events::EventHandlerNonNull HTMLElementImp::getOnseeking()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"seeking"));
}

void HTMLElementImp::setOnseeking(events::EventHandlerNonNull onseeking)
{
    setEventHandler(u"seeking", onseeking);
}

events::EventHandlerNonNull HTMLElementImp::getOnselect()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"select"));
}

void HTMLElementImp::setOnselect(events::EventHandlerNonNull onselect)
{
    setEventHandler(u"select", onselect);
}

events::EventHandlerNonNull HTMLElementImp::getOnshow()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"show"));
}

void HTMLElementImp::setOnshow(events::EventHandlerNonNull onshow)
{
    setEventHandler(u"show", onshow);
}

events::EventHandlerNonNull HTMLElementImp::getOnstalled()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"stalled"));
}

void HTMLElementImp::setOnstalled(events::EventHandlerNonNull onstalled)
{
    setEventHandler(u"stalled", onstalled);
}

events::EventHandlerNonNull HTMLElementImp::getOnsubmit()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"submit"));
}

void HTMLElementImp::setOnsubmit(events::EventHandlerNonNull onsubmit)
{
    setEventHandler(u"submit", onsubmit);
}

events::EventHandlerNonNull HTMLElementImp::getOnsuspend()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"suspend"));
}

void HTMLElementImp::setOnsuspend(events::EventHandlerNonNull onsuspend)
{
    setEventHandler(u"suspend", onsuspend);
}

events::EventHandlerNonNull HTMLElementImp::getOntimeupdate()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"timeupdate"));
}

void HTMLElementImp::setOntimeupdate(events::EventHandlerNonNull ontimeupdate)
{
    setEventHandler(u"timeupdate", ontimeupdate);
}

events::EventHandlerNonNull HTMLElementImp::getOnvolumechange()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"volumechange"));
}

void HTMLElementImp::setOnvolumechange(events::EventHandlerNonNull onvolumechange)
{
    setEventHandler(u"volumechange", onvolumechange);
}

events::EventHandlerNonNull HTMLElementImp::getOnwaiting()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"waiting"));
}

void HTMLElementImp::setOnwaiting(events::EventHandlerNonNull onwaiting)
{
    setEventHandler(u"waiting", onwaiting);
}

Element HTMLElementImp::getOffsetParent()
{
    // TODO: implement me!
    return nullptr;
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

void HTMLElementImp::handleMutationBackground(events::MutationEvent mutation)
{
    switch (mutation.getAttrChange()) {
    case events::MutationEvent::MODIFICATION:
    case events::MutationEvent::ADDITION:
        getStyle().setProperty(u"background-image", u"url(" + mutation.getNewValue() + u")", u"non-css");
        break;
    case events::MutationEvent::REMOVAL:
    default:
        getStyle().setProperty(u"background-image", u"", u"non-css");
        break;
    }
}

void HTMLElementImp::handleMutationColor(events::MutationEvent mutation, const std::u16string& prop)
{
    switch (mutation.getAttrChange()) {
    case events::MutationEvent::MODIFICATION:
    case events::MutationEvent::ADDITION:
        getStyle().setProperty(prop, mutation.getNewValue(), u"non-css");
        break;
    case events::MutationEvent::REMOVAL:
    default:
        getStyle().setProperty(prop, u"", u"non-css");
        break;
    }
}

void HTMLElementImp::handleMutationBorder(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());
    if (mapToPixelLength(value) && value != u"0") {
        style.setProperty(u"border-width", value, u"non-css");
        style.setProperty(u"border-style", u"solid", u"non-css");
    }
}

void HTMLElementImp::handleMutationHref(events::MutationEvent mutation)
{
    switch (mutation.getAttrChange()) {
    case events::MutationEvent::MODIFICATION:
    case events::MutationEvent::ADDITION:
        if (!toInteger(getAttribute(u"tabindex"), tabIndex))
            tabIndex = 0;
        break;
    case events::MutationEvent::REMOVAL:
    default:
        tabIndex = -1;
        break;
    }
}

// cf. http://www.w3.org/TR/html5/infrastructure.html#reflect
std::u16string HTMLElementImp::getAttributeAsURL(const std::u16string& name)
{
    std::u16string value(getAttribute(name));
    if (value.empty())
        return value;
    if (DocumentPtr document = getOwnerDocumentImp()) {
        URL base(document->getDocumentURI());
        URL url(base, value);
        value = url;
    } else
        value.clear();
    return value;
}

bool HTMLElementImp::getAttributeAsBoolean(const std::u16string& name)
{
    return hasAttribute(name);
}

void HTMLElementImp::setAttributeAsBoolean(const std::u16string& name, bool on)
{
    if (on)
        setAttribute(name, u"");
    else
        removeAttribute(name);
}

int HTMLElementImp::getAttributeAsInteger(const std::u16string& name, int defaultValue)
{
    std::u16string attr(getAttribute(name));
    int value;
    if (toInteger(attr, value))
        return value;
    return defaultValue;
}

void HTMLElementImp::setAttributeAsInteger(const std::u16string& name, int value)
{
    setAttribute(name, boost::lexical_cast<std::u16string>(value));
}

unsigned int HTMLElementImp::getAttributeAsUnsigned(const std::u16string& name, unsigned int defaultValue)
{
    std::u16string attr(getAttribute(name));
    unsigned int value;
    if (toUnsigned(attr, value))
        return value;
    return defaultValue;
}

void HTMLElementImp::setAttributeAsUnsigned(const std::u16string& name, unsigned int value)
{
    setAttribute(name, boost::lexical_cast<std::u16string>(value));
}

}}}}  // org::w3c::dom::bootstrap
