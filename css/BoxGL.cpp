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
        switch (edge) {
        case LEFT:
        case RIGHT:
            glLineWidth(fabsf(g - a));
            glLineStipple(fabsf(g - a), (borderStyle == CSSBorderStyleValueImp::Dotted) ? 0xaaaa : 0xcccc);
            break;;
        case TOP:
        case BOTTOM:
            glLineWidth(fabsf(h - b));
            glLineStipple(fabsf(h - b), (borderStyle == CSSBorderStyleValueImp::Dotted) ? 0xaaaa : 0xcccc);
            break;;
        }
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

void Box::renderBorder(ViewCSSImp* view, float left, float top)
{
    glPushMatrix();
    glTranslatef(left, top, 0.0f);
    glDisable(GL_TEXTURE_2D);

    float ll = marginLeft;
    float lr = ll + borderLeft;
    float rl = lr + getPaddingWidth();
    float rr = rl + borderRight;
    float tt = marginTop;
    float tb = tt + borderTop;
    float bt = tb + getPaddingHeight();
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
                glVertex2f(-left, -top);
                glVertex2f(-left + view->getScrollWidth(), -top);
                glVertex2f(-left + view->getScrollWidth(), -top + containingBlock->height);
                glVertex2f(-left, -top + containingBlock->height);
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
            if (!style->backgroundAttachment.isFixed())
                backgroundImage->render(view, 0, 0, rl - lr, bt - tb, backgroundLeft, backgroundTop);
            else {
                float fixedX = left + lr - view->getWindow()->getScrollX();
                float fixedY = top + tb - view->getWindow()->getScrollY();
                backgroundImage->render(view, 0, 0, rl - lr, bt - tb, backgroundLeft - fixedX, backgroundTop - fixedY);
            }
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

void Box::renderVerticalScrollBar(float w, float h, float pos, float total)
{
    float overflow = total - h;
    if (0.0f < overflow) {
        float size = h * (h / total);
        pos *= (h - size) / (total - h);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(w, 0);
            glVertex2f(w, pos);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(w - 4, pos);
            glVertex2f(w - 4, 0);
        glEnd();
        glBegin(GL_TRIANGLES);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(w, pos);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(w, pos + 4);
            glVertex2f(w - 4, pos);
        glEnd();
        glBegin(GL_QUADS);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(w, pos + size);
            glVertex2f(w, h);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(w - 4, h);
            glVertex2f(w - 4, pos + size);
        glEnd();
        glBegin(GL_TRIANGLES);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(w, pos + size);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(w, pos + size - 4);
            glVertex2f(w - 4, pos + size);
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
}

void Box::renderHorizontalScrollBar(float w, float h, float pos, float total)
{
    float overflow = total - w;
    if (0.0f < overflow) {
        float size = w * (w / total);
        pos *= (w - size) / (total - w);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(0, h);
            glVertex2f(pos, h);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(pos, h - 4);
            glVertex2f(0, h - 4);
        glEnd();
        glBegin(GL_TRIANGLES);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(pos, h);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(pos + 4, h);
            glVertex2f(pos, h - 4);
        glEnd();
        glBegin(GL_QUADS);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(pos + size, h);
            glVertex2f(w, h);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(w, h - 4);
            glVertex2f(pos + size, h - 4);
        glEnd();
        glBegin(GL_TRIANGLES);
            glColor4ub(0, 0, 0, 32);
            glVertex2f(pos + size, h);
            glColor4ub(0, 0, 0, 0);
            glVertex2f(pos + size - 4, h);
            glVertex2f(pos + size, h - 4);
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }
}

unsigned BlockLevelBox::renderBegin(ViewCSSImp* view)
{
    unsigned overflow = CSSOverflowValueImp::Visible;
    glPushMatrix();
    if (!isAnonymous() || isTableBox()) {
        float scrollX = 0.0f;
        float scrollY = 0.0f;
        if (style->position.isFixed() && style->parentStyle) {
            scrollX = view->getWindow()->getScrollX();
            scrollY = view->getWindow()->getScrollY();
            glTranslatef(scrollX, scrollY, 0.0f);
        }
        renderBorder(view, x, y);
        if (style->parentStyle) {
            overflow = style->overflow.getValue();
            if (overflow != CSSOverflowValueImp::Visible) {
                // TODO: Support the cumulative intersection.
                float left = x + marginLeft + borderLeft + scrollX;
                float top = y + marginTop + borderTop + scrollY;
                float w = getPaddingWidth();
                float h = getPaddingHeight();
                glViewport(left, view->getInitialContainingBlock()->getHeight() - (top + h), w, h);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(left, left + w, top + h, top, -1000.0, 1.0);
                glMatrixMode(GL_MODELVIEW);

                glPushMatrix();
                Element element = interface_cast<Element>(node);
                glTranslatef(-element.getScrollLeft(), -element.getScrollTop(), 0.0f);
            }
        }
        renderTableBorders(view);
    }
    return overflow;
}

void BlockLevelBox::renderEnd(ViewCSSImp* view, unsigned overflow)
{
    if (!isAnonymous() && style->parentStyle) {
        if (overflow != CSSOverflowValueImp::Visible) {
            glPopMatrix();

            // TODO: Support the cumulative intersection.
            float w = view->getInitialContainingBlock()->getWidth();
            float h = view->getInitialContainingBlock()->getHeight();
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, h, 0, -1000.0, 1.0);
            glMatrixMode(GL_MODELVIEW);

            if (overflow != CSSOverflowValueImp::Hidden) {
                glPushMatrix();
                    glTranslatef(x + marginLeft + borderLeft, y + marginTop + borderTop, 0.0f);
                    float w = getPaddingWidth();
                    float h = getPaddingHeight();
                    Element element = interface_cast<Element>(node);
                    float tw = 0.0f;
                    float th = 0.0f;
                    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
                        th += child->getTotalHeight() + child->getClearance();
                        tw = std::max(tw, child->getTotalWidth());
                    }
                    renderVerticalScrollBar(w, h, element.getScrollTop(), th);
                    renderHorizontalScrollBar(w, h, element.getScrollLeft(), tw);
                glPopMatrix();
            }
        }
    }
    glPopMatrix();
}

