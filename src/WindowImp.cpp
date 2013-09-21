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
    global(0),
    scrollX(0),
    scrollY(0),
    clickListener(boost::bind(&WindowImp::handleClick, this, _1, _2)),
    mouseMoveListener(boost::bind(&WindowImp::handleMouseMove, this, _1, _2)),
    mediaCheck(false)
{
    addEventListener(u"click", clickListener, false, EventTargetImp::UseDefault);
    addEventListener(u"mousemove", mouseMoveListener, false, EventTargetImp::UseDefault);
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

WindowProxyPtr WindowImp::getWindowProxy() const
{
    if (document)
        return document->getDefaultWindow();
    return nullptr;
}

void WindowImp::setDocument(const DocumentPtr& document)
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
    EventListenerPtr listener = getEventHandlerListener(type);
    if (listener) {
        listener->setEventHandler(handler);
        return;
    }
    listener = std::make_shared<EventListenerImp>(boost::bind(&ECMAScriptContext::dispatchEvent, getContext(), _1, _2));
    // TODO: catch exception?
    listener->setEventHandler(handler);
    addEventListener(type, listener, false, EventTargetImp::UseEventHandler);
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
        if (document)
            document->incrementLoadEventDelayCount();
        request->send();
    }
    return request;
}

void WindowImp::notify()
{
    if (document)
        document->decrementLoadEventDelayCount();
}

void WindowImp::handleClick(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    auto defaultView = document->getDefaultWindow();
    if (!defaultView)
        return;
    ViewCSSImp* view = defaultView->getView();
    if (!view)
        return;

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);

    switch (mouse.getButton()) {
    case 0:
        moveX = mouse.getScreenX();
        moveY = mouse.getScreenY();
        break;
    case 1:
        defaultView->setZoom(1.0f);
        break;
    case 3:
        defaultView->setZoom(defaultView->getZoom() - 0.1f);
        break;
    case 4:
        defaultView->setZoom(defaultView->getZoom() + 0.1f);
        break;
    case 7:
        defaultView->getHistory().back();
        break;
    case 8:
        defaultView->getHistory().forward();
        break;
    default:
        break;
    }
}

void WindowImp::handleMouseMove(EventListenerImp* listener, events::Event event)
{
    if (event.getDefaultPrevented())
        return;
    auto defaultView = document->getDefaultWindow();
    if (!defaultView)
        return;
    ViewCSSImp* view = defaultView->getView();
    if (!view)
        return;

    bool canScroll = view->canScroll();

    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    unsigned short buttons = mouse.getButtons();

    if ((buttons & 1) && canScroll)
        defaultView->scrollBy(moveX - mouse.getScreenX(), moveY - mouse.getScreenY());

    moveX = mouse.getScreenX();
    moveY = mouse.getScreenY();
}

CSSStyleDeclarationPtr WindowImp::getComputedStyle(Element element)
{
    auto found = map.find(element);
    if (found != map.end()) {
        auto style = found->second;
        assert(style);
        style->reset();
        style->setFlags(CSSStyleDeclarationImp::ComputedStyle);
        return style;
    }
    CSSStyleDeclarationPtr style = std::make_shared<CSSStyleDeclarationImp>() ;
    if (style) {
        style->setOwner(document);
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
    html::MediaQueryList mediaQueryList(std::make_shared<MediaQueryListImp>(std::static_pointer_cast<WindowImp>(self()), media_query_list));
    if (mediaQueryList)
        mediaQueryLists.push_back(mediaQueryList);
    return mediaQueryList;
}

bool WindowImp::evaluateMedia()
{
    for (auto i = mediaQueryLists.begin(); i != mediaQueryLists.end(); ++i) {
        auto mql = std::dynamic_pointer_cast<MediaQueryListImp>(i->self());
        assert(mql);
        mql->evaluate();
    }

    bool result = false;
    for (auto i = viewMediaQueryLists.begin(); i != viewMediaQueryLists.end(); ++i) {
        auto mql = std::dynamic_pointer_cast<MediaQueryListImp>(i->self());
        assert(mql);
        result |= mql->evaluate();
    }
    if (result)
        setMediaCheck(true);
    return result;
}

void WindowImp::removeMedia(html::MediaQueryList mediaQueryList)
{
    mediaQueryLists.remove(mediaQueryList);
}

void WindowImp::flushMediaQueryLists(ViewCSSImp* view)
{
    viewMediaQueryLists.clear();
    view->flushMediaQueryLists(viewMediaQueryLists);
}


}}}}  // org::w3c::dom::bootstrap
