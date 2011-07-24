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

#include "WindowImp.h"

#include <new>
#include <iostream>
#include <boost/version.hpp>
#include <boost/bind.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "DOMImplementationImp.h"
#include "DocumentImp.h"
#include "html/HTMLParser.h"
#include "css/ViewCSSImp.h"

#include "NodeImp.h"  // for eval()

#include "KeyboardEventImp.h"
#include "MouseEventImp.h"

#include "js/Script.h"

#include "Test.util.h"

#include "http/HTTPConnection.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

void evalTree(Node node)
{
    while (node) {
        if (node.hasChildNodes())
            eval(node.getFirstChild());
        dynamic_cast<NodeImp*>(node.self())->eval();;
        node = node.getNextSibling();
    }
}

}

WindowImp::WindowImp(WindowImp* parent) :
    request(parent ? parent->getLocation().getHref() : u""),
    history(this),
    window(0),
    view(0),
    boxTree(0),
    parent(parent),
    detail(0),
    width(816),     // US letter size, 96 DPI
    height(1056),
    redisplay(false)
{
    if (parent)
        parent->childWindows.push_back(this);
}

WindowImp::~WindowImp()
{
    if (parent) {
        for (auto i = parent->childWindows.begin(); i != parent->childWindows.end(); ++i) {
            if (*i == this) {
                parent->childWindows.erase(i);
                break;
            }
        }
    }
}

void WindowImp::refreshView()
{
    assert(window->getDocument());

    delete view;

    view = new(std::nothrow) ViewCSSImp(window->getDocument(), getDOMImplementation()->getDefaultCSSStyleSheet());
    if (!view)
        return;
    view->cascade();
    printComputedValues(window->getDocument(), view);  // for debug
    view->setSize(width, height);
    boxTree = view->layOut();

    detail = 0;

    redisplay = true;
}

void WindowImp::setDocumentWindow(const DocumentWindowPtr& window)
{
    this->window = window;
    refreshView();
}

bool WindowImp::poll()
{
    for (auto i = childWindows.begin(); i != childWindows.end(); ++i) {
        WindowImp* child = *i;
        if (child->poll()) {
            redisplay |= true;
            if (boxTree)
                boxTree->setFlags(1); // TODO: refine
        }
    }

    HttpConnectionManager::getIOService().poll();
    switch (request.getReadyState())
    {
    case HttpRequest::UNSENT:
        break;
    case HttpRequest::OPENED:
    case HttpRequest::HEADERS_RECEIVED:
    case HttpRequest::LOADING:
        break;
    case HttpRequest::DONE:
        if (!window->getDocument()) {
            if (!request.getErrorFlag()) {
                // TODO: Check header
                window->setDocument(getDOMImplementation()->createDocument(u"", u"", 0));
            }
            if (!window->getDocument())
                break;

            DocumentImp* imp = dynamic_cast<DocumentImp*>(window->getDocument().self());
            if (imp) {
                imp->setDefaultView(this);
                imp->setURL(request.getRequestMessage().getURL());
                history.update(window);
            }

#if 104400 <= BOOST_VERSION
            boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request.getContentDescriptor(), boost::iostreams::never_close_handle);
#else
            boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request.getContentDescriptor(), false);
#endif
            stream.seekg(0, std::ios::beg);
            HTMLInputStream htmlInputStream(stream, "utf-8");  // TODO check header
            HTMLTokenizer tokenizer(&htmlInputStream);
            HTMLParser parser(window->getDocument(), &tokenizer);
            parser.mainLoop();  // TODO: run thin in background

            // Each HTML element will have a style attribute if there's the style content attribute.
            // Each HTML style element will have a style sheet.
            evalTree(window->getDocument());
            dumpTree(std::cerr, window->getDocument());

            // load
            if (EventImp* event = new(std::nothrow) EventImp) {
                event->initEvent(u"load", false, false);
                window->dispatchEvent(event);
            }

            refreshView();
        } else if (boxTree && boxTree->isFlagged()) {
            view->cascade();
            view->setSize(8.5f * 96, 11.0f * 96);  // US letter size, 96 DPI
            boxTree = view->layOut();
            redisplay = true;
        }
    }
    bool result = redisplay;
    redisplay = false;
    return result;
}

void WindowImp::render()
{
    if (view) {
        view->render();
        boxTree->dump(view);
    }
}

