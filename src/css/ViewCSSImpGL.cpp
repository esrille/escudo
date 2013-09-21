/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "ViewCSSImp.h"

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>

#include <new>

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "DocumentImp.h"
#include "WindowProxy.h"

#include "Box.h"
#include "StackingContext.h"

#include "font/FontDatabase.h"
#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"

#include "Test.util.h"

namespace {

FontManagerBackEndGL backend;

const int Point = 33;

}

namespace org { namespace w3c { namespace dom { namespace bootstrap {

FontTexture* ViewCSSImp::selectFont(const CSSStyleDeclarationPtr& style)
{
    FontManager* manager = backend.getFontManager();
    unsigned s = style->fontStyle.getStyle();
    unsigned w = style->fontWeight.getWeight();
    for (auto i = style->fontFamily.getFamilyNames().begin(); i != style->fontFamily.getFamilyNames().end(); ++i) {
        if (FontFace* face = manager->getFontFace(*i, s, w))
            return face->getFontTexture(Point, s, w);
    }
    unsigned g = style->fontFamily.getGeneric();
    if (!g)
        g = CSSFontFamilyValueImp::SansSerif;
    if (FontFace* face = manager->getFontFace(g, s, w))
        return face->getFontTexture(Point, s, w);
    return 0;
}

FontTexture* ViewCSSImp::selectAltFont(const CSSStyleDeclarationPtr& style, FontTexture* current, char32_t u)
{
    assert(current);
    FontManager* manager = backend.getFontManager();
    unsigned s = style->fontStyle.getStyle();
    unsigned w = style->fontWeight.getWeight();
    bool skipped = false;
    for (auto i = style->fontFamily.getFamilyNames().begin(); i != style->fontFamily.getFamilyNames().end(); ++i) {
        FontFace* face = manager->getFontFace(*i, s, w);
        if (!face)
            continue;
        if (face->getFamilyName() == current->getFace()->getFamilyName()) {
            // TODO: Deal with filesnames registered more than once.
            skipped = true;
            continue;
        }
        if (skipped && face->hasGlyph(u))
            return face->getFontTexture(Point, s, w);
    }
    unsigned g = style->fontFamily.getGeneric();
    if (!g)
        g = CSSFontFamilyValueImp::SansSerif;
    if (FontFace* face = manager->getAltFontFace(g, s, w, current, u))
        return face->getFontTexture(Point, s, w);
    return 0;
}

void ViewCSSImp::beginTranslucent()
{
    if (auto imp = window->getWindowProxy())
        imp->beginTranslucent();
}

void ViewCSSImp::endTranslucent(float alpha)
{
    if (auto imp = window->getWindowProxy())
        imp->endTranslucent(alpha);
}

void ViewCSSImp::render(unsigned parentClipCount)
{
    last = getTick();

    // reset clipCount
    clipCount = 0;
    glStencilFunc(GL_EQUAL, 0, 0xFF);

    glPushMatrix();
    glScalef(zoom, zoom, zoom);
    glTranslatef(-window->getScrollX(), -window->getScrollY(), 0.0f);
    if (stackingContexts) {
        stackingContexts->resetScrollSize();
        if (boxTree) {
            boxTree->scrollWidth = getInitialContainingBlock()->getWidth();
            boxTree->scrollHeight = getInitialContainingBlock()->getHeight();
        }
        stackingContexts->resolveScrollSize(this);
        stackingContexts->render(this);
    }
    glPopMatrix();

    if (boxTree) {
        float s = boxTree->x + boxTree->stackingContext->getRelativeX();
        float t = boxTree->y + boxTree->stackingContext->getRelativeY();
        boxTree->scrollWidth = std::max(boxTree->scrollWidth, s + boxTree->getBlockWidth());
        boxTree->scrollHeight = std::max(boxTree->scrollHeight, t + boxTree->getBlockHeight());
        updateScrollWidth(boxTree->getScrollWidth());
        updateScrollHeight(boxTree->getScrollHeight());
        if (canScroll()) {
            Box::renderVerticalScrollBar(initialContainingBlock.width, initialContainingBlock.height, window->getScrollY(), scrollHeight);
            Box::renderHorizontalScrollBar(initialContainingBlock.width, initialContainingBlock.height, window->getScrollX(), scrollWidth);
        }
    }

    // restore clipCount
    glStencilFunc(GL_EQUAL, parentClipCount, 0xFF);
}

void ViewCSSImp::renderCanvas(unsigned color)
{
    if (!color)
        return;

    glColor4ub(color >> 16, color >> 8, color, color >> 24);
    float l = window->getScrollX();
    float r = l + initialContainingBlock.width;
    float t = window->getScrollY();
    float b = t + initialContainingBlock.height;
    glBegin(GL_QUADS);
    glVertex2f(l, t);
    glVertex2f(r, t);
    glVertex2f(r, b);
    glVertex2f(l, b);
    glEnd();
}

void ViewCSSImp::clip(float left, float top, float w, float h)
{
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_EQUAL, clipCount, 0xFF);
    glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
    glRectf(left, top, left + w, top + h);
    ++clipCount;    // TODO: check overflow
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, clipCount, 0xFF);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void ViewCSSImp::unclip(float left, float top, float w, float h)
{
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
    glRectf(left, top, left + w, top + h);
    --clipCount;
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, clipCount, 0xFF);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

namespace {

const int cursorMap[CSSCursorValueImp::CursorsMax] = {
    GLUT_CURSOR_INHERIT,            // Auto
    GLUT_CURSOR_CROSSHAIR,          // Crosshair
    GLUT_CURSOR_LEFT_ARROW,         // Default
    GLUT_CURSOR_INFO,               // Pointer
    GLUT_CURSOR_DESTROY,            // Move
    GLUT_CURSOR_RIGHT_SIDE,         // EResize
    GLUT_CURSOR_TOP_RIGHT_CORNER,   // NEResize
    GLUT_CURSOR_TOP_LEFT_CORNER,    // NWResize
    GLUT_CURSOR_TOP_SIDE,           // NResize
    GLUT_CURSOR_BOTTOM_RIGHT_CORNER,// SEResize
    GLUT_CURSOR_BOTTOM_LEFT_CORNER, // SWResize
    GLUT_CURSOR_BOTTOM_SIDE,        // SResize
    GLUT_CURSOR_LEFT_SIDE,          // WResize
    GLUT_CURSOR_TEXT,               // Text
    GLUT_CURSOR_WAIT,               // Wait
    GLUT_CURSOR_HELP,               // Help
    GLUT_CURSOR_WAIT,               // Progress
};

}

Element ViewCSSImp::setHovered(Element target)
{
    assert(boxTree);
    if (hovered == target)
        return hovered;
    CSSStyleDeclarationPtr next = getStyle(target);
    CSSStyleDeclarationPtr curr = getStyle(hovered);

    Element prev = hovered;
    hovered = target; // TODO: Fix synchronization issues with the background thread.

    if (next) {
        glutSetCursor(cursorMap[next->cursor.getValue()]);
        CSSStyleDeclarationPtr affected;
        if (curr && (affected = curr->getAffectedByHover())) {
            affected->requestReconstruct(Box::NEED_STYLE_RECALCULATION);
            affected->clearFlags(CSSStyleDeclarationImp::Computed);
        }
        if (affected = next->getAffectedByHover()) {
            affected->requestReconstruct(Box::NEED_STYLE_RECALCULATION);
            affected->clearFlags(CSSStyleDeclarationImp::Computed);
        }
    }
    return prev;
}

}}}}  // org::w3c::dom::bootstrap

void initFonts(int* argc, char* argv[])
{
    FontManager* manager = backend.getFontManager();
    FontDatabase::loadBaseFonts(manager);
    for (int i = 1; i < *argc; ++i) {
        if (strcmp(argv[i], "-testfonts") == 0) {
            FontDatabase::loadTestFonts(manager);
            for (; i < *argc; ++i)
                argv[i] = argv[i + 1];
            --*argc;
            break;
        }
    }
}
