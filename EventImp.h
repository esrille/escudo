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

#ifndef EVENT_IMP_H
#define EVENT_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/events/Event.h>

#include <org/w3c/dom/events/EventInit.h>
#include <org/w3c/dom/events/EventTarget.h>

#include <map>
#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class EventImp : public ObjectMixin<EventImp>
{
    std::u16string type;
    events::EventTarget target;
    events::EventTarget currentTarget;
    unsigned short phase;

    bool stopPropagationFlag;
    bool stopImmediatePropagationFlag;
    bool bubbleFlag;
    bool cancelFlag;
    bool canceledFlag;
    bool trustedFlag;
    bool initializedFlag;
    bool dispatchFlag;

public:
    EventImp();
    EventImp(std::u16string type);
    EventImp(std::u16string type, events::EventInit eventInitDict);

    void setEventPhase(unsigned short phase) {
        this->phase = phase;
    }
    bool getStopPropagationFlag() const {
        return stopPropagationFlag;
    }
    bool getStopImmediatePropagationFlag() const {
        return stopImmediatePropagationFlag;
    }

    bool getDispatchFlag() const {
        return dispatchFlag;
    }
    void setDispatchFlag(bool flag) {
        dispatchFlag = flag;
    }
    void setTarget(events::EventTarget target) {
        this->target = target;
    }
    void setCurrentTarget(events::EventTarget target) {
        this->currentTarget = target;
    }

    // Event
    std::u16string getType() __attribute__((weak));
    events::EventTarget getTarget() __attribute__((weak));
    events::EventTarget getCurrentTarget() __attribute__((weak));
    unsigned short getEventPhase() __attribute__((weak));
    void stopPropagation() __attribute__((weak));
    void stopImmediatePropagation() __attribute__((weak));
    bool getBubbles() __attribute__((weak));
    bool getCancelable() __attribute__((weak));
    void preventDefault() __attribute__((weak));
    bool getDefaultPrevented() __attribute__((weak));
    bool getIsTrusted() __attribute__((weak));
    DOMTimeStamp getTimeStamp() __attribute__((weak));
    void initEvent(std::u16string type, bool bubbles, bool cancelable) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return events::Event::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::Event::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // EVENT_IMP_H
