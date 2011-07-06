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

#ifndef WINDOW_IMP_H
#define WINDOW_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/Window.h>

#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/html/StyleMedia.h>
#include <org/w3c/dom/html/Screen.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventListener.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/Transferable.h>
#include <org/w3c/dom/html/Window.h>
#include <org/w3c/dom/html/BarProp.h>
#include <org/w3c/dom/html/History.h>
#include <org/w3c/dom/html/Location.h>
#include <org/w3c/dom/html/ApplicationCache.h>
#include <org/w3c/dom/html/Function.h>
#include <org/w3c/dom/html/Navigator.h>
#include <org/w3c/dom/html/External.h>
#include <org/w3c/dom/html/UndoManager.h>

#include <deque>

#include <org/w3c/dom/css/CSSStyleSheet.h>

#include "EventTargetImp.h"
#include "HistoryImp.h"
#include "LocationImp.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class ViewCSSImp;

class WindowImp : public ObjectMixin<WindowImp, EventTargetImp>
{
    HttpRequest request;
    Retained<HistoryImp> history;
    Document document;
    ViewCSSImp* view;
    Box* boxTree;

    WindowImp* parent;
    std::deque<WindowImp*> childWindows;

    // for MouseEvent
    int detail;

    // for display
    unsigned width;
    unsigned height;
    bool redisplay;  // set true to force redisplay

public:
    WindowImp(WindowImp* parent = 0);
    ~WindowImp();

    static css::CSSStyleSheet defaultStyleSheet;

    void setSize(unsigned w, unsigned h) {
        width = w;
        height = h;
    }
    ViewCSSImp* getView() const {
        return view;
    }

    bool poll();
    void render();
    void setDocument(Document document);

    // mouse()
    //   button:
    //     0: left (primary)
    //     1: middle (auxiliary)
    //     2: right (secondary)
    //     3: up (wheel)
    //     4: down (wheel)
    //     5: left (wheel)
    //     6: right (wheel)
    //     7: back
    //     8: forward
    //   state:
    //     0: down (GLUT_DOWN)
    //     1: up (GLUT_UP)
    //   modifiers:
    //     1: shift
    //     2: ctrl
    //     4: alt
    void mouse(int button, int state, int x, int y, int modifiers);
    void mouseMove(int x, int y, int modifiers);

    void keydown(unsigned charCode, unsigned keyCode, int modifiers);
    void keyup(unsigned charCode, unsigned keyCode, int modifiers);

    // Window customized
    html::Window open(std::u16string url = u"about:blank", std::u16string target = u"_blank", std::u16string features = u"", bool replace = false);

