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

#include "WindowImp.h"
#include "js/esjsapi.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

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
        request->send();
    }
}

}}}}  // org::w3c::dom::bootstrap

