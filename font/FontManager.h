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
    {}

    virtual void addImage(uint8_t* image) = 0;
    virtual void deleteImage(uint8_t* image) = 0;
    virtual void updateImage(uint8_t* image, FontGlyph* glyph) = 0;
    virtual void renderText(FontTexture* font, const char16_t* text, size_t length, float letterSpacing, float wordSpacing) = 0;

    virtual void beginRender() = 0;
    virtual void renderGlyph(FontTexture* fontTexture, FontGlyph* glyph) = 0;
    virtual void endRender() = 0;
};

class FontManager
{
    friend class FontFace;

    FontManagerBackEnd* backend;
    FT_Library library;
    // a map from font filename to FontFace
    std::map<const char*, FontFace*> faces;

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
    FontFace* getFontFace(const char* fontFilename) throw ();

    FontManagerBackEnd* getBackEnd() const
    {
        return backend;
    }
};

class FontFace
{
    friend class FontTexture;

    FontManager* manager;

    const char* filename;
    std::vector<int32_t> charmap;
    int32_t glyphCount;
    FT_Face face;
    // a map from nominal font size in pixels to FontTexture
    std::multimap<unsigned int, FontTexture*> textures;

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
    FontFace(FontManager* manager, const char* filename, long index = 0);
    ~FontFace();

    const char* getFilename() const {
        return filename;
    }
    FontManager* getManager() const {
        return manager;
    }
    FontManagerBackEnd* getBackEnd() const {
        return manager->getBackEnd();
    }

    bool hasGlyph(char32_t u) const;

    /** Gets the font texture object of the specified font size.
     * @param point nominal font size in pixels
     * @return the font texture object, or zero upon failure
     */
    FontTexture* getFontTexture(unsigned int point, bool bold, bool oblique);
};

class FontTexture
{
    static const size_t Sizes = 3;  // for 11px, 22px, 44px, etc.

    FontFace* face;
    FontGlyph* glyphs;
    FT_Size sizes[Sizes];
    unsigned int point; // nominal font point sized
    short ascender;
    short descender;
    short lineGap;
    short xHeight;
    short lineThroughPosition;
    short lineThroughSize;

    bool bold;
    bool oblique;

    std::vector<uint8_t*> images;
    FT_Vector pen;
    unsigned ymax;
    float bearingGap;

    // Allocates a new texture plane
    uint8_t* addImage()
    {
        uint8_t* image = new uint8_t[Width * (Height + Height / 3 + 1)];
        // Way small font glyphs are rendered as gray boxes.
        memset(getMipmapImage(image, Sizes), 0x20, Width * (Height + Height / 3 + 1) - ((getMipmapImage(image, Sizes) - image)));
        images.push_back(image);
        FontManagerBackEnd* backend = face->getBackEnd();
        backend->addImage(image);
        return image;
    }

    // Releases the texture plane
    void deleteImage(uint8_t* image)
    {
        FontManagerBackEnd* backend = face->getBackEnd();
        backend->deleteImage(image);
        delete[] image;
    }

    // Updates texture sub image
    void updateImage(uint8_t* image, FontGlyph* glyph)
    {
        FontManagerBackEnd* backend = face->getBackEnd();
        backend->updateImage(image, glyph);
    }

    bool storeGlyph(FontGlyph* glyph, FT_UInt glyphIndex);
    uint8_t* drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot);
    void drawBitmap(FontGlyph* glyph, FT_GlyphSlot slot, int level);

public:
    FontTexture(FontFace* face, unsigned int point, bool bold = false, bool oblique = false);
    ~FontTexture();

    /** Gets the font glyph object of the specified Unicode character.
     * @param ucode Unicode character number
     * @return the font glyph object
     */
    FontGlyph* getGlyph(int32_t ucode);
    uint8_t* getImage(FontGlyph* glyph);
    bool isMissingGlyph(const FontGlyph* glyph) const {
        return glyph == glyphs;
    }

    unsigned getPoint() const {
        return point;
    }
    bool getBold() const {
        return bold;
    }
    bool getOblique() const {
        return oblique;
    }

    FontFace* getFace() const {
        return face;
    }

    float getScale(float point) const {
        return point / this->point / 64.0f;
    }

    float measureText(const char16_t* text, float point);
    float measureText(const char16_t* text, size_t length, float point,
                      unsigned transform, bool isFirstCharacter,
                      float letterSpacing, float wordSpacing,
                      FontGlyph*& glyph, std::u16string& transformed);

    void renderText(const char16_t* text, size_t length, float letterSpacing = 0.0f, float wordSpacing = 0.0f) {
        face->getBackEnd()->renderText(this, text, length, letterSpacing, wordSpacing);
    }

    void beginRender() {
        face->getBackEnd()->beginRender();
    }
    void renderGlyph(FontGlyph* glyph) {
        face->getBackEnd()->renderGlyph(this, glyph);
    }
    void endRender() {
        face->getBackEnd()->endRender();
    }

    float getSize(float point) const {
        return point / 72 * 96;  // TODO XXX DPI
    }

    float getLineGap(float point) const {
        return (getSize(point) * lineGap) / (ascender - descender);
    }

    float getXHeight(float point) const {
        return (getSize(point) * xHeight) / (ascender - descender);
    }

    float getAscender(float point) const {
        return (getSize(point) * ascender) / (ascender - descender);
    }

    float getDescender(float point) const {
        return (getSize(point) * descender) / (ascender - descender);
    }

    float getLineHeight(float point) const {
        return (getSize(point) * (ascender - descender + lineGap)) / (ascender - descender);
    }

    float getUnderlinePosition(float point) const {
        return (getSize(point) * -face->face->underline_position) / (ascender - descender);
    }
    float getUnderlineThickness(float point) const {
        return (getSize(point) * face->face->underline_thickness) / (ascender - descender);
    }

    float getLineThroughPosition(float point) const{
        return (getSize(point) * lineThroughPosition) / (ascender - descender);
    }

    float getLineThroughThickness(float point) const{
        return (getSize(point) * lineThroughSize) / (ascender - descender);
    }

    float getBearingGap() const {
        return bearingGap;
    }

    static uint8_t* getMipmapImage(uint8_t* image, unsigned int level) {
        unsigned px = Width;
        while (0 < level--) {
            image += px * px;
            px >>= 1;
        }
        return image;
    }

    static const int Width = 1024;
    static const int Height = 1024;
    static const int Level = 11;    // 2^(Level-1) = Width = Height
    static const int Offset = 1 << Sizes;
    static const int Align = 1 << (Sizes - 1);
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
