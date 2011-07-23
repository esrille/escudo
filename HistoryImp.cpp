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

#include "HistoryImp.h"

#include <org/w3c/dom/html/Location.h>

#include <iostream>
#include "utf.h"

#include "WindowImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HistoryImp::update(const DocumentWindowPtr& window)
{
    if (!sessionHistory.empty())
        sessionHistory.erase(sessionHistory.begin() + currentSession + 1, sessionHistory.end());
    SessionHistoryEntry e(window);
    sessionHistory.push_back(e);
    currentSession = getLength() - 1;

    for (auto i = sessionHistory.begin(); i != sessionHistory.end(); ++i)
        std::cerr << i->window->getDocument().getURL() << ' ';
    std::cerr << '\n';
}

int HistoryImp::getLength()
{
    return static_cast<int>(sessionHistory.size());
}

Any HistoryImp::getState()
{
    // TODO: implement me!
    return 0;
}

void HistoryImp::go()
{
    if (sessionHistory.empty())
        return;
    SessionHistoryEntry& e = sessionHistory[currentSession];
    e.window->getDocument().getLocation().reload();
}

void HistoryImp::go(int delta)
{
    if (sessionHistory.empty())
        return;
    if (delta == 0) {
        go();
        return;
    }
    int nextSession = currentSession + delta;
    if (getLength() <= nextSession)
        nextSession = getLength() - 1;
    else if (nextSession < 0)
        nextSession = 0;
    if (nextSession != currentSession) {
        currentSession = nextSession;
        SessionHistoryEntry& e = sessionHistory[currentSession];
        window->setDocumentWindow(e.window);
    }
}

void HistoryImp::back()
{
    go(-1);
}

void HistoryImp::forward()
{
    go(1);
}

void HistoryImp::pushState(Any data, std::u16string title)
{
    // TODO: implement me!
}

void HistoryImp::pushState(Any data, std::u16string title, std::u16string url)
{
    // TODO: implement me!
}

void HistoryImp::replaceState(Any data, std::u16string title)
{
    // TODO: implement me!
}

void HistoryImp::replaceState(Any data, std::u16string title, std::u16string url)
{
    // TODO: implement me!
}

}
}
}
}
