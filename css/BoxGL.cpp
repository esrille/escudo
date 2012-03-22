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
#include "Table.h"

#include "html/HTMLReplacedElementImp.h"

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
            break;
        case TOP:
        case BOTTOM:
            glLineWidth(fabsf(h - b));
            glLineStipple(fabsf(h - b), (borderStyle == CSSBorderStyleValueImp::Dotted) ? 0xaaaa : 0xcccc);
            break;
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
                break;
            case RIGHT:
            case BOTTOM:
                bright = true;
                break;
            }
        }
        if (borderStyle == CSSBorderStyleValueImp::Outset) {
            switch (edge) {
            case TOP:
            case LEFT:
                bright = true;
                break;
            case RIGHT:
            case BOTTOM:
                dark = true;
                break;
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

void Box::renderBorder(ViewCSSImp* view, float left, float top,
                       CSSStyleDeclarationImp* style, unsigned backgroundColor, BoxImage* backgroundImage,
                       float ll, float lr, float rl, float rr, float tt, float tb, float bt, float bb,
                       Box* leftEdge, Box* rightEdge)
{
    glPushMatrix();
    glTranslatef(left, top, 0.0f);
    glDisable(GL_TEXTURE_2D);

    if (backgroundColor) {
        glColor4ub(backgroundColor >> 16, backgroundColor >> 8, backgroundColor, backgroundColor >> 24);
        glBegin(GL_QUADS);
            if (getParentBox()) {
                glVertex2f(ll, tt);
                glVertex2f(rr, tt);
                glVertex2f(rr, bb);
                glVertex2f(ll, bb);
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
            glTranslatef(lr, tb, 0.0f);
            if (!style->backgroundAttachment.isFixed())
                backgroundImage->render(view, -borderLeft, -borderTop, rr - ll, bb - tt, backgroundLeft, backgroundTop);
            else {
                float fixedX = left + lr - view->getWindow()->getScrollX();
                float fixedY = top + tb - view->getWindow()->getScrollY();
                for (Element element = interface_cast<Element>(getNode()); element; element = element.getParentElement()) {
                    fixedX -= element.getScrollLeft();
                    fixedY -= element.getScrollTop();
                }
                backgroundImage->render(view, -borderLeft, -borderTop, rr - ll, bb - tt, backgroundLeft - fixedX, backgroundTop - fixedY);
            }
        } else {
            const ContainingBlock* containingBlock = getContainingBlock(view);
            glTranslatef(lr, tb, 0.0f);
            if (!style->backgroundAttachment.isFixed())
                backgroundImage->render(view, -lr, -tb, containingBlock->width, containingBlock->height, backgroundLeft, backgroundTop);
            else {
                float fixedX = left + lr - view->getWindow()->getScrollX();
                float fixedY = top + tb - view->getWindow()->getScrollY();
                backgroundImage->render(view,
                                        -lr + view->getWindow()->getScrollX(),
                                        -tb + view->getWindow()->getScrollY(),
                                        containingBlock->width + view->getWindow()->getScrollX(),
                                        containingBlock->height + view->getWindow()->getScrollY(),
                                        backgroundLeft - fixedX, backgroundTop - fixedY);
            }
        }
        glPopMatrix();
    }

    if (borderTop)
        renderBorderEdge(view, TOP,
                         style->borderTopStyle.getValue(),
                         style->borderTopColor.getARGB(),
                         ll, tt, rr, tt, rl, tb, lr, tb);
    if (rightEdge && rightEdge->borderRight)
        renderBorderEdge(view, RIGHT,
                         rightEdge->style->borderRightStyle.getValue(),
                         rightEdge->style->borderRightColor.getARGB(),
                         rl, bt, rl, tb, rr, tt, rr, bb);
    if (borderBottom)
        renderBorderEdge(view, BOTTOM,
                         style->borderBottomStyle.getValue(),
                         style->borderBottomColor.getARGB(),
                         lr, bt, rl, bt, rr, bb, ll, bb);
    if (leftEdge && leftEdge->borderLeft)
        renderBorderEdge(view, LEFT,
                         leftEdge->style->borderLeftStyle.getValue(),
                         leftEdge->style->borderLeftColor.getARGB(),
                         ll, bb, ll, tt, lr, tb, lr, bt);

    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Box::renderBorder(ViewCSSImp* view, float left, float top)
{
    float ll = marginLeft;
    float lr = ll + borderLeft;
    float rl = lr + getPaddingWidth();
    float rr = rl + borderRight;
    float tt = marginTop;
    float tb = tt + borderTop;
    float bt = tb + getPaddingHeight();
    float bb = bt + borderBottom;
    renderBorder(view, left, top, getStyle(), backgroundColor, backgroundImage, ll, lr, rl, rr, tt, tb, bt, bb, this, this);
}

void Box::renderOutline(ViewCSSImp* view, float left, float top, float right, float bottom, float outlineWidth, unsigned outline, unsigned color)
{
    glDisable(GL_TEXTURE_2D);

    float ll = left - outlineWidth;
    float lr = left;
    float rl = right;
    float rr = right + outlineWidth;
    float tt = top - outlineWidth;
    float tb = top;
    float bt = bottom;
    float bb = bottom + outlineWidth;

    // TODO: Support 'invert'.

    renderBorderEdge(view, TOP, outline, color,
                     ll, tt, rr, tt, rl, tb, lr, tb);
    renderBorderEdge(view, RIGHT, outline, color,
                     rl, bt, rl, tb, rr, tt, rr, bb);
    renderBorderEdge(view, BOTTOM, outline, color,
                     lr, bt, rl, bt, rr, bb, ll, bb);
    renderBorderEdge(view, LEFT, outline, color,
                     ll, bb, ll, tt, lr, tb, lr, bt);

    glEnable(GL_TEXTURE_2D);
}

void Box::renderOutline(ViewCSSImp* view, float left, float top)
{
    if (outlineWidth <= 0.0f)
        return;
    left += marginLeft + borderLeft;
    top += marginTop + borderTop;
    float right = left + getPaddingWidth();
    float bottom = top + getPaddingHeight();
    renderOutline(view, left, top, right, bottom, outlineWidth, style->outlineStyle.getValue(), style->outlineColor.getARGB());
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

unsigned BlockLevelBox::renderBegin(ViewCSSImp* view, bool noBorder)
{
    unsigned overflow = CSSOverflowValueImp::Visible;
    glPushMatrix();
    if (!isAnonymous() || isTableBox()) {
        float scrollX = 0.0f;
        float scrollY = 0.0f;
        if (isFixed() && style->parentStyle) {
            scrollX = view->getWindow()->getScrollX();
            scrollY = view->getWindow()->getScrollY();
            glTranslatef(scrollX, scrollY, 0.0f);
        }
        if (!noBorder && isVisible())
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
        if (!noBorder && isVisible() && isTableBox())
            dynamic_cast<TableWrapperBox*>(getParentBox())->renderTableBorders(view);
    }
    return overflow;
}

void BlockLevelBox::renderEnd(ViewCSSImp* view, unsigned overflow, bool scrollBar)
{
    if (!isAnonymous()) {
        if (style->parentStyle && overflow != CSSOverflowValueImp::Visible) {
            glPopMatrix();

            // TODO: Support the cumulative intersection.
            float w = view->getInitialContainingBlock()->getWidth();
            float h = view->getInitialContainingBlock()->getHeight();
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, h, 0, -1000.0, 1.0);
            glMatrixMode(GL_MODELVIEW);

            if (scrollBar && overflow != CSSOverflowValueImp::Hidden) {
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
                    if (isVisible()) {
                        renderVerticalScrollBar(w, h, element.getScrollTop(), th);
                        renderHorizontalScrollBar(w, h, element.getScrollLeft(), tw);
                    }
                glPopMatrix();
            }
        }
    }
    glPopMatrix();
}

void BlockLevelBox::renderNonInline(ViewCSSImp* view, StackingContext* stackingContext)
{
    if (shadow)
        return;
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (!child->isAnonymous() && child->isPositioned())
            continue;
        if (BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(child)) {
            unsigned overflow = block->renderBegin(view);
            block->renderNonInline(view, stackingContext);
            block->renderEnd(view, overflow, false);
        } else if (LineBox* lineBox = dynamic_cast<LineBox*>(child)) {
            for (auto box = lineBox->getFirstChild(); box; box = box->getNextSibling()) {
                if (box->style && box->style->isFloat())
                    stackingContext->addFloat(box);
                else if (CellBox* cellBox = dynamic_cast<CellBox*>(box))
                    cellBox->renderNonInline(view, stackingContext);
            }
        }
    }
}

void BlockLevelBox::renderInline(ViewCSSImp* view, StackingContext* stackingContext)
{
    if (shadow) {
        shadow->render();
        return;
    }

    if (HTMLReplacedElementImp* replaced = dynamic_cast<HTMLReplacedElementImp*>(getNode().self())) {
        if (BoxImage* image = replaced->getImage()) {
            if (isVisible()) {
                glPushMatrix();
                glTranslatef(x + getBlankLeft(), y + getBlankTop(), 0.0f);
                image->render(view, 0, 0, width, height, 0, 0);
                glPopMatrix();
                glEnable(GL_TEXTURE_2D);
            }
            return;
        }
    }

    bool hasOutline = false;
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (!child->isAnonymous() && child->isPositioned())
            continue;
        if (BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(child)) {
            unsigned overflow = block->renderBegin(view, true);
            block->renderInline(view, stackingContext);
            block->renderEnd(view, overflow);
            hasOutline |= (0.0f < block->getOutlineWidth());
        } else
            child->render(view, stackingContext);
    }

    if (hasOutline) {
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            if (!child->isAnonymous() && child->isPositioned())
                continue;
            if (BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(child)) {
                if (0.0f < block->getOutlineWidth()) {
                    unsigned overflow = block->renderBegin(view, true);
                    block->renderOutline(view, block->x, block->y + block->getTopBorderEdge());
                    block->renderEnd(view, overflow, false);
                }
            }
        }
    }

    if (isTableBox())
        dynamic_cast<TableWrapperBox*>(getParentBox())->renderTableOutlines(view);
}

void BlockLevelBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    unsigned overflow = renderBegin(view);
    renderNonInline(view, stackingContext);
    renderInline(view, stackingContext);
    renderEnd(view, overflow);
}

void LineBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (child->style && (child->style->isFloat() || !child->isAnonymous() && child->isPositioned()))
            continue;
        child->render(view, stackingContext);
    }
}

void InlineLevelBox::renderMultipleBackground(ViewCSSImp* view)
{
    Box* box;
    InlineLevelBox* head;
    InlineLevelBox* tail = this;
    InlineLevelBox* lastBox = dynamic_cast<InlineLevelBox*>(style->getLastBox());
    assert(lastBox);
    for (box = this; box && box != lastBox; box = box->getNextSibling()) {
        if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box))
            tail = i;
    }

    float ll = marginLeft;
    float lr = ll + borderLeft;
    float rl;
    float rr;
    float tt = marginTop;
    float tb = tt + borderTop;
    float bt = tb + getPaddingHeight();
    float bb = bt + borderBottom;

    if (box) {
        rr = (lastBox->x + lastBox->getTotalWidth() - lastBox->marginRight) - x;
        rl = rr - lastBox->borderRight;
        renderBorder(view, x, y - getBlankTop(), getStyle(), backgroundColor, backgroundImage, ll, lr, rl, rr, tt, tb, bt, bb, this, lastBox);
    } else {
        rr = rl = (tail->getX() + tail->getTotalWidth()) - x;
        renderBorder(view, x, y - getBlankTop(), getStyle(), backgroundColor, backgroundImage, ll, lr, rl, rr, tt, tb, bt, bb, this, 0);
        LineBox* lineBox = dynamic_cast<LineBox*>(getParentBox());
        assert(lineBox);
        float baseline = lineBox->getY() + lineBox->getBaseline();
        for (;;) {
            LineBox* nextLine = dynamic_cast<LineBox*>(lineBox->getNextSibling());
            if (!nextLine) {
                // This is a line box in an anonymous block. The last box should be in
                // one of the following anonymous block(s).
                BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(lineBox->getParentBox());
                assert(block);
                while (block = dynamic_cast<BlockLevelBox*>(block->getNextSibling())) {
                    if (block->isAnonymous()) {
                        nextLine = dynamic_cast<LineBox*>(block->getFirstChild());
                        if (nextLine)
                            break;
                    }
                }
            }
            lineBox = nextLine;
            if (!lineBox)   // TODO: Check when this happens.
                break;
            assert(lineBox);
            head = tail = 0;
            for (box = lineBox->getFirstChild(); box; box = box->getNextSibling()) {
                if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box)) {
                    if (!head)
                        head = i;
                    tail = i;
                    if (i == lastBox)
                        break;
                }
            }
            if (box) {
                ll = head->marginLeft;
                lr = ll + head->borderLeft;
                rr = (lastBox->x + lastBox->getTotalWidth() - lastBox->marginRight) - head->x;
                rl = rr - lastBox->borderRight;
                renderBorder(view, head->x, lastBox->y - lastBox->getBlankTop(), getStyle(), backgroundColor, backgroundImage, ll, lr, rl, rr, tt, tb, bt, bb, 0, lastBox);
                break;
            }
            if (head) {
                ll = lr = 0;
                rr = rl = (tail->getX() + tail->getTotalWidth()) - head->x;
                // TODO: Calculate 'top' accurately.
                renderBorder(view, head->x, y - getBlankTop() + (lineBox->getY() + lineBox->getBaseline() - baseline), getStyle(), backgroundColor, backgroundImage, ll, lr, rl, rr, tt, tb, bt, bb, 0, 0);
            }
        }
    }
}