void WindowImp::mouse(int button, int up, int x, int y, int modifiers)
{
    if (!view)
        return;

    events::MouseEvent event(0);

    Box* box = view->lookupTarget(x, y);
    ViewCSSImp* shadow = box->getShadow();
    if (shadow) {
        html::Window child = shadow->getDocument().getDefaultView();
        if (child) {
            WindowImp* childWindow = dynamic_cast<WindowImp*>(child.self());
            childWindow->mouse(button, up, x, y, modifiers);
            return;
        }
    }

    // mousedown, mousemove
    if (event = new(std::nothrow) MouseEventImp) {
        if (!up)
            ++detail;
        event.initMouseEvent(up ? u"mouseup" : u"mousedown",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, button, 0);
        box->getTargetNode().dispatchEvent(event);
    }

    if (!up || detail == 0)
        return;
    Box* clickBox = view->lookupTarget(x, y);
    if (box != clickBox)
        return;

    // click
    if (event = new(std::nothrow) MouseEventImp) {
        event.initMouseEvent(u"click",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, button, 0);
        box->getTargetNode().dispatchEvent(event);
    }
}

void WindowImp::mouseMove(int x, int y, int modifiers)
{
    if (!view)
        return;

    Box* box = view->lookupTarget(x, y);
    ViewCSSImp* shadow = box->getShadow();
    if (shadow) {
        html::Window child = shadow->getDocument().getDefaultView();
        if (child) {
            WindowImp* childWindow = dynamic_cast<WindowImp*>(child.self());
            childWindow->mouseMove(x, y, modifiers);
            return;
        }
    }

    // mousemove
    events::MouseEvent event(0);
    if (event = new(std::nothrow) MouseEventImp) {
        detail = 0;
        event.initMouseEvent(u"mousemove",
                            true, true, this, detail, x, y, x, y,
                            modifiers & 2, modifiers & 4, modifiers & 1, false, 0, 0);
        box->getTargetNode().dispatchEvent(event);
    }
}

void WindowImp::keydown(unsigned charCode, unsigned keyCode, int modifiers)
{
    Document document = window->getDocument();
    if (!document)
        return;
    Element e = document.getActiveElement();
    if (!e)
        return;
    events::KeyboardEvent event = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    event.initKeyboardEvent(u"keydown", true, true, this,
                            u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);

    if (!charCode)
        return;
    events::KeyboardEvent text = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    text.initKeyboardEvent(u"keypress", true, true, this,
                            u"", u"", 0, u"", false, u"");
    e.dispatchEvent(text);
}

void WindowImp::keyup(unsigned charCode, unsigned keyCode, int modifiers)
{
    Document document = window->getDocument();
    if (!document)
        return;
    Element e = document.getActiveElement();
    if (!e)
        return;
    events::KeyboardEvent event = new(std::nothrow) KeyboardEventImp(modifiers, charCode, keyCode, 0);
    event.initKeyboardEvent(u"keyup", true, true, this,
                            u"", u"", 0, u"", false, u"");
    e.dispatchEvent(event);
}

//
// html::Window - implemented
//

html::Window WindowImp::getWindow()
{
    return this;
}

html::Window WindowImp::getSelf()
{
    return this;
}

Document WindowImp::getDocument()
{
    return window->getDocument();
}

html::Location WindowImp::getLocation()
{
    return getDocument().getLocation();
}

void WindowImp::setLocation(std::u16string href)
{
    getLocation().setHref(href);
}

html::History WindowImp::getHistory()
{
    return &history;
}

Any WindowImp::getFrames()
{
    return this;
}

Any WindowImp::getLength()
{
    return childWindows.size();
}

html::Window WindowImp::getTop()
{
    WindowImp* top = this;
    while (top->parent)
        top = top->parent;
    return top;
}

html::Window WindowImp::getParent()
{
    return parent;
}

html::Window WindowImp::open(std::u16string url, std::u16string target, std::u16string features, bool replace)
{
    // TODO: add more details
    window = new(std::nothrow) DocumentWindow;

    if (url.empty())
        url = u"about:blank";
    request.abort();
    request.open(u"get", url);
    request.send();
    HttpConnectionManager::getIOService().poll();
    return this;
}

html::Window WindowImp::getElement(unsigned int index)
{
    if (index < childWindows.size())
        return childWindows[index];
    return 0;
}

void WindowImp::alert(std::u16string message)
{
    std::cerr << message << '\n';
}

//
// html::Window - just generated
//

std::u16string WindowImp::getName()
{
    // TODO: implement me!
    return u"";
}

void WindowImp::setName(std::u16string name)
{
    // TODO: implement me!
}

html::UndoManager WindowImp::getUndoManager()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Any WindowImp::getLocationbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setLocationbar(Any locationbar)
{
    // TODO: implement me!
}

