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

#include "StackingContext.h"
#include "CSSStyleDeclarationImp.h"
#include "ViewCSSImp.h"

#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

enum
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

std::map<uint8_t*, GLuint> texnames;

GLuint addImage(uint8_t* image, unsigned width, unsigned heigth, unsigned repeat, GLenum format)
{
    GLuint texname;

    glGenTextures(1, &texname);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat & BoxImage::RepeatS) ? GL_REPEAT : (repeat & BoxImage::Clamp) ? GL_CLAMP_TO_EDGE : GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat & BoxImage::RepeatT) ? GL_REPEAT : (repeat & BoxImage::Clamp) ? GL_CLAMP_TO_EDGE : GL_CLAMP_TO_BORDER);
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
    if (it != texnames.end()) {
        glDeleteTextures(1, &it->second);
        texnames.erase(it);
    }
}

void getOriginScreenPosition(float& x, float& y)
{
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    x = m[12];
    y = m[13];
}

}  // namespace

BoxImage::~BoxImage()
{
    if (state == CompletelyAvailable)
        deleteImage(pixels);
    delete pixels;
}

void BoxImage::render(ViewCSSImp* view, float x, float y, float width, float height, float left, float top)
{
    if (state != CompletelyAvailable)
        return;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    if (repeat & BoxImage::Clamp)
        glScalef(1.0f / width, 1.0f / height, 0.0f);
    else
        glScalef(1.0f / naturalWidth, 1.0f / naturalHeight, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    GLuint texname = getTexname(pixels, naturalWidth, naturalHeight, repeat, format);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texname);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(x - left, y - top);
        glVertex2f(x, y);
        glTexCoord2f(x - left + width, y - top);
        glVertex2f(x + width, y);
        glTexCoord2f(x - left + width, y - top + height);
        glVertex2f(x + width, y + height);
        glTexCoord2f(x - left, y - top + height);
        glVertex2f(x, y + height);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Box::renderBorderEdge(ViewCSSImp* view, int edge, unsigned borderStyle, unsigned color,
                           float a, float b, float c, float d,
                           float e, float f, float g, float h)
{
    if (borderStyle == CSSBorderStyleValueImp::None ||
        borderStyle == CSSBorderStyleValueImp::Hidden)
        return;

    if (color == 0x00000000)
        return;

    GLubyte alpha = color >> 24;
    GLubyte red = color >> 16;
    GLubyte green = color >> 8;
    GLubyte blue = color;

    if (borderStyle == CSSBorderStyleValueImp::Double) {
        float offset;
        offset = (g - a) / 3;
        float i = a + offset;
        float m = g - offset;
        offset = (h - b) / 3;
        float j = b + offset;
        float n = h - offset;
        offset = (c - e) / 3;
        float k = c - offset;
        float o = e + offset;
        offset = (f - d) / 3;
        float l = d + offset;
        float p = f - offset;
        glColor4ub(red, green, blue, alpha);
        glBegin(GL_QUADS);
            glVertex2f(a, b);
            glVertex2f(c, d);
            glVertex2f(k, l);
            glVertex2f(i, j);
        glEnd();
        glBegin(GL_QUADS);
            glVertex2f(m, n);
            glVertex2f(o, p);
            glVertex2f(e, f);
            glVertex2f(g, h);
        glEnd();
        return;
    }

    if (borderStyle == CSSBorderStyleValueImp::Dotted ||
        borderStyle == CSSBorderStyleValueImp::Dashed) {
        glEnable(GL_LINE_STIPPLE);
        glLineWidth(fabsf(g - a));
        glLineStipple(fabsf(g - a), (borderStyle == CSSBorderStyleValueImp::Dotted) ? 0xaaaa : 0xcccc);
        glColor4ub(red, green, blue, alpha);
        glBegin(GL_LINES);
            glVertex2f((a + g) / 2, (b + h) / 2);
            glVertex2f((c + e) / 2, (d + f) / 2);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
        return;
    }

    if (borderStyle == CSSBorderStyleValueImp::Groove ||
        borderStyle == CSSBorderStyleValueImp::Ridge) {
        GLubyte redDark = std::max(red - 32, 0);
        GLubyte greenDark = std::max(green - 32, 0);
        GLubyte blueDark = std::max(blue - 32, 0);
        GLubyte redBright = std::min(red + 32, 255);
        GLubyte greenBright = std::min(green + 32, 255);
        GLubyte blueBright = std::min(blue + 32, 255);
        float offset;
        offset = (g - a) / 2;
        float i = a + offset;
        offset = (h - b) / 2;
        float j = b + offset;
        offset = (c - e) / 2;
        float k = c - offset;
        offset = (f - d) / 2;
        float l = d + offset;
        if (borderStyle == CSSBorderStyleValueImp::Groove)
            glColor4ub(redDark, greenDark, blueDark, alpha);
        else
            glColor4ub(redBright, greenBright, blueBright, alpha);
        glBegin(GL_QUADS);
            glVertex2f(a, b);
            glVertex2f(c, d);
            glVertex2f(k, l);
            glVertex2f(i, j);
        glEnd();
        if (borderStyle == CSSBorderStyleValueImp::Groove)
            glColor4ub(redBright, greenBright, blueBright, alpha);
        else
            glColor4ub(redDark, greenDark, blueDark, alpha);
        glBegin(GL_QUADS);
            glVertex2f(i, j);
            glVertex2f(k, l);
            glVertex2f(e, f);
            glVertex2f(g, h);
        glEnd();
        return;
    }

    if (borderStyle == CSSBorderStyleValueImp::Inset ||
        borderStyle == CSSBorderStyleValueImp::Outset) {
        bool dark = false;
        bool bright = false;
        if (borderStyle == CSSBorderStyleValueImp::Inset) {
            switch (edge) {
            case TOP:
            case LEFT:
                dark = true;
                break;;
            case RIGHT:
            case BOTTOM:
                bright = true;
                break;;
            }
        }
        if (borderStyle == CSSBorderStyleValueImp::Outset) {
            switch (edge) {
            case TOP:
            case LEFT:
                bright = true;
                break;;
            case RIGHT:
            case BOTTOM:
                dark = true;
                break;;
            }
        }
        if (dark) {
            red = std::max(red - 32, 0);
            green = std::max(green - 32, 0);
            blue = std::max(blue - 32, 0);
        } else if (bright) {
            red = std::min(red + 32, 255);
            green = std::min(green + 32, 255);
            blue = std::min(blue + 32, 255);
        }
    }

    glColor4ub(red, green, blue, alpha);
    glBegin(GL_QUADS);
        glVertex2f(a, b);
        glVertex2f(c, d);
        glVertex2f(e, f);
        glVertex2f(g, h);
    glEnd();
}

void Box::renderBorder(ViewCSSImp* view)
{
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, positioned ? stackingContext->getZ1() : stackingContext->getZ3());

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
            if (getParentBox()) {
                glVertex2f(lr, tb);
                glVertex2f(rl, tb);
                glVertex2f(rl, bt);
                glVertex2f(lr, bt);
            } else {
                const ContainingBlock* containingBlock = getContainingBlock(view);
                glVertex2f(0, 0);
                glVertex2f(containingBlock->width, 0);
                glVertex2f(containingBlock->width, containingBlock->height);
                glVertex2f(0, containingBlock->height);
            }
        glEnd();
    }

    if (backgroundImage && backgroundImage->getState() == BoxImage::CompletelyAvailable) {
        GLfloat border[] = { ((backgroundColor >> 16) & 0xff) / 255.0f,
                             ((backgroundColor >> 8) & 0xff) / 255.0f,
                             ((backgroundColor) & 0xff) / 255.0f,
                             ((backgroundColor >> 24) & 0xff) / 255.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
        glPushMatrix();
        if (getParentBox()) {
            // TODO: check padding
            glTranslatef(lr, tb, 0.0f);
            backgroundImage->render(view, 0, 0, rl - lr, bt - tb, backgroundLeft, backgroundTop);
        } else {
            const ContainingBlock* containingBlock = getContainingBlock(view);
            glTranslatef(ll, tt, 0.0f);
            backgroundImage->render(view, -ll, -tt, containingBlock->width, containingBlock->height, backgroundLeft, backgroundTop);
        }
        glPopMatrix();
    }

    if (borderTop)
        renderBorderEdge(view, TOP,
                         style->borderTopStyle.getValue(),
                         style->borderTopColor.getARGB(),
                         ll, tt, rr, tt, rl, tb, lr, tb);
    if (borderRight)
        renderBorderEdge(view, RIGHT,
                         style->borderRightStyle.getValue(),
                         style->borderRightColor.getARGB(),
                         rl, bt, rl, tb, rr, tt, rr, bb);
    if (borderBottom)
        renderBorderEdge(view, BOTTOM,
                         style->borderBottomStyle.getValue(),
                         style->borderBottomColor.getARGB(),
                         lr, bt, rl, bt, rr, bb, ll, bb);
    if (borderLeft)
        renderBorderEdge(view, LEFT,
                         style->borderLeftStyle.getValue(),
                         style->borderLeftColor.getARGB(),
                         ll, bb, ll, tt, lr, tb, lr, bt);

    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void BlockLevelBox::render(ViewCSSImp* view)
{
    if (!stackingContext)
        return;
    glPushMatrix();
    if (!getParentBox())
        glTranslatef(-view->getWindow()->getScrollX(), -view->getWindow()->getScrollY(), 0.0f);
    glTranslatef(offsetH, offsetV, 0.0f);
    getOriginScreenPosition(x, y);

    renderBorder(view);
    glTranslatef(getBlankLeft(), getBlankTop(), 0.0f);

    unsigned overflow = CSSOverflowValueImp::Visible;
    if (style)
        overflow = style->overflow.getValue();
    if (overflow == CSSOverflowValueImp::Hidden) {
        float left = x + getBlankLeft();
        float top = y + getBlankTop();
        float h = view->getInitialContainingBlock()->getHeight();
        glViewport(left, h - (top + height), width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(left, left + width, top + height, top, -1000.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }
    if (shadow)
        shadow->render();
    else {
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            child->render(view);
            glTranslatef(0.0f, child->getTotalHeight(), 0.0f);
        }
    }
    if (overflow == CSSOverflowValueImp::Hidden) {
        float w = view->getInitialContainingBlock()->getWidth();
        float h = view->getInitialContainingBlock()->getHeight();
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1000.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }

    glPopMatrix();
}

void LineBox::render(ViewCSSImp* view)
{
    glPushMatrix();
    glTranslatef(offsetH, offsetV, 0.0f);
    getOriginScreenPosition(x, y);
    glTranslatef(getBlankLeft(), getBlankTop(), 0.0f);  // Node floats are placed inside margins.
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        child->render(view);
        if (!child->isAbsolutelyPositioned())
            glTranslatef(child->getTotalWidth(), 0.0f, 0.0f);
    }
    glPopMatrix();
}

void InlineLevelBox::render(ViewCSSImp* view)
{
    assert(stackingContext);
    glPushMatrix();
    glTranslatef(offsetH, offsetV, 0.0f);
    if (0 < data.length())
        glTranslatef(0, baseline - font->getAscender(point) - getBlankTop(), 0.0f);
    renderBorder(view);
    glTranslatef(getBlankLeft(), getBlankTop(), 0.0f);
    getOriginScreenPosition(x, y);
    if (shadow)
        shadow->render();
    else if (getFirstChild())  // for inline-block
        getFirstChild()->render(view);
    else if (0 < data.length()) {
        glTranslatef(0.0f, font->getAscender(point), stackingContext->getZ3());
        glPushMatrix();
            glScalef(point / font->getPoint(), point / font->getPoint(), 1.0);
            unsigned color = getStyle()->color.getARGB();
            glColor4ub(color >> 16, color >> 8, color, color >> 24);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            font->renderText(data.c_str(), data.length());
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPopMatrix();
        if (getStyle()->textDecorationContext.hasDecoration()) {
            unsigned lineDecoration = getStyle()->textDecorationContext.decoration;
            glDisable(GL_TEXTURE_2D);
            LineBox* lineBox = dynamic_cast<LineBox*>(getParentBox());
            assert(lineBox);
            glLineWidth(lineBox->getUnderlineThickness());
            unsigned color = getStyle()->textDecorationContext.color;
            glColor4ub(color >> 16, color >> 8, color, color >> 24);
            if (lineDecoration & CSSTextDecorationValueImp::Underline) {
                glBegin(GL_LINES);
                    glVertex2f(0.0f, lineBox->getUnderlinePosition());
                    glVertex2f(getTotalWidth(), lineBox->getUnderlinePosition());
                glEnd();
            }
            glEnable(GL_TEXTURE_2D);
        }
    }
    glPopMatrix();
}

}}}}  // org::w3c::dom::bootstrap
