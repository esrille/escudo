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

#include "Bmp.h"

#include <assert.h>

#include <algorithm>
#include <cstdio>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

bool BitmapFileHeader::read(std::FILE* file)
{
    uint8_t header[14];

    if (std::fread(header, 1, sizeof header, file) != sizeof header)
        return false;
    type = (header[1] << 8) | header[0];
    if (type != (('M' << 8) | 'B'))
        return false;
    fileSize = (header[5] << 24) | (header[4] << 16) | (header[3] << 8) | header[2];
    reserved1 = (header[7] << 8) | header[6];
    reserved2 = (header[9] << 8) | header[8];
    offset = (header[13] << 24) | (header[12] << 16) | (header[11] << 8) | header[10];
    return reserved1 == 0 && reserved2 == 0 && offset < fileSize;
}

bool BitmapInfoheader::read(std::FILE* file)
{
    uint8_t header[40];

    if (std::fread(header, 1, sizeof header, file) != sizeof header)
        return false;
    headerSize = (header[3] << 24) | (header[2] << 16) | (header[1] << 8) | header[0];
    if (headerSize < 40)
        return false;
    width = (header[7] << 24) | (header[6] << 16) | (header[5] << 8) | header[4];
    if (width <= 0)
        return false;
    height = (header[11] << 24) | (header[10] << 16) | (header[9] << 8) | header[8];
    if (height == 0)
        return false;
    planeCount = (header[13] << 8) | header[12];
    if (planeCount != 1)
        return false;
    bitCount = (header[15] << 8) | header[14];
    compression = (header[19] << 24) | (header[18] << 16) | (header[17] << 8) | header[16];
    imageSize = (header[23] << 24) | (header[22] << 16) | (header[21] << 8) | header[20];
    pxPerMeter = (header[27] << 24) | (header[26] << 16) | (header[25] << 8) | header[24];
    pyPerMeter = (header[31] << 24) | (header[30] << 16) | (header[29] << 8) | header[28];
    usedColors = (header[35] << 24) | (header[34] << 16) | (header[33] << 8) | header[32];
    importantColors = (header[39] << 24) | (header[38] << 16) | (header[37] << 8) | header[36];

    switch (bitCount) {
    case 0:
        break;
    case 1:
        if (usedColors == 0)
            usedColors = 2;
        else if (usedColors != 2)
            return false;
        break;
    case 4:
        if (usedColors == 0)
            usedColors = 16;
        else if (16 < usedColors)
            return false;
        break;
    case 8:
        if (usedColors == 0)
            usedColors = 256;
        else if (256 < usedColors)
            return false;
        break;
    case 16:
        if (usedColors != 0)
            return false;
        redMask =   0x7c00;
        greenMask = 0x03e0;
        blueMask =  0x001f;
        alphaMask = 0x0000;
        break;
    case 24:
        break;
    case 32:
        if (usedColors != 0)
            return false;
        redMask =   0x00ff0000;
        greenMask = 0x0000ff00;
        blueMask =  0x000000ff;
        alphaMask = 0xff000000;
        break;
    default:
        return false;
    }
    switch (compression) {
    case BITFIELDS:
        if (bitCount != 16 && bitCount != 32)
            return false;
        if (!readBitFields(file))
            return false;
        break;
    case RLE8:
        if (bitCount != 8)
            return false;
        break;
    case RLE4:
        if (bitCount != 4)
            return false;
    default:
        break;
    }

    if (std::fseek(file, headerSize - 40, SEEK_CUR) == -1)
        return false;
    return true;
}

bool BitmapInfoheader::readColorTable(std::FILE* file, RGBQuad* quad)
{
    switch (bitCount) {
    case 1:
    case 4:
    case 8:
        break;
    default:
        return true;
    }
    for (size_t i = 0; i < usedColors; ++i, ++quad) {
        if (!RGBQuad::read(file, quad))
            return false;
    }
    return true;
}

