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
#include <org/w3c/dom/html/MediaQueryList.h>
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

#include "DocumentWindow.h"
#include "EventTargetImp.h"
#include "HistoryImp.h"
#include "LocationImp.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class ViewCSSImp;

class WindowImp : public ObjectMixin<WindowImp>
{
    HttpRequest request;
    Retained<HistoryImp> history;

    DocumentWindowPtr window;
    ViewCSSImp* view;
    Box* boxTree;

    WindowImp* parent;
    std::deque<WindowImp*> childWindows;

    // for MouseEvent
    int detail;
    unsigned short buttons;

    // for display
    unsigned width;
    unsigned height;
    bool redisplay;  // set true to force redisplay
    bool zoomable;
    float zoom;

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

    void setFlagsToBoxTree(unsigned f);

    bool poll();
    void render();
    DocumentWindowPtr getDocumentWindow() const {
        return window;
    }
    void setDocumentWindow(const DocumentWindowPtr& window);
    void refreshView();

    DocumentWindowPtr activate();

    void enableZoom(bool value);
    float getZoom() const;
    void setZoom(float value);

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
    //
    // return true to propagate the event to the underlying layer
    bool mouse(int button, int state, int x, int y, int modifiers);
    bool mouseMove(int x, int y, int modifiers);
    bool keydown(unsigned charCode, unsigned keyCode, int modifiers);
    bool keyup(unsigned charCode, unsigned keyCode, int modifiers);

    // CSSOM View support operations
    // Maybe we should keep the old DocumentView interface
    // to make these operations usable via the formal way from
    // DocumentImp.
    Element elementFromPoint(float x, float y);

    // Window customized
    html::Window open(std::u16string url = u"about:blank", std::u16string target = u"_blank", std::u16string features = u"", bool replace = false);

