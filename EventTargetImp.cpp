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

#include "EventTargetImp.h"

#include <org/w3c/dom/events/Event.h>

#include "EventImp.h"
#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void EventTargetImp::invoke(EventImp* event)
{
    auto found = map.find(event->getType());
    if (found == map.end())
        return;
    std::list<Listener>& listeners = found->second;  // TODO listeners should be a static copy
    event->setCurrentTarget(this);
    for (auto i = listeners.begin(); i != listeners.end(); ++i) {
        if (event->getStopImmediatePropagationFlag())
            return;
        switch (event->getEventPhase()) {
        case events::Event::CAPTURING_PHASE:
            if (i->useCapture)
                i->listener.handleEvent(event);
            break;
        case events::Event::BUBBLING_PHASE:
            if (!i->useCapture)
                i->listener.handleEvent(event);
            break;
        case events::Event::AT_TARGET:
            i->listener.handleEvent(event);
            break;
        default:
            break;
        }
    }
}

// EventTarget
void EventTargetImp::addEventListener(std::u16string type, events::EventListener listener, bool useCapture)
{
    if (!listener)
        return;
    Listener item{ listener, useCapture };

    auto found = map.find(type);
    if (found == map.end()) {
        std::list<Listener> listeners;
        listeners.push_back(item);
        map.insert(std::pair<std::u16string, std::list<Listener>>(type, listeners));
        return;
    }

    std::list<Listener>& listeners = found->second;
    for (auto i = listeners.begin(); i != listeners.end(); ++i) {
        if (*i == item)
            return;
    }
    listeners.push_back(item);
}

void EventTargetImp::removeEventListener(std::u16string type, events::EventListener listener, bool useCapture)
{
    if (!listener)
        return;
    Listener item{ listener, useCapture };

    auto found = map.find(type);
    if (found == map.end())
        return;

    std::list<Listener>& listeners = found->second;
    for (auto i = listeners.begin(); i != listeners.end(); ++i) {
        if (*i == item) {
            listeners.erase(i);
            return;
        }
    }
}

bool EventTargetImp::dispatchEvent(events::Event evt)
{
    EventImp* event = dynamic_cast<EventImp*>(evt.self());
    NodeImp* node = dynamic_cast<NodeImp*>(this);
    if (!event || !node)
        return false;
    event->setDispatchFlag(true);
    event->setTarget(this);
    if (!node->parentNode)
        invoke(event);
    else {
        std::list<NodeImp*> eventPath;
        for (NodeImp* ancestor = node->parentNode; ancestor; ancestor = ancestor->parentNode)
            eventPath.push_front(ancestor);
        event->setEventPhase(events::Event::CAPTURING_PHASE);
        for (auto i = eventPath.begin(); i != eventPath.end(); ++i) {
            if (event->getStopPropagationFlag())
                break;
            (*i)->invoke(event);
        }
        event->setEventPhase(events::Event::AT_TARGET);
        if (!event->getStopPropagationFlag())
            node->invoke(event);
        if (event->getBubbles()) {
            event->setEventPhase(events::Event::BUBBLING_PHASE);
            for (auto i = eventPath.rbegin(); i != eventPath.rend(); ++i) {
                if (event->getStopPropagationFlag())
                    break;
                (*i)->invoke(event);
            }
        }
    }
    event->setDispatchFlag(false);
    event->setEventPhase(events::Event::AT_TARGET);
    event->setCurrentTarget(0);
    return !event->getDefaultPrevented();
}

EventTargetImp::EventTargetImp() :
    ObjectMixin()
{
}

EventTargetImp::EventTargetImp(EventTargetImp* org) :
    ObjectMixin()
{
}

}}}}  // org::w3c::dom::bootstrap
