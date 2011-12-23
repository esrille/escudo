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

#include "MediaListImp.h"
#include "css/CSSSerialize.h"

#include <assert.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace stylesheets;

namespace {

const char16_t* const allType = u"all";

const char16_t* const mediaTypes[] = {
    u"braille",
    u"embossed",
    u"handheld",
    u"print",
    u"projection",
    u"screen",
    u"speech",
    u"tty",
    u"tv"
};

const size_t mediaTypesCount = sizeof mediaTypes / sizeof mediaTypes[0];

unsigned getMediaTypeBits(std::u16string& media)
{
    assert(mediaTypesCount <= 32);
    toLower(media);
    if (media.compare(allType) == 0)
        return MediaListImp::All;
    for (int i = 0; i < mediaTypesCount; ++i) {
        if (media.compare(mediaTypes[i]) == 0)
            return 1u << i;
    }
    return 0;
}

}

// MediaList
std::u16string MediaListImp::getMediaText()
{
    if (types == All)
        return allType;
    std::u16string text;
    bool first = true;
    for (int i = 0; i < mediaTypesCount; ++i) {
        if (types & (1u << i)) {
            if (first)
                first = false;
            else
                text += u", ";
            text += mediaTypes[i];
        }
    }
    return text;
}

void MediaListImp::setMediaText(std::u16string mediaText)
{
    // TODO: implement me.
}

unsigned int MediaListImp::getLength()
{
    return (types == All) ? 1 : __builtin_popcount(types);
}

std::u16string MediaListImp::item(unsigned int index)
{
    if (getLength() <= index)
        return u"";
    if (types == All)
        return allType;
    for (size_t i = 0; i < mediaTypesCount; ++i) {
        if (types & (1u << i)) {
            if (index == 0)
                return mediaTypes[i];
            --index;
        }
    }
    return u"";
}

void MediaListImp::appendMedium(std::u16string medium)
{
    types |= getMediaTypeBits(medium);
}

void MediaListImp::deleteMedium(std::u16string medium)
{
    types &= ~getMediaTypeBits(medium);
}

}}}}  // org::w3c::dom::bootstrap
