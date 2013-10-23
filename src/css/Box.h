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

#ifndef ES_CSSBOX_H
#define ES_CSSBOX_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Text.h>

#include <algorithm>
#include <list>
#include <string>

#include <boost/intrusive_ptr.hpp>

#include "http/HTTPRequest.h"
#include "CSSStyleDeclarationImp.h"
#include "FormattingContext.h"
#include "StackingContext.h"

struct FontGlyph;
class FontTexture;

namespace org { namespace w3c { namespace dom {

class Element;

namespace bootstrap {

class ContainingBlock;
class Box;
class Block;
class LineBox;
class InlineBox;
class TableWrapperBox;

class BoxImage;
class FormattingContext;
class ViewCSSImp;
class WindowProxy;

typedef std::shared_ptr<ContainingBlock> ContainingBlockPtr;
typedef std::shared_ptr<Box> BoxPtr;
typedef std::shared_ptr<Block> BlockPtr;
typedef std::shared_ptr<LineBox> LineBoxPtr;
typedef std::shared_ptr<InlineBox> InlineBoxPtr;
typedef std::shared_ptr<TableWrapperBox> TableWrapperBoxPtr;

typedef std::shared_ptr<WindowProxy> WindowProxyPtr;

class ContainingBlock : public std::enable_shared_from_this<ContainingBlock>
{
protected:
    // for debugging
    unsigned uid;

public:
    ContainingBlock();
    virtual ~ContainingBlock();

    float width {0.0f};
    float height {0.0f};

    float getWidth() const {
        return width;
    }
    float getHeight() const {
        return height;
    }

    long count_() const {
        return shared_from_this().use_count() - 1;
    }
};

class Box : public ContainingBlock
{
    friend class ViewCSSImp;
    friend class Block;
    friend class LineBox;
    friend class InlineBox;
    friend class FormattingContext;
    friend class BoxImage;
    friend class StackingContext;

public:
    static const unsigned short BLOCK_LEVEL_BOX = 1;
    static const unsigned short LINE_BOX = 2;
    static const unsigned short INLINE_LEVEL_BOX = 3;

    // flags
    static const unsigned short NEED_SELECTOR_MATCHING = 0x01;
    static const unsigned short NEED_STYLE_RECALCULATION = 0x02;
    static const unsigned short NEED_EXPANSION = 0x04;
    static const unsigned short NEED_CHILD_EXPANSION = 0x08;
    static const unsigned short NEED_REFLOW = 0x10;
    static const unsigned short NEED_CHILD_REFLOW = 0x20;
    static const unsigned short NEED_REPOSITION = 0x40;
    static const unsigned short NEED_REPAINT = 0x80;
    static const unsigned short NEED_SELECTOR_REMATCHING = 0x100;

    static const unsigned short NEED_TABLE_REFLOW = 0x8000;

protected:
    Node node;
    std::weak_ptr<Box> parentBox;
    BoxPtr firstChild;
    BoxPtr lastChild;
    BoxPtr previousSibling;
    BoxPtr nextSibling;
    std::weak_ptr<Box> containingBox;
    unsigned int childCount;

    float clearance;
    float marginTop;
    float marginRight;
    float marginBottom;
    float marginLeft;
    float paddingTop;
    float paddingRight;
    float paddingBottom;
    float paddingLeft;
    float borderTop;
    float borderRight;
    float borderBottom;
    float borderLeft;

    unsigned position;
    float offsetH;
    float offsetV;

    unsigned visibility;

    StackingContextPtr stackingContext;
    BoxPtr nextFloat;

    bool intrinsic;  // do not change width and height

    float x;  // in screen coord
    float y;  // in screen coord

    std::weak_ptr<Block> clipBox;

    // background
    unsigned backgroundColor;
    HttpRequestPtr backgroundRequest;
    BoxImage* backgroundImage;
    float backgroundLeft;
    float backgroundTop;
    unsigned backgroundStart;

    CSSStyleDeclarationPtr style;

