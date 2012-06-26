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

#ifndef ES_BMP_H
#define ES_BMP_H

#include <stdint.h>

#include <cstdio>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

struct RGBQuad;

struct BitmapFileHeader
{
    uint16_t type;        // 'BM' (0x42, 0x4d)
    uint32_t fileSize;
    uint16_t reserved1;   // Must always be zero.
    uint16_t reserved2;   // Must always be zero.
    uint32_t offset;      // Offset to bits

    bool read(std::FILE* file);
};

struct BitmapInfoheader
{
    enum {
        // compression
        RGB = 0,
        RLE8,
        RLE4,
        BITFIELDS,
        JPEG,
        PNG,

        EndOfLine = 0,
        EndOfBitmap = 1,
        Delta = 2
    };
    uint32_t headerSize;  // At least 40.
    int32_t  width;
    int32_t  height;
    uint16_t planeCount;
    int16_t  bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  pxPerMeter;
    int32_t  pyPerMeter;
    uint32_t usedColors;
    uint32_t importantColors;

    // Since version 5: Read if compression is BITFIELDS.
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
    uint32_t alphaMask;

    size_t getWidth() const {
        return width;
    }
    size_t getHeight() const {
        return (0 <= height) ? height : -height;
    }

    bool read(std::FILE* file);
    bool readColorTable(std::FILE* file, RGBQuad* colorTable);
    bool readPixels(std::FILE* file, const RGBQuad* colorTable, void* pixels);

private:
    bool readBitFields(std::FILE* file);
};

struct RGBQuad
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;

    uint32_t toRGBA() const {
        return (0xff << 24) | (blue << 16) | (green << 8) | red;
    }

    static bool read(std::FILE* file, RGBQuad* quad);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_BMP_H
