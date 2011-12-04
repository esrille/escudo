/*
 * Copyright 2011 Esrille Inc.
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
#include "WindowImp.h"
#include "css/ViewCSSImp.h"
#include "js/esjsapi.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

DocumentWindow::DocumentWindow() :
    document(0),
    global(0),
    scrollX(0),
    scrollY(0),
    clickListener(boost::bind(&DocumentWindow::handleClick, this, _1)),
    mouseMoveListener(boost::bind(&DocumentWindow::handleMouseMove, this, _1))
{
    addEventListener(u"click", &clickListener);
    addEventListener(u"mousemove", &mouseMoveListener);
}

DocumentWindow::~DocumentWindow()
{
    if (global) {
        putGlobal(static_cast<JSObject*>(global));
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
    if (proxy && global) {
        JS_SetGlobalObject(jscontext, static_cast<JSObject*>(global));
        JS_SetPrivate(jscontext, static_cast<JSObject*>(global), proxy);
        proxy->setPrivate(global);
    }
}

void DocumentWindow::preload(const std::u16string& base, const std::u16string& urlString)
{
    URL url(base, urlString);

    for (auto i = cache.begin(); i != cache.end(); ++i) {
        HttpRequest* request = *i;
        if (request->getRequestMessage().getURL() == url)
            return;
    }

    HttpRequest* request = new(std::nothrow) HttpRequest(base);
    if (request) {
        cache.push_back(request);
        request->open(u"GET", urlString);
        request->setHanndler(boost::bind(&DocumentWindow::notify, this));
        if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self()))
            imp->incrementLoadEventDelayCount();
        request->send();
    }
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

void DocumentWindow::handleClick(events::Event event)
{
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowImp* imp = dynamic_cast<WindowImp*>(defaultView.self());
    if (!imp)
        return;
    ViewCSSImp* view = imp->getView();

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();

    switch (mouse.getButton()) {
    case 0:
        moveX = mouse.getScreenX();
        moveY = mouse.getScreenY();
        break;
    case 3:
        view->setZoom(view->getZoom() - 0.1f);
        break;
    case 4:
        view->setZoom(view->getZoom() + 0.1f);
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

void DocumentWindow::handleMouseMove(events::Event event)
{
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowImp* imp = dynamic_cast<WindowImp*>(defaultView.self());
    if (!imp)
        return;
    ViewCSSImp* view = imp->getView();
    bool canScroll = view->canScroll();

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();

    if ((buttons & 1) && canScroll)
        defaultView.scrollBy(moveX - mouse.getScreenX(), moveY - mouse.getScreenY());

    moveX = mouse.getScreenX();
    moveY = mouse.getScreenY();
}

}}}}  // org::w3c::dom::bootstrap

