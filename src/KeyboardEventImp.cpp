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

#include "KeyboardEventImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

std::u16string KeyboardEventImp::getChar()
{
    // TODO: implement me!
    return u"";
}

std::u16string KeyboardEventImp::getKey()
{
    // TODO: implement me!
    return u"";
}

unsigned int KeyboardEventImp::getLocation()
{
    // TODO: implement me!
    return 0;
}

bool KeyboardEventImp::getCtrlKey()
{
    return modifiers & 2;
}

bool KeyboardEventImp::getShiftKey()
{
    return modifiers & 1;
}

bool KeyboardEventImp::getAltKey()
{
    return modifiers & 4;
}

bool KeyboardEventImp::getMetaKey()
{
    return modifiers & 8;
}

bool KeyboardEventImp::getRepeat()
{
    // TODO: implement me!
    return 0;
}

std::u16string KeyboardEventImp::getLocale()
{
    // TODO: implement me!
    return u"";
}

bool KeyboardEventImp::getModifierState(std::u16string keyArg)
{
    // TODO: implement me!
    return 0;
}

void KeyboardEventImp::initKeyboardEvent(std::u16string typeArg, bool canBubbleArg, bool cancelableArg, html::Window viewArg, std::u16string charArg, std::u16string keyArg, unsigned int locationArg, std::u16string modifiersListArg, bool repeat, std::u16string localeArg)
{
    // TODO: implement me!
    initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, 0);
}

unsigned int KeyboardEventImp::getCharCode()
{
    return charCode;
}

unsigned int KeyboardEventImp::getKeyCode()
{
    return keyCode;
}

unsigned int KeyboardEventImp::getWhich()
{
    return which;
}

}
}
}
}
