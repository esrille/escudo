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

#include "FontManager.h"

#include <algorithm>

#include <freetype/tttables.h>
#include <freetype/ftsizes.h>
#include <unicode/utypes.h>

#include "utf.h"
#include "TextIterator.h"

using namespace org::w3c::dom::bootstrap;

namespace {

// cf. CSSWhiteSpaceValueImp
enum {
    Normal,
    Pre,
    Nowrap,
    PreWrap,
    PreLine
};

bool isCollapsingSpace(unsigned value) {
    switch (value) {
    case Normal:
    case Nowrap:
    case PreLine:
        return true;
    default:
        return false;
    }
}

const bool USE_HINTING = false;

}
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

FontFace::FontFace(FontManager* manager, const std::string fontFilename, long index) try :
    manager(manager),
    charmap(0)
{
    FT_Error error = FT_New_Face(manager->library, fontFilename.c_str(), index, &face);
    if (error) {
        face = 0;
        throw std::runtime_error(__func__);
    }
    initCharmap();
} catch (...) {
    if (face)
        FT_Done_Face(face);
    throw;
}

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
    bearingGap(0.0f)
{
    addImage();
    glyphs = new FontGlyph[face->glyphCount];

    sizes[0] = face->face->size;
    for (int i = 1; i < Sizes; ++i) {
        FT_Error error = FT_New_Size(face->face, &sizes[i]);
        if (error)
            throw std::runtime_error(__func__);
    }

    unsigned px = (point * 96) / 72;  // TODO: make dpi configurable
    for (int i = 0; i < Sizes; ++i) {
        FT_Activate_Size(sizes[i]);
        FT_Error error = FT_Set_Pixel_Sizes(face->face, 0, px);
        if (error)
            throw std::runtime_error(__func__);
        px >>= 1;
    }
    FT_Activate_Size(sizes[0]);

    ascender = face->face->ascender;
    descender = face->face->descender;
    pen.x = pen.y = Offset;  // (0, 0) is reserved for an uninitialized font glyph
    ymax = 0;

    lineGap = 0;
    xHeight = ascender / 2;
    if (FT_IS_SFNT(face->face)) {
        if (TT_OS2_* os2 = static_cast<TT_OS2_*>(FT_Get_Sfnt_Table(face->face, ft_sfnt_os2))) {
            lineGap = os2->usWinAscent + os2->usWinDescent - face->face->units_per_EM;
            xHeight = os2->sxHeight;
            float b = (point * 96.0f) / 72.0f * 64.0f;
            b *= static_cast<float>(ascender) / (ascender - descender);
            if (USE_HINTING) {
                b /= 64.0f;
                bearingGap = (roundf(b) - b) * 64.0f;
            } else {
                bearingGap = ceilf(b) - b;
                if (0.0f < bearingGap)
                    bearingGap -= 64.0f;
            }
        }
    }

    // Store the missing glyph (0) as the 1st entry
    if (!storeGlyph(glyphs, 0))
        throw std::runtime_error(__func__);
} catch (...) {
    delete glyphs;
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
    FT_Error error = FT_Load_Glyph(face->face, glyphIndex, USE_HINTING ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);
    if (error)
        return false;

    // convert to an anti-aliased bitmap
    error = FT_Render_Glyph(face->face->glyph, FT_RENDER_MODE_NORMAL);
    if (error)
        return false;

    uint8_t* image;

    FT_GlyphSlot slot = face->face->glyph;
    try {
        image = drawBitmap(glyph, slot);
    } catch (...) {
        return false;
    }
    assert(glyph->isInitialized());

    // update mipmap textures.
    for (int i = 1; i < Sizes; ++i) {
        FT_Activate_Size(sizes[i]);
        // load glyph image into the slot (erase previous one)
        FT_Error error = FT_Load_Glyph(face->face, glyphIndex, USE_HINTING ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);
        if (error)
            return false;
        // convert to an anti-aliased bitmap
        error = FT_Render_Glyph(face->face->glyph, FT_RENDER_MODE_NORMAL);
        if (error)
            return false;
        FT_GlyphSlot slot = face->face->glyph;
        try {
            drawBitmap(glyph, slot, i);
        } catch (...) {
            return false;
        }
    }
    FT_Activate_Size(sizes[0]);

    updateImage(image, glyph);

    return true;
}

