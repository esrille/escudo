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

#ifndef ORG_W3C_DOM_BOOTSTRAP_EVENTTARGETIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_EVENTTARGETIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/events/EventTarget.h>

#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventListener.h>

#include <map>
#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class EventImp;
class EventListenerImp;

typedef std::shared_ptr<EventImp> EventPtr;
typedef std::shared_ptr<EventListenerImp> EventListenerPtr;

class EventTargetImp : public ObjectMixin<EventTargetImp>
{
public:
    enum {
        UseDefault = 0x01,
        UseCapture = 0x02,
        UseEventHandler = 0x04
    };

private:
    struct Listener
    {
        events::EventListener listener;
        unsigned flags;

        bool operator==(const Listener& other) const
        {
            return listener == other.listener && flags == other.flags;
        }
        bool operator!=(const Listener& other) const
        {
            return listener != other.listener || flags != other.flags;
        }
        bool useDefault() const {
            return flags & UseDefault;
        }
        bool useCapture() const {
            return flags & UseCapture;
        }
        bool useEventHandler() const {
            return flags & UseEventHandler;
        }
    };

    std::map<std::u16string, std::list<Listener>> map;

public:
    EventTargetImp();
    EventTargetImp(const EventTargetImp& other);

    virtual void invoke(const EventPtr& event);

    EventListenerPtr getEventHandlerListener(const std::u16string& type);

    Object getEventHandler(const std::u16string& type);
    virtual void setEventHandler(const std::u16string& type, Object handler) {}

    // EventTarget
    virtual void addEventListener(const std::u16string& type, events::EventListener listener, bool capture = false, unsigned flags = 0);
    virtual void removeEventListener(const std::u16string& type, events::EventListener listener, bool capture = false, unsigned flags = 0);
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

typedef std::shared_ptr<EventTargetImp> EventTargetPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_EVENTTARGETIMP_H_INCLUDED