    unsigned short flags;

    WindowProxyPtr childWindow;

    void setParentBox(const BoxPtr& box) {
        parentBox = box;
        containingBox.reset();
    }
    void renderBorderEdge(ViewCSSImp* view, int edge, unsigned borderStyle, unsigned color,
                          float a, float b, float c, float d,
                          float e, float f, float g, float h);
public:
    Box(Node node);
    virtual ~Box();

    bool isSane() const;

    BoxPtr self() const {
        return std::const_pointer_cast<Box>(std::static_pointer_cast<const Box>(shared_from_this()));
    }

    virtual unsigned getBoxType() const = 0;

    virtual bool isAnonymous() const {
        return !node;
    }

    Node getNode() const {
        return node;
    }
    WindowProxyPtr getChildWindow() const {
        return childWindow;
    }

    Node getTargetNode() const {
        BoxPtr box = self();
        do {
            if (box->node)
                return box->node;
        } while ((box = box->getParentBox()));
        return nullptr;
    }

    BoxPtr removeChild(const BoxPtr& item);
    BoxPtr insertBefore(const BoxPtr& item, const BoxPtr& after);
    BoxPtr appendChild(const BoxPtr& item);

    void removeChildren();
    void removeDescendants();

    BoxPtr getContainingBox() const {
        return containingBox.lock();
    }
    void setContainingBox(const BoxPtr& box);

    BoxPtr getParentBox() const {
        return parentBox.lock();
    }
    bool hasChildBoxes() const {
        return childCount;
    }
    BoxPtr getFirstChild() const {
        return firstChild;
    }
    BoxPtr getLastChild() const {
        return lastChild;
    }
    BoxPtr getPreviousSibling() const {
        return previousSibling;
    }
    BoxPtr getNextSibling() const {
        return nextSibling;
    }

    BlockPtr getClipBox() const {
        return clipBox.lock();
    }

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }

    float getMarginTop() const {
        return marginTop;
    }
    float getMarginRight() const {
        return marginRight;
    }
    float getMarginBottom() const {
        return marginBottom;
    }
    float getMarginLeft() const {
        return marginLeft;
    }

    float getBorderTop() const {
        return borderTop;
    }
    float getBorderRight() const {
        return borderRight;
    }
    float getBorderBottom() const {
        return borderBottom;
    }
    float getBorderLeft() const {
        return borderLeft;
    }

    float getPaddingTop() const {
        return paddingTop;
    }
    float getPaddingRight() const {
        return paddingRight;
    }
    float getPaddingBottom() const {
        return paddingBottom;
    }
    float getPaddingLeft() const {
        return paddingLeft;
    }

    bool hasMargins() const {  // have non-zero margins?
        return marginTop != 0.0f || marginRight != 0.0f || marginBottom != 0.0f || marginLeft != 0.0f;
    }

    bool hasClearance() const {
        return !isnan(clearance);
    }
    float getClearance() const {
        return hasClearance() ? clearance : 0.0f;
    }

    float getBlankLeft() const {
        return marginLeft + borderLeft + paddingLeft;
    }
    float getBlankRight() const {
        return marginRight + borderRight + paddingRight;
    }
    float getTotalWidth() const {
        return marginLeft + borderLeft + paddingLeft + width + paddingRight + borderRight + marginRight;
    }
    float getBorderWidth() const {
        return borderLeft + paddingLeft + width + paddingRight + borderRight;
    }
    float getPaddingWidth() const {
        return paddingLeft + width + paddingRight;
    }
    float getBlankTop() const {
        return marginTop + borderTop + paddingTop;
    }
    float getBlankBottom() const {
        return marginBottom + borderBottom + paddingBottom;
    }
    float getTotalHeight() const {
        return marginTop + borderTop + paddingTop + height + paddingBottom + borderBottom + marginBottom;
    }
    float getBorderHeight() const {
        return borderTop + paddingTop + height + paddingBottom + borderBottom;
    }
    float getPaddingHeight() const {
        return paddingTop + height + paddingBottom;
    }
    float getOutlineWidth() const;

