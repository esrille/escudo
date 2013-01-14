/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "DocumentWindow.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "DocumentImp.h"
#include "ECMAScript.h"
#include "WindowImp.h"
#include "css/ViewCSSImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

DocumentWindow::DocumentWindow() :
    document(0),
    global(0),
    scrollX(0),
    scrollY(0),
    clickListener(boost::bind(&DocumentWindow::handleClick, this, _1, _2)),
    mouseMoveListener(boost::bind(&DocumentWindow::handleMouseMove, this, _1, _2))
{
    addEventListener(u"click", &clickListener, false, EventTargetImp::UseDefault);
    addEventListener(u"mousemove", &mouseMoveListener, false, EventTargetImp::UseDefault);
}

DocumentWindow::~DocumentWindow()
{
    if (global) {
        delete global;
        global = 0;
    }
    while (!cache.empty()) {
        HttpRequest* request = cache.front();
        delete request;
        cache.pop_front();
    }
}

void DocumentWindow::activate()
{
    activate(dynamic_cast<WindowImp*>(document.getDefaultView().self()));
}

void DocumentWindow::activate(WindowImp* proxy)
{
    if (proxy && global)
        global->activate(proxy);
}

void DocumentWindow::setEventHandler(const std::u16string& type, Object handler)
{
    EventListenerImp* listener = getEventHandlerListener(type);
    if (listener) {
        listener->setEventHandler(handler);
        return;
    }
    listener = new(std::nothrow) EventListenerImp(boost::bind(&ECMAScriptContext::dispatchEvent, getContext(), _1, _2));
    if (listener) {
        listener->setEventHandler(handler);
        addEventListener(type, listener, false, EventTargetImp::UseEventHandler);
    }
}

HttpRequest* DocumentWindow::preload(const std::u16string& base, const std::u16string& urlString)
{
    URL url(base, urlString);
    if (url.isEmpty())
        return 0;

    for (auto i = cache.begin(); i != cache.end(); ++i) {
        HttpRequest* request = *i;
        if (request->getRequestMessage().getURL() == url)
            return request;
    }

    HttpRequest* request = new(std::nothrow) HttpRequest(base);
    if (request) {
        cache.push_back(request);
        request->open(u"GET", urlString);
        request->setHandler(boost::bind(&DocumentWindow::notify, this));
        if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self()))
            imp->incrementLoadEventDelayCount();
        request->send();
    }
    return request;
}

void DocumentWindow::notify()
{
    if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self()))
        imp->decrementLoadEventDelayCount();
}

void DocumentWindow::scroll(int x, int y)
{
    scrollX = x;
    scrollY = y;
}

void DocumentWindow::handleClick(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowImp* imp = dynamic_cast<WindowImp*>(defaultView.self());
    if (!imp)
        return;
    ViewCSSImp* view = imp->getView();
    if (!view)
        return;

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();

    switch (mouse.getButton()) {
    case 0:
        moveX = mouse.getScreenX();
        moveY = mouse.getScreenY();
        break;
    case 1:
        imp->setZoom(1.0f);
        break;
    case 3:
        imp->setZoom(imp->getZoom() - 0.1f);
        break;
    case 4:
        imp->setZoom(imp->getZoom() + 0.1f);
        break;
    case 7:
        defaultView.getHistory().back();
        break;
    case 8:
        defaultView.getHistory().forward();
        break;
    default:
        break;
    }
}

void DocumentWindow::handleMouseMove(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowImp* imp = dynamic_cast<WindowImp*>(defaultView.self());
    if (!imp)
        return;
    ViewCSSImp* view = imp->getView();
    if (!view)
        return;
    bool canScroll = view->canScroll();

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();

    if ((buttons & 1) && canScroll)
        defaultView.scrollBy(moveX - mouse.getScreenX(), moveY - mouse.getScreenY());

    moveX = mouse.getScreenX();
    moveY = mouse.getScreenY();
}

}}}}  // org::w3c::dom::bootstrap

