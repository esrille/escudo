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

#ifndef ORG_W3C_DOM_BOOTSTRAP_MEDIAQUERYLISTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_MEDIAQUERYLISTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/MediaQueryList.h>

#include <org/w3c/dom/html/MediaQueryListListener.h>

#include <list>

#include "DocumentWindow.h"
#include "MediaListImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class WindowImp;

class MediaQueryListImp : public ObjectMixin<MediaQueryListImp>
{
    enum {
        Unknown,
        Match,
        NotMatch
    };
    int state;
    DocumentWindowPtr window;
    stylesheets::MediaList mediaList {0};
    std::list<html::MediaQueryListListener> listeners;

public:
    MediaQueryListImp(DocumentWindowPtr window, std::u16string query);
    ~MediaQueryListImp();

    bool evaluate();

    // MediaQueryList
    std::u16string getMedia();
    bool getMatches();
    void addListener(html::MediaQueryListListener listener);
    void removeListener(html::MediaQueryListListener listener);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::MediaQueryList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::MediaQueryList::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_MEDIAQUERYLISTIMP_H_INCLUDED