    float getEffectiveTotalWidth() const;

    // for block level box
    float getBlockWidth() const {
        float w = marginLeft + getBorderWidth();
        if (0.0f < marginRight)
            w += marginRight;
        return w;
    }
    float getBlockHeight() const {
        float h = marginTop + getBorderHeight();
        if (0.0f < marginBottom)
            h += marginBottom;
        return h;
    }

    float getVerticalOffset() const {
        return offsetV;
    }

    void expandMargins(float t, float r, float b, float l) {
        marginTop += t;
        marginRight += r;
        marginBottom += b;
        marginLeft += l;
    }

    void expandBorders(float t, float r, float b, float l) {
        borderTop += t;
        borderRight += r;
        borderBottom += b;
        borderLeft += l;
    }

    virtual float shrinkTo();
    virtual void fit(float w, FormattingContext* context) {}

    void toViewPort(float& x, float& y) const {
        const Box* box = this;
        do {
            box = box->towardViewPort(x, y);
        } while (box);
    }
    const Box* towardViewPort(float& x, float& y) const {
        x += offsetH + getBlankLeft();
        y += offsetV + getBlankTop();
        if (BoxPtr box = getParentBox())
            return box->towardViewPort(this, x, y);
        return 0;
    }
    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        return this;
    }

    CSSStyleDeclarationPtr getStyle() const {
        return style;
    }
    void setStyle(const CSSStyleDeclarationPtr& style);
    void unresolveStyle();

    bool isStatic() const {
        return position == CSSPositionValueImp::Static;
    }
    bool isRelative() const {
        return position == CSSPositionValueImp::Relative;
    }
    bool isAbsolute() const {
        return position == CSSPositionValueImp::Absolute;
    }
    bool isFixed() const {
        return position == CSSPositionValueImp::Fixed;
    }
    bool isPositioned() const {
        return !isStatic();
    }
    bool isAbsolutelyPositioned() const {
        return isPositioned() && !isRelative();
    }
    void setPosition(unsigned value) {
        position = value;
    }

    virtual ContainingBlockPtr getContainingBlock(ViewCSSImp* view) const;

    bool isFlowOf(const BlockPtr& flowRoot) const;

    bool isInFlow() const {
        // cf. 9.3 Positioning schemes
        return !isFloat() && !isAbsolutelyPositioned() && getParentBox();
    }

    virtual bool isFloat() const {
        return false;
    }
    virtual bool isClipped() const {
        return false;
    }

    void updatePadding();
    void updateBorderWidth();

    void resolveReplacedWidth(float intrinsicWidth, float intrinsicHeight);
    void applyReplacedMinMax(float w, float h);

    virtual void resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr&clip) = 0;
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context) {
        return true;
    }

    virtual float getMCW() const {
        return 0.0f;
    }

    bool isVisible() const {
        return visibility == CSSVisibilityValueImp::Visible;
    }

    virtual void render(ViewCSSImp* view, StackingContext* stackingContext) = 0;
    void renderBorder(ViewCSSImp* view, float left, float top,
                      const CSSStyleDeclarationPtr& style, unsigned backgroundColor, BoxImage* backgroundImage,
                      float ll, float lr, float rl, float rr, float tt, float tb, float bt, float bb,
                      const BoxPtr& leftEdge, const BoxPtr& rightEdge);
    void renderBorder(ViewCSSImp* view, float left, float top);
    void renderOutline(ViewCSSImp* view, float left, float top, float right, float bottom, float outlineWidth, unsigned outline, unsigned color);
    void renderOutline(ViewCSSImp* view, float left, float top);

    virtual void dump(std::string indent = "") = 0;

    bool needLayout() const {
        return flags & (NEED_REFLOW | NEED_CHILD_REFLOW);
    }

    unsigned short getFlags() const {
        return flags;
    }
    void setFlags(unsigned short f);
    void clearFlags(unsigned short f = 0xffff);
    unsigned short gatherFlags() const;

