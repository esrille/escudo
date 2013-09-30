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

#ifndef VIEWCSS_IMP_H
#define VIEWCSS_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/html/HTMLTemplateElement.h>

#include <map>

#include "WindowImp.h"
#include "ElementImp.h"
#include "EventListenerImp.h"

#include "Box.h"
#include "CounterImp.h"
#include "CSSRuleListImp.h"

#include "font/FontManager.h"

#include "html/MediaQueryListImp.h"

namespace org { namespace w3c { namespace dom {

class Document;

namespace bootstrap {

class StackingContext;

class ViewCSSImp
{
    friend class CSSPseudoClassSelector;    // TODO: only for match()

    static const unsigned MaxFontSizes = 8;

    ContainingBlock initialContainingBlock;

    WindowPtr window;
    unsigned dpi;
    unsigned mediumFontSize;  // [px]
    float fontSizeTable[MaxFontSizes];
    float zoom;

    Retained<EventListenerImp> mutationListener;

    std::map<MediaListImp*, MediaQueryListPtr> mediaListMap;
    bool mediaCheck;

    // Selector matching
    std::map<Element, CSSStyleDeclarationPtr> map;
    std::list<Element> hoverList;
    unsigned overflow;

    // Style recalculation
    StackingContextPtr stackingContexts;

    // Reflow
    Element hovered;
    BlockPtr boxTree;       // A box tree under construction
    std::list<CounterImpPtr> counterList;
    int quotingDepth;
    float scrollWidth;
    float scrollHeight;

    // Repaint
    unsigned clipCount;
    Box* hoveredBox;
    unsigned short flags{0};

    // Animation
    unsigned last;   // in 1/100 sec for GIF
    unsigned delay;  // in 1/100 sec for GIF

    void removeComputedStyle(Element element);

    void handleMutation(EventListenerImp* listener, events::Event event);
    void collectRules(CSSRuleListImp::RuleSet& set, Element element, css::CSSRuleList list, unsigned importance, MediaListPtr mediaList = nullptr);
    void updateStyleRules(Element element, const CSSStyleDeclarationPtr& style, CSSStyleDeclarationPtr parentStyle);
    bool expandBinding(Element element, const CSSStyleDeclarationPtr& style);

public:
    ViewCSSImp(WindowPtr window);
    virtual ~ViewCSSImp();

    DocumentPtr getDocument() const {
        return window->getDocument();
    }
    WindowPtr getWindow() const {
        return window;
    }

    // Media query
    MediaQueryListPtr matchMedia(const MediaListPtr& mediaList);
    void flushMediaQueryLists(std::list<html::MediaQueryList>& list) {
        for (auto i = mediaListMap.begin(); i != mediaListMap.end(); ++i)
            list.push_back(i->second);
    }

    // Selector matching
    void addStyle(const Element& element, const CSSStyleDeclarationPtr& style);
    void constructComputedStyles();
    unsigned constructComputedStyle(Node node, CSSStyleDeclarationPtr parentStyle, unsigned propagateFlags = 0);

    // Style recalculation
    void calculateComputedStyles();
    void calculateComputedStyle(Element element, const CSSStyleDeclarationPtr& parentStyle, CSSAutoNumberingValueImp::CounterContext* counterContext, unsigned flags);
    Element updatePseudoElement(const CSSStyleDeclarationPtr& style, int id, Element element, Element pseudoElement, CSSAutoNumberingValueImp::CounterContext* counterContext);

    // Reflow
    HttpRequest* preload(const std::u16string& base, const std::u16string& url) {
        if (window)
            return window->preload(base, url);
        return 0;
    }
    Block* createBlock(Element element, Block* parentBox, const CSSStyleDeclarationPtr& style, bool newContext, bool asTablePart = false);
    Block* constructBlock(Node node, Block* parentBox, const CSSStyleDeclarationPtr& style, Block* prevBox, bool asTablePart = false);
    Block* constructBlock(Text text, Block* parentBox, const CSSStyleDeclarationPtr& style, Block* prevBox);
    Block* constructBlock(Element element, Block* parentBox, const CSSStyleDeclarationPtr& parentStyle, CSSStyleDeclarationPtr style, Block* prevBox, bool asTablePart = false);
    Block* constructBlocks();
    Block* layOut();
    Block* dump();
    void resolveXY(float left, float top);

    // Repaint
    void beginTranslucent();
    void endTranslucent(float alpha);
    void render(unsigned clipCount);
    void renderCanvas(unsigned color);
    unsigned getBackgroundColor();

    // Misc.

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
        return stackingContexts.get();
    }
    void setStackingContexts(StackingContext* contexts) {
        stackingContexts = contexts;
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
        for (unsigned i = 0; i < MaxFontSizes; ++i) {
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

    unsigned getClipCount() const {
        return clipCount;
    }

    Element setHovered(Element node);
    bool isHovered(Element node);

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
        setFlags(Box::NEED_REFLOW);
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

    CSSStyleDeclarationPtr getStyle(Element elt, Nullable<std::u16string> pseudoElt = Nullable<std::u16string>());

    Block* getTree() const {
        return boxTree.get();
    }
    void setFlags(unsigned short flags) {
        if (boxTree)
            boxTree->setFlags(flags);
        else if (flags & Box::NEED_REPAINT)
            this->flags |= flags;
    }
    unsigned short gatherFlags() const {
        if (boxTree)
            return flags | boxTree->gatherFlags();
        return flags;
    }
    void clearFlags(unsigned short flags = 0xffff) {
        if (boxTree)
            boxTree->clearFlags(flags);
        this->flags &= ~flags;
    }

    unsigned getLast() const {
        return last;
    }
    unsigned getDelay() const {
        return delay;
    }
    unsigned setDelay(unsigned value) {
        if (delay == 0)
            delay = value;
        else
            delay = std::min(delay, value);
        return delay;
    }
    bool hasExpired(unsigned t) {
        if (0 < delay && (static_cast<int>(last + delay) - static_cast<int>(t)) <= 0) {
            delay = 0;
            return true;
        }
        return false;
    }

    void clip(float left, float top, float w, float h);
    void unclip(float left, float top, float w, float h);

    bool getMediaCheck() const {
        return mediaCheck;
    }
    void setMediaCheck(bool value) {
        mediaCheck = true;
    }

    // ViewCSS
    virtual css::CSSStyleDeclaration getComputedStyle(Element elt, Nullable<std::u16string> pseudoElt);

    // TODO: Refine the following OpenGL specific part
    FontTexture* selectFont(const CSSStyleDeclarationPtr& style);
    FontTexture* selectAltFont(const CSSStyleDeclarationPtr& style, FontTexture* current, char32_t u);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // VIEWCSS_IMP_H
