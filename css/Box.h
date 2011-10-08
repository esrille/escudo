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

#ifndef ES_CSSBOX_H
#define ES_CSSBOX_H

// cf. http://www.w3.org/TR/css3-box/

#include <Object.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/html/HTMLImageElement.h>

#include <list>
#include <string>

#include <boost/intrusive_ptr.hpp>

#include "http/HTTPRequest.h"
#include "css/CSSStyleDeclarationImp.h"

class FontTexture;

namespace org { namespace w3c { namespace dom {

class Element;

namespace bootstrap {

class Box;
class LineBox;
class BlockLevelBox;
class StackingContext;

class ViewCSSImp;

class ContainingBlock : public ObjectMixin<ContainingBlock>
{
public:
    ContainingBlock() :
        width(0.0f),
        height(0.0f)
    {
    }
    virtual ~ContainingBlock() {}

    float width;
    float height;

    float getWidth() const {
        return width;
    }
    float getHeight() const {
        return height;
    }
};

class FormattingContext
{
    friend class BlockLevelBox;

    LineBox* lineBox;
    float x;
    float leftover;
    char16_t prevChar;
    std::list<BlockLevelBox*> left;   // active float boxes at the left side
    std::list<BlockLevelBox*> right;  // active float boxes at the right side
    std::list<Node> floatNodes;       // float boxes not layed out yet

public:
    FormattingContext() :
        lineBox(0),
        x(0.0f),
        leftover(0.0f),
        prevChar(0) {
    }
    LineBox* addLineBox(ViewCSSImp* view, BlockLevelBox* parentBox);
    void addFloat(BlockLevelBox* floatBox, float totalWidth);
    float getLeftEdge() const;
    float getRightEdge() const;
    float getLeftRemainingHeight() const;
    float getRightRemainingHeight() const;
    bool shiftDownLineBox();
    void nextLine(BlockLevelBox* parentBox, bool moreFloats = false);
    void updateRemainingHeight(float height);
    float clear(unsigned value);
};

class BoxImage
{
public:
    static const int Unavailable = 0;
    static const int Sent = 1;
    static const int PartiallyAvailable = 2;
    static const int CompletelyAvailable = 3;
    static const int Broken = 4;

    static const unsigned RepeatS = 1;
    static const unsigned RepeatT = 2;
    static const unsigned Clamp = 4;

private:
    Box* box;
    int state;
    unsigned char* pixels;  // in argb32 format
    unsigned naturalWidth;
    unsigned naturalHeight;
    unsigned repeat;
    unsigned format;
    mutable html::HTMLImageElement img;
    HttpRequest request;

    void open(const std::u16string& url);
    void notify();

public:
    BoxImage(Box* box = 0);
    BoxImage(Box* box, const std::u16string& base, const std::u16string& url, unsigned repeat = 0);
    BoxImage(Box* box, const std::u16string& base, html::HTMLImageElement& img);
    ~BoxImage();

    int getState() const {
        return state;
    }
    unsigned getWidth() const {
        unsigned w = 0;
        if (img)
            w = img.getWidth();
        return w ? w : naturalWidth;    // TODO: make it proportional
    }
    unsigned getHeight() const {
        unsigned h = 0;
        if (img)
            h = img.getHeight();
        return h ? h : naturalHeight;   // TODO: make it proportional
    }
    unsigned getNaturalWidth() const {
        return naturalWidth;
    }
    unsigned getNaturalHeight() const {
        return naturalHeight;
    }
    void render(ViewCSSImp* view, float x, float y, float width, float height, float left, float top);
};

class Box : public ContainingBlock
{
    friend class ViewCSSImp;
    friend class BlockLevelBox;
    friend class LineBox;
    friend class FormattingContext;
    friend class BoxImage;
    friend class StackingContext;

public:
    static const unsigned short BLOCK_LEVEL_BOX = 1;
    static const unsigned short LINE_BOX = 2;
    static const unsigned short INLINE_LEVEL_BOX = 3;
protected:
    Node node;
    Box* parentBox;
    Box* firstChild;
    Box* lastChild;
    Box* previousSibling;
    Box* nextSibling;
    unsigned int childCount;

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

    float offsetH;
    float offsetV;

    bool positioned;
    StackingContext* stackingContext;

    float x;  // in screen coord
    float y;  // in screen coord

    // background
    unsigned backgroundColor;
    BoxImage* backgroundImage;
    float backgroundLeft;
    float backgroundTop;

    CSSStyleDeclarationPtr style;
    FormattingContext* formattingContext;

    unsigned flags;

    ViewCSSImp* shadow;

    void renderBorderEdge(ViewCSSImp* view, int edge, unsigned borderStyle, unsigned color,
                          float a, float b, float c, float d,
                          float e, float f, float g, float h);
public:
    Box(Node node);
    virtual ~Box();

    virtual unsigned getBoxType() const = 0;

    virtual bool isAnonymous() const {
        return !node;
    }

    Node getNode() const {
        return node;
    }
    ViewCSSImp* getShadow() const {
        return shadow;
    }

