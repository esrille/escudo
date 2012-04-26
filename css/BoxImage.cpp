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

#include <jpeglib.h>
#include <png.h>
#include <stdio.h>

#include <GL/gl.h>

#include <boost/bind.hpp>

#include "utf.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// image/png - 89 50 4E 47 0D 0A 1A 0A
// image/gif - "GIF87a" or "GIF89a"
// image/jpeg - FF D8
// image/bmp - "BM"
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
        png_set_gray_1_2_4_to_8(png_ptr);
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
    if (fread(sig, 1, 2, file) != 2 || sig[0] != 0xFF || sig[1] != 0xD8)
        return 0;
    rewind(file);

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

}  // namespace

BoxImage::BoxImage(Box* box, unsigned repeat) :
    box(box),
    state(Unavailable),
    pixels(0),
    naturalWidth(0),
    naturalHeight(0),
    repeat(repeat),
    format(GL_RGBA)
{
}

BoxImage::BoxImage(Box* box, const std::u16string& base, const std::u16string& url, unsigned repeat) :
    box(box),
    state(Unavailable),
    pixels(0),
    naturalWidth(0),
    naturalHeight(0),
    repeat(repeat),
    format(GL_RGBA),
    request(base)
{
    open(url);
}

void BoxImage::open(const std::u16string& url)
{
    request.open(u"GET", url);
    request.setHanndler(boost::bind(&BoxImage::notify, this));
    request.send();
    state = Sent;
    if (request.getReadyState() != HttpRequest::DONE)
        return;
    if (!request.getError()) {
        if (FILE* file = request.openFile()) {
            open(file);
            fclose(file);
            return;
        }
    }
    state = Broken;
}

void BoxImage::open(FILE* file)
{
    assert(file);
    pixels = readAsPng(file, naturalWidth, naturalHeight, format);
    if (!pixels) {
        rewind(file);
        pixels = readAsJpeg(file, naturalWidth, naturalHeight, format);
    }
    if (pixels)
        state = CompletelyAvailable;
    else
        state = Broken;
}

void BoxImage::notify()
{
    if (state == Sent) {
        if (request.getStatus() == 200 && box)
            box->setFlags(2);   // for updating render tree.
        else
            state = Unavailable;
    }
}

}}}}  // org::w3c::dom::bootstrap
