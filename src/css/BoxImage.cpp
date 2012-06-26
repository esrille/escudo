/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include "Box.h"

#include <gif_lib.h>
#include <jpeglib.h>
#include <png.h>
#include <stdio.h>

#include <GL/gl.h>

#include <boost/bind.hpp>

#include "Bmp.h"

#include "utf.h"
#include "http/HTTPRequest.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// image/png - 89 50 4E 47 0D 0A 1A 0A
// image/gif - "GIF87a" or "GIF89a"
// image/jpeg - FF D8
// image/x-bmp - "BM"
// image/vnd.microsoft.icon - 00 00 01 00 (.ico), 00 00 02 00 (.cur)

namespace {

unsigned char* readAsPng(FILE* file, unsigned& width, unsigned& height, unsigned& format)
{
    png_byte header[8];
    if (fread(header, 1, 8, file) != 8 || png_sig_cmp(header, 0, 8))
        return 0;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 0;
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 0;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    png_init_io(png_ptr, file);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    unsigned bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    unsigned color_type = png_get_color_type(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
        color_type = GL_RGBA;
    }
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    switch (color_type) {
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_PALETTE:
        format = GL_RGB;
        break;
    default:
        format = GL_RGBA;
        break;
    }
    png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    unsigned rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    png_bytep data = (png_bytep) malloc(rowbytes * height);
    png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (unsigned i = 0; i < height; i++)
        row_pointers[i] = &data[rowbytes * i];
    png_read_image(png_ptr, row_pointers);
    free(row_pointers);

    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    return data;
}

unsigned char* readAsJpeg(FILE* file, unsigned& width, unsigned& height, unsigned& format)
{
    unsigned char sig[2];
    if (fread(sig, 1, sizeof sig, file) != sizeof sig || sig[0] != 0xFF || sig[1] != 0xD8)
        return 0;
    fseek(file, -(sizeof sig), SEEK_CUR);

    JSAMPARRAY img;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);  // TODO: set our own error handdler
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, true);
    width = cinfo.image_width;
    height = cinfo.image_height;
    jpeg_start_decompress(&cinfo);

    unsigned char* data = (unsigned char*) malloc(height * width * cinfo.out_color_components);
    img = (JSAMPARRAY) malloc(sizeof(JSAMPROW) * height);
    for (unsigned i = 0; i < height; ++i)
        img[i] = (JSAMPROW) &data[cinfo.out_color_components * width * i];

    while(cinfo.output_scanline < cinfo.output_height)
        jpeg_read_scanlines(&cinfo,
                            img + cinfo.output_scanline,
                            cinfo.output_height - cinfo.output_scanline);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    free(img);

    if (cinfo.out_color_components == 1)
        format = GL_LUMINANCE;
    else
        format = GL_RGB;
    return data;
}

uint16_t readUint16(const char* p)
{
    return static_cast<uint16_t >(p[0]) + (static_cast<uint16_t >(p[1]) << 8);
}

unsigned char* expandColors(unsigned char* dst, unsigned char* src, unsigned count, int transparentIndex, GifColorType* colors)
{
    for (int i = 0; i < count; ++i, ++src) {
        if (*src == transparentIndex) {
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
        } else {
            GifColorType* color = &colors[*src];
            *dst++ = color->Red;
            *dst++ = color->Green;
            *dst++ = color->Blue;
            *dst++ = 255;
        }
    }
    return src;
}

unsigned char* readAsGif(FILE* file, unsigned& width, unsigned& height, unsigned& format, unsigned &frameCount, std::vector<uint16_t>& delays, unsigned& loop)
{
    unsigned char sig[6];
    if (fread(sig, 1, sizeof sig, file) != sizeof sig)
        return 0;
    if (memcmp(sig, GIF87_STAMP, 6) && memcmp(sig, GIF89_STAMP, 6))
        return 0;
    fseek(file, -(sizeof sig), SEEK_CUR);
    long pos = ftell(file);

    int fd = dup(fileno(file));
    lseek(fd, pos, SEEK_SET);
    GifFileType* gif = DGifOpenFileHandle(fd);
    if (!gif)
        return 0;
    if (DGifSlurp(gif) != GIF_OK || gif->ImageCount < 1) {
        DGifCloseFile(gif);
        return 0;
    }

    width = gif->SWidth;
    height = gif->SHeight;
    frameCount = gif->ImageCount;
    loop = 1;
    unsigned char* data = (unsigned char*) malloc(width * height * 4 * frameCount);
    if (!data) {
        DGifCloseFile(gif);
        return 0;
    }

    if (1 < frameCount)
        delays.resize(frameCount, 10);

    format = GL_RGBA;
    for (int frame = 0; frame < frameCount; ++frame) {
        SavedImage* image = &gif->SavedImages[frame];
        int transparentIndex = -1;
        int delay = 0;
        for (int i = 0; i < image->ExtensionBlockCount; ++i) {
            ExtensionBlock* ext = image->ExtensionBlocks + i;
            switch (ext->Function) {
            case GRAPHICS_EXT_FUNC_CODE:
                if (ext->ByteCount == 4) {
                    if ((ext->Bytes[0] & 1))    // transparent?
                        transparentIndex = ext->Bytes[3];
                    delays[frame] = readUint16(ext->Bytes + 1);
                }
                break;
            case APPLICATION_EXT_FUNC_CODE:
                if (ext->ByteCount == 11 && (!memcmp(ext->Bytes, "NETSCAPE2.0", 11) || !memcmp(ext->Bytes, "ANIMEXTS1.0", 11))) {
                    ++ext;  // Move to the sub-block
                    if (ext->ByteCount == 3) {
                        loop = readUint16(ext->Bytes + 1);
                    }
                }
                break;
            default:
                break;
            }
        }
        unsigned char* p0 = data + frame * width * height * 4;
        if (!gif->Image.Interlace)
            expandColors(p0, image->RasterBits, width * height, transparentIndex, gif->SColorMap->Colors);
        else {
            unsigned char* index = image->RasterBits;
            for (int row = 0; row < height; row += 8)
                index = expandColors(p0 + width * row, index, width, transparentIndex, gif->SColorMap->Colors);
            for (int row = 4; row < height; row += 8)
                index = expandColors(p0 + width * row, index, width, transparentIndex, gif->SColorMap->Colors);
            for (int row = 2; row < height; row += 4)
                index = expandColors(p0 + width * row, index, width, transparentIndex, gif->SColorMap->Colors);
            for (int row = 1; row < height; row += 2)
                index = expandColors(p0 + width * row, index, width, transparentIndex, gif->SColorMap->Colors);
        }
    }
    DGifCloseFile(gif);
    return data;
}

