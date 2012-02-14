/*
 * Copyright 2010-2012 Esrille Inc.
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

#include <strings.h>

#include <algorithm>
#include <iostream>
#include <set>

#include <unicode/utypes.h>

#include <freetype/ftoutln.h>
#include <freetype/ftsizes.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include "utf.h"
#include "TextIterator.h"

#include "css/CSSStyleDeclarationImp.h"

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

FontManager::FontManager(FontManagerBackEnd* backend) :
    backend(backend)
{
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
        throw std::runtime_error(__func__);
}

FontManager::~FontManager()
{
    for (int i = 0; i < 6; ++i) {
        while (!genericLists[i].empty()) {
            FontFace* face = genericLists[i].front();
            genericLists[i].pop_front();
            delete face;
        }
    }
    FT_Done_FreeType(library);
}

FontFace* FontManager::loadFont(const char* fontFilename)
{
    FontFace* face = new(std::nothrow) FontFace(this, fontFilename);
    if (face)
        genericLists[face->getGeneric()].push_back(face);
    return face;
}

void FontManager::registerFont(const std::u16string& familyName, FontFace* face)
{
    faces.insert(std::pair<const std::u16string, FontFace*>(familyName, face));
}

FontFace* FontManager::getFontFace(unsigned generic, unsigned style, unsigned weight, int mask)
{
    FontFace* chosen = 0;
    unsigned score = 0;
    while (mask) {
        mask &= ~(1 << generic);
        for (auto it = genericLists[generic].begin(); it != genericLists[generic].end(); ++it) {
            FontFace* face = *it;
            if (chosen && strcmp(chosen->getFamilyName(), face->getFamilyName()))
                break;
            unsigned newScore = face->getScore(style, weight);
            if (!chosen || score < newScore) {
                chosen = face;
                score = newScore;
            }
        }
        if (chosen)
            return chosen;
        generic = ffs(mask) - 1;
    }
    return 0;
}

FontFace* FontManager::getAltFontFace(unsigned generic, unsigned style, unsigned weight,
                                      FontTexture* current, char32_t u)
{
    assert(current);
    FontFace* currentFace = current->getFace();
    FontFace* chosen = 0;
    unsigned score = 0;
    auto it = std::find(genericLists[currentFace->getGeneric()].begin(),
                        genericLists[currentFace->getGeneric()].end(),
                        currentFace);
    assert(it != genericLists[currentFace->getGeneric()].end());
    while (++it != genericLists[currentFace->getGeneric()].end()) {
        FontFace* face = *it;
        if (strcmp(face->getFamilyName(), currentFace->getFamilyName()) == 0)
            continue;
        if (chosen && strcmp(chosen->getFamilyName(), face->getFamilyName()))
            break;
        if (!face->hasGlyph(u))
            continue;
        unsigned newScore = face->getScore(style, weight);
        if (!chosen || score < newScore) {
            chosen = face;
            score = newScore;
        }
    }
    if (chosen)
        return chosen;

    int mask = 0x3f & ~(1 << generic);
    if (generic != currentFace->getGeneric())
        mask &= ~((1 << (currentFace->getGeneric() + 1)) - 1);
    generic = ffs(mask) - 1;
    return getFontFace(generic, style, weight, mask);
}

FontFace* FontManager::getFontFace(const std::u16string& familyName, unsigned style, unsigned weight)
{
    FontFace* chosen = 0;
    unsigned score = 0;
    for (auto it = faces.find(familyName); it != faces.end(); ++it) {
        if (compareIgnoreCase(it->first, familyName))
            break;
        FontFace* face = it->second;
        unsigned newScore = face->getScore(style, weight);
        if (!chosen || score < newScore) {
            chosen = face;
            score = newScore;
        }
    }
    return chosen;
}

//
// FontFace
//

FontFace::FontFace(FontManager* manager, const char* filename, long index) try :
    manager(manager),
    filename(filename),
    charmap(0),
    generic(CSSFontFamilyValueImp::None),
    style(CSSFontStyleValueImp::Normal),
    weight(400) // normal
{
    FT_Error error = FT_New_Face(manager->library, filename, index, &face);
    if (error) {
        face = 0;
        throw std::runtime_error(__func__);
    }

    initCharmap();

    std::set<std::u16string> familyNames;
    familyNames.insert(toString(face->family_name));
    if (FT_IS_SFNT(face)) {
        // cf. http://www.microsoft.com/typography/otspec/name.htm
        unsigned count = FT_Get_Sfnt_Name_Count(face);
        for (unsigned i = 0; i < count; ++i) {
            FT_SfntName sfntName;
            FT_Get_Sfnt_Name(face, i, &sfntName);
            if (sfntName.name_id == 1) {  // Font Family name
                if (sfntName.platform_id == 3) {
                    if (sfntName.encoding_id == 1) {
                        // Windows && Unicode BMP (UCS-2)
                        std::u16string name;
                        for (unsigned j = 0; j < sfntName.string_len; j+= 2)
                            name += (sfntName.string[j] << 8) | sfntName.string[j + 1];
                        familyNames.insert(name);
                    }
                }
            }
        }

        // cf. http://www.microsoft.com/typography/otspec/os2.htm
        if (TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face, ft_sfnt_os2))) {
            if (os2->fsSelection & 0x001)
                style = CSSFontStyleValueImp::Italic;
            else if (os2->fsSelection & 0x200)
                style = CSSFontStyleValueImp::Oblique;
            if (os2->fsSelection & 0x020)
                weight = 700;
            switch (os2->panose[3]) {    // bProportion
            case 9: // Monospaced
                generic = CSSFontFamilyValueImp::Monospace;
                break;
            default:
                switch (os2->panose[0]) {   // bFamilyType
                case 2: // Latin Text
                    switch (os2->panose[1]) {   // bSerifStyle
                    case 2:  // Cove
                    case 3:  // Obtuse Cove
                    case 4:  // Square Cove
                    case 5:  // Obtuse Square Cove
                        generic = CSSFontFamilyValueImp::Serif;
                        break;
                    case 11: // Normal Sans
                    case 12: // Obtuse Sans
                    case 13: // Perpendicular Sans
                        generic = CSSFontFamilyValueImp::SansSerif;
                        break;
                    default:
                        break;
                    }
                    break;
                case 3: // Latin Hand Written
                    generic = CSSFontFamilyValueImp::Cursive;
                    break;
                case 4: // Latin Decorative
                    generic = CSSFontFamilyValueImp::Fantasy;
                    break;
                default:
                    break;
                }
                break;
            }
            switch (os2->panose[2]) {   // bWeight
            case 2:  // Very Light
                weight = 100;
                break;
            case 3:  // Light
                weight = 200;
                break;
            case 4:  // Thin
                weight = 300;
                break;
            case 5:  // Book
                weight = 400;
                break;
            case 6:  // Medium
                weight = 500;
                break;
            case 7:  // Demi
                weight = 600;
                break;
            case 8:  // Bold
                weight = 700;
                break;
            case 9:  // Heavy
                weight = 800;
                break;
            case 10:  // Black
            case 11:  // Extra Black
                weight = 900;
                break;
            default:
                break;
            }
        }
    }
    for (auto i = familyNames.begin(); i != familyNames.end(); ++i)
        manager->registerFont(*i, this);

} catch (...) {
    if (face)
        FT_Done_Face(face);
    throw;
}

FontFace::~FontFace()
{
    for (auto it = textures.begin(); it != textures.end(); ++it)
        delete it->second;
    FT_Done_Face(face);
}

unsigned FontFace::getScore(unsigned style, unsigned weight) const
{
    unsigned score = 0;
    if (getStyle() == style)
        score += 200;
    else if (style == CSSFontStyleValueImp::Italic && getStyle() == CSSFontStyleValueImp::Oblique)
        score += 100;
    if (getWeight() == weight)
        score += 10;
    else if (weight <= 400) {
        if (weight == 400 && getWeight() == 500)
            score += 9;
        else if (getWeight() < weight)
            score += 9 - (weight - getWeight()) / 100;
        else
            score += (1000 - getWeight()) / 100;
    } else if (500 <= weight) {
        if (weight == 500 && getWeight() == 400)
            score += 9;
        else if (weight < getWeight())
            score += 9 - (getWeight() - weight) / 100;
        else
            score += getWeight() / 100;
    }
    return score;
}

bool FontFace::hasGlyph(char32_t ucode) const
{
    std::vector<int32_t>::const_iterator result;
    result = std::lower_bound(charmap.begin(), charmap.end(), ucode);
    return *result == ucode;
}

FontTexture* FontFace::getFontTexture(unsigned int point, bool bold, bool oblique)
{
    for (auto it = textures.find(point); it != textures.end(); ++it) {
        FontTexture* font = it->second;
        if (font->getPoint() == point && font->getBold() == bold && font->getOblique() == oblique)
            return font;
    }
    FontTexture* texture = 0;
    try {
        texture = new FontTexture(this, point, bold, oblique);
        textures.insert(std::pair<unsigned int, FontTexture*>(point, texture));
    } catch (...) {
        delete texture;
        return 0;
    }
    return texture;
}

FontTexture* FontFace::getFontTexture(unsigned int point, unsigned style, unsigned weight)
{
    bool bold = false;
    if (700 <= weight && getWeight() <= 400)
        bold = true;

    bool oblique = false;
    if ((style  == CSSFontStyleValueImp::Italic || style == CSSFontStyleValueImp::Oblique) &&
        !(getStyle() & CSSFontStyleValueImp::Italic || getStyle() & CSSFontStyleValueImp::Oblique))
        oblique = true;

    return getFontTexture(point, bold, oblique);
}

//
// FontTexture
//

FontTexture::FontTexture(FontFace* face, unsigned int point, bool bold, bool oblique) try :
    face(face),
    glyphs(0),
    point(point),
    bold(bold),
    oblique(oblique),
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
    lineThroughPosition = xHeight / 2;
    lineThroughSize = face->face->units_per_EM / 20;
    if (FT_IS_SFNT(face->face)) {
        if (TT_OS2* os2 = static_cast<TT_OS2*>(FT_Get_Sfnt_Table(face->face, ft_sfnt_os2))) {
            lineGap = os2->usWinAscent + os2->usWinDescent - face->face->units_per_EM;
            xHeight = os2->sxHeight;
            lineThroughPosition = os2->yStrikeoutPosition;
            lineThroughSize = os2->yStrikeoutSize;
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
    std::vector<int32_t>::const_iterator result;
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
    static const FT_Matrix matrix { 0x10000, 0x05000,     // 1.0, 0.3125
                                    0,       0x10000 };   // 0.0, 1.0

    // load glyph image into the slot (erase previous one)
    FT_Error error = FT_Load_Glyph(face->face, glyphIndex, USE_HINTING ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);
    if (error)
        return false;

    if (oblique)
        FT_Outline_Transform(&face->face->glyph->outline, &matrix);
    if (bold)
        FT_Outline_Embolden(&face->face->glyph->outline, (700 - 400) / 8);

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

        if (oblique)
            FT_Outline_Transform(&face->face->glyph->outline, &matrix);
        if (bold)
            FT_Outline_Embolden(&face->face->glyph->outline, (700 - 400) / 8);

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
    return width * getScale(point);
}

float FontTexture::measureText(const char16_t* text, size_t length, float point,
                               unsigned transform, bool isFirstCharacter,
                               float letterSpacing, float wordSpacing,
                               FontGlyph*& glyph, std::u16string& transformed)
{
    float width = 0.0f;
    const char16_t* p = text;
    const char16_t* end = text + length;
    char32_t u;
    while (p < end && (p = utf16to32(p, &u)) && u) {
        if (u != '\n' && u != u'\u200B') {
            switch (transform) {
            case 1:  // capitalize
                if (u == u'\u00A0')  // NBSP
                    isFirstCharacter = true;
                else if (isFirstCharacter && !u_ispunct(u)) {
                    u = u_totitle(u);
                    isFirstCharacter = false;
                }
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
            glyph = getGlyph(u);
            width += glyph->advance * getScale(point);
            append(transformed, u);
        }
        if (u == ' ' || u == u'\u00A0')  // SP or NBSP
            width += wordSpacing;
        width += letterSpacing;
    }
    return width;
}