void InlineLevelBox::renderEmptyBox(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle)
{
    parentStyle->resolve(view, getContainingBlock(view));
    LineBox* lineBox = dynamic_cast<LineBox*>(getParentBox());
    assert(lineBox);
    float paddingHeight = parentStyle->paddingTop.getPx() + parentStyle->paddingBottom.getPx();
    float top = lineBox->getY();
    float leading = 0.0f;
    if (FontTexture* font = parentStyle->getFontTexture()) {
        float point = view->getPointFromPx(parentStyle->fontSize.getPx());
        paddingHeight += font->getLineHeight(point);
        leading = std::max(lineBox->getStyle()->lineHeight.getPx(), parentStyle->lineHeight.getPx()) - font->getLineHeight(point);
        top += parentStyle->verticalAlign.getOffset(view, parentStyle, lineBox, font, point, leading);
    }
    top -= parentStyle->marginTop.getPx() + parentStyle->paddingTop.getPx() + parentStyle->borderTopWidth.getPx();

    Box* box;
    InlineLevelBox* head;
    InlineLevelBox* tail = this;
    InlineLevelBox* lastBox = dynamic_cast<InlineLevelBox*>(parentStyle->getLastBox());
    assert(lastBox);
    for (box = this; box && box != lastBox; box = box->getNextSibling()) {
        if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box))
            tail = i;
    }

    float ll = 0.0f;
    float lr = 0.0f;  // TODO: if there's a border, this box shouldn't be empty.
    float rl;
    float rr;
    float tt = parentStyle->marginTop.getPx();
    float tb = tt + parentStyle->borderTopWidth.getPx();
    float bt = tb + paddingHeight;
    float bb = bt + parentStyle->borderBottomWidth.getPx();

    if (box) {
        rr = (lastBox->x + lastBox->getTotalWidth() - lastBox->marginRight) - x;
        rl = rr - lastBox->borderRight;
        renderBorder(view, x, top,
                     parentStyle, parentStyle->backgroundColor.getARGB(), 0,
                     ll, lr, rl, rr, tt, tb, bt, bb, this, lastBox);
    } else {
        rr = rl = (tail->getX() + tail->getTotalWidth()) - x;
        renderBorder(view, x, top,
                     parentStyle, parentStyle->backgroundColor.getARGB(), 0,
                     ll, lr, rl, rr, tt, tb, bt, bb, this, 0);
        float baseline = lineBox->getY() + lineBox->getBaseline();
        for (;;) {
            LineBox* nextLine = dynamic_cast<LineBox*>(lineBox->getNextSibling());
            if (!nextLine) {
                // This is a line box in an anonymous block. The last box should be in
                // one of the following anonymous block(s).
                BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(lineBox->getParentBox());
                assert(block);
                while (block = dynamic_cast<BlockLevelBox*>(block->getNextSibling())) {
                    if (block->isAnonymous()) {
                        nextLine = dynamic_cast<LineBox*>(block->getFirstChild());
                        if (nextLine)
                            break;
                    }
                }
            }
            lineBox = nextLine;
            if (!lineBox)   // TODO: Check when this happens.
                break;
            assert(lineBox);
            head = tail = 0;
            for (box = lineBox->getFirstChild(); box; box = box->getNextSibling()) {
                if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box)) {
                    if (!head)
                        head = i;
                    tail = i;
                    if (i == lastBox)
                        break;
                }
            }
            if (box) {
                ll = head->marginLeft;
                lr = ll + head->borderLeft;
                rr = (lastBox->x + lastBox->getTotalWidth() - lastBox->marginRight) - head->x;
                rl = rr - lastBox->borderRight;
                renderBorder(view, head->x, lastBox->y - lastBox->getBlankTop(),
                             parentStyle, parentStyle->backgroundColor.getARGB(), 0,
                             ll, lr, rl, rr, tt, tb, bt, bb, 0, lastBox);
                break;
            }
            if (head) {
                ll = lr = 0;
                rr = rl = (tail->getX() + tail->getTotalWidth()) - head->x;
                // TODO: Calculate 'top' accurately.
                renderBorder(view, head->x, top + (lineBox->getY() + lineBox->getBaseline() - baseline),
                             parentStyle, parentStyle->backgroundColor.getARGB(), 0,
                             ll, lr, rl, rr, tt, tb, bt, bb, 0, 0);
            }
        }
    }
}

