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

}}}}  // org::w3c::dom::bootstrap