    bool isInside(int u, int v) const {
        // TODO: InlineBox needs to be treated differently.
        float l = x + marginLeft;
        if (u < l)
            return false;
        float t = y + marginTop;
        if (v < t)
            return false;
        float r = l + getBorderWidth();
        if (r <= u)
            return false;
        float b = t + getBorderHeight();
        return v < b;
    }

    BoxPtr boxFromPoint(int x, int y, StackingContext* context = 0) {
        if (context && stackingContext && stackingContext != context)
            return 0;
        if (!isAnonymous() && Element::hasInstance(node)) {
            Element element = interface_cast<Element>(node);
            x += element.getScrollLeft();
            y += element.getScrollTop();
        }
        for (BoxPtr box = getFirstChild(); box; box = box->getNextSibling()) {
            if (BoxPtr target = box->boxFromPoint(x, y, context)) {
                if (!isClipped() || isInside(x, y))
                    return target;
            }
        }
        return isInside(x, y) ? self() : nullptr;
    }

    void updateScrollSize();
    void resetScrollSize();

    static void renderVerticalScrollBar(float w, float h, float pos, float total);
    static void renderHorizontalScrollBar(float w, float h, float pos, float total);
    static void unionRect(float& l, float& t, float& w, float& h,
                          float ll, float tt, float ww, float hh) {
        if (ll < l + w && l < ll + ww && tt < t + h && t < tt + hh) {
            if (l < ll) {
                w -= (ll - l);
                l = ll;
            } else
                ww -= (l - ll);
            if (ww < w)
                w = ww;
            if (t < tt) {
                h -= (tt - t);
                t = tt;
            } else
                hh -= (t - tt);
            if (hh < h)
                h = hh;
            return;
        }
        w = h = 0.0f;
    }

    static Element getContainingElement(Node node);
};

// paragraph
// ‘display’ of ‘block’, ‘list-item’, ‘table’, ‘table-*’ (i.e., all table boxes) or <template>.
class Block : public Box
{
    friend class ViewCSSImp;
    friend class FormattingContext;
    friend class TableWrapperBox;
    friend BoxPtr Box::removeChild(const BoxPtr& item);

    FormattingContext* formattingContext;
    unsigned textAlign;

    // for a collapsed-through box
    bool marginUsed;
    float topBorderEdge;
    float consumed;

    // for a floating box
    bool inserted;  // set to true if inserted in a linebox.
    float edge;
    float remainingHeight;

    // for an absolutely positioned box
    ContainingBlockPtr absoluteBlock;

    // A list of nodes to be formatted in line boxes; note if a block-level box contains
    // block-level boxes, 'inlines' must be empty.
    std::list<Node> inlines;
    Element floatingFirstLetter;
    std::map<Node, BlockPtr> blockMap;  // inline blocks, floating boxes, absolutely positioned boxes, etc. held by line boxes
    TableWrapperBoxPtr anonymousTable;  // for ViewCSSImp::constructBlocks

    // The default baseline and line-height for the line boxes.
    float defaultBaseline;
    float defaultLineHeight;

    // for automatic table layout
    float mcw;

    // for reflow
    SavedFormattingContext savedFormattingContext;
    float scrollWidth;
    float scrollHeight;

