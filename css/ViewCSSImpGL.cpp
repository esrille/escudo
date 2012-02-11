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

namespace {

FontTexture* getFontTexture(FontFileInfo* info, CSSStyleDeclarationImp* style)
{
    if (!info)
        return 0;

    backend.getFontFace(info->filename);

    bool bold = false;
    if (700 <= style->fontWeight.getWeight() && info->weight <= 400)
        bold = true;

    bool oblique = false;
    if ((style->fontStyle.getStyle() == CSSFontStyleValueImp::Italic ||
         style->fontStyle.getStyle() == CSSFontStyleValueImp::Oblique) &&
        !(info->style & CSSFontStyleValueImp::Italic || info->style & CSSFontStyleValueImp::Oblique))
        oblique = true;

    return backend.getFontTexture(Point, bold, oblique);
}

}

FontTexture* ViewCSSImp::selectFont(CSSStyleDeclarationImp* style)
{
    FontFileInfo* info = FontFileInfo::chooseFont(style);
    return getFontTexture(info, style);
}

FontTexture* ViewCSSImp::selectAltFont(CSSStyleDeclarationImp* style, FontTexture* current, char32_t u)
{
    FontFileInfo* info = FontFileInfo::chooseAltFont(style, current, u);
    return getFontTexture(info, style);
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
