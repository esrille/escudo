/*
 * Copyright 2013 Esrille Inc.
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

#include "MediaQueryListImp.h"

#include <algorithm>
#include <boost/bind.hpp>

#include "Task.h"
#include "WindowImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

MediaQueryListImp::MediaQueryListImp(DocumentWindowPtr window, std::u16string query) :
    state(Unknown),
    window(window)
{
    mediaList.setMediaText(query);
}

MediaQueryListImp::~MediaQueryListImp()
{
    window->removeMedia(this);
}

bool MediaQueryListImp::evaluate()
{
    int old = state;
    state = getMatches() ? Match : NotMatch;
    if (old == state || old == Unknown)
        return state;
    for (auto i = listeners.begin(); i != listeners.end(); ++i) {
        Task task(*i, boost::bind<void>(*i, html::MediaQueryList(this)));
        window->putTask(task);
    }
    return state;
}


// MediaQueryList

std::u16string MediaQueryListImp::getMedia()
{
    return mediaList.getMediaText();
}

bool MediaQueryListImp::getMatches()
{
    return mediaList.matches(window->getWindowImp());
}

void MediaQueryListImp::addListener(html::MediaQueryListListener listener)
{
    auto found = std::find(listeners.begin(), listeners.end(), listener);
    if (found == listeners.end())
        listeners.push_back(listener);
}

void MediaQueryListImp::removeListener(html::MediaQueryListListener listener)
{
    auto found = std::find(listeners.begin(), listeners.end(), listener);
    if (found != listeners.end())
        listeners.erase(found);
}

}
}
}
}
