/*
 * Copyright 2012 Esrille Inc.
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

#ifndef ES_ICO_H
#define ES_ICO_H

#include <stdint.h>

#include <cstdio>
#include <deque>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class BoxImage;

struct IconHeader
{
    // imageType
    enum {
        ICO = 1,
        CUR = 2
    };
    uint16_t reserved;     // Must always be zero.
    uint16_t imageType;
    uint16_t planeCount;

    bool isIco() const {
        return imageType == ICO;
    }
    bool isCur() const {
        return imageType == CUR;
    }
    bool read(std::FILE* file);
};

struct IconDirectoryEntry
{
    uint8_t width;
    uint8_t height;
    uint8_t colorCount;
    uint8_t reserved;
    uint16_t planeCount;
    uint16_t bitCount;
    uint32_t imageSize;
    uint32_t imageOffset;

    bool read(std::FILE* file);
    size_t getWidth() const {
        return width;
    }
    size_t getHeight() const {
        return height;
    }
};

class IcoImage
{
    IconHeader header;
    std::deque<IconDirectoryEntry> directory;
public:
    bool open(std::FILE* file);

    size_t getPlaneCount() const {
        return header.planeCount;
    }
    bool isIco() const {
        return header.isIco();
    }
    bool isCur() const {
        return header.isCur();
    }
    const IconDirectoryEntry& getEntry(size_t n) const {
        return directory.at(n);
    }

    BoxImage* open(std::FILE* file, size_t n);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_ICO_H
