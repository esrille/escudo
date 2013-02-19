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

#ifndef ORG_W3C_DOM_BOOTSTRAP_WINDOWIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_WINDOWIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/Window.h>

#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/html/MediaQueryList.h>
#include <org/w3c/dom/html/Screen.h>
#include <org/w3c/dom/events/EventTarget.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/events/EventHandlerNonNull.h>
#include <org/w3c/dom/events/OnErrorEventHandlerNonNull.h>
#include <org/w3c/dom/html/Window.h>
#include <org/w3c/dom/html/BarProp.h>
#include <org/w3c/dom/html/History.h>
#include <org/w3c/dom/html/Location.h>
#include <org/w3c/dom/html/ApplicationCache.h>
#include <org/w3c/dom/html/Navigator.h>
#include <org/w3c/dom/html/External.h>
#include <org/w3c/dom/Document.h>

#include <condition_variable>
#include <cstdio>
#include <deque>
#include <mutex>
#include <thread>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include <org/w3c/dom/css/CSSStyleSheet.h>

#include "Canvas.h"
#include "DocumentWindow.h"
#include "ElementImp.h"
#include "EventTargetImp.h"
#include "HistoryImp.h"
#include "LocationImp.h"
#include "NavigatorImp.h"
#include "html/HTMLInputStream.h"
#include "html/HTMLParser.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class IcoImage;
class ViewCSSImp;

class WindowImp : public ObjectMixin<WindowImp>
{
    class EventTask
    {
    public:
        enum {
            Null = 0,
            MouseDown,
            MouseUp,
            MouseMove,
            KeyDown,
            KeyUp
        };
        int type;
        int modifiers;
        union {
            struct {
                int x;
                int y;
                int button;
            };
            struct {
                unsigned charCode;
                unsigned keyCode;
            };
        };
        EventTask(int mouseType, int modifiers, int x, int y, int button = 0) :
            type(mouseType),
            modifiers(modifiers),
            x(x),
            y(y),
            button(button)
        {}
        EventTask(int keyType, unsigned charCode, unsigned keyCode) :
            type(keyType),
            modifiers(modifiers),
            charCode(charCode),
            keyCode(keyCode)
        {}
    };

    class BackgroundTask
    {
    public:
        enum {
            Init = 0,
            Cascading,
            Cascaded,
            Layouting,
            Done,
        };
        enum {
            Abort = 1,
            Cascade = 4,
            Layout = 8,
            Restart = 16
        };

    private:
        WindowImp* window;
        std::mutex mutex;
        std::condition_variable cond;
        volatile int state;
        volatile unsigned flags;
        ViewCSSImp* view;
        volatile bool xfered;

        void deleteView();

    public:
        BackgroundTask(WindowImp* window);
        ~BackgroundTask();
        void operator()();
        unsigned sleep();
        void wakeUp(unsigned flags);
        void abort();
        void restart(unsigned flags = 0);
        ViewCSSImp* getView();
        int getState() const {
            return state;
        }
        bool isIdle() const {
            return state == Done && !flags && xfered;
        }
    };

    class Parser {
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream;
        HTMLInputStream htmlInputStream;
        HTMLTokenizer tokenizer;
        HTMLParser parser;
    public:
        Parser(DocumentImp* document, int fd, const std::string& optionalEncoding);

        Token getToken() {
            return tokenizer.getToken();
        }
        bool processToken(Token& token) {
            return parser.processToken(token);
        }
        const std::string& getEncoding() {
            return htmlInputStream.getEncoding();
        }

        bool processPendingParsingBlockingScript() {
            return parser.processPendingParsingBlockingScript();
        }
    };

    HttpRequest request;
    Retained<NavigatorImp> navigator;
    Retained<HistoryImp> history;
    BackgroundTask backgroundTask;
    std::thread thread;

    DocumentWindowPtr window;
    ViewCSSImp* view;
    unsigned viewFlags;

    WindowImp* parent;
    std::deque<WindowImp*> childWindows;
    ElementImp* frameElement;

    std::deque<EventTask> eventQueue;

    Parser* parser;

    // for MouseEvent
    Element clickTarget;
    int detail;
    unsigned short buttons;

    // for display
    Canvas canvas;
    float scrollWidth;
    float scrollHeight;
    unsigned width;
    unsigned height;
    bool redisplay;  // set true to force redisplay
    bool zoomable;
    float zoom;

