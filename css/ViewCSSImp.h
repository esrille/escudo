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

#ifndef VIEWCSS_IMP_H
#define VIEWCSS_IMP_H

#include <Object.h>

#include <org/w3c/dom/css/CSSStyleDeclaration.h>

#include <map>
#include <set>

#include "ElementImp.h"
#include "EventListenerImp.h"

#include "Box.h"
#include "CSSPropertyValueImp.h"

#include "font/FontManager.h"

namespace org { namespace w3c { namespace dom {

class Document;

namespace bootstrap {

class ViewCSSImp
{
    friend class BlockLevelBox; // TODO just for layOutInlineReplaced for now...

    Document document;
    css::CSSStyleSheet defaultStyleSheet;
    std::map<Element, CSSStyleDeclarationPtr> map;
    std::map<Node, BlockLevelBox*> floatMap;
    BlockLevelBox* boxTree;
    ContainingBlock initialContainingBlock;

    unsigned dpi;
    unsigned mediumFontSize;  // [px]

    static const unsigned MaxFontSizes = 8;
    float fontSizeTable[MaxFontSizes];

    Retained<EventListenerImp> mutationListener;
    void handleMutation(events::Event event);

    void cascade(Node node, CSSStyleDeclarationImp* parentStyle = 0);

    BlockLevelBox* layOutBlockBoxes(Node node, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style);
    BlockLevelBox* layOutBlockBoxes(Text text, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style);
    BlockLevelBox* layOutBlockBoxes(Element element, BlockLevelBox* parentBox, BlockLevelBox* siblingBox, CSSStyleDeclarationImp* style);

public:
    ViewCSSImp(Document document, css::CSSStyleSheet defaultStyleSheet);
    ~ViewCSSImp();

    Document getDocument() const {
        return document;
    }

    void cascade() {
        map.clear();
        cascade(document, 0);
    }

    BlockLevelBox* layOutBlockBoxes();
    BlockLevelBox* layOut();
    BlockLevelBox* dump();

    void render() {
        if (boxTree)
            boxTree->render(this);
    }

    // For the containing block of the root element's top-level boxes, we need to explicitly specify the size of the box.
    void setSize(float w, float h) {
        initialContainingBlock.width = w;
        initialContainingBlock.height = h;
    }
    const ContainingBlock* getInitialContainingBlock() const {
        return &initialContainingBlock;
    }

    unsigned getDPI() const {
        return dpi;
    }
    void setDPI(unsigned size) {
        dpi = size;
        // TODO: update fontSizeTable, too?
    }

    BlockLevelBox* getFloatBox(Node node) {
        auto i = floatMap.find(node);
        if (i == floatMap.end())
            return 0;
        return i->second;
    }

    unsigned getMediumFontSize() const {
        return mediumFontSize;
    }
    void setMediumFontSize(unsigned size) {
        mediumFontSize = size;
        fontSizeTable[0] = size * (3.0f / 5.0f);
        fontSizeTable[1] = size * (3.0f / 4.0f);
        fontSizeTable[2] = size * (8.0f / 9.0f);
        fontSizeTable[3] = size;
        fontSizeTable[4] = size * (6.0f / 5.0f);
        fontSizeTable[5] = size * (3.0f / 2.0f);
        fontSizeTable[6] = size * (2.0f / 1.0f);
        fontSizeTable[7] = size * (3.0f / 1.0f);
    }

    float getFontSize(unsigned i) const {
        if (MaxFontSizes <= i)
            i = MaxFontSizes - 1;
        return fontSizeTable[i];
    }
    float getLargerFontSize(float size) const {
        for (int i = 0; i < MaxFontSizes; ++i) {
            if (size < fontSizeTable[i])
                return fontSizeTable[i];
        }
        return fontSizeTable[MaxFontSizes - 1];
    }
    float getSmallerFontSize(float size) const {
        for (int i = MaxFontSizes - 1; 0 <= i; --i) {
            if (fontSizeTable[i] < size)
                return fontSizeTable[i];
        }
        return fontSizeTable[0];
    }

    // length
    float getPx(CSSNumericValue& n, float reference = 0.0f) {
        switch (n.unit) {
        case css::CSSPrimitiveValue::CSS_PERCENTAGE:
            return reference * n.number / 100.0f;
        case css::CSSPrimitiveValue::CSS_EMS:
        case css::CSSPrimitiveValue::CSS_EXS:
            return reference * n.number;
        case css::CSSPrimitiveValue::CSS_PX:
            return n.number;
        case css::CSSPrimitiveValue::CSS_CM:
            return n.number / 2.54f * dpi;
        case css::CSSPrimitiveValue::CSS_MM:
            return n.number / 25.4f * dpi;
        case css::CSSPrimitiveValue::CSS_IN:
            return n.number * dpi;
        case css::CSSPrimitiveValue::CSS_PT:
            return n.number / 72 * dpi;
        case css::CSSPrimitiveValue::CSS_PC:
            return n.number * 12 / 72 * dpi;
        default:
            // TODO: error
            return 0.0f;
        }
    }
    float getPointFromPx(float px) {
        return px / dpi * 72;
    }

    Box* lookupTarget(int& x, int& y);

    CSSStyleDeclarationImp* getStyle(Element elt, Nullable<std::u16string> pseudoElt = Nullable<std::u16string>());

    // TODO OpenGL specific part...
    FontTexture* selectFont(CSSStyleDeclarationImp* style);

    // ViewCSS
    virtual css::CSSStyleDeclaration getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // VIEWCSS_IMP_H
