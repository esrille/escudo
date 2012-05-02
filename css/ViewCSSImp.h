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

#ifndef VIEWCSS_IMP_H
#define VIEWCSS_IMP_H

#include <Object.h>

#include <org/w3c/dom/css/CSSStyleDeclaration.h>

#include <map>
#include <set>

#include "DocumentWindow.h"
#include "ElementImp.h"
#include "EventListenerImp.h"

#include "Box.h"
#include "CounterImp.h"
#include "CSSRuleListImp.h"

#include "font/FontManager.h"

namespace org { namespace w3c { namespace dom {

class Document;

namespace bootstrap {

class StackingContext;

class ViewCSSImp
{
    static const unsigned MaxFontSizes = 8;

    css::CSSStyleSheet defaultStyleSheet;
    css::CSSStyleSheet userStyleSheet;
    Retained<ContainingBlock> initialContainingBlock;

    DocumentWindowPtr window;
    unsigned dpi;
    unsigned mediumFontSize;  // [px]
    float fontSizeTable[MaxFontSizes];
    float zoom;

    Node hovered;
    Retained<EventListenerImp> mutationListener;

    // cascade
    std::map<Element, CSSStyleDeclarationPtr> map;
    StackingContext* stackingContexts;
    std::list<CSSStyleSheetImp*> styleSheets;
    unsigned overflow;

    // layout
    BlockLevelBoxPtr boxTree;
    std::map<Node, BlockLevelBoxPtr> floatMap;
    std::list<BlockLevelBox*> absoluteList;
    std::list<CounterImpPtr> counterList;
    int quotingDepth;
    float scrollWidth;
    float scrollHeight;

    void handleMutation(events::Event event);
    void findDeclarations(CSSRuleListImp::DeclarationSet& set, Element element, css::CSSRuleList list, unsigned importance);

public:
    ViewCSSImp(DocumentWindowPtr window, css::CSSStyleSheet defaultStyleSheet, css::CSSStyleSheet userStyleSheet = 0);
    ~ViewCSSImp();

    Document getDocument() const {
        return window->getDocument();
    }
    DocumentWindowPtr getWindow() const {
        return window;
    }

    void cascade();
    void cascade(Node node, CSSStyleDeclarationImp* parentStyle = 0);

    HttpRequest* preload(const std::u16string& base, const std::u16string& url) {
        if (window)
            return window->preload(base, url);
        return 0;
    }

    BlockLevelBox* createBlockLevelBox(Element element,  BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, bool newContext, bool asBlock);
    BlockLevelBox* layOutBlockBoxes(Node node, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, CSSAutoNumberingValueImp::CounterContext* counterContext);
    BlockLevelBox* layOutBlockBoxes(Text text, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, CSSAutoNumberingValueImp::CounterContext* counterContext);
    BlockLevelBox* layOutBlockBoxes(Element element, BlockLevelBox* parentBox, CSSStyleDeclarationImp* style, CSSAutoNumberingValueImp::CounterContext* counterContext, bool asBlock = false);
    BlockLevelBox* layOutBlockBoxes();
    BlockLevelBox* layOut();
    BlockLevelBox* dump();

    void resolveXY(float left, float top);
    void render();

    // For the containing block of the root element's top-level boxes, we need to explicitly specify the size of the box.
    void setSize(float w, float h) {
        initialContainingBlock.width = w;
        initialContainingBlock.height = h;
    }
    float getWidth() const {
        return initialContainingBlock.width;
    }
    float getHeight() const {
        return initialContainingBlock.height;
    }

    const ContainingBlock* getInitialContainingBlock() const {
        return &initialContainingBlock;
    }

    StackingContext* getStackingContexts() const {
        return stackingContexts;
    }
    void setStackingContexts(StackingContext* contexts) {
        stackingContexts = contexts;
    }

    unsigned getDPI() const {
        return dpi;
    }
    void setDPI(unsigned size) {
        dpi = size;
        // TODO: update fontSizeTable, too?
    }

    void addFloatBox(Element element, BlockLevelBox* floatingBox, CSSStyleDeclarationImp* style) {
        if (floatingBox) {
            floatMap[element] = floatingBox;
            map[element] = style;
        }
    }
    BlockLevelBox* getFloatBox(Node node) {
        auto i = floatMap.find(node);
        if (i == floatMap.end())
            return 0;
        return i->second.get();
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
        case css::CSSPrimitiveValue::CSS_NUMBER:
            return n.number;
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
            return NAN;
        }
    }
    float getPointFromPx(float px) {
        return px / dpi * 72;
    }
    float getPxFromPoint(float point) {
        return point * dpi / 72;
    }

    Box* boxFromPoint(int x, int y);

    float getScrollWidth() const {
        return scrollWidth;
    }
    float updateScrollWidth(float w) {
        scrollWidth = std::max(scrollWidth, w);
        return scrollWidth;
    }
    float getScrollHeight() const {
        return scrollHeight;
    }
    float updateScrollHeight(float h) {
        scrollHeight = std::max(scrollHeight, h);
        return scrollHeight;
    }

    Node getHovered() const {
        return hovered;
    }
    // Set hovered to node.
    void setHovered(Node node);
    bool isHovered(Node node);

    bool canScroll() const {
        // Note the 'visible' value is interpreted as 'auto' in the viewport.
        return overflow != CSSOverflowValueImp::Hidden;
    }

    float getZoom() const {
        return zoom;
    }
    void setZoom(float value) {
        if (value < 0.01f || 100.0f < value || zoom == value)
            return;
        zoom = value;
        if (boxTree)
            boxTree->setFlags(2);
    }

    CounterImpPtr getCounter(const std::u16string identifier);
    void clearCounters() {
        counterList.clear();
    }

    int incrementQuotingDepth() {
        return quotingDepth++;
    }
    int decrementQuotingDepth() {
        return --quotingDepth;
    }

    CSSStyleDeclarationImp* getStyle(Element elt, Nullable<std::u16string> pseudoElt = Nullable<std::u16string>());

    // TODO OpenGL specific part...
    FontTexture* selectFont(CSSStyleDeclarationImp* style);
    FontTexture* selectAltFont(CSSStyleDeclarationImp* style, FontTexture* current, char32_t u);

    BlockLevelBox* getTree() const {
        return boxTree.get();
    }
    void setFlags(unsigned f) {
        if (boxTree)
            boxTree->setFlags(f);
    }
    unsigned getFlags() const {
        return boxTree ? boxTree->getFlags() : 0;
    }
    void clearFlags() {
        if (boxTree)
            boxTree->clearFlags();
    }

    // ViewCSS
    virtual css::CSSStyleDeclaration getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // VIEWCSS_IMP_H
