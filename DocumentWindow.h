/*
 * Copyright 2011, 2012 Esrille Inc.
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

#ifndef DOCUMENT_WINDOW_H
#define DOCUMENT_WINDOW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Document.h>

#include <boost/intrusive_ptr.hpp>

#include "EventListenerImp.h"
#include "EventTargetImp.h"
#include "ECMAScript.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class WindowImp;
class HttpRequest;

// DocumentWindow implements the Window object
class DocumentWindow : public EventTargetImp
{
    Document document;
    ECMAScriptContext* global;
    std::list<HttpRequest*> cache;
    int scrollX;
    int scrollY;
    int moveX;
    int moveY;
    Retained<EventListenerImp> clickListener;
    Retained<EventListenerImp> mouseMoveListener;

    DocumentWindow(const DocumentWindow& window) = delete;
    DocumentWindow& operator=(const DocumentWindow&) = delete;

    void handleClick(events::Event event);
    void handleMouseMove(events::Event event);
    void notify();

public:
    DocumentWindow();
    ~DocumentWindow();

    Document getDocument() const {
        return document;
    }
    void setDocument(const Document& document) {
        this->document = document;
        if (global)
            delete global;
        global = new(std::nothrow) ECMAScriptContext;
    }

    ECMAScriptContext* getContext() {
        return global;
    }

    void activate();
    void activate(WindowImp* proxy);

    HttpRequest* preload(const std::u16string& base, const std::u16string& url);

    // CSSOM View
    int getScrollX() const {
        return scrollX;
    }
    int getScrollY() const {
        return scrollY;
    }
    void scroll(int x, int y);
};

typedef boost::intrusive_ptr<DocumentWindow> DocumentWindowPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // DOCUMENT_WINDOW_H