    bool faviconOverridable;

    // for report
    unsigned windowDepth;

    void mouse(const EventTask& task);
    void mouseMove(const EventTask& task);
    void keydown(const EventTask& task);
    void keyup(const EventTask& task);

public:
    WindowImp(WindowImp* parent = 0, ElementImp* frameElement = 0);
    ~WindowImp();

    static css::CSSStyleSheet defaultStyleSheet;

    ViewCSSImp* getView() const {
        return view;
    }
    void setSize(unsigned w, unsigned h);
    void setFlags(unsigned f);

    bool isBindingDocumentWindow() const;

    bool poll();
    void render(ViewCSSImp* parentView);
    DocumentWindowPtr getDocumentWindow() const {
        return window;
    }
    void updateView(ViewCSSImp* next);
    void setDocumentWindow(const DocumentWindowPtr& window);

    float getScrollWidth() const {
        return scrollWidth;
    }
    float getScrollHeight() const {
        return scrollHeight;
    }

    void enter();
    void exit();

    void enableZoom(bool value);
    float getZoom() const;
    void setZoom(float value);

    bool getFaviconOverridable() const {
        return faviconOverridable;
    }
    void setFaviconOverridable(bool value) {
        faviconOverridable = value;
    }

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
    void mouse(int button, int state, int x, int y, int modifiers);
    void mouseMove(int x, int y, int modifiers);
    void keydown(unsigned charCode, unsigned keyCode, int modifiers);
    void keyup(unsigned charCode, unsigned keyCode, int modifiers);

    // CSSOM View support operations
    // Maybe we should keep the old DocumentView interface
    // to make these operations usable via the formal way from
    // DocumentImp.
    Element elementFromPoint(float x, float y);

    void setFavicon(IcoImage* ico, std::FILE* file);
    void setFavicon(BoxImage* image);
    void setFavicon();

    ElementImp* getFrameElementImp() const {
        return frameElement;
    }

