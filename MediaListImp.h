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

#ifndef MEDIALIST_IMP_H
#define MEDIALIST_IMP_H

#include <Object.h>
#include <org/w3c/dom/stylesheets/MediaList.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class MediaListImp : public ObjectMixin<MediaListImp>
{
public:
    enum {
        All = 0x1ff,

        Braille = 0x1,
        Embossed = 0x2,
        Handheld = 0x4,
        Print = 0x8,
        Projection = 0x10,
        Screen = 0x20,
        Speech = 0x40,
        Tty = 0x80,
        Tv = 0x100,
    };

private:
    unsigned types;

public:
    MediaListImp(unsigned types = 0) :
        types(types)
    {}
    MediaListImp(const MediaListImp& other) :
        types(other.types)
    {}

    MediaListImp& operator=(const MediaListImp& other) {
        types = other.types;
        return *this;
    }
    
    void clear() {
        types = 0;
    }
    bool hasMedium(unsigned bit) {
        return types & bit;
    }

    // MediaList
    std::u16string getMediaText();
    void setMediaText(std::u16string mediaText);
    unsigned int getLength();
    std::u16string item(unsigned int index);
    void appendMedium(std::u16string medium);
    void deleteMedium(std::u16string medium);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return stylesheets::MediaList::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return stylesheets::MediaList::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // MEDIALIST_IMP_H
