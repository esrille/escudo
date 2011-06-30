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

#include "MouseEventImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// MouseEvent

void MouseEventImp::initMouseEvent(std::u16string typeArg, bool canBubbleArg, bool cancelableArg, html::Window viewArg, int detailArg,
                                   int screenXArg, int screenYArg, int clientXArg, int clientYArg, bool ctrlKeyArg, bool altKeyArg, bool shiftKeyArg, bool metaKeyArg, unsigned short buttonArg, events::EventTarget relatedTargetArg)
{
    screenX = screenXArg;
    screenY = screenYArg;
    clientX = clientXArg;
    clientY = clientYArg;
    ctrlKey = ctrlKeyArg;
    altKey = altKeyArg;
    shiftKey = shiftKeyArg;
    metaKey = metaKeyArg;
    button = buttonArg;
    relatedTarget = relatedTargetArg;
    initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, detailArg);
}

bool MouseEventImp::getModifierState(std::u16string keyArg)
{
}


}}}}  // org::w3c::dom::bootstrap
