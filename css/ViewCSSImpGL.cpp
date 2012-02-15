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

#include "ViewCSSImp.h"

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>

#include <new>

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "DocumentImp.h"

#include "Box.h"
#include "StackingContext.h"

#include "font/FontDatabase.h"
#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"

namespace {

FontManagerBackEndGL backend;

const int Point = 33;

}

namespace org { namespace w3c { namespace dom { namespace bootstrap {

FontTexture* ViewCSSImp::selectFont(CSSStyleDeclarationImp* style)
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

FontTexture* ViewCSSImp::selectAltFont(CSSStyleDeclarationImp* style, FontTexture* current, char32_t u)
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

void ViewCSSImp::render()
{
    glPushMatrix();
    glScalef(zoom, zoom, zoom);
    glTranslatef(-window->getScrollX(), -window->getScrollY(), 0.0f);
    if (stackingContexts)
        stackingContexts->render(this);
    glPopMatrix();

    if (overflow != CSSOverflowValueImp::Hidden) {
        Box::renderVerticalScrollBar(initialContainingBlock.width, initialContainingBlock.height, window->getScrollY(), boxTree->getTotalHeight());
        Box::renderHorizontalScrollBar(initialContainingBlock.width, initialContainingBlock.height, window->getScrollX(), scrollWidth);
    }
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