    // Window
    html::Window getWindow();
    Any getSelf();
    void setSelf(Any self);
    Document getDocument();
    std::u16string getName();
    void setName(const std::u16string& name);
    html::Location getLocation();
    void setLocation(const std::u16string& location);
    html::History getHistory();
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
    void setStatus(const std::u16string& status);
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
    html::Window open(const std::u16string& url = u"about:blank", const std::u16string& target = u"_blank", const std::u16string& features = u"", bool replace = false);
    html::Window getElement(unsigned int index);
    Object getElement(const std::u16string& name);
    void setElement(const std::u16string& name, Any value);
    html::Navigator getNavigator();
    html::External getExternal();
    html::ApplicationCache getApplicationCache();
    void alert(const std::u16string& message);
    bool confirm(const std::u16string& message);
    Nullable<std::u16string> prompt(const std::u16string& message);
    Nullable<std::u16string> prompt(const std::u16string& message, const std::u16string& _default);
    void print();
    Any showModalDialog(const std::u16string& url);
    Any showModalDialog(const std::u16string& url, Any argument);
    events::EventHandlerNonNull getOnabort();
    void setOnabort(events::EventHandlerNonNull onabort);
    events::EventHandlerNonNull getOnafterprint();
    void setOnafterprint(events::EventHandlerNonNull onafterprint);
    events::EventHandlerNonNull getOnbeforeprint();
    void setOnbeforeprint(events::EventHandlerNonNull onbeforeprint);
    events::EventHandlerNonNull getOnbeforeunload();
    void setOnbeforeunload(events::EventHandlerNonNull onbeforeunload);
    events::EventHandlerNonNull getOnblur();
    void setOnblur(events::EventHandlerNonNull onblur);
    events::EventHandlerNonNull getOncancel();
    void setOncancel(events::EventHandlerNonNull oncancel);
    events::EventHandlerNonNull getOncanplay();
    void setOncanplay(events::EventHandlerNonNull oncanplay);
    events::EventHandlerNonNull getOncanplaythrough();
    void setOncanplaythrough(events::EventHandlerNonNull oncanplaythrough);
    events::EventHandlerNonNull getOnchange();
    void setOnchange(events::EventHandlerNonNull onchange);
    events::EventHandlerNonNull getOnclick();
    void setOnclick(events::EventHandlerNonNull onclick);
    events::EventHandlerNonNull getOnclose();
    void setOnclose(events::EventHandlerNonNull onclose);
    events::EventHandlerNonNull getOncontextmenu();
    void setOncontextmenu(events::EventHandlerNonNull oncontextmenu);
    events::EventHandlerNonNull getOncuechange();
    void setOncuechange(events::EventHandlerNonNull oncuechange);
    events::EventHandlerNonNull getOndblclick();
    void setOndblclick(events::EventHandlerNonNull ondblclick);
    events::EventHandlerNonNull getOndrag();
    void setOndrag(events::EventHandlerNonNull ondrag);
    events::EventHandlerNonNull getOndragend();
    void setOndragend(events::EventHandlerNonNull ondragend);
    events::EventHandlerNonNull getOndragenter();
    void setOndragenter(events::EventHandlerNonNull ondragenter);
    events::EventHandlerNonNull getOndragleave();
    void setOndragleave(events::EventHandlerNonNull ondragleave);
    events::EventHandlerNonNull getOndragover();
    void setOndragover(events::EventHandlerNonNull ondragover);
    events::EventHandlerNonNull getOndragstart();
    void setOndragstart(events::EventHandlerNonNull ondragstart);
    events::EventHandlerNonNull getOndrop();
    void setOndrop(events::EventHandlerNonNull ondrop);
    events::EventHandlerNonNull getOndurationchange();
    void setOndurationchange(events::EventHandlerNonNull ondurationchange);
    events::EventHandlerNonNull getOnemptied();
    void setOnemptied(events::EventHandlerNonNull onemptied);
    events::EventHandlerNonNull getOnended();
    void setOnended(events::EventHandlerNonNull onended);
    events::OnErrorEventHandlerNonNull getOnerror();
    void setOnerror(events::OnErrorEventHandlerNonNull onerror);
    events::EventHandlerNonNull getOnfocus();
    void setOnfocus(events::EventHandlerNonNull onfocus);
    events::EventHandlerNonNull getOnhashchange();
    void setOnhashchange(events::EventHandlerNonNull onhashchange);
    events::EventHandlerNonNull getOninput();
    void setOninput(events::EventHandlerNonNull oninput);
    events::EventHandlerNonNull getOninvalid();
    void setOninvalid(events::EventHandlerNonNull oninvalid);
    events::EventHandlerNonNull getOnkeydown();
    void setOnkeydown(events::EventHandlerNonNull onkeydown);
    events::EventHandlerNonNull getOnkeypress();
    void setOnkeypress(events::EventHandlerNonNull onkeypress);
    events::EventHandlerNonNull getOnkeyup();
    void setOnkeyup(events::EventHandlerNonNull onkeyup);
    events::EventHandlerNonNull getOnload();
    void setOnload(events::EventHandlerNonNull onload);
    events::EventHandlerNonNull getOnloadeddata();
    void setOnloadeddata(events::EventHandlerNonNull onloadeddata);
    events::EventHandlerNonNull getOnloadedmetadata();
    void setOnloadedmetadata(events::EventHandlerNonNull onloadedmetadata);
    events::EventHandlerNonNull getOnloadstart();
    void setOnloadstart(events::EventHandlerNonNull onloadstart);
    events::EventHandlerNonNull getOnmessage();
    void setOnmessage(events::EventHandlerNonNull onmessage);
    events::EventHandlerNonNull getOnmousedown();
    void setOnmousedown(events::EventHandlerNonNull onmousedown);
    events::EventHandlerNonNull getOnmousemove();
    void setOnmousemove(events::EventHandlerNonNull onmousemove);
    events::EventHandlerNonNull getOnmouseout();
    void setOnmouseout(events::EventHandlerNonNull onmouseout);
    events::EventHandlerNonNull getOnmouseover();
    void setOnmouseover(events::EventHandlerNonNull onmouseover);
    events::EventHandlerNonNull getOnmouseup();
    void setOnmouseup(events::EventHandlerNonNull onmouseup);
    events::EventHandlerNonNull getOnmousewheel();
    void setOnmousewheel(events::EventHandlerNonNull onmousewheel);
    events::EventHandlerNonNull getOnoffline();
    void setOnoffline(events::EventHandlerNonNull onoffline);
    events::EventHandlerNonNull getOnonline();
    void setOnonline(events::EventHandlerNonNull ononline);
    events::EventHandlerNonNull getOnpause();
    void setOnpause(events::EventHandlerNonNull onpause);
    events::EventHandlerNonNull getOnplay();
    void setOnplay(events::EventHandlerNonNull onplay);
    events::EventHandlerNonNull getOnplaying();
    void setOnplaying(events::EventHandlerNonNull onplaying);
    events::EventHandlerNonNull getOnpagehide();
    void setOnpagehide(events::EventHandlerNonNull onpagehide);
    events::EventHandlerNonNull getOnpageshow();
    void setOnpageshow(events::EventHandlerNonNull onpageshow);
    events::EventHandlerNonNull getOnpopstate();
    void setOnpopstate(events::EventHandlerNonNull onpopstate);
    events::EventHandlerNonNull getOnprogress();
    void setOnprogress(events::EventHandlerNonNull onprogress);
    events::EventHandlerNonNull getOnratechange();
    void setOnratechange(events::EventHandlerNonNull onratechange);
    events::EventHandlerNonNull getOnreset();
    void setOnreset(events::EventHandlerNonNull onreset);
    events::EventHandlerNonNull getOnresize();
    void setOnresize(events::EventHandlerNonNull onresize);
    events::EventHandlerNonNull getOnscroll();
    void setOnscroll(events::EventHandlerNonNull onscroll);
    events::EventHandlerNonNull getOnseeked();
    void setOnseeked(events::EventHandlerNonNull onseeked);
    events::EventHandlerNonNull getOnseeking();
    void setOnseeking(events::EventHandlerNonNull onseeking);
    events::EventHandlerNonNull getOnselect();
    void setOnselect(events::EventHandlerNonNull onselect);
    events::EventHandlerNonNull getOnshow();
    void setOnshow(events::EventHandlerNonNull onshow);
    events::EventHandlerNonNull getOnstalled();
    void setOnstalled(events::EventHandlerNonNull onstalled);
    events::EventHandlerNonNull getOnstorage();
    void setOnstorage(events::EventHandlerNonNull onstorage);
    events::EventHandlerNonNull getOnsubmit();
    void setOnsubmit(events::EventHandlerNonNull onsubmit);
    events::EventHandlerNonNull getOnsuspend();
    void setOnsuspend(events::EventHandlerNonNull onsuspend);
    events::EventHandlerNonNull getOntimeupdate();
    void setOntimeupdate(events::EventHandlerNonNull ontimeupdate);
    events::EventHandlerNonNull getOnunload();
    void setOnunload(events::EventHandlerNonNull onunload);
    events::EventHandlerNonNull getOnvolumechange();
    void setOnvolumechange(events::EventHandlerNonNull onvolumechange);
    events::EventHandlerNonNull getOnwaiting();
    void setOnwaiting(events::EventHandlerNonNull onwaiting);
    // Window-1
    css::CSSStyleDeclaration getComputedStyle(Element elt);
    css::CSSStyleDeclaration getComputedStyle(Element elt, const std::u16string& pseudoElt);
    // Window
    html::MediaQueryList matchMedia(const std::u16string& media_query_list);
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
    void addEventListener(const std::u16string& type, events::EventListener listener, bool capture = false);
    void removeEventListener(const std::u16string& type, events::EventListener listener, bool capture = false);
    bool dispatchEvent(events::Event event);
    // WindowBase64
    std::u16string btoa(const std::u16string& btoa);
    std::u16string atob(const std::u16string& atob);
    // WindowTimers
    int setTimeout(events::EventHandlerNonNull handler);
    int setTimeout(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments = Variadic<Any>());
    int setTimeout(const std::u16string& handler);
    int setTimeout(const std::u16string& handler, int timeout, Variadic<Any> arguments = Variadic<Any>());
    void clearTimeout(int handle);
    int setInterval(events::EventHandlerNonNull handler);
    int setInterval(events::EventHandlerNonNull handler, int timeout, Variadic<Any> arguments = Variadic<Any>());
    int setInterval(const std::u16string& handler);
    int setInterval(const std::u16string& handler, int timeout, Variadic<Any> arguments = Variadic<Any>());
    void clearInterval(int handle);
    // LineEndings
    std::u16string toNativeLineEndings(const std::u16string& string);
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

#endif  // ORG_W3C_DOM_BOOTSTRAP_WINDOWIMP_H_INCLUDED
