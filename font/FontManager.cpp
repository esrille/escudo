/*
 * Copyright 2010 Esrille Inc.
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

#include "FontManager.h"

#include <algorithm>

#include <freetype/tttables.h>

#include "utf.h"
#include "TextIterator.h"

using namespace org::w3c::dom::bootstrap;

//
// FontManager
//

FontManager::~FontManager()
{
    for (std::map<std::string, FontFace*>::iterator it = faces.begin(); it != faces.end(); ++it)
        delete it->second;
    FT_Done_FreeType(library);
}

FontFace* FontManager::getFontFace(const std::string fontFilename) throw ()
{
    std::map<std::string, FontFace*>::iterator it = faces.find(fontFilename);
    if (it != faces.end())
        return it->second;
    FontFace* face = 0;
    try {
        face = new FontFace(this, fontFilename);
        faces.insert(std::pair<std::string, FontFace*>(fontFilename, face));
    } catch (...) {
        delete face;
        return 0;
    }
    return face;
}

//
// FontFace
//

FontFace::~FontFace()
{
    for (std::map<unsigned int, FontTexture*>::iterator it = textures.begin(); it != textures.end(); ++it)
        delete it->second;
    FT_Done_Face(face);
}

FontTexture* FontFace::getFontTexture(unsigned int point)
{
    std::map<unsigned int, FontTexture*>::iterator it = textures.find(point);
    if (it != textures.end())
        return it->second;
    FontTexture* texture = 0;
    try {
        texture = new FontTexture(this, point);
        textures.insert(std::pair<unsigned int, FontTexture*>(point, texture));
    } catch (...) {
        delete texture;
        return 0;
    }
    return texture;
}

//
// FontTexture
//

FontTexture::FontTexture(FontFace* face, unsigned int point) try :
    face(face),
    glyphs(0),
    point(point),
    image(0)
{
    addImage();
    glyphs = new FontGlyph[face->glyphCount];
    // TODO: make dpi configurable
    FT_Error error = FT_Set_Pixel_Sizes(face->face, 0, (point * 96) / 72);
    // FT_Error error = FT_Set_Char_Size(face->face, point * 64, point * 64, 96, 96);
    if (error)
        throw std::runtime_error(__func__);
    ascender = face->face->ascender;
    descender = face->face->descender;
    pen.x = pen.y = 1;  // (0, 0) is reserved for an uninitialized font glyph
    ymax = 0;

    xHeight = ascender / 2;
    if (FT_IS_SFNT(face->face)) {
        if (TT_OS2_* os2 = static_cast<TT_OS2_*>(FT_Get_Sfnt_Table(face->face, ft_sfnt_os2)))
            xHeight = os2->sxHeight;
    }

    // Store the missing glyph (0) as the 1st entry
    if (!storeGlyph(glyphs, 0))
        throw std::runtime_error(__func__);
    } catch (...) {
    delete glyphs;
    delete image;
    throw;
}

FontTexture::~FontTexture()
{
    for (std::vector<uint8_t*>::iterator it = images.begin(); it != images.end(); ++it)
        deleteImage(*it);
    delete[] glyphs;
}

FontGlyph* FontTexture::getGlyph(int32_t ucode)
{
    std::vector<int32_t>::iterator result;
    result = std::lower_bound(face->charmap.begin(), face->charmap.end(), ucode);
    if (*result != ucode)
        return glyphs;
    FontGlyph* glyph = &glyphs[result - face->charmap.begin()];
    if (!glyph->isInitialized()) {
        FT_UInt glyphIndex = FT_Get_Char_Index(face->face, ucode);
        assert(glyphIndex);
        if (!glyphIndex)
            return glyphs;
        if (!storeGlyph(glyph, glyphIndex))
            return glyphs;
    }
    return glyph;
}

uint8_t* FontTexture::getImage(FontGlyph* glyph)
{
    // TODO: check range
    return images[glyph->y / Height];
}

bool FontTexture::storeGlyph(FontGlyph* glyph, FT_UInt glyphIndex)
{
    // load glyph image into the slot (erase previous one)
    FT_Error error = FT_Load_Glyph(face->face, glyphIndex, FT_LOAD_DEFAULT);
    if (error)
        return false;

    // convert to an anti-aliased bitmap
    error = FT_Render_Glyph(face->face->glyph, FT_RENDER_MODE_NORMAL);
    if (error)
        return false;

    FT_GlyphSlot slot = face->face->glyph;
    try {
        drawBitmap(glyph, slot);
    } catch (...) {
        return false;
    }
    assert(glyph->isInitialized());
    return true;
}

void FontTexture::drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot)
{
    FT_Bitmap* bitmap = &slot->bitmap;
    if (Width < pen.x + bitmap->width) {
        pen.x = 0;
        pen.y += ymax + Offset;
        ymax = 0;
    }
    if (pen.y / Height != (pen.y + bitmap->rows) / Height) {
        addImage();
        pen.x = 0;
        pen.y = (pen.y / Height + 1) * Height;
        ymax = 0;
    }

    glyph->x = pen.x;
    glyph->y = pen.y;
    glyph->left = slot->metrics.horiBearingX;
    glyph->top = slot->metrics.horiBearingY;
    glyph->width = bitmap->width;
    glyph->height = bitmap->rows;
    glyph->advance = slot->advance.x;

    for (FT_Int i = glyph->y, p = 0; p < bitmap->rows; ++i, ++p) {
        for (FT_Int j = glyph->x, q = 0; q < bitmap->width; ++j, ++q) {
            image[i * Width + j] |= bitmap->buffer[p * bitmap->width + q];
        }
    }
    pen.x += glyph->width + Offset;
    if (ymax < glyph->height)
        ymax = glyph->height;

    updateImage(image, glyph, slot);
}

float FontTexture::measureText(const char16_t* text, float point)
{
    char32_t u;
    int width = 0;
    while ((text = utf16to32(text, &u)) && u) {
        FontGlyph* glyph = getGlyph(u);
        width += glyph->advance;
    }
    return width / 64.0f * point / this->point;
}

size_t FontTexture::fitText(const char16_t* text, size_t length, float point, float& leftover, size_t* next, float* required)
{
    const float scale = point / this->point / 64.0f;
    char32_t u;
    float width = 0.0f;
    size_t posLast = 0;
    TextIterator ti;  // TODO: keep one ti
    ti.setText(text, length);
    while (ti.next()) {
        size_t pos = *ti;
        float advance = 0.0f;
        for (size_t i = posLast; i < pos; ++i) {
            text = utf16to32(text, &u);
            FontGlyph* glyph = getGlyph(u);
            advance += glyph->advance;
        }
        advance *= scale;
        if (leftover < advance) {
            if (next)
                *next = pos;
            if (required)
                *required = advance;
            break;
        }
        leftover -= advance;
        posLast = pos;
    }
    return posLast;
}
