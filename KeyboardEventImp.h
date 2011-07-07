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

#ifndef ORG_W3C_DOM_BOOTSTRAP_KEYBOARDEVENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_KEYBOARDEVENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/events/KeyboardEvent.h>
#include "UIEventImp.h"

#include <org/w3c/dom/events/UIEvent.h>
#include <org/w3c/dom/html/Window.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class KeyboardEventImp : public ObjectMixin<KeyboardEventImp, UIEventImp>
{
    unsigned int modifiers;
    unsigned int charCode;
    unsigned int keyCode;
    unsigned int which;

public:
    KeyboardEventImp(unsigned int modifiers, unsigned int charCode, unsigned int keyCode, unsigned int which) :
        modifiers(modifiers),
        charCode(charCode),
        keyCode(keyCode),
        which(which)
    {
    }

    // KeyboardEvent
    std::u16string getChar();
    std::u16string getKey();
    unsigned int getLocation();
    bool getCtrlKey();
    bool getShiftKey();
    bool getAltKey();
    bool getMetaKey();
    bool getRepeat();
    std::u16string getLocale();
    bool getModifierState(std::u16string keyArg);
    void initKeyboardEvent(std::u16string typeArg, bool canBubbleArg, bool cancelableArg, html::Window viewArg, std::u16string charArg, std::u16string keyArg, unsigned int locationArg, std::u16string modifiersListArg, bool repeat, std::u16string localeArg);
    // KeyboardEvent-41
    unsigned int getCharCode();
    unsigned int getKeyCode();
    unsigned int getWhich();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return events::KeyboardEvent::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::KeyboardEvent::getMetaData();
    }

};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_KEYBOARDEVENTIMP_H_INCLUDED
