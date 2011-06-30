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

const char16_t* const mediaTypes[] = {
    u"all",
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

int getMediaTypeNumber(std::u16string& media)
{
    assert(mediaTypesCount <= 32);
    int n = 0;
    toLower(media);
    for (auto i = mediaTypes; i < mediaTypes + mediaTypesCount; ++i, ++n) {
        if (media.compare(*i) == 0)
            return n;
    }
    return -1;
}

}

// MediaList
std::u16string MediaListImp::getMediaText()
{
    std::u16string text;
    bool first = true;

    for (size_t i = 0; i < mediaTypesCount; ++i) {
        if (types.test(i)) {
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
    return types.count();
}

std::u16string MediaListImp::item(unsigned int index)
{
    if (getLength() <= index)
        return u"";
    for (size_t i = 0; i < mediaTypesCount; ++i) {
        if (types.test(i)) {
            if (index == 0)
                return mediaTypes[i];
            --index;
        }
    }
    return u"";
}

void MediaListImp::appendMedium(std::u16string medium)
{
    int n = getMediaTypeNumber(medium);
    if (0 <= n)
        types.set(n, true);
}

void MediaListImp::deleteMedium(std::u16string medium)
{
    int n = getMediaTypeNumber(medium);
    if (0 <= n)
        types.set(n, false);
}

MediaListImp::MediaListImp()
{
}

}}}}  // org::w3c::dom::bootstrap
