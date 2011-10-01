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

#ifndef ES_FONTMANAGER_H
#define ES_FONTMANAGER_H

#include <assert.h>
#include <stdint.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// FreeType 2, http://www.freetype.org/index2.html
#include <ft2build.h>
#include FT_FREETYPE_H

class FontFace;
class FontTexture;
struct FontGlyph;

class FontManagerBackEnd
{
public:
    virtual ~FontManagerBackEnd()
    {
    }

    virtual void addImage(uint8_t* image) = 0;
    virtual void deleteImage(uint8_t* image) = 0;
    virtual void updateImage(uint8_t* image, FontGlyph* glyph, FT_GlyphSlot slot) = 0;
    virtual void renderText(FontTexture* font, const char16_t* text, size_t length) = 0;
};

class FontManager
{
    friend class FontFace;

    FontManagerBackEnd* backend;
    FT_Library library;
    // a map from font filename to FontFace
    std::map<std::string, FontFace*> faces;

public:
    FontManager(FontManagerBackEnd* backend = 0) :
        backend(backend)
    {
        FT_Error error = FT_Init_FreeType(&library);
        if (error)
            throw std::runtime_error(__func__);
    }

    ~FontManager();

    /** Gets the font face object for the specified font file.
     * @param fontFilename font filename
     * @return the font face object, or zero upon failure
     */
    FontFace* getFontFace(const std::string fontFilename) throw ();

    FontManagerBackEnd* getBackEnd() const
    {
        return backend;
    }
};

class FontFace
{
    friend class FontTexture;

    FontManager* manager;

    std::vector<int32_t> charmap;
    int32_t glyphCount;
    FT_Face face;
    // a map from nominal font size in pixels to FontTexture
    std::map<unsigned int, FontTexture*> textures;

    void initCharmap() throw ()
    {
        glyphCount = 0;
        charmap.push_back(0);
        ++glyphCount;
        FT_UInt index;
        FT_ULong ucode = FT_Get_First_Char(face, &index);
        while (index) {
            charmap.push_back(ucode);
            ++glyphCount;
            ucode = FT_Get_Next_Char(face, ucode, &index);
        }
    }

public:
    FontFace(FontManager* manager, const std::string fontFilename, long index = 0) try :
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

    ~FontFace();

    /** Gets the font texture object of the specified font size.
     * @param point nominal font size in pixels
     * @return the font texture object, or zero upon failure
     */
    FontTexture* getFontTexture(unsigned int point);
};

class FontTexture
{
    FontFace* face;
    FontGlyph* glyphs;
    unsigned int point; // nominal font point sized
    short ascender;
    short descender;
    short xHeight;

    std::vector<uint8_t*> images;
    uint8_t* image;  // intensity texture image of Width x Height texels
    FT_Vector pen;
    unsigned ymax;

    // Allocates a new texture plane
    void addImage()
    {
        image = new uint8_t[Width * Height];
        images.push_back(image);
        FontManagerBackEnd* backend = face->manager->getBackEnd();
        backend->addImage(image);
    }

    // Releases the texture plane
    void deleteImage(uint8_t* image)
    {
        FontManagerBackEnd* backend = face->manager->getBackEnd();
        backend->deleteImage(image);
        delete[] image;
    }

    // Updates texture sub image
    void updateImage(uint8_t* image, FontGlyph* glyph, FT_GlyphSlot slot)
    {
        FontManagerBackEnd* backend = face->manager->getBackEnd();
        backend->updateImage(image, glyph, slot);
    }

    bool storeGlyph(FontGlyph* glyph, FT_UInt glyphIndex);
    void drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot);

public:
    FontTexture(FontFace* face, unsigned int point);
    ~FontTexture();

    /** Gets the font glyph object of the specified Unicode character.
     * @param ucode Unicode character number
     * @return the font glyph object
     */
    FontGlyph* getGlyph(int32_t ucode);
    uint8_t* getImage(FontGlyph* glyph);

    unsigned getPoint() const {
        return point;
    }

    FontFace* getFace() const {
        return face;
    }

    float measureText(const char16_t* text, float point);
    size_t fitText(const char16_t* text, size_t length, float point, float& leftover, size_t* next = 0, float* required = 0);
    size_t fitTextWithTransform(std::u16string& text, size_t length, float point, unsigned transform, float& leftover, size_t* next = 0, float* required = 0);
    std::u16string fitTextWithTransformation(const char16_t* text, size_t length, float point, unsigned transform,
                                             float& leftover,
                                             size_t* lenght, size_t* transformedLength,
                                             size_t* next = 0, float* required = 0);

    void renderText(const char16_t* text, size_t length) {
        face->manager->getBackEnd()->renderText(this, text, length);
    }

    float getHeight(float point) const {
        return point / 72 * 96;  // TODO XXX DPI
    }

    float getXHeight(float point) const {
        return (getHeight(point) * xHeight) / (ascender - descender);
    }

    float getAscender(float point) const {
        return (getHeight(point) * ascender) / (ascender - descender);
    }

    float getUnderlinePosition(float point) const {
        return (getHeight(point) * -face->face->underline_position) / (ascender - descender);
    }

    float getUnderlineThickness(float point) const {
        return (getHeight(point) * face->face->underline_thickness) / (ascender - descender);
    }

    static const int Width = 1024;
    static const int Height = 1024;
    static const int Offset = 1;
};

struct FontGlyph
{
    short advance;
    unsigned short x; // pos
    unsigned y;       // pos
    short left;
    short top;
    unsigned short width;
    unsigned short height;

    FontGlyph() :
        advance(0),
        x(0),
        y(0),
        left(0),
        top(0),
        width(0),
        height(0)
    {
    }

    bool isInitialized()
    {
        return x || y;
    }
};

#endif // ES_FONTMANAGER_H
