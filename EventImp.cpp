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

#include "EventImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

EventImp::EventImp() :
    ObjectMixin(),
    target(0),
    currentTarget(0),
    phase(0),
    stopPropagationFlag(false),
    stopImmediatePropagationFlag(false),
    bubbleFlag(false),
    cancelFlag(false),
    canceledFlag(false),
    trustedFlag(false),
    initializedFlag(false),
    dispatchFlag(false)
{
}

EventImp::EventImp(std::u16string type) :
    ObjectMixin(),
    target(0),
    currentTarget(0),
    dispatchFlag(false)
{
    initEvent(type, true, false);
}

EventImp::EventImp(std::u16string type, events::EventInit eventInitDict) :
    ObjectMixin(),
    target(0),
    currentTarget(0),
    dispatchFlag(false)
{
    initEvent(type, eventInitDict.getBubbles(), eventInitDict.getCancelable());
}

std::u16string EventImp::getType()
{
    return type;
}

events::EventTarget EventImp::getTarget()
{
    return target;
}

events::EventTarget EventImp::getCurrentTarget()
{
    return currentTarget;
}

unsigned short EventImp::getEventPhase()
{
    return phase;
}

void EventImp::stopPropagation()
{
    stopPropagationFlag = true;
}

void EventImp::stopImmediatePropagation()
{
    stopPropagationFlag = true;
    stopImmediatePropagationFlag = true;
}

bool EventImp::getBubbles()
{
    return bubbleFlag;
}

bool EventImp::getCancelable()
{
    return cancelFlag;
}

void EventImp::preventDefault()
{
    if (cancelFlag)
        canceledFlag = true;
}

bool EventImp::getDefaultPrevented()
{
    return canceledFlag;
}

bool EventImp::getIsTrusted()
{
    return trustedFlag;
}

DOMTimeStamp EventImp::getTimeStamp()
{
    // TODO: implement me!
    return 0;
}

void EventImp::initEvent(std::u16string type, bool bubbles, bool cancelable)
{
    initializedFlag = true;
    if (dispatchFlag)
        return;

    stopPropagationFlag = false;
    stopImmediatePropagationFlag = false;
    canceledFlag = false;
    trustedFlag = false;
    target = 0;
    this->type = type;
    if (bubbles)
        bubbleFlag = true;
    if (cancelable)
        cancelFlag = true;
}

}  // org::w3c::dom::bootstrap

namespace events {

namespace {

class Constructor : public Object
{
public:
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        bootstrap::EventImp* evt = 0;
        switch (argc) {
        case 1:
            evt = new(std::nothrow) bootstrap::EventImp(argv[0].toString());
            break;
        case 2:
            evt = new(std::nothrow) bootstrap::EventImp(argv[0].toString(), argv[1].toObject());
            break;
        default:
            break;
        }
        return evt;
    }
    Constructor() :
        Object(this) {
    }
};

}  // namespace

Object Event::getConstructor()
{
    static Constructor constructor;
    return constructor.self();
}

}

}}}  // org::w3c::dom::bootstrap
