/*
 * Copyright 2011 Esrille Inc.
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

#include <png.h>
#include <stdio.h>

#include <boost/bind.hpp>

#include "utf.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// image/png - 89 50 4E 47 0D 0A 1A 0A
// image/gif - "GIF87a" or "GIF89a"
// image/jpeg - FF D8
// image/bmp - "BM"
// image/vnd.microsoft.icon - 00 00 01 00 (.ico), 00 00 02 00 (.cur)

namespace {

unsigned char* readAsPng(FILE* file, unsigned* widthPtr, unsigned* heightPtr)
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

    unsigned width = png_get_image_width(png_ptr, info_ptr);
    unsigned height = png_get_image_height(png_ptr, info_ptr);
    unsigned bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    unsigned color_type = png_get_color_type(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_RGB)
        png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
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

    *widthPtr = width;
    *heightPtr = height;
    return data;
}

}  // namespace

BoxImage::BoxImage(Box* box, const std::u16string& base, const std::u16string& url, unsigned repeat) :
    box(box),
    state(Unavailable),
    pixels(0),
    naturalWidth(0),
    naturalHeight(0),
    repeat(repeat),
    img(static_cast<html::HTMLImageElement*>(0) /* nullptr */),
    request(base)
{
    open(url);
}

BoxImage::BoxImage(Box* box, const std::u16string& base, html::HTMLImageElement& img) :
    box(box),
    state(Unavailable),
    pixels(0),
    naturalWidth(0),
    naturalHeight(0),
    repeat(0),
    img(img),
    request(base)
{
    open(img.getSrc());
}

void BoxImage::open(const std::u16string& url)
{
    request.open(u"GET", url);
    request.setHanndler(boost::bind(&BoxImage::notify, this));
    request.send();
    state = Sent;
    if (request.getReadyState() != HttpRequest::DONE)
        return;
    if (request.getErrorFlag()) {
        state = Broken;
        return;
    }
    FILE* file = request.openFile();
    if (!file) {
        state = Broken;
        return;
    }
    pixels = readAsPng(file, &naturalWidth, &naturalHeight);
    if (pixels)
        state = CompletelyAvailable;
    else
        state = Broken;
    fclose(file);
}

void BoxImage::notify()
{
    if (state == Sent) {
        // TODO: update render tree!!
        box->flags = 1;
    }
}

}}}}  // org::w3c::dom::bootstrap
