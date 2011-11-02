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

#ifndef ES_FONTMANAGERBACKENDGL_H
#define ES_FONTMANAGERBACKENDGL_H

#include "FontManager.h"

#include <GL/glut.h>

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

    void addImage(uint8_t* image)
    {
        GLuint texname;

        glGenTextures(1, &texname);
        bindTexture(texname);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY4, FontTexture::Width, FontTexture::Height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
        texnames.insert(std::pair<uint8_t*, GLuint>(image, texname));
    }

    void deleteImage(uint8_t* image)
    {
        std::map<uint8_t*, GLuint>::iterator it = texnames.find(image);
        assert(it != texnames.end());
        glDeleteTextures(1, &it->second);
        texnames.erase(it);
    }

    void updateImage(uint8_t* image, FontGlyph* glyph, FT_GlyphSlot slot)
    {
        bindImage(image);
        glTexSubImage2D(GL_TEXTURE_2D, 0, glyph->x, glyph->y % FontTexture::Height,
                        glyph->width, glyph->height,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
    }

    FontFace* getFontFace(const char* fontFilename) throw ()
    {
        face = getFontManager()->getFontFace(fontFilename);
        return face;
    }

    FontTexture* getFontTexture(unsigned int px)
    {
        if (!face)
            return 0;
        fontTexture = face->getFontTexture(px);
        return fontTexture;
    }

    void renderString(const char* string)
    {
        char32_t u;

        if (!fontTexture)
            return;
        setMatrixMode();
        while ((string = utf8to32(string, &u)) && u) {
            FontGlyph* glyph = fontTexture->getGlyph(u);
            uint8_t* image = fontTexture->getImage(glyph);
            bindImage(image);
            unsigned y = glyph->y % FontTexture::Height;
            glTranslatef(glyph->left / 64.f, -(glyph->top - fontTexture->getBearingGap()) / 64.f, 0.0);
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
            glTranslatef((-glyph->left + glyph->advance) / 64.0, (glyph->top - fontTexture->getBearingGap()) / 64.f, 0.0);
        }
    }

    void renderText(FontTexture* fontTexture, const char16_t* text, size_t length)
    {
        setMatrixMode();
        const char16_t* last = text + length;
        while (text < last) {
            char32_t u;
            text = utf16to32(text, &u);
            FontGlyph* glyph = fontTexture->getGlyph(u);
            uint8_t* image = fontTexture->getImage(glyph);
            bindImage(image);
            unsigned y = glyph->y % FontTexture::Height;
            glTranslatef(glyph->left / 64.f, -(glyph->top - fontTexture->getBearingGap()) / 64.f, 0.0);
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
            glTranslatef((-glyph->left + glyph->advance) / 64.0, (glyph->top - fontTexture->getBearingGap()) / 64.f, 0.0);
        }
    }
};

#endif // ES_FONTMANAGERBACKENDGL_H
