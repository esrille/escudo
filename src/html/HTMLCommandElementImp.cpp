/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "HTMLCommandElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Node
Node HTMLCommandElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLCommandElementImp(this, deep);
}

// HTMLCommandElement
std::u16string HTMLCommandElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLCommandElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

Nullable<std::u16string> HTMLCommandElementImp::getLabel()
{
    // TODO: implement me!
    return u"";
}

void HTMLCommandElementImp::setLabel(Nullable<std::u16string> label)
{
    // TODO: implement me!
}

Nullable<std::u16string> HTMLCommandElementImp::getIcon()
{
    // TODO: implement me!
    return u"";
}

void HTMLCommandElementImp::setIcon(Nullable<std::u16string> icon)
{
    // TODO: implement me!
}

bool HTMLCommandElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLCommandElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

bool HTMLCommandElementImp::getChecked()
{
    // TODO: implement me!
    return 0;
}

void HTMLCommandElementImp::setChecked(bool checked)
{
    // TODO: implement me!
}

std::u16string HTMLCommandElementImp::getRadiogroup()
{
    // TODO: implement me!
    return u"";
}

void HTMLCommandElementImp::setRadiogroup(std::u16string radiogroup)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