void BlockLevelBox::renderContent(ViewCSSImp* view, StackingContext* stackingContext)
{
    if (shadow) {
        shadow->render();
        return;
    }

    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (child->style && child->style->isPositioned() && !child->isAnonymous())
            continue;
        child->render(view, stackingContext);
    }
}

void BlockLevelBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    unsigned overflow = renderBegin(view);
    renderContent(view, stackingContext);
    renderEnd(view, overflow);
}

void LineBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (child->style) {
            if (child->style->isPositioned() && !child->isAnonymous())
                continue;
            if (child->style->isFloat()) {
                stackingContext->addFloat(child);
                continue;
            }
        }
        child->render(view, stackingContext);
    }
}

void InlineLevelBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    assert(stackingContext);
    if (font)
        renderBorder(view, x, y - getBlankTop());
    else
        renderBorder(view, x, y);
    if (shadow)
        shadow->render();
    else if (getFirstChild())  // for inline-block
        getFirstChild()->render(view, stackingContext);
    else if (font) {
        glPushMatrix();
            glTranslatef(x + getBlankLeft(), y + font->getAscender(point), 0.0f);
            LineBox* lineBox = dynamic_cast<LineBox*>(getParentBox());
            assert(lineBox);
            unsigned lineDecoration = getStyle()->textDecorationContext.decoration;
            if (lineDecoration & (CSSTextDecorationValueImp::Underline | CSSTextDecorationValueImp::Overline)) {
                glDisable(GL_TEXTURE_2D);
                glLineWidth(lineBox->getUnderlineThickness());
                unsigned color = getStyle()->textDecorationContext.color;
                glColor4ub(color >> 16, color >> 8, color, color >> 24);
                if (lineDecoration & CSSTextDecorationValueImp::Underline) {
                    glBegin(GL_LINES);
                        glVertex2f(0.0f, lineBox->getUnderlinePosition());
                        glVertex2f(getTotalWidth(), lineBox->getUnderlinePosition());
                    glEnd();
                }
                if (lineDecoration & CSSTextDecorationValueImp::Overline) {
                    glBegin(GL_LINES);
                        glVertex2f(0.0f, -lineBox->getBaseline());
                        glVertex2f(getTotalWidth(), -lineBox->getBaseline());
                    glEnd();
                }
                glEnable(GL_TEXTURE_2D);
            }
            glPushMatrix();
                glScalef(point / font->getPoint(), point / font->getPoint(), 1.0);
                unsigned color = getStyle()->color.getARGB();
                glColor4ub(color >> 16, color >> 8, color, color >> 24);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                renderText(view, data, point);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glPopMatrix();
            if (lineDecoration & CSSTextDecorationValueImp::LineThrough) {
                glDisable(GL_TEXTURE_2D);
                glLineWidth(lineBox->getLineThroughThickness());
                unsigned color = getStyle()->textDecorationContext.color;
                glColor4ub(color >> 16, color >> 8, color, color >> 24);
                glBegin(GL_LINES);
                    glVertex2f(0.0f, -lineBox->getLineThroughPosition());
                    glVertex2f(getTotalWidth(), -lineBox->getLineThroughPosition());
                glEnd();
                glEnable(GL_TEXTURE_2D);
            }
        glPopMatrix();
    }
}