    void getPsuedoStyles(ViewCSSImp* view, FormattingContext* context, const CSSStyleDeclarationPtr& style,
                         CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle);
    void nextLine(ViewCSSImp* view, FormattingContext* context, CSSStyleDeclarationPtr& activeStyle,
                  CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle,
                  const CSSStyleDeclarationPtr& style, bool linefeed, FontTexture*& font, float& point);
    size_t layOutFloatingFirstLetter(ViewCSSImp* view, FormattingContext* context, const std::u16string& data, const CSSStyleDeclarationPtr& firstLetterStyle);
    float measureText(ViewCSSImp* view, const CSSStyleDeclarationPtr& activeStyle,
                      const char16_t* text, size_t length, float point, bool isFirstCharacter,
                      FontGlyph*& glyph, std::u16string& transformed);
    bool layOutText(ViewCSSImp* view, Node text, FormattingContext* context,
                    std::u16string data, Element element, const CSSStyleDeclarationPtr& style);
    void layOutInlineBlock(ViewCSSImp* view, Node node, const BlockPtr& inlineBlock, FormattingContext* context);
    void layOutFloat(ViewCSSImp* view, Node node, const BlockPtr&floatBox, FormattingContext* context);
    void layOutAbsolute(ViewCSSImp* view, Node node, const BlockPtr&absBox, FormattingContext* context);  // 1st pass
    bool layOutInline(ViewCSSImp* view, FormattingContext* context, float originalMargin = 0.0f);
    void layOutInlineBlocks(ViewCSSImp* view);
    void layOutChildren(ViewCSSImp* view, FormattingContext* context);

    bool layOutReplacedElement(ViewCSSImp* view, Element element, const CSSStyleDeclarationPtr& style);

    void applyMinMaxHeight(FormattingContext* context);

    float getBaseline(const BoxPtr& box) const;

    void notifyBackground(Document document);

protected:
    // resolveAbsoluteWidth's return values
    enum {
        Left = 4u,
        Width = 2u,
        Right = 1u,
        Top = 32u,
        Height = 16u,
        Bottom = 8u
    };

    float collapseMarginTop(FormattingContext* context);
    bool undoCollapseMarginTop(FormattingContext* context, float before);
    void collapseMarginBottom(FormattingContext* context);
    void adjustCollapsedThroughMargins(FormattingContext* context);
    void moveUpCollapsedThroughMargins(FormattingContext* context);

public:
    Block(Node node = nullptr, const CSSStyleDeclarationPtr& style = nullptr);
    virtual ~Block();

    BlockPtr self() const {
        return std::const_pointer_cast<Block>(std::static_pointer_cast<const Block>(shared_from_this()));
    }

    virtual unsigned getBoxType() const {
        return BLOCK_LEVEL_BOX;
    }
    bool isFlowRoot() const {
        return formattingContext;
    }

    FormattingContext* updateFormattingContext(FormattingContext* context);
    FormattingContext* restoreFormattingContext(FormattingContext* context);
    FormattingContext* establishFormattingContext();

    void clearInlines();

    void shrinkToFit(FormattingContext* context);
    virtual float shrinkTo();
    virtual void fit(float w, FormattingContext* context);

    virtual const BoxPtr towardViewPort(const BoxPtr& child, float& x, float& y) const {
        if (const BoxPtr box = child->getPreviousSibling()) {
            x -= box->getBlankLeft() + box->offsetH;
            y += box->height + box->getBlankBottom() - box->offsetV;
            return box;
        }
        return self();
    }

    float getBaseline() const;
    float getTopBorderEdge() const {
        return topBorderEdge;
    }

    unsigned getTextAlign() const {
        return textAlign;
    }

    bool hasInline() const {
        return !inlines.empty();
    }
    void insertInline(Node node) {
        inlines.push_back(node);
    }
    void spliceInline(const BlockPtr& box) {
        inlines.splice(inlines.begin(), box->inlines);
    }

    void addBlock(const Node& node, const BlockPtr& block) {
        blockMap[node] = block;
    }
    BlockPtr findBlock(const Node& node) {
        BlockPtr box = self();
        do {
            auto i = box->blockMap.find(node);
            if (i != box->blockMap.end())
                return i->second;
        } while (box->isAnonymous() && (box = std::dynamic_pointer_cast<Block>(box->getParentBox())));
        return 0;
    }
    bool removeBlock(const Node& node) {
        BlockPtr box = self();
        do {
            auto i = box->blockMap.find(node);
            if (i != box->blockMap.end()) {
                box->blockMap.erase(i);
                return true;
            }
        } while (box->isAnonymous() && (box = std::dynamic_pointer_cast<Block>(box->getParentBox())));
        return false;
    }
    void clearBlocks() {
        blockMap.clear();
    }

