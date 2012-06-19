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

#include "Ico.h"

#include <assert.h>

#include "Box.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

bool IconHeader::read(std::FILE* file)
{
    uint8_t header[6];

    if (std::fread(header, 1, sizeof header, file) != sizeof header)
        return false;
    reserved = (header[1] << 8) | header[0];
    if (reserved != 0)
        return false;
    imageType = (header[3] << 8) | header[2];
    planeCount = (header[5] << 8) | header[4];
    return isCur() || isIco();
}

bool IconDirectoryEntry::read(std::FILE* file)
{
    uint8_t ent[16];

    if (std::fread(ent, 1, sizeof ent, file) != sizeof ent)
        return false;
    width = ent[0];
    height = ent[1];
    colorCount = ent[2];
    reserved = ent[3];
    planeCount = (ent[5] << 8) | ent[4];
    bitCount = (ent[7] << 8) | ent[6];
    imageSize = (ent[11] << 24) | (ent[10] << 16) | (ent[9] << 8) | ent[8];
    imageOffset = (ent[15] << 24) | (ent[14] << 16) | (ent[13] << 8) | ent[12];
    return true;
}

bool IcoImage::open(FILE* file)
{
    assert(file);
    if (!header.read(file))
        return false;
    for (int i = 0; i < header.planeCount; ++i) {
        IconDirectoryEntry ent;
        if (!ent.read(file))
            return false;
        directory.push_back(ent);
    }
    return true;
}

BoxImage* IcoImage::open(std::FILE* file, size_t n)
{
    if (getPlaneCount() < n)
        return 0;
    const IconDirectoryEntry& ent(getEntry(n));
    if (std::fseek(file, ent.imageOffset, SEEK_SET) == -1)
        return 0;
    BoxImage* image = new(std::nothrow) BoxImage;
    if (!image)
        return 0;
    image->open(file);
    if (image->getState() == BoxImage::Broken) {
        // TODO: treat as bitmaps
        delete image;
        return 0;
    }
    return image;
}

}}}}  // org::w3c::dom::bootstrap