Any WindowImp::getMenubar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setMenubar(Any menubar)
{
    // TODO: implement me!
}

Any WindowImp::getPersonalbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setPersonalbar(Any personalbar)
{
    // TODO: implement me!
}

Any WindowImp::getScrollbars()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setScrollbars(Any scrollbars)
{
    // TODO: implement me!
}

Any WindowImp::getStatusbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setStatusbar(Any statusbar)
{
    // TODO: implement me!
}

Any WindowImp::getToolbar()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setToolbar(Any toolbar)
{
    // TODO: implement me!
}

std::u16string WindowImp::getStatus()
{
    // TODO: implement me!
    return u"";
}

void WindowImp::setStatus(std::u16string status)
{
    // TODO: implement me!
}

void WindowImp::close()
{
    // TODO: implement me!
}

void WindowImp::stop()
{
    // TODO: implement me!
}

void WindowImp::focus()
{
    // TODO: implement me!
}

void WindowImp::blur()
{
    // TODO: implement me!
}

void WindowImp::setFrames(Any frames)
{
    // TODO: implement me!
}

void WindowImp::setLength(Any length)
{
    // TODO: implement me!
}

html::Window WindowImp::getOpener()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOpener(html::Window opener)
{
    // TODO: implement me!
}

Element WindowImp::getFrameElement()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Any WindowImp::getElement(std::u16string name)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::setElement(std::u16string name, Any value)
{
    // TODO: implement me!
}

html::Navigator WindowImp::getNavigator()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::External WindowImp::getExternal()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::ApplicationCache WindowImp::getApplicationCache()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

bool WindowImp::confirm(std::u16string message)
{
    // TODO: implement me!
    return 0;
}

Nullable<std::u16string> WindowImp::prompt(std::u16string message)
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> WindowImp::prompt(std::u16string message, std::u16string _default)
{
    // TODO: implement me!
    return u"";
}

void WindowImp::print()
{
    // TODO: implement me!
}

Any WindowImp::showModalDialog(std::u16string url)
{
    // TODO: implement me!
    return 0;
}

Any WindowImp::showModalDialog(std::u16string url, Any argument)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::postMessage(Any message, std::u16string targetOrigin)
{
    // TODO: implement me!
}

void WindowImp::postMessage(Any message, std::u16string targetOrigin, Sequence<html::Transferable> transfer)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnabort()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnabort(html::Function onabort)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnafterprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnafterprint(html::Function onafterprint)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnbeforeprint()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnbeforeprint(html::Function onbeforeprint)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnbeforeunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnbeforeunload(html::Function onbeforeunload)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnblur()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnblur(html::Function onblur)
{
    // TODO: implement me!
}

html::Function WindowImp::getOncanplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncanplay(html::Function oncanplay)
{
    // TODO: implement me!
}