    virtual bool isFloat() const;
    virtual bool isClipped() const {
        return !isAnonymous() && style && style->overflow.isClipped();
    }
    bool canScroll() const {
        // Note the root box is scrolled by the viewport.
        return getParentBox() && !isAnonymous() && style && style->overflow.canScroll();
    }

    bool isCollapsedThrough() const;
    float getInternalClearances() const;

    virtual ContainingBlockPtr getContainingBlock(ViewCSSImp* view) const;
    void setContainingBlock(ViewCSSImp* view);

    unsigned resolveAbsoluteWidth(const ContainingBlockPtr& containingBlock, float& left, float& right, float r = NAN);
    unsigned applyAbsoluteMinMaxWidth(const ContainingBlockPtr& containingBlock, float& left, float& right, unsigned autoMask);
    unsigned resolveAbsoluteHeight(const ContainingBlockPtr& containingBlock, float& top, float& bottom, float r = NAN);
    unsigned applyAbsoluteMinMaxHeight(const ContainingBlockPtr& containingBlock, float& top, float& bottom, unsigned autoMask);

    bool hasAnonymousBox(const BoxPtr& prev) const;
    // Gets the anonymous child box. Creates one if there's none even
    // if there's no children; if so, the existing texts are moved to the
    // new anonymous box.
    BlockPtr getAnonymousBox(const BoxPtr& prev);

    bool isCollapsableInside() const;
    bool isCollapsableOutside() const;

    void resolveBackground(ViewCSSImp* view);
    void resolveBackgroundPosition(ViewCSSImp* view, const ContainingBlockPtr& containingBlock);
    void resolveHeight();
    virtual float resolveWidth(float w, FormattingContext* context, float r = NAN);
    float setWidth(float w, float maxWidth, float minWidth);
    void resolveFloatWidth(float w, float maxWidth, float minWidth);

    virtual float getMCW() const {
        return mcw;
    }

    virtual void resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr&clip);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view, StackingContext* stackingContext);
    virtual void dump(std::string indent = "");

    virtual void layOutAbsolute(ViewCSSImp* view);  // 2nd pass
    void layOutAbsoluteEnd(float left, float top);

    unsigned renderBegin(ViewCSSImp* view, bool noBorder = false);
    void renderEnd(ViewCSSImp* view, unsigned overflow, bool scrollBar = true);
    void renderNonInline(ViewCSSImp* view, StackingContext* stackingContext);
    void renderInline(ViewCSSImp* view, StackingContext* stackingContext);

    bool isTableBox() const;
    float updateMCW(float w) {
        mcw = std::max(w, mcw);
        return mcw;
    }

    float getScrollWidth() const {
        return scrollWidth;
    }
    float getScrollHeight() const {
        return scrollHeight;
    }
    void updateScrollWidth(float r);
    void updateScrollHeight(float b);
    void resetScrollWidth();
    void resetScrollHeight();
};

// line of text
// ‘display’ of ‘inline’, ‘inline-block’, ‘inline-table’ or ‘ruby’.
class LineBox : public Box
{
    friend class Block;
    friend class FormattingContext;

    float baseline;
    float underlinePosition;
    float underlineThickness;
    float lineThroughPosition;
    float lineThroughThickness;

    float leftGap;    // the gap between the first inline box and the last left floating box
    float rightGap;   // the gap between the last inline box and the 1st right floating box
    std::weak_ptr<Block> rightBox;  // the 1st right floating box

public:
    LineBox(const CSSStyleDeclarationPtr& style);

    LineBoxPtr self() const {
        return std::const_pointer_cast<LineBox>(std::static_pointer_cast<const LineBox>(shared_from_this()));
    }

    virtual unsigned getBoxType() const {
        return LINE_BOX;
    }

