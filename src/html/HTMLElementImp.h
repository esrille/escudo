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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLTemplateElement.h>

#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/DOMTokenList.h>
#include <org/w3c/dom/DOMSettableTokenList.h>
#include <org/w3c/dom/DOMStringMap.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/events/EventHandlerNonNull.h>
#include <org/w3c/dom/events/MutationEvent.h>
#include <org/w3c/dom/events/OnErrorEventHandlerNonNull.h>
#include <org/w3c/dom/html/HTMLMenuElement.h>
#include <org/w3c/dom/xbl2/XBLImplementation.h>

#include "ElementImp.h"
#include "EventListenerImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class CSSStyleDeclarationImp;
class HTMLTemplateElementImp;

class HTMLElementImp : public ObjectMixin<HTMLElementImp, ElementImp>
{
    css::CSSStyleDeclaration style;
    int scrollTop;
    int scrollLeft;
    int moveX;
    int moveY;
    Retained<EventListenerImp> clickListener;
    Retained<EventListenerImp> mouseMoveListener;
    Retained<EventListenerImp> mutationListener;

    void handleClick(EventListenerImp* listener, events::Event event);
    void handleMouseMove(EventListenerImp* listener, events::Event event);
    void handleMutation(EventListenerImp* listener, events::Event event);

    // XBL 2.0
    Object bindingImplementation;
    html::HTMLTemplateElement shadowTree;
    events::EventTarget shadowTarget;
    xbl2::XBLImplementation shadowImplementation;

    void invokeShadowTarget(EventImp* event);

protected:
    int tabIndex;

    virtual void setEventHandler(const std::u16string& type, Object handler);

public:
    HTMLElementImp(DocumentImp* ownerDocument, const std::u16string& localName);
    HTMLElementImp(HTMLElementImp* org, bool deep);
    ~HTMLElementImp();

    virtual void eval();
    virtual void handleMutation(events::MutationEvent mutation);

    Box* getBox();

    // XBL 2.0 internal
    virtual void invoke(EventImp* event);
    void setShadowTree(HTMLTemplateElementImp* e);
    void setShadowTree(html::HTMLTemplateElement& e) {
        shadowTree = e;
    }
    html::HTMLTemplateElement getShadowTree() {
        return shadowTree;
    }
    void setShadowImplementation(xbl2::XBLImplementation& t) {
        shadowImplementation = t;
    }
    xbl2::XBLImplementation getShadowImplementation() {
        return shadowImplementation;
    }

    virtual void generateShadowContent(CSSStyleDeclarationImp* style);

    // Node
    virtual Node cloneNode(bool deep = true);

    // Element (CSSOM view)
    virtual views::ClientRectList getClientRects();
    virtual views::ClientRect getBoundingClientRect();
    virtual void scrollIntoView(bool top = true);
    virtual int getScrollTop();
    virtual void setScrollTop(int scrollTop);
    virtual int getScrollLeft();
    virtual void setScrollLeft(int scrollLeft);
    virtual int getScrollWidth();
    virtual int getScrollHeight();
    virtual int getClientTop();
    virtual int getClientLeft();
    virtual int getClientWidth();
    virtual int getClientHeight();

