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

#include "StyleSheetImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// StyleSheet
std::u16string StyleSheetImp::getType()
{
    return std::u16string();
}

std::u16string StyleSheetImp::getHref()
{
    // TODO: implement me!
    return u"";
}

Node StyleSheetImp::getOwnerNode()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

stylesheets::StyleSheet StyleSheetImp::getParentStyleSheet()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string StyleSheetImp::getTitle()
{
    // TODO: implement me!
    return u"";
}

stylesheets::MediaList StyleSheetImp::getMedia()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void StyleSheetImp::setMedia(std::u16string media)
{
    // TODO: implement me!
}

bool StyleSheetImp::getDisabled()
{
    return disabled;
}

void StyleSheetImp::setDisabled(bool disabled)
{
    this->disabled = disabled;
}

StyleSheetImp::StyleSheetImp() :
    disabled(false)
{
}

}}}}  // org::w3c::dom::bootstrap