bool BitmapInfoheader::readBitFields(std::FILE* file)
{
    assert(compression == BITFIELDS);

    uint8_t data[16];
    if (std::fread(data, 1, sizeof data, file) != sizeof data)
        return false;
    redMask = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
    greenMask = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];
    blueMask = (data[11] << 24) | (data[10] << 16) | (data[9] << 8) | data[8];
    alphaMask = (data[15] << 24) | (data[14] << 16) | (data[13] << 8) | data[12];
    return true;
}

namespace {

uint32_t convert(uint32_t color, uint32_t mask, unsigned shift)
{
    int pop = __builtin_popcount(mask);
    if (pop == 0)
        return 0;
    color &= mask;
    int r = __builtin_ffs(mask);
    if (8 < pop)
        r += pop - 8;
    if (1 < r)
        color >>= (r - 1);
    if (8 <= pop)
        return color << shift;
    if (color == (1u << pop) - 1)
        return 0xff << shift;
    shift += (8 - pop);
    return color << shift;
}

}

bool BitmapInfoheader::readPixels(std::FILE* file, const RGBQuad* colorTable, void* pixels)
{
    size_t lineWidth = ((width * bitCount / 8) + 3) & ~3;
    uint8_t line[lineWidth];
    if (compression == RGB || compression == BITFIELDS) {
        int from, to, inc;
        if (0 <= height) {
            from = height - 1;
            to = 0;
            inc = -1;
        } else {
            from = 0;
            to = -height - 1;
            inc = 1;
        }
        for (int y = from; y != to; y += inc) {
            if (std::fread(line, 1, lineWidth, file) != lineWidth)
                return false;
            uint8_t *src = line;
            uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y;
            switch (bitCount) {
            case 1:
                for (int32_t x = 0; x < width;) {
                    uint32_t color = *src++;
                    int max = std::min(x + 8, width);
                    while (x++ < max) {
                        *dst++ = colorTable[(color & 0x80) ? 1 : 0].toRGBA();
                        color <<= 1;
                    }
                }
                break;
            case 4:
                for (int32_t x = 0; x < width; ++x) {
                    uint32_t s = *src++;
                    uint32_t color = (s >> 4) & 0x0f;
                    if (usedColors <= color)
                        return false;
                    *dst++ = colorTable[color].toRGBA();
                    color = s & 0x0f;
                    if (usedColors <= color)
                        return false;
                    if (width <= ++x)
                        break;
                    *dst++ = colorTable[color].toRGBA();
                }
                break;
            case 8:
                for (int32_t x = 0; x < width; ++x) {
                    uint32_t color = *src++;
                    if (usedColors <= color)
                        return false;
                    *dst++ = colorTable[color].toRGBA();
                }
                break;
            case 16:
                for (int32_t x = 0; x < width; ++x) {
                    uint32_t color = *src++;
                    color |= (*src++) << 8;
                    uint32_t rgba = 0;
                    rgba |= convert(color, redMask, 0);
                    rgba |= convert(color, greenMask, 8);
                    rgba |= convert(color, blueMask, 16);
                    if (alphaMask)
                        rgba |= convert(color, alphaMask, 24);
                    else
                        rgba |= 0xff000000;
                    *dst++ = rgba;
                }
                break;
            case 24:
                for (int32_t x = 0; x < width; ++x) {
                    uint32_t rgba = 0xff000000 | (*src++ << 16);
                    rgba |= *src++ << 8;
                    rgba |= *src++;
                    *dst++ = rgba;
                }
                break;
            case 32:
                for (int32_t x = 0; x < width; ++x) {
                    uint32_t color = *src++;
                    color |= (*src++) << 8;
                    color |= (*src++) << 16;
                    color |= (*src++) << 24;
                    uint32_t rgba = 0;
                    rgba |= convert(color, redMask, 0);
                    rgba |= convert(color, greenMask, 8);
                    rgba |= convert(color, blueMask, 16);
                    if (alphaMask)
                        rgba |= convert(color, alphaMask, 24);
                    else
                        rgba |= 0xff000000;
                    *dst++ = rgba;
                }
                break;
            default:
                break;
            }
        }
        return true;
    } else if (compression == RLE8) {
        int x = 0;
        int y = getHeight() - 1;
        for (;;) {
            int count = fgetc(file);
            int color = fgetc(file);
            if (count == EOF || color == EOF)
                break;
            if (0 < count) {
                if (width < x + count)
                    return false;
                if (usedColors <= color)
                    return false;
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                x += count;
                while (0 < count--)
                    *dst++ = colorTable[color].toRGBA();
            } else if (color == EndOfBitmap) {
                while (0 <= y) {
                    count = width - x;
                    uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                    while (0 < count--)
                        *dst++ = colorTable[0].toRGBA();
                    x = 0;
                    --y;
                }
                return true;
            } else if (color == EndOfLine) {
                count = width - x;
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                while (0 < count--)
                    *dst++ = colorTable[0].toRGBA();
                x = 0;
                --y;
            } else if (color == Delta) {
                int rx = 0;
                int ry = 0;
                rx = fgetc(file);
                ry = fgetc(file);
                if (rx == EOF || ry == EOF)
                    return false;
                x += rx;
                y -= ry;
                if (width <= x || y < 0)
                    return false;
            } else {
                count = color;
                if (width < x + count)
                    return false;
                bool skip = (count & 1);
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                x += count;
                while (0 < count--) {
                    color = fgetc(file);
                    if (color == EOF || usedColors <= color)
                        return false;
                    *dst++ = colorTable[color].toRGBA();
                }
                if (skip)
                    fgetc(file);
            }
        }
    } else if (compression == RLE4) {
        int x = 0;
        int y = getHeight() - 1;
        for (;;) {
            int count = fgetc(file);
            int color = fgetc(file);
            if (count == EOF || color == EOF || (count & 1))
                break;
            if (0 < count) {
                if ((count & 1) || width < x + count)
                    return false;
                int c0 = (color >> 4) & 0x0f;
                int c1 = color & 0x0f;
                if (usedColors <= c0 || usedColors <= c1)
                    return false;
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                x += count;
                count /= 2;
                while (0 < count--) {
                    *dst++ = colorTable[c0].toRGBA();
                    *dst++ = colorTable[c1].toRGBA();
                }
            } else if (color == EndOfBitmap) {
                while (0 <= y) {
                    count = width - x;
                    uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                    while (0 < count--)
                        *dst++ = colorTable[0].toRGBA();
                    x = 0;
                    --y;
                }
                return true;
            } else if (color == EndOfLine) {
                count = width - x;
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                while (0 < count--)
                    *dst++ = colorTable[0].toRGBA();
                x = 0;
                --y;
            } else if (color == Delta) {
                int rx = 0;
                int ry = 0;
                rx = fgetc(file);
                ry = fgetc(file);
                if (rx == EOF || ry == EOF)
                    return false;
                x += rx;
                y -= ry;
                if (width <= x || y < 0)
                    return false;
            } else {
                count = color;
                if ((count & 1) || width < x + count)
                    return false;
                uint32_t* dst = reinterpret_cast<uint32_t*>(pixels) + width * y + x;
                x += count;
                count /= 2;
                bool skip = (count & 1);
                while (0 < count--) {
                    color = fgetc(file);
                    if (color == EOF)
                        return false;
                    int c0 = (color >> 4) & 0x0f;
                    int c1 = color & 0x0f;
                    if (usedColors <= c0 || usedColors <= c1)
                        return false;
                    *dst++ = colorTable[c0].toRGBA();
                    *dst++ = colorTable[c1].toRGBA();
                }
                if (skip)
                    fgetc(file);
            }
        }
    }
    return false;
}

bool RGBQuad::read(std::FILE* file, RGBQuad* quad)
{
    assert(sizeof(RGBQuad) == 4);
    return std::fread(quad, 1, 4, file) == 4;
}

}}}}  // org::w3c::dom::bootstrap
