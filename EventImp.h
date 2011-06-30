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

#include <Object.h>
#include <org/w3c/dom/events/Event.h>

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
    std::u16string getType();
    events::EventTarget getTarget();
    events::EventTarget getCurrentTarget();
    static const unsigned short CAPTURING_PHASE = 1;
    static const unsigned short AT_TARGET = 2;
    static const unsigned short BUBBLING_PHASE = 3;
    unsigned short getEventPhase();
    void stopPropagation();
    void stopImmediatePropagation();
    bool getBubbles();
    bool getCancelable();
    void preventDefault();
    bool getDefaultPrevented();
    bool getIsTrusted();
    DOMTimeStamp getTimeStamp();
    void initEvent(std::u16string type, bool bubbles, bool cancelable);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return events::Event::dispatch(this, selector, id, argc, argv);
    }

    EventImp();
};

}}}}  // org::w3c::dom::bootstrap

#endif  // EVENT_IMP_H