    virtual const BoxPtr towardViewPort(const BoxPtr& child, float& x, float& y) const {
        for (BoxPtr box = child->getPreviousSibling(); box; box = box->getPreviousSibling()) {
            if (box->isAbsolutelyPositioned())
                continue;
            x += box->width + box->getBlankRight() - box->offsetH;
            y -= box->getBlankTop() + box->offsetV;
            return box;
        }
        return self();
    }

    float getBaseline() const {
        return baseline;
    }
    float getUnderlinePosition() const {
        return underlinePosition;
    }
    float getUnderlineThickness() const {
        return underlineThickness;
    }
    float getLineThroughPosition() const {
        return lineThroughPosition;
    }
    float getLineThroughThickness() const {
        return lineThroughThickness;
    }

    // Returns true if this box contains other than floating boxes or absolutely positioned boxes.
    bool hasInlineBox() const {
        return height != 0.0f;  // TODO: Check whether this is correct.
    }

    BlockPtr getRightBox() const {
        return rightBox.lock();
    }

    virtual void resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr&clip);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view, StackingContext* stackingContext);
    virtual void dump(std::string indent);
    virtual float shrinkTo();
    virtual void fit(float w, FormattingContext* context);
};

// words inside a line
class InlineBox : public Box
{
    friend class Block;
    friend class FormattingContext;

    FontTexture* font;
    float point;
    float baseline;
    float leading;
    std::u16string data;

    size_t wrap;
    float wrapWidth;

    int emptyInline;    // 0: none, 1: first, 2: last, 3: both, 4: empty

    void renderText(ViewCSSImp* view, const std::u16string& data, float point);
    void renderMultipleBackground(ViewCSSImp* view);
    void renderEmptyBox(ViewCSSImp* view, const CSSStyleDeclarationPtr& parentStyle);

public:
    InlineBox(Node node, const CSSStyleDeclarationPtr& style);

    InlineBoxPtr self() const {
        return std::const_pointer_cast<InlineBox>(std::static_pointer_cast<const InlineBox>(shared_from_this()));
    }

    virtual unsigned getBoxType() const {
        return INLINE_LEVEL_BOX;
    }

    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        // In the case of the inline-block, InlineBox holds a block-level box
        // as its only child.
        return this;
    }

    virtual bool isAnonymous() const {
        return !style || (font && !style->display.isInline()) || !style->display.isInlineLevel();
    }
    bool isInline() const {
        return style && style->display.isInline();
    }

    bool isEmptyInlineAtFirst(const CSSStyleDeclarationPtr& style, Element& element, Node& node);
    bool isEmptyInlineAtLast(const CSSStyleDeclarationPtr& style, Element& element, Node& node);

    void clearBlankLeft() {
        marginLeft = paddingLeft = borderLeft = 0.0f;
    }
    void clearBlankRight() {
        marginRight = paddingRight = borderRight = 0.0f;
    }

    size_t getWrap() const {
        return wrap;
    }
    bool hasWrapBox() const {
        // Check font to skip inline-block, etc.
        return font && (data.empty() || wrap < data.length());
    }
    std::u16string getWrapText() const {
        return data.substr(wrap);
    }
    InlineBoxPtr split();

    // has non-zero margins, padding, or borders?
    bool hasHeight() const {
        return 0.0f < getBorderWidth() || 0.0f < getBorderHeight() || hasMargins();
    }

    float getLeading() const {
        return leading;
    }

    float atEndOfLine();

    FontTexture* getFont() const {
        return font;
    }
    float getPoint() const {
        return point;
    }

    float getBaseline() const {
        return baseline;
    }
    float getSub() const;
    float getSuper() const;

    void resolveWidth();
    void setData(FontTexture* font, float point, const std::u16string& data, size_t wrap, float wrapWidth);
    const std::u16string& getData() const {
        return data;
    }
    virtual void resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr&clip);
    virtual void render(ViewCSSImp* view, StackingContext* stackingContext);
    void renderOutline(ViewCSSImp* view);

    virtual void dump(std::string indent);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSBOX_H
