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

#include "Box.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>

#include "CSSStyleDeclarationImp.h"
#include "ViewCSSImp.h"

#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

std::map<uint8_t*, GLuint> texnames;

GLuint addImage(uint8_t* image, unsigned width, unsigned heigth, unsigned repeat, GLenum format)
{
    GLuint texname;

    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat & 1) ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat & 2) ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, heigth, 0, format, GL_UNSIGNED_BYTE, image);
    texnames.insert(std::pair<uint8_t*, GLuint>(image, texname));
    return texname;
}

GLuint getTexname(uint8_t* image, unsigned width, unsigned height, unsigned repeat, GLenum format)
{
    std::map<uint8_t*, GLuint>::iterator it = texnames.find(image);
    if (it == texnames.end())
        return addImage(image, width, height, repeat, format);
    return it->second;
}

void deleteImage(uint8_t* image)
{
    std::map<uint8_t*, GLuint>::iterator it = texnames.find(image);
    assert(it != texnames.end());
    glDeleteTextures(1, &it->second);
    texnames.erase(it);
}

}  // namespace

void BoxImage::render(ViewCSSImp* view, float x, float y, float width, float height)
{
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.0f / naturalWidth, 1.0f / naturalHeight, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    GLuint texname = getTexname(pixels, naturalWidth, naturalHeight, repeat, format);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname);
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
            glTexCoord2f(x, y);
            glVertex2f(x, y);
            glTexCoord2f(x + width, y);
            glVertex2f(x + width, y);
            glTexCoord2f(x + width, y + height);
            glVertex2f(x + width, y + height);
            glTexCoord2f(x, y + height);
            glVertex2f(x, y + height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Box::renderBorder(ViewCSSImp* view)
{
    glDisable(GL_TEXTURE_2D);

    float ll = marginLeft;
    float lr = ll + borderLeft;
    float rl = lr + paddingLeft + width + paddingRight;
    float rr = rl + borderRight;
    float tt = marginTop;
    float tb = tt + borderTop;
    float bt = tb + paddingTop + height + paddingBottom;
    float bb = bt + borderBottom;

    if (backgroundColor != 0x00000000) {
        glColor4ub(backgroundColor >> 16, backgroundColor >> 8, backgroundColor, backgroundColor >> 24);
        glBegin(GL_QUADS);
            glVertex2f(lr, tb);
            glVertex2f(rl, tb);
            glVertex2f(rl, bt);
            glVertex2f(lr, bt);
        glEnd();
    }

    if (backgroundImage) {
        glPushMatrix();
        glTranslatef(ll, tt, 0.0f);
        if (getParentBox())
            backgroundImage->render(view, 0, 0, rr - ll, bb - tt);
        else {
            const ContainingBlock* containingBlock = getContainingBlock(view);
            backgroundImage->render(view, -ll, -tt, containingBlock->width, containingBlock->height);
        }
        glPopMatrix();
    }

    // assume solid for now
    unsigned color;
    if (borderTop) {
        color = style->borderTopColor.getARGB();
        glColor4ub(color >> 16, color >> 8, color, color >> 24);
        glBegin(GL_QUADS);
            glVertex2f(ll, tt);
            glVertex2f(rr, tt);
            glVertex2f(rl, tb);
            glVertex2f(lr, tb);
        glEnd();
    }
    if (borderRight) {
        color = style->borderRightColor.getARGB();
        glColor4ub(color >> 16, color >> 8, color, color >> 24);
        glBegin(GL_QUADS);
            glVertex2f(rr, tt);
            glVertex2f(rr, bb);
            glVertex2f(rl, bt);
            glVertex2f(rl, tb);
        glEnd();
    }
    if (borderBottom) {
        color = style->borderBottomColor.getARGB();
        glColor4ub(color >> 16, color >> 8, color, color >> 24);
        glBegin(GL_QUADS);
            glVertex2f(rr, bb);
            glVertex2f(ll, bb);
            glVertex2f(lr, bt);
            glVertex2f(rl, bt);
        glEnd();
    }
    if (borderLeft) {
        color = style->borderLeftColor.getARGB();
        glColor4ub(color >> 16, color >> 8, color, color >> 24);
        glBegin(GL_QUADS);
            glVertex2f(ll, bb);
            glVertex2f(ll, tt);
            glVertex2f(lr, tb);
            glVertex2f(lr, bt);
        glEnd();
    }
    glEnable(GL_TEXTURE_2D);
}

void BlockLevelBox::render(ViewCSSImp* view)
{
    glPushMatrix();
    glTranslatef(offsetH, offsetV, 0.0f);
    renderBorder(view);
    glTranslatef(marginLeft + paddingLeft + borderLeft,
                 marginTop + paddingTop + borderTop,
                 0.0f);
    if (shadow)
        shadow->render();
    else {
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            child->render(view);
            glTranslatef(0.0f,
                        child->marginTop + child->borderTop + child->paddingTop +
                        child->height +
                        child->marginBottom + child->borderBottom + child->paddingBottom , 0.0f);
        }
    }
    glPopMatrix();
}

void LineBox::render(ViewCSSImp* view)
{
    glPushMatrix();
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        child->render(view);
        if (!child->isAbsolutelyPositioned()) {
            glTranslatef(child->marginLeft + child->borderLeft + child->paddingLeft +
                         child->width +
                         child->marginRight + child->borderRight + child->paddingRight,
                         0.0f, 0.0f);
        }
    }
    glPopMatrix();
}

void InlineLevelBox::render(ViewCSSImp* view)
{
    glPushMatrix();
    glTranslatef(offsetH, offsetV, 0.0f);
    renderBorder(view);
    glTranslatef(marginLeft + paddingLeft + borderLeft,
                 marginTop + paddingTop + borderTop,
                 0.0f);
    if (shadow)
        shadow->render();
    else if (getFirstChild())  // for inline-block
        getFirstChild()->render(view);
    else if (0 < data.length()) {
        glTranslatef(0.0f, baseline, 0.0f);
        glScalef(point / font->getPoint(), point / font->getPoint(), 1.0);
        unsigned color = getStyle()->color.getARGB();
        glColor4ub(color >> 16, color >> 8, color, color >> 24);
        font->renderText(data.c_str(), data.length());
    }
    glPopMatrix();
}

}}}}  // org::w3c::dom::bootstrap
