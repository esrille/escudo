/*
 * Copyright 2010-2013 Esrille Inc.
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

StyleSheetImp::StyleSheetImp() :
    owner(0),
    parent(0),
    disabled(false)
{
}

void StyleSheetImp::setHref(const std::u16string& location)
{
    href = location;
}

void StyleSheetImp::setOwnerNode(Node node)
{
    owner = node;
}

void StyleSheetImp::setParentStyleSheet(stylesheets::StyleSheet sheet)
{
    parent = sheet;
}

// StyleSheet
std::u16string StyleSheetImp::getType()
{
    return u"";
}

Nullable<std::u16string> StyleSheetImp::getHref()
{
    return href;
}

Node StyleSheetImp::getOwnerNode()
{
    return owner;
}

stylesheets::StyleSheet StyleSheetImp::getParentStyleSheet()
{
    return parent;
}

Nullable<std::u16string> StyleSheetImp::getTitle()
{
    // TODO: implement me!
    return u"";
}

stylesheets::MediaList StyleSheetImp::getMedia()
{
    return &mediaList;
}

void StyleSheetImp::setMedia(const std::u16string& media)
{
    mediaList.setMediaText(media);
}

bool StyleSheetImp::getDisabled()
{
    return disabled;
}

void StyleSheetImp::setDisabled(bool disabled)
{
    this->disabled = disabled;
}

}}}}  // org::w3c::dom::bootstrap