    Node getTargetNode() const {
        const Box* box = this;
        do {
            if (box->node)
                return box->node;
        } while (box = box->parentBox);
        return 0;
    }

    Box* removeChild(Box* item);
    Box* insertBefore(Box* item, Box* after);
    Box* appendChild(Box* item);

    Box* getParentBox() const;
    bool hasChildBoxes() const;
    Box* getFirstChild() const;
    Box* getLastChild() const;
    Box* getPreviousSibling() const;
    Box* getNextSibling() const;

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
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

    virtual float shrinkTo();
    virtual void fit(float w) {}

    void toViewPort(float& x, float& y) const {
        const Box* box = this;
        do {
            box = box->towardViewPort(x, y);
        } while (box);
    }
    const Box* towardViewPort(float& x, float& y) const {
        x += offsetH + getBlankLeft();
        y += offsetV + getBlankTop();
        if (const Box* box = getParentBox())
            return box->towardViewPort(this, x, y);
        return 0;
    }
    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        return this;
    }

    CSSStyleDeclarationImp* getStyle() const {
        return style.get();
    }
    void setStyle(CSSStyleDeclarationImp* style) {
        this->style = style;
        if (style) {
            positioned = style->isPositioned();
            stackingContext = style->getStackingContext();
        }
    }

    virtual const ContainingBlock* getContainingBlock(ViewCSSImp* view) const;

    FormattingContext* updateFormattingContext(FormattingContext* context);
    FormattingContext* establishFormattingContext() {
        if (!formattingContext);
            formattingContext = new(std::nothrow) FormattingContext;
        return formattingContext;
    }
    bool isFlowRoot() const {
        return formattingContext;
    }
    bool isFlowOf(const Box* floatRoot) const;

    virtual bool isAbsolutelyPositioned() const {
        return false;
    }

    void updatePadding();
    void updateBorderWidth();

    void resolveOffset(CSSStyleDeclarationImp* style);
    virtual void resolveOffset(ViewCSSImp* view);
    virtual void resolveXY(ViewCSSImp* view, float left, float top) = 0;
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context) {
        return true;
    }

    virtual void render(ViewCSSImp* view) = 0;
    void renderBorder(ViewCSSImp* view, float left, float top);

    virtual void dump(ViewCSSImp* view, std::string indent = "") = 0;

    void setFlags(unsigned f) {
        flags |= f;
    }
    void clearFlags() {
        flags = 0;
        for (Box* i = firstChild; i; i = i->nextSibling)
            i->clearFlags();
    }
    unsigned getFlags() const {
        unsigned f = flags;
        for (const Box* i = firstChild; i; i = i->nextSibling)
            f |= i->getFlags();
        return f;
    }

    bool isInside(int s, int t) const {
        if (s < x || t < y || x + width <= s || y + height <= t)
            return false;
        return true;
    }

    Box* boxFromPoint(int x, int y) {
        for (Box* box = getFirstChild(); box; box = box->getNextSibling()) {
            if (Box* target = box->boxFromPoint(x, y))
                return target;
        }
        return isInside(x, y) ? this : 0;
    }
};

typedef boost::intrusive_ptr<Box> BoxPtr;

// paragraph
// ‘display’ of ‘block’, ‘list-item’, ‘table’, ‘table-*’ (i.e., all table boxes) or <template>.
class BlockLevelBox : public Box
{
    friend class FormattingContext;

    unsigned textAlign;

    // for float box
    float edge;
    float remainingHeight;

    // for abs boxes
    Retained<ContainingBlock> absoluteBlock;

    // A block-level box may contain either line boxes or block-level boxes, but not both.
    std::list<Node> inlines;
    unsigned treeOrder;

    void nextLine(ViewCSSImp* view, FormattingContext* context, CSSStyleDeclarationImp*& activeStyle,
                  CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle,
                  CSSStyleDeclarationImp* style, FontTexture*& font, float& point);
    bool layOutText(ViewCSSImp* view, Node text, FormattingContext* context,
                    std::u16string data, Element element, CSSStyleDeclarationImp* style);
    void layOutInlineReplaced(ViewCSSImp* view, Node node, FormattingContext* context,
                              Element element, CSSStyleDeclarationImp* style);
    void layOutFloat(ViewCSSImp* view, Node node, BlockLevelBox* floatBox, FormattingContext* context);
    void layOutAbsolute(ViewCSSImp* view, Node node, BlockLevelBox* absBox, FormattingContext* context);  // 1st pass
    bool layOutInline(ViewCSSImp* view, FormattingContext* context, float originalMargin = 0.0f);
    void layOutChildren(ViewCSSImp* view, FormattingContext* context);

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

public:
    BlockLevelBox(Node node = 0, CSSStyleDeclarationImp* style = 0) :
        Box(node),
        textAlign(CSSTextAlignValueImp::Default),
        edge(0.0f),
        remainingHeight(0.0f),
        treeOrder(0)
    {
        setStyle(style);
    }

    virtual unsigned getBoxType() const {
        return BLOCK_LEVEL_BOX;
    }

