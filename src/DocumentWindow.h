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

#ifndef DOCUMENT_WINDOW_H
#define DOCUMENT_WINDOW_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <boost/intrusive_ptr.hpp>

#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/html/MediaQueryList.h>

#include "EventListenerImp.h"
#include "EventTargetImp.h"
#include "ECMAScript.h"
#include "Task.h"
#include "css/CSSStyleDeclarationImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HttpRequest;
class MediaQueryListImp;
class WindowImp;

// DocumentWindow implements the Window object
class DocumentWindow : public EventTargetImp
{
    Document document;
    TaskQueue taskQueue;
    ECMAScriptContext* global;
    std::list<HttpRequest*> cache;
    int scrollX;
    int scrollY;
    int moveX;
    int moveY;
    Retained<EventListenerImp> clickListener;
    Retained<EventListenerImp> mouseMoveListener;

    // computed style memory manager
    std::map<Element, CSSStyleDeclarationPtr> map;

    std::list<MediaQueryListImp*> mediaQueryLists;

    DocumentWindow(const DocumentWindow& window) = delete;
    DocumentWindow& operator=(const DocumentWindow&) = delete;

    void handleClick(EventListenerImp* listener, events::Event event);
    void handleMouseMove(EventListenerImp* listener, events::Event event);
    void notify();

public:
    DocumentWindow();
    ~DocumentWindow();

    WindowImp* getWindowImp() const;

    Document getDocument() const {
        return document;
    }
    void setDocument(const Document& document);

    TaskQueue& getTaskQueue() {
        return taskQueue;
    }

    ECMAScriptContext* getContext() {
        return global;
    }

    void enter(WindowImp* proxy);
    void exit(WindowImp* proxy);

    void putTask(const Task& task) {
        taskQueue.push(task);
    }
    void eventLoop() {
        Task task;
        while (taskQueue.tryPop(task))
            task.run();
    }

    void setEventHandler(const std::u16string& type, Object handler);

    HttpRequest* preload(const std::u16string& base, const std::u16string& url);

    CSSStyleDeclarationPtr getComputedStyle(Element elt);
    void putComputedStyle(Element elt);

    void evaluateMedia();
    void removeMedia(MediaQueryListImp* mediaQueryList);

    // CSSOM View
    html::MediaQueryList matchMedia(const std::u16string& media_query_list);
    int getScrollX() const {
        return scrollX;
    }
    int getScrollY() const {
        return scrollY;
    }
    void scroll(int x, int y) {
        scrollX = x;
        scrollY = y;
    }

};

typedef boost::intrusive_ptr<DocumentWindow> DocumentWindowPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // DOCUMENT_WINDOW_H