void InlineLevelBox::render(ViewCSSImp* view, StackingContext* stackingContext)
{
    assert(stackingContext);

    if (!isAnonymous() && style->getBox() == this && 0.0f < getTotalWidth()) {
        std::list<CSSStyleDeclarationImp*> parentStyleList;
        for (CSSStyleDeclarationImp* parentStyle = getStyle()->getParentStyle();
             parentStyle && parentStyle->display.isInline() && parentStyle->getBox() == this;
             parentStyle = parentStyle->getParentStyle())
        {
            if (parentStyle->visibility.isVisible())
                parentStyleList.push_front(parentStyle);
        }
        for (auto i = parentStyleList.begin(); i != parentStyleList.end(); ++i)
            renderEmptyBox(view, *i);
    }

    if (isVisible()) {
        if (!font)
            renderBorder(view, x, y);
        else if (!style->hasMultipleBoxes())
            renderBorder(view, x, y - getBlankTop());
        else if (style->getBox() == this)
            renderMultipleBackground(view);
    }
    if (shadow)
        shadow->render();
    else if (getFirstChild())  // for inline-block
        getFirstChild()->render(view, stackingContext);
    else if (font && isVisible()) {
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

    if (isVisible())
        renderOutline(view);
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

void InlineLevelBox::renderOutline(ViewCSSImp* view)
{
    if (getFirstChild()) {  // for inline-block
        getFirstChild()->renderOutline(view, x,  y);
        return;
    }
    if (outlineWidth <= 0.0f || !font)
        return;
    if (!style->hasMultipleBoxes()) {
        Box::renderOutline(view, x, y - getBlankTop());
        return;
    }
    if (style->getBox() != this)
        return;

    // TODO: Render the outline across several lines.
    Box* box;
    InlineLevelBox* head;
    InlineLevelBox* tail = this;
    InlineLevelBox* lastBox = dynamic_cast<InlineLevelBox*>(style->getLastBox());
    assert(lastBox);
    for (box = this; box && box != lastBox; box = box->getNextSibling()) {
        if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box))
            tail = i;
    }

    float left = x + marginLeft + borderLeft;
    float top = y - paddingTop;
    if (box)
        Box::renderOutline(view, left, top, left + getPaddingWidth(), top + getPaddingHeight(),
                           outlineWidth, style->outlineStyle.getValue(), style->outlineColor.getARGB());
    else {
        float right = tail->getX() + tail->getBlankLeft() + tail->width + tail->paddingRight;
        float bottom = top + getPaddingHeight() - outlineWidth;
        Box::renderOutline(view, left, top, right, bottom,
                           outlineWidth, style->outlineStyle.getValue(), style->outlineColor.getARGB());
        LineBox* lineBox = dynamic_cast<LineBox*>(getParentBox());
        assert(lineBox);
        float baseline = lineBox->getY() + lineBox->getBaseline();
        for (;;) {
            LineBox* nextLine = dynamic_cast<LineBox*>(lineBox->getNextSibling());
            if (!nextLine) {
                // This is a line box in an anonymous block. The last box should be in
                // one of the following anonymous block(s).
                BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(lineBox->getParentBox());
                assert(block);
                while (block = dynamic_cast<BlockLevelBox*>(block->getNextSibling())) {
                    if (block->isAnonymous()) {
                        nextLine = dynamic_cast<LineBox*>(block->getFirstChild());
                        if (nextLine)
                            break;
                    }
                }
            }
            lineBox = nextLine;
            if (!lineBox)   // TODO: Check when this happens.
                break;
            assert(lineBox);
            head = tail = 0;
            for (box = lineBox->getFirstChild(); box; box = box->getNextSibling()) {
                if (InlineLevelBox* i = dynamic_cast<InlineLevelBox*>(box)) {
                    if (!head)
                        head = i;
                    tail = i;
                    if (i == lastBox)
                        break;
                }
            }
            if (box) {
                left = head->x + head->marginLeft + head->borderLeft;
                top = lastBox->y - lastBox->paddingTop;
                right = lastBox->x + lastBox->getBlankLeft() + lastBox->width + lastBox->paddingRight;
                bottom = top + lastBox->getPaddingHeight() - outlineWidth;
                Box::renderOutline(view, left, top, right, bottom,
                                   outlineWidth, style->outlineStyle.getValue(), style->outlineColor.getARGB());
                break;
            }
            if (head) {
                left = lineBox->getX();
                // TODO: Calculate 'top' accurately.
                top = y - paddingTop + (lineBox->getY() + lineBox->getBaseline() - baseline);
                right = tail->x + tail->getBlankLeft() + tail->width + tail->paddingRight;
                bottom = top + getPaddingHeight() - outlineWidth;
                Box::renderOutline(view, left, top, right, bottom,
                                   outlineWidth, style->outlineStyle.getValue(), style->outlineColor.getARGB());
            }
        }
    }
}

}}}}  // org::w3c::dom::bootstrap