    // Window
    html::Window getWindow();
    html::Window getSelf();
    Document getDocument();
    std::u16string getName();
    void setName(std::u16string name);
    html::Location getLocation();
    void setLocation(std::u16string location);
    html::History getHistory();
    html::UndoManager getUndoManager();
    Any getLocationbar();
    void setLocationbar(Any locationbar);
    Any getMenubar();
    void setMenubar(Any menubar);
    Any getPersonalbar();
    void setPersonalbar(Any personalbar);
    Any getScrollbars();
    void setScrollbars(Any scrollbars);
    Any getStatusbar();
    void setStatusbar(Any statusbar);
    Any getToolbar();
    void setToolbar(Any toolbar);
    std::u16string getStatus();
    void setStatus(std::u16string status);
    void close();
    void stop();
    void focus();
    void blur();
    Any getFrames();
    void setFrames(Any frames);
    Any getLength();
    void setLength(Any length);
    html::Window getTop();
    html::Window getOpener();
    void setOpener(html::Window opener);
    html::Window getParent();
    Element getFrameElement();
    html::Window getElement(unsigned int index);
    Any getElement(std::u16string name);
    void setElement(std::u16string name, Any value);
    html::Navigator getNavigator();
    html::External getExternal();
    html::ApplicationCache getApplicationCache();
    void alert(std::u16string message);
    bool confirm(std::u16string message);
    Nullable<std::u16string> prompt(std::u16string message);
    Nullable<std::u16string> prompt(std::u16string message, std::u16string _default);
    void print();
    Any showModalDialog(std::u16string url);
    Any showModalDialog(std::u16string url, Any argument);
    void postMessage(Any message, std::u16string targetOrigin);
    void postMessage(Any message, std::u16string targetOrigin, Sequence<html::Transferable> transfer);
    html::Function getOnabort();
    void setOnabort(html::Function onabort);
    html::Function getOnafterprint();
    void setOnafterprint(html::Function onafterprint);
    html::Function getOnbeforeprint();
    void setOnbeforeprint(html::Function onbeforeprint);
    html::Function getOnbeforeunload();
    void setOnbeforeunload(html::Function onbeforeunload);
    html::Function getOnblur();
    void setOnblur(html::Function onblur);
    html::Function getOncanplay();
    void setOncanplay(html::Function oncanplay);
    html::Function getOncanplaythrough();
    void setOncanplaythrough(html::Function oncanplaythrough);
    html::Function getOnchange();
    void setOnchange(html::Function onchange);
    html::Function getOnclick();
    void setOnclick(html::Function onclick);
    html::Function getOncontextmenu();
    void setOncontextmenu(html::Function oncontextmenu);
    html::Function getOncuechange();
    void setOncuechange(html::Function oncuechange);
    html::Function getOndblclick();
    void setOndblclick(html::Function ondblclick);
    html::Function getOndrag();
    void setOndrag(html::Function ondrag);
    html::Function getOndragend();
    void setOndragend(html::Function ondragend);
    html::Function getOndragenter();
    void setOndragenter(html::Function ondragenter);
    html::Function getOndragleave();
    void setOndragleave(html::Function ondragleave);
    html::Function getOndragover();
    void setOndragover(html::Function ondragover);
    html::Function getOndragstart();
    void setOndragstart(html::Function ondragstart);
    html::Function getOndrop();
    void setOndrop(html::Function ondrop);
    html::Function getOndurationchange();
    void setOndurationchange(html::Function ondurationchange);
    html::Function getOnemptied();
    void setOnemptied(html::Function onemptied);
    html::Function getOnended();
    void setOnended(html::Function onended);
    html::Function getOnerror();
    void setOnerror(html::Function onerror);
    html::Function getOnfocus();
    void setOnfocus(html::Function onfocus);
    html::Function getOnhashchange();
    void setOnhashchange(html::Function onhashchange);
    html::Function getOninput();
    void setOninput(html::Function oninput);
    html::Function getOninvalid();
    void setOninvalid(html::Function oninvalid);
    html::Function getOnkeydown();
    void setOnkeydown(html::Function onkeydown);
    html::Function getOnkeypress();
    void setOnkeypress(html::Function onkeypress);
    html::Function getOnkeyup();
    void setOnkeyup(html::Function onkeyup);
    html::Function getOnload();
    void setOnload(html::Function onload);
    html::Function getOnloadeddata();
    void setOnloadeddata(html::Function onloadeddata);
    html::Function getOnloadedmetadata();
    void setOnloadedmetadata(html::Function onloadedmetadata);
    html::Function getOnloadstart();
    void setOnloadstart(html::Function onloadstart);
    html::Function getOnmessage();
    void setOnmessage(html::Function onmessage);
    html::Function getOnmousedown();
    void setOnmousedown(html::Function onmousedown);
    html::Function getOnmousemove();
    void setOnmousemove(html::Function onmousemove);
    html::Function getOnmouseout();
    void setOnmouseout(html::Function onmouseout);
    html::Function getOnmouseover();
    void setOnmouseover(html::Function onmouseover);
    html::Function getOnmouseup();
    void setOnmouseup(html::Function onmouseup);
    html::Function getOnmousewheel();
    void setOnmousewheel(html::Function onmousewheel);
    html::Function getOnoffline();
    void setOnoffline(html::Function onoffline);
    html::Function getOnonline();
    void setOnonline(html::Function ononline);
    html::Function getOnpause();
    void setOnpause(html::Function onpause);
    html::Function getOnplay();
    void setOnplay(html::Function onplay);
    html::Function getOnplaying();
    void setOnplaying(html::Function onplaying);
    html::Function getOnpagehide();
    void setOnpagehide(html::Function onpagehide);
    html::Function getOnpageshow();
    void setOnpageshow(html::Function onpageshow);
    html::Function getOnpopstate();
    void setOnpopstate(html::Function onpopstate);
    html::Function getOnprogress();
    void setOnprogress(html::Function onprogress);
    html::Function getOnratechange();
    void setOnratechange(html::Function onratechange);
    html::Function getOnreadystatechange();
    void setOnreadystatechange(html::Function onreadystatechange);
    html::Function getOnredo();
    void setOnredo(html::Function onredo);
    html::Function getOnreset();
    void setOnreset(html::Function onreset);
    html::Function getOnresize();
    void setOnresize(html::Function onresize);
    html::Function getOnscroll();
    void setOnscroll(html::Function onscroll);
    html::Function getOnseeked();
    void setOnseeked(html::Function onseeked);
    html::Function getOnseeking();
    void setOnseeking(html::Function onseeking);
    html::Function getOnselect();
    void setOnselect(html::Function onselect);
    html::Function getOnshow();
    void setOnshow(html::Function onshow);
    html::Function getOnstalled();
    void setOnstalled(html::Function onstalled);
    html::Function getOnstorage();
    void setOnstorage(html::Function onstorage);
    html::Function getOnsubmit();
    void setOnsubmit(html::Function onsubmit);
    html::Function getOnsuspend();
    void setOnsuspend(html::Function onsuspend);
    html::Function getOntimeupdate();
    void setOntimeupdate(html::Function ontimeupdate);
    html::Function getOnundo();
    void setOnundo(html::Function onundo);
    html::Function getOnunload();
    void setOnunload(html::Function onunload);
    html::Function getOnvolumechange();
    void setOnvolumechange(html::Function onvolumechange);
    html::Function getOnwaiting();
    void setOnwaiting(html::Function onwaiting);
    // Window-4
    css::CSSStyleDeclaration getComputedStyle(Element elt);
    css::CSSStyleDeclaration getComputedStyle(Element elt, std::u16string pseudoElt);
    // Window-5
    html::MediaQueryList matchMedia(std::u16string media_query_list);
    html::Screen getScreen();
    int getInnerWidth();
    int getInnerHeight();
    int getScrollX();
    int getPageXOffset();
    int getScrollY();
    int getPageYOffset();
    void scroll(int x, int y);
    void scrollTo(int x, int y);
    void scrollBy(int x, int y);
    int getScreenX();
    int getScreenY();
    int getOuterWidth();
    int getOuterHeight();
    // EventTarget
    void addEventListener(std::u16string type, events::EventListener listener, bool capture = false);
    void removeEventListener(std::u16string type, events::EventListener listener, bool capture = false);
    bool dispatchEvent(events::Event event);
    // WindowBase64
    std::u16string btoa(std::u16string btoa);
    std::u16string atob(std::u16string atob);
    // WindowTimers
    int setTimeout(Any handler);
    int setTimeout(Any handler, Any timeout, Variadic<Any> args = Variadic<Any>());
    void clearTimeout(int handle);
    int setInterval(Any handler);
    int setInterval(Any handler, Any timeout, Variadic<Any> args = Variadic<Any>());
    void clearInterval(int handle);
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
