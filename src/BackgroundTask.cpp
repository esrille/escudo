/*
 * Copyright 2012, 2013 Esrille Inc.
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
#include "css/ViewCSSImp.h"
#include "html/HTMLParser.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

WindowImp::BackgroundTask::BackgroundTask(WindowImp* window) :
    window(window),
    state(Init),
    flags(0),
    view(0),
    xfered(false)
{
}

WindowImp::BackgroundTask::~BackgroundTask()
{
    deleteView();
}

void WindowImp::BackgroundTask::deleteView()
{
    if (!xfered)
        delete view;
    view = 0;
    xfered = false;
}

void WindowImp::BackgroundTask::operator()()
{
    if (!window)
        return;

    unsigned command;
    while (!((command = sleep()) & Abort)) {
        if (!window->getDocumentWindow()) {
            state = Init;
            continue;
        }

        //
        // Restart
        //
        if (command & Restart) {
            command &= ~Restart;
            deleteView();
            state = Init;
        }

        // A binding document does not need a view.
        if (window->isBindingDocumentWindow()) {
            command &= ~(Cascade | Layout);
            state = Done;
            continue;
        }

        //
        // Cascade
        //
        if (!view || (command & Cascade)) {
            command &= ~Cascade;
            state = Cascading;
            recordTime("%*sselector matching begin", window->windowDepth * 2, "");
            if (!view)
                view = new(std::nothrow) ViewCSSImp(window->getDocumentWindow());
            if (view) {
                view->constructComputedStyles();
                state = Cascaded;
            } else
                state = Init;
            recordTime("%*sselector matching end", window->windowDepth * 2, "");
            continue;
        }

        //
        // Layout
        //
        if (command & Layout) {
            command &= ~Layout;
            state = Layouting;
            view->setSize(window->width, window->height);   // TODO: sync with mainloop
            recordTime("%*sstyle recalculation begin", window->windowDepth * 2, "");
            view->calculateComputedStyles();
            recordTime("%*sstyle recalculation end", window->windowDepth * 2, "");
            recordTime("%*sreflow begin", window->windowDepth * 2, "");
            view->layOut();
            recordTime("%*sreflow end", window->windowDepth * 2, "");
            view->setFlags(Box::NEED_REPAINT);
        }

        state = Done;
    }
}


unsigned WindowImp::BackgroundTask::sleep()
{
    std::unique_lock<std::mutex> lock(mutex);
    while (!flags)
        cond.wait(lock);
    unsigned result = flags;
    flags = 0;
    return result;
}

void WindowImp::BackgroundTask::wakeUp(unsigned flags)
{
    std::lock_guard<std::mutex> lock(mutex);
    xfered = false;
    this->flags |= flags;
    cond.notify_one();
}

void WindowImp::BackgroundTask::abort()
{
    // TODO: Cancel ongoing tasks.
    wakeUp(Abort);
}

void WindowImp::BackgroundTask::restart(unsigned flags)
{
    // TODO: Cancel ongoing tasks.
    std::lock_guard<std::mutex> lock(mutex);
    this->flags &= ~Abort;
    this->flags |= Restart | flags;
    cond.notify_one();
}

ViewCSSImp* WindowImp::BackgroundTask::getView()
{
    assert(state == Done || state == Init);
    if (!xfered && view) {
        xfered = true;
        return view;
    }
    return 0;
}

}}}}  // org::w3c::dom::bootstrap