unsigned char* readAsBmp(FILE* file, unsigned& width, unsigned& height, unsigned& format)
{
    BitmapFileHeader fileHeader;
    if (!fileHeader.read(file))
        return 0;
    BitmapInfoheader header;
    if (!header.read(file))
        return 0;
    RGBQuad colorTable[header.usedColors ? header.usedColors : 1];
    if (0 < header.usedColors && !header.readColorTable(file, colorTable))
        return 0;
    width = header.getWidth();
    height = header.getHeight();
    format = GL_RGBA;
    unsigned char* data = static_cast<unsigned char*>(malloc(width * height * 4));
    if (!data)
        return 0;
    if (std::fseek(file, fileHeader.offset, SEEK_SET) == -1)
        return false;
    bool result = header.readPixels(file, colorTable, data);
    if (!result) {
        free(data);
        return 0;
    }
    return data;
}

// file must points to BitmapInfoheader
unsigned char* readAsIco(FILE* file, unsigned& width, unsigned& height, unsigned& format)
{
    BitmapInfoheader header;
    if (!header.read(file))
        return 0;
    RGBQuad colorTable[header.usedColors ? header.usedColors : 1];
    if (0 < header.usedColors && !header.readColorTable(file, colorTable))
        return 0;
    width = header.getWidth();
    height = header.getHeight();
    if (width * 2 == height) {   // has AND plane?
        header.height /= 2;
        height /= 2;
    }
    format = GL_RGBA;
    unsigned char* data = static_cast<unsigned char*>(malloc(width * height * 4));
    if (!data)
        return 0;
    // TODO: Process AND plane.
    // TODO: Support JPEG and PNG.
    bool result = header.readPixels(file, colorTable, data);
    if (!result) {
        free(data);
        return 0;
    }
    return data;
}

}  // namespace

BoxImage::BoxImage(unsigned repeat) :
    state(Unavailable),
    flags(0),
    pixels(0),
    naturalWidth(0),
    naturalHeight(0),
    repeat(repeat),
    format(GL_RGBA),
    frameCount(1),
    delays(1),
    total(0.0f)
{
}

void BoxImage::open(FILE* file)
{
    assert(file);
    long pos = ftell(file);
    pixels = readAsIco(file, naturalWidth, naturalHeight, format);
    if (!pixels) {
        fseek(file, pos, SEEK_SET);
        pixels = readAsPng(file, naturalWidth, naturalHeight, format);
    }
    if (!pixels) {
        fseek(file, pos, SEEK_SET);
        pixels = readAsJpeg(file, naturalWidth, naturalHeight, format);
    }
    if (!pixels) {
        fseek(file, pos, SEEK_SET);
        pixels = readAsGif(file, naturalWidth, naturalHeight, format, frameCount, delays, loop);
    }
    if (!pixels) {
        fseek(file, pos, SEEK_SET);
        pixels = readAsBmp(file, naturalWidth, naturalHeight, format);
    }
    if (!pixels) {
        state = Broken;
        return;
    }
    state = CompletelyAvailable;
    total = 0.0f;
    for (auto i = 0; i < delays.size(); ++i)
        total += delays[i];
}

unsigned BoxImage::getCurrentFrame(unsigned t, unsigned& delay, unsigned start)
{
    if (frameCount <= 1 || total == 0.0f)
        return 0;
    if (loop) {
        t -= start;
        if (total * loop <= t)
            return 0;
    }
    t %= total;
    unsigned d = 0.0f;
    for (unsigned i = 0; i < delays.size(); ++i) {
        d += delays[i];
        if (t < d) {
            delay = d - t;
            return i;
        }
    }
    return 0;
}

//
// HttpRequest
//

BoxImage* HttpRequest::getBoxImage(unsigned repeat)
{
    if (!boxImage)
        boxImage = new(std::nothrow) BoxImage(repeat);
    if (!boxImage)
        return 0;

    switch (getReadyState()) {
    case UNSENT:
        boxImage->setState(BoxImage::Unavailable);
        break;
    case OPENED:
    case HEADERS_RECEIVED:
    case LOADING:
    case COMPLETE:
        boxImage->setState(BoxImage::Sent);
        break;
    case DONE:
        if (boxImage->getState() < BoxImage::PartiallyAvailable) {
            if (getError()) {
                boxImage->setState(BoxImage::Unavailable);
                break;
            }
            if (FILE* file = openFile()) {
                boxImage->open(file);
                fclose(file);
            }
        }
        break;
    default:
        boxImage->setState(BoxImage::Unavailable);
        break;
    }
    return boxImage;
}

}}}}  // org::w3c::dom::bootstrap