html::Function WindowImp::getOncanplaythrough()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncanplaythrough(html::Function oncanplaythrough)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnchange(html::Function onchange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnclick(html::Function onclick)
{
    // TODO: implement me!
}

html::Function WindowImp::getOncontextmenu()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncontextmenu(html::Function oncontextmenu)
{
    // TODO: implement me!
}

html::Function WindowImp::getOncuechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOncuechange(html::Function oncuechange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndblclick()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndblclick(html::Function ondblclick)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndrag()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndrag(html::Function ondrag)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndragend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragend(html::Function ondragend)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndragenter()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragenter(html::Function ondragenter)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndragleave()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragleave(html::Function ondragleave)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndragover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragover(html::Function ondragover)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndragstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndragstart(html::Function ondragstart)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndrop()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndrop(html::Function ondrop)
{
    // TODO: implement me!
}

html::Function WindowImp::getOndurationchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOndurationchange(html::Function ondurationchange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnemptied()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnemptied(html::Function onemptied)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnended()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnended(html::Function onended)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnerror()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnerror(html::Function onerror)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnfocus()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnfocus(html::Function onfocus)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnhashchange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnhashchange(html::Function onhashchange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOninput()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOninput(html::Function oninput)
{
    // TODO: implement me!
}

html::Function WindowImp::getOninvalid()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOninvalid(html::Function oninvalid)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnkeydown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeydown(html::Function onkeydown)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnkeypress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeypress(html::Function onkeypress)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnkeyup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnkeyup(html::Function onkeyup)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnload(html::Function onload)
{
    if (!window)
        return;
    window->addEventListener(u"load",
                             new(std::nothrow) EventListenerImp(boost::bind(callFunction, onload, _1)),
                             false);
}

html::Function WindowImp::getOnloadeddata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadeddata(html::Function onloadeddata)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnloadedmetadata()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadedmetadata(html::Function onloadedmetadata)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnloadstart()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnloadstart(html::Function onloadstart)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmessage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmessage(html::Function onmessage)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmousedown()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousedown(html::Function onmousedown)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmousemove()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousemove(html::Function onmousemove)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmouseout()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseout(html::Function onmouseout)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmouseover()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseover(html::Function onmouseover)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmouseup()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmouseup(html::Function onmouseup)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnmousewheel()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnmousewheel(html::Function onmousewheel)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnoffline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnoffline(html::Function onoffline)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnonline()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnonline(html::Function ononline)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnpause()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpause(html::Function onpause)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnplay()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnplay(html::Function onplay)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnplaying()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnplaying(html::Function onplaying)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnpagehide()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpagehide(html::Function onpagehide)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnpageshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpageshow(html::Function onpageshow)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnpopstate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnpopstate(html::Function onpopstate)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnprogress()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnprogress(html::Function onprogress)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnratechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnratechange(html::Function onratechange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnreadystatechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnreadystatechange(html::Function onreadystatechange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnredo()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnredo(html::Function onredo)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnreset()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnreset(html::Function onreset)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnresize()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnresize(html::Function onresize)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnscroll()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnscroll(html::Function onscroll)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnseeked()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnseeked(html::Function onseeked)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnseeking()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnseeking(html::Function onseeking)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnselect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnselect(html::Function onselect)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnshow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnshow(html::Function onshow)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnstalled()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnstalled(html::Function onstalled)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnstorage()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnstorage(html::Function onstorage)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnsubmit()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnsubmit(html::Function onsubmit)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnsuspend()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnsuspend(html::Function onsuspend)
{
    // TODO: implement me!
}

html::Function WindowImp::getOntimeupdate()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOntimeupdate(html::Function ontimeupdate)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnundo()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnundo(html::Function onundo)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnunload()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnunload(html::Function onunload)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnvolumechange()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnvolumechange(html::Function onvolumechange)
{
    // TODO: implement me!
}

html::Function WindowImp::getOnwaiting()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void WindowImp::setOnwaiting(html::Function onwaiting)
{
    // TODO: implement me!
}

css::CSSStyleDeclaration WindowImp::getComputedStyle(Element elt)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

css::CSSStyleDeclaration WindowImp::getComputedStyle(Element elt, std::u16string pseudoElt)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::StyleMedia WindowImp::getStyleMedia()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Screen WindowImp::getScreen()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

int WindowImp::getInnerWidth()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getInnerHeight()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getPageXOffset()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getPageYOffset()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::scroll(int x, int y)
{
    // TODO: implement me!
}

void WindowImp::scrollTo(int x, int y)
{
    // TODO: implement me!
}

void WindowImp::scrollBy(int x, int y)
{
    // TODO: implement me!
}

int WindowImp::getScreenX()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getScreenY()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getOuterWidth()
{
    // TODO: implement me!
    return 0;
}

int WindowImp::getOuterHeight()
{
    // TODO: implement me!
    return 0;
}

void WindowImp::addEventListener(std::u16string type, events::EventListener listener)
{
    // TODO: implement me!
}

void WindowImp::addEventListener(std::u16string type, events::EventListener listener, bool capture)
{
    // TODO: implement me!
}

void WindowImp::removeEventListener(std::u16string type, events::EventListener listener)
{
    // TODO: implement me!
}

void WindowImp::removeEventListener(std::u16string type, events::EventListener listener, bool capture)
{
    // TODO: implement me!
}

bool WindowImp::dispatchEvent(events::Event event)
{
    // TODO: implement me!
    return 0;
}

std::u16string WindowImp::btoa(std::u16string btoa)
{
    // TODO: implement me!
    return u"";
}

std::u16string WindowImp::atob(std::u16string atob)
{
    // TODO: implement me!
    return u"";
}

int WindowImp::setTimeout(Any handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setTimeout(Any handler, Any timeout, Variadic<Any> args)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::clearTimeout(int handle)
{
    // TODO: implement me!
}

int WindowImp::setInterval(Any handler)
{
    // TODO: implement me!
    return 0;
}

int WindowImp::setInterval(Any handler, Any timeout, Variadic<Any> args)
{
    // TODO: implement me!
    return 0;
}

void WindowImp::clearInterval(int handle)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
