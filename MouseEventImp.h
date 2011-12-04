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

#ifndef MOUSE_EVENT_IMP_H
#define MOUSE_EVENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/events/MouseEvent.h>

#include <org/w3c/dom/events/EventTarget.h>

#include "UIEventImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class MouseEventImp : public ObjectMixin<MouseEventImp, UIEventImp>
{
    int screenX;
    int screenY;
    int clientX;
    int clientY;
    bool ctrlKey;
    bool altKey;
    bool shiftKey;
    bool metaKey;
    unsigned short button;
    events::EventTarget relatedTarget;

    unsigned short buttons;
    int pageX;
    int pageY;
    int offsetX;
    int offsetY;

public:
    MouseEventImp() :
        relatedTarget(0)
    {
    }

    void setButtons(unsigned short value)
    {
        buttons = value;
    }

    // MouseEvent
    int getScreenX()
    {
        return screenX;
    }
    int getScreenY()
    {
        return screenY;
    }
    int getClientX()
    {
        return clientX;
    }
    int getClientY()
    {
        return clientY;
    }
    bool getCtrlKey()
    {
        return ctrlKey;
    }
    bool getShiftKey()
    {
        return shiftKey;
    }
    bool getAltKey()
    {
        return altKey;
    }
    bool getMetaKey()
    {
        return metaKey;
    }
    unsigned short getButton()
    {
        return button;
    }
    unsigned short getButtons()
    {
        return buttons;
    }
    events::EventTarget getRelatedTarget()
    {
        return relatedTarget;
    }
    void initMouseEvent(std::u16string typeArg, bool canBubbleArg, bool cancelableArg, html::Window viewArg, int detailArg, int screenXArg, int screenYArg, int clientXArg, int clientYArg, bool ctrlKeyArg, bool altKeyArg, bool shiftKeyArg, bool metaKeyArg, unsigned short buttonArg, events::EventTarget relatedTargetArg);
    bool getModifierState(std::u16string keyArg);
    // MouseEvent-42
    int getPageX()
    {
        return pageX;
    }
    int getPageY()
    {
        return pageY;
    }
    int getX()
    {
        return getClientX();
    }
    int getY()
    {
        return getClientY();
    }
    int getOffsetX()
    {
        return offsetX;
    }
    int getOffsetY()
    {
        return offsetY;
    }

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return events::MouseEvent::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::MouseEvent::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // MOUSE_EVENT_IMP_H