    // HTMLElement
    virtual std::u16string getTitle();
    virtual void setTitle(const std::u16string& title);
    virtual std::u16string getLang();
    virtual void setLang(const std::u16string& lang);
    virtual bool getTranslate();
    virtual void setTranslate(bool translate);
    virtual std::u16string getDir();
    virtual void setDir(const std::u16string& dir);
    virtual DOMStringMap getDataset();
    virtual bool getHidden();
    virtual void setHidden(bool hidden);
    virtual void click();
    virtual int getTabIndex();
    virtual void setTabIndex(int tabIndex);
    virtual void focus();
    virtual void blur();
    virtual std::u16string getAccessKey();
    virtual void setAccessKey(const std::u16string& accessKey);
    virtual std::u16string getAccessKeyLabel();
    virtual bool getDraggable();
    virtual void setDraggable(bool draggable);
    virtual DOMSettableTokenList getDropzone();
    virtual void setDropzone(const std::u16string& dropzone);
    virtual std::u16string getContentEditable();
    virtual void setContentEditable(const std::u16string& contentEditable);
    virtual bool getIsContentEditable();
    virtual html::HTMLMenuElement getContextMenu();
    virtual void setContextMenu(html::HTMLMenuElement contextMenu);
    virtual bool getSpellcheck();
    virtual void setSpellcheck(bool spellcheck);
    virtual Nullable<std::u16string> getCommandType();
    virtual Nullable<std::u16string> getCommandLabel();
    virtual Nullable<std::u16string> getCommandIcon();
    virtual Nullable<bool> getCommandHidden();
    virtual Nullable<bool> getCommandDisabled();
    virtual Nullable<bool> getCommandChecked();
    virtual css::CSSStyleDeclaration getStyle();
    virtual events::EventHandlerNonNull getOnabort();
    virtual void setOnabort(events::EventHandlerNonNull onabort);
    virtual events::EventHandlerNonNull getOnblur();
    virtual void setOnblur(events::EventHandlerNonNull onblur);
    virtual events::EventHandlerNonNull getOncancel();
    virtual void setOncancel(events::EventHandlerNonNull oncancel);
    virtual events::EventHandlerNonNull getOncanplay();
    virtual void setOncanplay(events::EventHandlerNonNull oncanplay);
    virtual events::EventHandlerNonNull getOncanplaythrough();
    virtual void setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough);
    virtual events::EventHandlerNonNull getOnchange();
    virtual void setOnchange(events::EventHandlerNonNull onchange);
    virtual events::EventHandlerNonNull getOnclick();
    virtual void setOnclick(events::EventHandlerNonNull onclick);
    virtual events::EventHandlerNonNull getOnclose();
    virtual void setOnclose(events::EventHandlerNonNull onclose);
    virtual events::EventHandlerNonNull getOncontextmenu();
    virtual void setOncontextmenu(events::EventHandlerNonNull oncontextmenu);
    virtual events::EventHandlerNonNull getOncuechange();
    virtual void setOncuechange(events::EventHandlerNonNull oncuechange);
    virtual events::EventHandlerNonNull getOndblclick();
    virtual void setOndblclick(events::EventHandlerNonNull ondblclick);
    virtual events::EventHandlerNonNull getOndrag();
    virtual void setOndrag(events::EventHandlerNonNull ondrag);
    virtual events::EventHandlerNonNull getOndragend();
    virtual void setOndragend(events::EventHandlerNonNull ondragend);
    virtual events::EventHandlerNonNull getOndragenter();
    virtual void setOndragenter(events::EventHandlerNonNull ondragenter);
    virtual events::EventHandlerNonNull getOndragleave();
    virtual void setOndragleave(events::EventHandlerNonNull ondragleave);
    virtual events::EventHandlerNonNull getOndragover();
    virtual void setOndragover(events::EventHandlerNonNull ondragover);
    virtual events::EventHandlerNonNull getOndragstart();
    virtual void setOndragstart(events::EventHandlerNonNull ondragstart);
    virtual events::EventHandlerNonNull getOndrop();
    virtual void setOndrop(events::EventHandlerNonNull ondrop);
    virtual events::EventHandlerNonNull getOndurationchange();
    virtual void setOndurationchange(events::EventHandlerNonNull ondurationchange);
    virtual events::EventHandlerNonNull getOnemptied();
    virtual void setOnemptied(events::EventHandlerNonNull onemptied);
    virtual events::EventHandlerNonNull getOnended();
    virtual void setOnended(events::EventHandlerNonNull onended);
    virtual events::OnErrorEventHandlerNonNull getOnerror();
    virtual void setOnerror(events::OnErrorEventHandlerNonNull onerror);
    virtual events::EventHandlerNonNull getOnfocus();
    virtual void setOnfocus(events::EventHandlerNonNull onfocus);
    virtual events::EventHandlerNonNull getOninput();
    virtual void setOninput(events::EventHandlerNonNull oninput);
    virtual events::EventHandlerNonNull getOninvalid();
    virtual void setOninvalid(events::EventHandlerNonNull oninvalid);
    virtual events::EventHandlerNonNull getOnkeydown();
    virtual void setOnkeydown(events::EventHandlerNonNull onkeydown);
    virtual events::EventHandlerNonNull getOnkeypress();
    virtual void setOnkeypress(events::EventHandlerNonNull onkeypress);
    virtual events::EventHandlerNonNull getOnkeyup();
    virtual void setOnkeyup(events::EventHandlerNonNull onkeyup);
    virtual events::EventHandlerNonNull getOnload();
    virtual void setOnload(events::EventHandlerNonNull onload);
    virtual events::EventHandlerNonNull getOnloadeddata();
    virtual void setOnloadeddata(events::EventHandlerNonNull onloadeddata);
    virtual events::EventHandlerNonNull getOnloadedmetadata();
    virtual void setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata);
    virtual events::EventHandlerNonNull getOnloadstart();
    virtual void setOnloadstart(events::EventHandlerNonNull onloadstart);
    virtual events::EventHandlerNonNull getOnmousedown();
    virtual void setOnmousedown(events::EventHandlerNonNull onmousedown);
    virtual events::EventHandlerNonNull getOnmousemove();
    virtual void setOnmousemove(events::EventHandlerNonNull onmousemove);
    virtual events::EventHandlerNonNull getOnmouseout();
    virtual void setOnmouseout(events::EventHandlerNonNull onmouseout);
    virtual events::EventHandlerNonNull getOnmouseover();
    virtual void setOnmouseover(events::EventHandlerNonNull onmouseover);
    virtual events::EventHandlerNonNull getOnmouseup();
    virtual void setOnmouseup(events::EventHandlerNonNull onmouseup);
    virtual events::EventHandlerNonNull getOnmousewheel();
    virtual void setOnmousewheel(events::EventHandlerNonNull onmousewheel);
    virtual events::EventHandlerNonNull getOnpause();
    virtual void setOnpause(events::EventHandlerNonNull onpause);
    virtual events::EventHandlerNonNull getOnplay();
    virtual void setOnplay(events::EventHandlerNonNull onplay);
    virtual events::EventHandlerNonNull getOnplaying();
    virtual void setOnplaying(events::EventHandlerNonNull onplaying);
    virtual events::EventHandlerNonNull getOnprogress();
    virtual void setOnprogress(events::EventHandlerNonNull onprogress);
    virtual events::EventHandlerNonNull getOnratechange();
    virtual void setOnratechange(events::EventHandlerNonNull onratechange);
    virtual events::EventHandlerNonNull getOnreset();
    virtual void setOnreset(events::EventHandlerNonNull onreset);
    virtual events::EventHandlerNonNull getOnscroll();
    virtual void setOnscroll(events::EventHandlerNonNull onscroll);
    virtual events::EventHandlerNonNull getOnseeked();
    virtual void setOnseeked(events::EventHandlerNonNull onseeked);
    virtual events::EventHandlerNonNull getOnseeking();
    virtual void setOnseeking(events::EventHandlerNonNull onseeking);
    virtual events::EventHandlerNonNull getOnselect();
    virtual void setOnselect(events::EventHandlerNonNull onselect);
    virtual events::EventHandlerNonNull getOnshow();
    virtual void setOnshow(events::EventHandlerNonNull onshow);
    virtual events::EventHandlerNonNull getOnstalled();
    virtual void setOnstalled(events::EventHandlerNonNull onstalled);
    virtual events::EventHandlerNonNull getOnsubmit();
    virtual void setOnsubmit(events::EventHandlerNonNull onsubmit);
    virtual events::EventHandlerNonNull getOnsuspend();
    virtual void setOnsuspend(events::EventHandlerNonNull onsuspend);
    virtual events::EventHandlerNonNull getOntimeupdate();
    virtual void setOntimeupdate(events::EventHandlerNonNull ontimeupdate);
    virtual events::EventHandlerNonNull getOnvolumechange();
    virtual void setOnvolumechange(events::EventHandlerNonNull onvolumechange);
    virtual events::EventHandlerNonNull getOnwaiting();
    virtual void setOnwaiting(events::EventHandlerNonNull onwaiting);
    // HTMLElement
    virtual Element getOffsetParent();
    virtual int getOffsetTop();
    virtual int getOffsetLeft();
    virtual int getOffsetWidth();
    virtual int getOffsetHeight();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLElement::getMetaData();
    }

    // Extra
    void handleMutationBackground(events::MutationEvent mutation);
    void handleMutationHref(events::MutationEvent mutation);

    static bool toUnsigned(std::u16string& value);
    static bool toPx(std::u16string& value);  // for cellspacing, cellpadding, etc.
    static bool toPxOrPercentage(std::u16string& value);  // for width, heigth, etc.

    static bool evalColor(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop);
    static bool evalBgcolor(HTMLElementImp* element) {
        return evalColor(element, u"bgcolor", u"background-color");
    }
    static bool evalBorder(HTMLElementImp* element);
    static bool evalPx(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop);
    static bool evalPxOrPercentage(HTMLElementImp* element, const std::u16string& attr, const std::u16string& prop);
    static bool evalHeight(HTMLElementImp* element) {
        return evalPxOrPercentage(element, u"height", u"height");
    }
    static bool evalWidth(HTMLElementImp* element) {
        return evalPxOrPercentage(element, u"width", u"width");
    }
    static bool evalHspace(HTMLElementImp* element, const std::u16string& prop = u"hspace");
    static bool evalVspace(HTMLElementImp* element, const std::u16string& prop = u"vspace");
    static bool evalMarginHeight(HTMLElementImp* element) {
        return evalVspace(element, u"marginheight");
    }
    static bool evalMarginWidth(HTMLElementImp* element) {
        return evalHspace(element, u"marginwidth");
    }
    static bool evalNoWrap(HTMLElementImp* element);
    static bool evalValign(HTMLElementImp* element);

    // xblEnteredDocument() should be called after view->constructComputedStyles().
    static void xblEnteredDocument(Node node);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLELEMENTIMP_H_INCLUDED