void InlineLevelBox::renderText(ViewCSSImp* view, const std::u16string& data, float point)
{
    CSSStyleDeclarationImp* activeStyle = getStyle();
    FontTexture* font = activeStyle->getFontTexture();
    float letterSpacing = activeStyle->letterSpacing.getPx() * font->getPoint() / point;
    float wordSpacing = activeStyle->wordSpacing.getPx() * font->getPoint() / point;
    unsigned variant = activeStyle->fontVariant.getValue();
    font->beginRender();
    const char16_t* p = data.c_str();
    const char16_t* end = p + data.length();
    char32_t u;
    while (p < end && (p = utf16to32(p, &u)) && u) {
        if (u == '\n' || u == u'\u200B')
            continue;
        char32_t caps = u;
        if (variant == CSSFontVariantValueImp::SmallCaps)
            caps = u_toupper(u);
        FontTexture* currentFont = font;
        FontGlyph* glyph = font->getGlyph(caps);
        if (font->isMissingGlyph(glyph)) {
            FontTexture* altFont = currentFont;
            while (altFont = activeStyle->getAltFontTexture(view, altFont, caps)) {
                FontGlyph* altGlyph = altFont->getGlyph(caps);
                if (!altFont->isMissingGlyph(altGlyph)) {
                    glyph = altGlyph;
                    currentFont = altFont;
                    break;
                }
            }
        }
        if (caps == u) {
            currentFont->renderGlyph(glyph);
            glTranslatef((-glyph->left + glyph->advance) / 64.0f, (glyph->top - currentFont->getBearingGap()) / 64.0f, 0.0f);
        } else {
            glPushMatrix();
            glScalef(currentFont->getSmallCapsScale(), currentFont->getSmallCapsScale(), 1.0);
            currentFont->renderGlyph(glyph);
            glPopMatrix();
            glTranslatef(glyph->advance / 64.0f * currentFont->getSmallCapsScale(), 0.0f, 0.0f);
        }
        float spacing = 0.0f;
        if (u == ' ' || u == u'\u00A0')  // SP or NBSP
            spacing += wordSpacing;
        spacing += letterSpacing;
        if (spacing != 0.0f)
            glTranslatef(spacing, 0.0f, 0.0f);
    }
    font->endRender();
}


}}}}  // org::w3c::dom::bootstrap