    void shrinkToFit();
    virtual float shrinkTo();
    virtual void fit(float w);

    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        if (const Box* box = child->getPreviousSibling()) {
            x -= box->getBlankLeft() + box->offsetH;
            y += box->height + box->getBlankBottom() - box->offsetV;
            return box;
        }
        return this;
    }

    unsigned getTextAlign() const {
        return textAlign;
    }

    bool hasInline() const {
        return !inlines.empty();
    }
    void insertInline(Node node) {
        inlines.push_front(node);
    }
    void spliceInline(BlockLevelBox* box) {
        inlines.splice(inlines.begin(), box->inlines);
    }
    unsigned getTreeOrder() const {
        return treeOrder;
    }

    virtual bool isAbsolutelyPositioned() const;
    bool isFloat() const;
    bool isFixed() const;

    virtual const ContainingBlock* getContainingBlock(ViewCSSImp* view) const {
        if (isAbsolutelyPositioned())
            return &absoluteBlock;
        if (getParentBox() && getParentBox()->getBoxType() == Box::INLINE_LEVEL_BOX)  // inline-block?
            return getParentBox();
        return Box::getContainingBlock(view);
    }
    void setContainingBlock(ViewCSSImp* view);

    unsigned resolveAbsoluteWidth(const ContainingBlock* containingBlock, float& right, float& bottom);
    void layOutAbsolute(ViewCSSImp* view, Node node);  // 2nd pass

    // Gets the first child box which is an anonymous box. Creates one if there's none
    // even if there's no children; if so, the existing texts are moved to the
    // new anonymous box.
    bool hasAnonymousBox()
    {
        return firstChild && firstChild->isAnonymous();
    }
    BlockLevelBox* getAnonymousBox();

    void resolveWidth(ViewCSSImp* view, const ContainingBlock* containingBlock, float available = 0);
    void resolveBackground(ViewCSSImp* view);
    void resolveMargin(ViewCSSImp* view, const ContainingBlock* containingBlock, float available);
    void resolveWidth(float w);
    void resolveNormalWidth(float w, float r = NAN);
    void resolveFloatWidth(float w, float r = NAN);
    float collapseMarginTop(FormattingContext* context);
    void collapseMarginBottom();

    virtual void resolveOffset(ViewCSSImp* view);
    virtual void resolveXY(ViewCSSImp* view, float left, float top);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent = "");

    unsigned renderBegin(ViewCSSImp* view);
    void renderEnd(ViewCSSImp* view, unsigned overflow);
    void renderContent(ViewCSSImp* view);
};

typedef boost::intrusive_ptr<BlockLevelBox> BlockLevelBoxPtr;

// line of text
// ‘display’ of ‘inline’, ‘inline-block’, ‘inline-table’ or ‘ruby’.
class LineBox : public Box
{
    friend class BlockLevelBox ;

    float baseline;
    float underlinePosition;
    float underlineThickness;

public:
    LineBox(CSSStyleDeclarationImp* style) :
        Box(0),
        baseline(0.0f),
        underlinePosition(0.0f),
        underlineThickness(1.0f)
    {
        setStyle(style);
        // Keep 'height' 0.0f here since float and positioned elements are
        // also placed in line boxes in this implementation.
    }

    virtual unsigned getBoxType() const {
        return LINE_BOX;
    }

    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        for (const Box* box = child->getPreviousSibling(); box; box = box->getPreviousSibling()) {
            if (box->isAbsolutelyPositioned())
                continue;
            x += box->width + box->getBlankRight() - box->offsetH;
            y -= box->getBlankTop() + box->offsetV;
            return box;
        }
        return this;
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

    virtual void resolveXY(ViewCSSImp* view, float left, float top);
    virtual bool layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent);
    virtual void fit(float w);
};

typedef boost::intrusive_ptr<LineBox> LineBoxPtr;

// words inside a line
class InlineLevelBox : public Box
{
    friend class BlockLevelBox ;

    FontTexture* font;
    float point;
    float baseline;
    std::u16string data;

public:
    InlineLevelBox(Node node, CSSStyleDeclarationImp* style) :
        Box(node),
        font(0),
        point(0.0f),
        baseline(0.0f)
    {
        setStyle(style);
    }

    virtual unsigned getBoxType() const {
        return INLINE_LEVEL_BOX;
    }

    virtual const Box* towardViewPort(const Box* child, float& x, float& y) const {
        // In the case of the inline-block, InlineLevelBox holds a block-level box
        // as its only child.
        return this;
    }

    virtual bool isAnonymous() const;

    void atEndOfLine();

    float getBaseline() const {
        return baseline;
    }
    void resolveWidth();
    virtual void resolveOffset(ViewCSSImp* view);
    void setData(FontTexture* font, float point, std::u16string data);
    std::u16string getData() const {
        return data;
    }
    virtual void resolveXY(ViewCSSImp* view, float left, float top);
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent);
};

typedef boost::intrusive_ptr<InlineLevelBox> InlineLevelBoxPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSBOX_H