    // Window
    html::Window getWindow() __attribute__((weak));
    html::Window getSelf() __attribute__((weak));
    Document getDocument() __attribute__((weak));
    std::u16string getName() __attribute__((weak));
    void setName(std::u16string name) __attribute__((weak));
    html::Location getLocation() __attribute__((weak));
    void setLocation(std::u16string location) __attribute__((weak));
    html::History getHistory() __attribute__((weak));
    html::UndoManager getUndoManager() __attribute__((weak));
    Any getLocationbar() __attribute__((weak));
    void setLocationbar(Any locationbar) __attribute__((weak));
    Any getMenubar() __attribute__((weak));
    void setMenubar(Any menubar) __attribute__((weak));
    Any getPersonalbar() __attribute__((weak));
    void setPersonalbar(Any personalbar) __attribute__((weak));
    Any getScrollbars() __attribute__((weak));
    void setScrollbars(Any scrollbars) __attribute__((weak));
    Any getStatusbar() __attribute__((weak));
    void setStatusbar(Any statusbar) __attribute__((weak));
    Any getToolbar() __attribute__((weak));
    void setToolbar(Any toolbar) __attribute__((weak));
    std::u16string getStatus() __attribute__((weak));
    void setStatus(std::u16string status) __attribute__((weak));
    void close() __attribute__((weak));
    void stop() __attribute__((weak));
    void focus() __attribute__((weak));
    void blur() __attribute__((weak));
    Any getFrames() __attribute__((weak));
    void setFrames(Any frames) __attribute__((weak));
    Any getLength() __attribute__((weak));
    void setLength(Any length) __attribute__((weak));
    html::Window getTop() __attribute__((weak));
    html::Window getOpener() __attribute__((weak));
    void setOpener(html::Window opener) __attribute__((weak));
    html::Window getParent() __attribute__((weak));
    Element getFrameElement() __attribute__((weak));
    html::Window getElement(unsigned int index) __attribute__((weak));
    Any getElement(std::u16string name) __attribute__((weak));
    void setElement(std::u16string name, Any value) __attribute__((weak));
    html::Navigator getNavigator() __attribute__((weak));
    html::External getExternal() __attribute__((weak));
    html::ApplicationCache getApplicationCache() __attribute__((weak));
    void alert(std::u16string message) __attribute__((weak));
    bool confirm(std::u16string message) __attribute__((weak));
    Nullable<std::u16string> prompt(std::u16string message) __attribute__((weak));
    Nullable<std::u16string> prompt(std::u16string message, std::u16string _default) __attribute__((weak));
    void print() __attribute__((weak));
    Any showModalDialog(std::u16string url) __attribute__((weak));
    Any showModalDialog(std::u16string url, Any argument) __attribute__((weak));
    void postMessage(Any message, std::u16string targetOrigin) __attribute__((weak));
    void postMessage(Any message, std::u16string targetOrigin, Sequence<html::Transferable> transfer) __attribute__((weak));
    html::Function getOnabort() __attribute__((weak));
    void setOnabort(html::Function onabort) __attribute__((weak));
    html::Function getOnafterprint() __attribute__((weak));
    void setOnafterprint(html::Function onafterprint) __attribute__((weak));
    html::Function getOnbeforeprint() __attribute__((weak));
    void setOnbeforeprint(html::Function onbeforeprint) __attribute__((weak));
    html::Function getOnbeforeunload() __attribute__((weak));
    void setOnbeforeunload(html::Function onbeforeunload) __attribute__((weak));
    html::Function getOnblur() __attribute__((weak));
    void setOnblur(html::Function onblur) __attribute__((weak));
    html::Function getOncanplay() __attribute__((weak));
    void setOncanplay(html::Function oncanplay) __attribute__((weak));
    html::Function getOncanplaythrough() __attribute__((weak));
    void setOncanplaythrough(html::Function oncanplaythrough) __attribute__((weak));
    html::Function getOnchange() __attribute__((weak));
    void setOnchange(html::Function onchange) __attribute__((weak));
    html::Function getOnclick() __attribute__((weak));
    void setOnclick(html::Function onclick) __attribute__((weak));
    html::Function getOncontextmenu() __attribute__((weak));
    void setOncontextmenu(html::Function oncontextmenu) __attribute__((weak));
    html::Function getOncuechange() __attribute__((weak));
    void setOncuechange(html::Function oncuechange) __attribute__((weak));
    html::Function getOndblclick() __attribute__((weak));
    void setOndblclick(html::Function ondblclick) __attribute__((weak));
    html::Function getOndrag() __attribute__((weak));
    void setOndrag(html::Function ondrag) __attribute__((weak));
    html::Function getOndragend() __attribute__((weak));
    void setOndragend(html::Function ondragend) __attribute__((weak));
    html::Function getOndragenter() __attribute__((weak));
    void setOndragenter(html::Function ondragenter) __attribute__((weak));
    html::Function getOndragleave() __attribute__((weak));
    void setOndragleave(html::Function ondragleave) __attribute__((weak));
    html::Function getOndragover() __attribute__((weak));
    void setOndragover(html::Function ondragover) __attribute__((weak));
    html::Function getOndragstart() __attribute__((weak));
    void setOndragstart(html::Function ondragstart) __attribute__((weak));
    html::Function getOndrop() __attribute__((weak));
    void setOndrop(html::Function ondrop) __attribute__((weak));
    html::Function getOndurationchange() __attribute__((weak));
    void setOndurationchange(html::Function ondurationchange) __attribute__((weak));
    html::Function getOnemptied() __attribute__((weak));
    void setOnemptied(html::Function onemptied) __attribute__((weak));
    html::Function getOnended() __attribute__((weak));
    void setOnended(html::Function onended) __attribute__((weak));
    html::Function getOnerror() __attribute__((weak));
    void setOnerror(html::Function onerror) __attribute__((weak));
    html::Function getOnfocus() __attribute__((weak));
    void setOnfocus(html::Function onfocus) __attribute__((weak));
    html::Function getOnhashchange() __attribute__((weak));
    void setOnhashchange(html::Function onhashchange) __attribute__((weak));
    html::Function getOninput() __attribute__((weak));
    void setOninput(html::Function oninput) __attribute__((weak));
    html::Function getOninvalid() __attribute__((weak));
    void setOninvalid(html::Function oninvalid) __attribute__((weak));
    html::Function getOnkeydown() __attribute__((weak));
    void setOnkeydown(html::Function onkeydown) __attribute__((weak));
    html::Function getOnkeypress() __attribute__((weak));
    void setOnkeypress(html::Function onkeypress) __attribute__((weak));
    html::Function getOnkeyup() __attribute__((weak));
    void setOnkeyup(html::Function onkeyup) __attribute__((weak));
    html::Function getOnload() __attribute__((weak));
    void setOnload(html::Function onload) __attribute__((weak));
    html::Function getOnloadeddata() __attribute__((weak));
    void setOnloadeddata(html::Function onloadeddata) __attribute__((weak));
    html::Function getOnloadedmetadata() __attribute__((weak));
    void setOnloadedmetadata(html::Function onloadedmetadata) __attribute__((weak));
    html::Function getOnloadstart() __attribute__((weak));
    void setOnloadstart(html::Function onloadstart) __attribute__((weak));
    html::Function getOnmessage() __attribute__((weak));
    void setOnmessage(html::Function onmessage) __attribute__((weak));
    html::Function getOnmousedown() __attribute__((weak));
    void setOnmousedown(html::Function onmousedown) __attribute__((weak));
    html::Function getOnmousemove() __attribute__((weak));
    void setOnmousemove(html::Function onmousemove) __attribute__((weak));
    html::Function getOnmouseout() __attribute__((weak));
    void setOnmouseout(html::Function onmouseout) __attribute__((weak));
    html::Function getOnmouseover() __attribute__((weak));
    void setOnmouseover(html::Function onmouseover) __attribute__((weak));
    html::Function getOnmouseup() __attribute__((weak));
    void setOnmouseup(html::Function onmouseup) __attribute__((weak));
    html::Function getOnmousewheel() __attribute__((weak));
    void setOnmousewheel(html::Function onmousewheel) __attribute__((weak));
    html::Function getOnoffline() __attribute__((weak));
    void setOnoffline(html::Function onoffline) __attribute__((weak));
    html::Function getOnonline() __attribute__((weak));
    void setOnonline(html::Function ononline) __attribute__((weak));
    html::Function getOnpause() __attribute__((weak));
    void setOnpause(html::Function onpause) __attribute__((weak));
    html::Function getOnplay() __attribute__((weak));
    void setOnplay(html::Function onplay) __attribute__((weak));
    html::Function getOnplaying() __attribute__((weak));
    void setOnplaying(html::Function onplaying) __attribute__((weak));
    html::Function getOnpagehide() __attribute__((weak));
    void setOnpagehide(html::Function onpagehide) __attribute__((weak));
    html::Function getOnpageshow() __attribute__((weak));
    void setOnpageshow(html::Function onpageshow) __attribute__((weak));
    html::Function getOnpopstate() __attribute__((weak));
    void setOnpopstate(html::Function onpopstate) __attribute__((weak));
    html::Function getOnprogress() __attribute__((weak));
    void setOnprogress(html::Function onprogress) __attribute__((weak));
    html::Function getOnratechange() __attribute__((weak));
    void setOnratechange(html::Function onratechange) __attribute__((weak));
    html::Function getOnreadystatechange() __attribute__((weak));
    void setOnreadystatechange(html::Function onreadystatechange) __attribute__((weak));
    html::Function getOnredo() __attribute__((weak));
    void setOnredo(html::Function onredo) __attribute__((weak));
    html::Function getOnreset() __attribute__((weak));
    void setOnreset(html::Function onreset) __attribute__((weak));
    html::Function getOnresize() __attribute__((weak));
    void setOnresize(html::Function onresize) __attribute__((weak));
    html::Function getOnscroll() __attribute__((weak));
    void setOnscroll(html::Function onscroll) __attribute__((weak));
    html::Function getOnseeked() __attribute__((weak));
    void setOnseeked(html::Function onseeked) __attribute__((weak));
    html::Function getOnseeking() __attribute__((weak));
    void setOnseeking(html::Function onseeking) __attribute__((weak));
    html::Function getOnselect() __attribute__((weak));
    void setOnselect(html::Function onselect) __attribute__((weak));
    html::Function getOnshow() __attribute__((weak));
    void setOnshow(html::Function onshow) __attribute__((weak));
    html::Function getOnstalled() __attribute__((weak));
    void setOnstalled(html::Function onstalled) __attribute__((weak));
    html::Function getOnstorage() __attribute__((weak));
    void setOnstorage(html::Function onstorage) __attribute__((weak));
    html::Function getOnsubmit() __attribute__((weak));
    void setOnsubmit(html::Function onsubmit) __attribute__((weak));
    html::Function getOnsuspend() __attribute__((weak));
    void setOnsuspend(html::Function onsuspend) __attribute__((weak));
    html::Function getOntimeupdate() __attribute__((weak));
    void setOntimeupdate(html::Function ontimeupdate) __attribute__((weak));
    html::Function getOnundo() __attribute__((weak));
    void setOnundo(html::Function onundo) __attribute__((weak));
    html::Function getOnunload() __attribute__((weak));
    void setOnunload(html::Function onunload) __attribute__((weak));
    html::Function getOnvolumechange() __attribute__((weak));
    void setOnvolumechange(html::Function onvolumechange) __attribute__((weak));
    html::Function getOnwaiting() __attribute__((weak));
    void setOnwaiting(html::Function onwaiting) __attribute__((weak));
    // Window-4
    css::CSSStyleDeclaration getComputedStyle(Element elt) __attribute__((weak));
    css::CSSStyleDeclaration getComputedStyle(Element elt, std::u16string pseudoElt) __attribute__((weak));
    // Window-5
    html::StyleMedia getStyleMedia() __attribute__((weak));
    html::Screen getScreen() __attribute__((weak));
    int getInnerWidth() __attribute__((weak));
    int getInnerHeight() __attribute__((weak));
    int getPageXOffset() __attribute__((weak));
    int getPageYOffset() __attribute__((weak));
    void scroll(int x, int y) __attribute__((weak));
    void scrollTo(int x, int y) __attribute__((weak));
    void scrollBy(int x, int y) __attribute__((weak));
    int getScreenX() __attribute__((weak));
    int getScreenY() __attribute__((weak));
    int getOuterWidth() __attribute__((weak));
    int getOuterHeight() __attribute__((weak));
    // EventTarget
    void addEventListener(std::u16string type, events::EventListener listener) __attribute__((weak));
    void addEventListener(std::u16string type, events::EventListener listener, bool capture) __attribute__((weak));
    void removeEventListener(std::u16string type, events::EventListener listener) __attribute__((weak));
    void removeEventListener(std::u16string type, events::EventListener listener, bool capture) __attribute__((weak));
    bool dispatchEvent(events::Event event) __attribute__((weak));
    // WindowBase64
    std::u16string btoa(std::u16string btoa) __attribute__((weak));
    std::u16string atob(std::u16string atob) __attribute__((weak));
    // WindowTimers
    int setTimeout(Any handler) __attribute__((weak));
    int setTimeout(Any handler, Any timeout, Variadic<Any> args = Variadic<Any>()) __attribute__((weak));
    void clearTimeout(int handle) __attribute__((weak));
    int setInterval(Any handler) __attribute__((weak));
    int setInterval(Any handler, Any timeout, Variadic<Any> args = Variadic<Any>()) __attribute__((weak));
    void clearInterval(int handle) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::Window::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::Window::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // WINDOW_IMP_H
