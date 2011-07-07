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

#ifndef EVENT_TARGET_IMP_H
#define EVENT_TARGET_IMP_H

#include <Object.h>
#include <org/w3c/dom/events/EventTarget.h>

#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventListener.h>

#include <map>
#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class EventImp;

class EventTargetImp : public ObjectMixin<EventTargetImp>
{
    struct Listener
    {
        events::EventListener listener;
        bool useCapture;

        bool operator==(const Listener& other) const
        {
            return listener == other.listener && useCapture == other.useCapture;
        }
    };
    std::map<std::u16string, std::list<Listener>> map;

    void invoke(EventImp* event);

public:
    EventTargetImp();
    EventTargetImp(EventTargetImp* org);

    // EventTarget
    virtual void addEventListener(std::u16string type, events::EventListener listener, bool capture = false);
    virtual void removeEventListener(std::u16string type, events::EventListener listener, bool capture = false);
    virtual bool dispatchEvent(events::Event event);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return events::EventTarget::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::EventTarget::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // EVENT_TARGET_IMP_H
