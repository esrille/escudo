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

#ifndef ORG_W3C_DOM_BOOTSTRAP_EVENTLISTENERIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_EVENTLISTENERIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/events/EventListener.h>

#include <org/w3c/dom/events/Event.h>

#include <boost/function.hpp>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class EventListenerImp : public ObjectMixin<EventListenerImp>
{
    boost::function<void (events::Event)> handler;
public:
    EventListenerImp(boost::function<void (events::Event)> handler) :
        handler(handler)
    {
    }
    // EventListener
    void handleEvent(events::Event event) {
        if (handler)
            handler(event);
    }
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return events::EventListener::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::EventListener::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_EVENTLISTENERIMP_H_INCLUDED
