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

#include "WindowImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "DocumentImp.h"
#include "ECMAScript.h"
#include "WindowProxy.h"
#include "css/ViewCSSImp.h"
#include "html/MediaQueryListImp.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

WindowImp::WindowImp() :
    document(0),
    global(0),
    scrollX(0),
    scrollY(0),
    clickListener(boost::bind(&WindowImp::handleClick, this, _1, _2)),
    mouseMoveListener(boost::bind(&WindowImp::handleMouseMove, this, _1, _2)),
    mediaCheck(false)
{
    addEventListener(u"click", &clickListener, false, EventTargetImp::UseDefault);
    addEventListener(u"mousemove", &mouseMoveListener, false, EventTargetImp::UseDefault);
}

WindowImp::~WindowImp()
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

WindowProxy* WindowImp::getWindowImp() const
{
    if (auto imp = dynamic_cast<DocumentImp*>(document.self()))
        return imp->getDefaultWindow();
    return 0;
}

void WindowImp::setDocument(const Document& document)
{
    this->document = document;
    if (global)
        delete global;
    global = new(std::nothrow) ECMAScriptContext;
    map.clear();
}

void WindowImp::enter(WindowProxy* proxy)
{
    if (global && isMainThread())
        global->enter(proxy);
}

void WindowImp::exit(WindowProxy* proxy)
{
    if (global && isMainThread())
        global->exit(proxy);
}

void WindowImp::setEventHandler(const std::u16string& type, Object handler)
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

HttpRequest* WindowImp::preload(const std::u16string& base, const std::u16string& urlString)
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
        request->setHandler(boost::bind(&WindowImp::notify, this));
        if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self()))
            imp->incrementLoadEventDelayCount();
        request->send();
    }
    return request;
}

void WindowImp::notify()
{
    if (DocumentImp* imp = dynamic_cast<DocumentImp*>(document.self()))
        imp->decrementLoadEventDelayCount();
}

void WindowImp::handleClick(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowProxy* imp = dynamic_cast<WindowProxy*>(defaultView.self());
    if (!imp)
        return;
    ViewCSSImp* view = imp->getView();
    if (!view)
        return;

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);

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

void WindowImp::handleMouseMove(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    html::Window defaultView = document.getDefaultView();
    if (!defaultView)
        return;
    WindowProxy* imp = dynamic_cast<WindowProxy*>(defaultView.self());
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

CSSStyleDeclarationPtr WindowImp::getComputedStyle(Element element)
{
    auto found = map.find(element);
    if (found != map.end()) {
        CSSStyleDeclarationImp* style = found->second.get();
        assert(style);
        style->reset();
        style->setFlags(CSSStyleDeclarationImp::ComputedStyle);
        return style;
    }
    CSSStyleDeclarationPtr style = new(std::nothrow) CSSStyleDeclarationImp;
    if (style) {
        style->setOwner(document.self());
        style->setFlags(CSSStyleDeclarationImp::ComputedStyle);
        map.insert({ element, style });
    }
    return style;
}

void WindowImp::putComputedStyle(Element element)
{
    map.erase(element);
}

html::MediaQueryList WindowImp::matchMedia(const std::u16string& media_query_list)
{
    MediaQueryListImp* mediaQueryList = new(std::nothrow) MediaQueryListImp(this, media_query_list);
    if (mediaQueryList)
        mediaQueryLists.push_back(mediaQueryList);
    return mediaQueryList;
}

bool WindowImp::evaluateMedia()
{
    for (auto i = mediaQueryLists.begin(); i != mediaQueryLists.end(); ++i)
        (*i)->evaluate();

    bool result = false;
    for (auto i = viewMediaQueryLists.begin(); i != viewMediaQueryLists.end(); ++i) {
        auto mql = dynamic_cast<MediaQueryListImp*>((*i).self());
        assert(mql);
        result |= mql->evaluate();
    }
    if (result)
        setMediaCheck(true);
    return result;
}

void WindowImp::removeMedia(MediaQueryListImp* mediaQueryList)
{
    mediaQueryLists.remove(mediaQueryList);
}

void WindowImp::flushMediaQueryLists(ViewCSSImp* view)
{
    viewMediaQueryLists.clear();
    view->flushMediaQueryLists(viewMediaQueryLists);
}


}}}}  // org::w3c::dom::bootstrap
