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

#ifndef ES_FONTMANAGERBACKENDGL_H
#define ES_FONTMANAGERBACKENDGL_H

#include "FontManager.h"

#include <GL/glut.h>
#include <unicode/uchar.h>

#include "utf.h"

class FontManagerBackEndGL : public FontManagerBackEnd
{
    std::map<uint8_t*, GLuint> texnames;
    FontManager* fontManager;
    FontFace* face;
    FontTexture* fontTexture;

    void bindTexture(GLuint texname)
    {
        glBindTexture(GL_TEXTURE_2D, texname);
    }

    GLuint getTexname(uint8_t* image)
    {
        std::map<uint8_t*, GLuint>::iterator it = texnames.find(image);
        assert(it != texnames.end());
        return it->second;
    }

    void setMatrixMode()
    {
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glScalef(1.0f / FontTexture::Width, 1.0 / FontTexture::Height, 0.0);
        glMatrixMode(GL_MODELVIEW);
    }

    void addImage(uint8_t* image)
    {
        GLuint texname;

        glGenTextures(1, &texname);
        bindTexture(texname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        unsigned px = FontTexture::Width;
        for (int level = 0; level < FontTexture::Level; ++level) {
            glTexImage2D(GL_TEXTURE_2D, level, GL_INTENSITY4, px, px, 0,
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, FontTexture::getMipmapImage(image, level));
            px >>= 1;
        }
        texnames.insert(std::pair<uint8_t*, GLuint>(image, texname));
    }

    void updateImage(uint8_t* image, FontGlyph* glyph)
    {
        bindImage(image);
        unsigned x = glyph->x;
        unsigned y = glyph->y % FontTexture::Height;
        unsigned w = glyph->width;
        unsigned h = glyph->height;
        unsigned px = FontTexture::Width;
        for (int level = 0; level < FontTexture::Level; ++level) {
            uint8_t* p = FontTexture::getMipmapImage(image, level) + px * y + x;
            for (int i = 0; i < h; ++i, p += px) {
                glTexSubImage2D(GL_TEXTURE_2D, level, x, y + i, w, 1,
                                GL_LUMINANCE, GL_UNSIGNED_BYTE, p);
            }
            px >>= 1;
            x >>= 1;
            y >>= 1;
            w >>= 1;
            h >>= 1;
            if (w == 0 || h == 0)
                break;
        }
    }

    void deleteImage(uint8_t* image)
    {
        std::map<uint8_t*, GLuint>::iterator it = texnames.find(image);
        assert(it != texnames.end());
        glDeleteTextures(1, &it->second);
        texnames.erase(it);
    }

public:
    FontManagerBackEndGL() :
        fontManager(0),
        face(0),
        fontTexture(0)
    {
    }

    ~FontManagerBackEndGL()
    {
        for (std::map<uint8_t*, GLuint>::iterator it = texnames.begin(); it != texnames.end(); ++it)
            glDeleteTextures(1, &it->second);
        delete fontManager;
    }

    FontManager* getFontManager()
    {
        if (!fontManager)
            fontManager = new FontManager(this);
        return fontManager;
    }

    void bindImage(uint8_t* image)
    {
        GLuint texname = getTexname(image);
        bindTexture(texname);
    }

    FontFace* getFontFace(const std::u16string& familyName) throw ()
    {
        face = getFontManager()->getFontFace(familyName);
        return face;
    }

    FontTexture* getFontTexture(unsigned int px, bool bold = false, bool oblique = false)
    {
        if (!face)
            return 0;
        fontTexture = face->getFontTexture(px, bold, oblique);
        return fontTexture;
    }

    void renderString(const char* string)
    {
        char32_t u;

        if (!fontTexture)
            return;
        beginRender();
        while ((string = utf8to32(string, &u)) && u) {
            FontGlyph* glyph = fontTexture->getGlyph(u);
            uint8_t* image = fontTexture->getImage(glyph);
            bindImage(image);
            unsigned y = glyph->y % FontTexture::Height;
            glTranslatef(glyph->left / 64.0f, -(glyph->top - fontTexture->getBearingGap()) / 64.0f, 0.0f);
            glBegin(GL_QUADS);
                    glTexCoord2i(glyph->x, y);
                    glVertex2i(0, 0);
                    glTexCoord2i(glyph->x + glyph->width, y);
                    glVertex2i(glyph->width, 0);
                    glTexCoord2i(glyph->x + glyph->width, y + glyph->height);
                    glVertex2i(glyph->width, glyph->height);
                    glTexCoord2i(glyph->x, y + glyph->height);
                    glVertex2i(0, glyph->height);
            glEnd();
            glTranslatef((-glyph->left + glyph->advance) / 64.0f, (glyph->top - fontTexture->getBearingGap()) / 64.0f, 0.0f);
        }
        endRender();
    }

    void renderText(FontTexture* fontTexture, const char16_t* text, size_t length,
                    float letterSpacing, float wordSpacing)
    {
        beginRender();
        const char16_t* end = text + length;
        const char16_t* n;
        for (const char16_t* p = text; p < end; p = n) {
            char32_t u;
            n = utf16to32(p, &u);
            if (u != '\n' && u != u'\u200B') {
                FontGlyph* glyph = fontTexture->getGlyph(u);
                renderGlyph(fontTexture, glyph);
                float spacing = 0.0f;
                if (u == ' ' || u == u'\u00A0')  // SP or NBSP
                    spacing += wordSpacing;
                spacing += letterSpacing;
                glTranslatef((-glyph->left + glyph->advance) / 64.0f + spacing, (glyph->top - fontTexture->getBearingGap()) / 64.0f, 0.0f);
            }
        }
        endRender();
    }

    void beginRender()
    {
        // TODO: sync
        for (auto i = updateList.begin(); i != updateList.end(); ++i) {
            if (i->second == Add)
                addImage(i->first);
            else if (i->second == Delete)
                deleteImage(i->first);
            else
                updateImage(i->first, i->second);
        }
        clear();
        setMatrixMode();
    }

    void renderGlyph(FontTexture* fontTexture, FontGlyph* glyph)
    {
        uint8_t* image = fontTexture->getImage(glyph);
        bindImage(image);
        unsigned y = glyph->y % FontTexture::Height;
        glTranslatef(glyph->left / 64.0f, -(glyph->top - fontTexture->getBearingGap()) / 64.0f, 0.0f);
        glBegin(GL_QUADS);
                glTexCoord2i(glyph->x, y);
                glVertex2i(0, 0);
                glTexCoord2i(glyph->x + glyph->width, y);
                glVertex2i(glyph->width, 0);
                glTexCoord2i(glyph->x + glyph->width, y + glyph->height);
                glVertex2i(glyph->width, glyph->height);
                glTexCoord2i(glyph->x, y + glyph->height);
                glVertex2i(0, glyph->height);
        glEnd();
    }

    void endRender()
    {
    }
};

#endif // ES_FONTMANAGERBACKENDGL_H