uint8_t* FontTexture::drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot)
{
    uint8_t* image = images.back();

    FT_Bitmap* bitmap = &slot->bitmap;
    unsigned w = (bitmap->width + Offset + Align - 1) & ~(Align - 1);
    unsigned h = (bitmap->rows + Offset + Align - 1) & ~(Align - 1);
    if (Width < pen.x + w) {
        pen.x = 0;
        pen.y += ymax;
        ymax = 0;
    }
    if (pen.y / Height != (pen.y + h) / Height) {
        image = addImage();
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

    for (FT_Int i = glyph->y % Height, p = 0; p < bitmap->rows; ++i, ++p) {
        for (FT_Int j = glyph->x, q = 0; q < bitmap->width; ++j, ++q)
            image[i * Width + j] |= bitmap->buffer[p * bitmap->width + q];
    }
    assert(((glyph->width + Offset + Align -1) & ~(Align - 1)) <= w);

    pen.x += w;
    if (ymax < h)
        ymax = h;
    return image;
}

void FontTexture::drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot, int level)
{
    uint8_t* image = images.back();

    FT_Bitmap* bitmap = &slot->bitmap;
    unsigned x = glyph->x >> level;
    unsigned y = (glyph->y % Height) >> level;
    uint8_t* m = getMipmapImage(image, level);
    unsigned px = Width >> level;
    for (FT_Int i = y, p = 0; p < bitmap->rows; ++i, ++p) {
        for (FT_Int j = x, q = 0; q < bitmap->width; ++j, ++q)
            m[i * px + j] = bitmap->buffer[p * bitmap->width + q];
    }

    glyph->width = std::max(static_cast<unsigned short>(bitmap->width << level), glyph->width);
    glyph->height = std::max(static_cast<unsigned short>(bitmap->rows << level), glyph->height);
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

size_t FontTexture::fitText(const char16_t* text, size_t length, float point, float& leftover, unsigned ws, size_t* next, float* required)
{
    const float scale = point / this->point / 64.0f;
    char32_t u = 0;
    size_t posLast = 0;
    TextIterator ti;  // TODO: keep one ti
    ti.setText(text, length);
    if (next)
        *next = 0;
    char32_t c = 0;
    while (ti.next() && c != '\n') {
        size_t pos = *ti;
        float advance = 0.0f;
        FontGlyph* glyph;
        for (size_t i = posLast; i < pos; ++i) {
            text = utf16to32(text, &c);
            if (c != '\n') {
                u = c;
                glyph = getGlyph(u);
                advance += glyph->advance;
            }
        }
        advance *= scale;
        if (leftover < advance && ws != Pre && ws != Nowrap) {
            if (u == u' ' && isCollapsingSpace(ws) && posLast < pos) {
                float trimmed = advance - glyph->advance * scale;
                if (trimmed <= leftover) {
                    leftover -= trimmed;
                    posLast = pos - 1;
                    break;
                }
            }
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

// TODO: Support full (language-specific) case mapping at some point.
// cf. http://userguide.icu-project.org/transforms/casemappings
std::u16string FontTexture::
fitTextWithTransformation(const char16_t* text, size_t length, float point, unsigned transform,
                          float& leftover,
                          size_t* lenght, size_t* transformedLength,
                          unsigned ws, size_t* next, float* required)
{
    std::u16string transformed;
    const float scale = point / this->point / 64.0f;
    char32_t u = 0;
    size_t posLast = 0;
    size_t posLastTransformed = 0;
    TextIterator ti;  // TODO: keep one ti
    ti.setText(text, length);
    if (next)
        *next = 0;
    char32_t c;
    while (ti.next() && c != '\n') {
        size_t pos = *ti;
        float advance = 0.0f;
        FontGlyph* glyph;
        for (size_t i = posLast; i < pos; ++i) {
            text = utf16to32(text, &c);
            if (c != '\n') {
                u = c;
                switch (transform) {
                case 1:  // capitalize
                    if (i == posLast)
                        u = u_totitle(u);
                    break;
                case 2:  // uppercase
                    u = u_toupper(u);
                    break;
                case 3:  // lowercase
                    u = u_tolower(u);
                    break;
                default:  // none
                    break;
                }
                char16_t buffer[3];
                if (char16_t* p = utf32to16(u, buffer)) {
                    *p = 0;
                    transformed += buffer;
                }
                glyph = getGlyph(u);
                advance += glyph->advance;
            }
        }
        advance *= scale;
        if (leftover < advance && ws != Pre && ws != Nowrap) {
            if (u == u' ' && isCollapsingSpace(ws) && posLast < pos) {
                float trimmed = advance - glyph->advance * scale;
                if (trimmed <= leftover) {
                    leftover -= trimmed;
                    posLast = pos - 1;
                    posLastTransformed = transformed.length() - 1;
                    transformed.erase(posLastTransformed);
                    break;
                }
            }
            if (next)
                *next = pos;
            if (required)
                *required = advance;
            break;
        }
        leftover -= advance;
        posLast = pos;
        posLastTransformed = transformed.length();
    }
    if (lenght)
        *lenght = posLast;
    if (transformedLength)
        *transformedLength = posLastTransformed;
    return transformed;
}