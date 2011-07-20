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
    void nextLine(BlockLevelBox* parentBox);
    void updateRemainingHeight(float height);
    void clear(Box* box, unsigned value);
};

class BoxImage
{
public:
    static const int Unavailable = 0;
    static const int Sent = 1;
    static const int PartiallyAvailable = 2;
    static const int CompletelyAvailable = 3;
    static const int Broken = 4;

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
    virtual ~BoxImage() {
        delete pixels;
    }
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
    float marginBottom;
    float marginLeft;
    float marginRight;
    float paddingTop;
    float paddingBottom;
    float paddingLeft;
    float paddingRight;
    float borderTop;
    float borderBottom;
    float borderLeft;
    float borderRight;

    float offsetH;
    float offsetV;

    // background
    unsigned backgroundColor;
    BoxImage* backgroundImage;
    float backgroundLeft;
    float backgroundTop;

    CSSStyleDeclarationPtr style;
    FormattingContext* formattingContext;

    unsigned flags;

    ViewCSSImp* shadow;

    void updatePadding();
    void updateBorderWidth();

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

    Box* removeChild(Box* item);
    Box* insertBefore(Box* item, Box* after);
    Box* appendChild(Box* item);

    Box* getParentBox() const;
    bool hasChildBoxes() const;
    Box* getFirstChild() const;
    Box* getLastChild() const;
    Box* getPreviousSibling() const;
    Box* getNextSibling() const;

    float getBlankLeft() const {
        return marginLeft + borderLeft + paddingLeft;
    }
    float getBlankRight() const {
        return marginRight + borderRight + paddingRight;
    }
    float getTotalWidth() const {
        return marginLeft + borderLeft + paddingLeft + width + marginRight + borderRight + paddingRight;
    }
    float getPaddingWidth() const {
        return paddingLeft + width + marginRight;
    }
    float getBlankTop() const {
        return marginTop + borderTop + paddingTop;
    }
    float getBlankBottom() const {
        return marginBottom + borderBottom + paddingBottom;
    }
    float getTotalHeight() const {
        return marginTop + borderTop + paddingTop + height + marginBottom + borderBottom + paddingBottom;
    }
    float getPaddingHeight() const {
        return paddingTop + height + marginBottom;
    }

    virtual void toViewPort(const Box* box, float& x, float& y) const = 0;
    void toViewPort(float& x, float& y) const {
        // TODO: make this work for abs box as well
        x += offsetH + getBlankLeft();
        y += offsetV + getBlankTop();
        if (Box* box = getParentBox())
            box->toViewPort(this, x, y);
    }

    virtual Box* toBox(int& x, int& y) const = 0;

    CSSStyleDeclarationImp* getStyle() const {
        return style.get();
    }
    void setStyle(CSSStyleDeclarationImp* style) {
        this->style = style;
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

    virtual void resolveOffset(ViewCSSImp* view);

    virtual void layOut(ViewCSSImp* view, FormattingContext* context) {}

    virtual void render(ViewCSSImp* view) = 0;
    void renderBorder(ViewCSSImp* view);

    virtual void dump(ViewCSSImp* view, std::string indent = "") = 0;

    void setFlags(unsigned f) {
        flags |= f;
    }
    bool isFlagged() const {
        unsigned f = flags;
        for (const Box* i = firstChild; i; i = i->nextSibling)
            f |= i->isFlagged();
        return f;
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

    void layOutText(ViewCSSImp* view, Text text, FormattingContext* context);
    void layOutInlineReplaced(ViewCSSImp* view, Node node, FormattingContext* context);
    void layOutFloat(ViewCSSImp* view, Node node, BlockLevelBox* floatBox, FormattingContext* context);
    void layOutAbsolute(ViewCSSImp* view, Node node, BlockLevelBox* absBox, FormattingContext* context);  // 1st pass
    void layOutInline(ViewCSSImp* view, FormattingContext* context);

public:
    BlockLevelBox(Node node = 0, CSSStyleDeclarationImp* style = 0) :
        Box(node),
        textAlign(CSSTextAlignValueImp::Default),
        edge(0.0f),
        remainingHeight(0.0f)
    {
        this->style = style;
    }

    virtual unsigned getBoxType() const {
        return BLOCK_LEVEL_BOX;
    }

    virtual void toViewPort(const Box* box, float& x, float& y) const {
        x += offsetH + getBlankLeft();
        y += offsetV + getBlankTop();
        for (auto i = box->getPreviousSibling(); i; i = i->getPreviousSibling())
            y += i->getTotalHeight();
        if (box = getParentBox())
            box->toViewPort(this, x, y);
    }

    virtual Box* toBox(int& x, int& y) const {
        x -= offsetH + getBlankLeft();
        y -= offsetV + getBlankTop();
        int cy = 0;
        int top = 0;
        int bottom = 0;
        for (auto i = getFirstChild(); i; i = i->getNextSibling()) {
            int h = i->getTotalHeight();
            top = cy + i->offsetV;
            cy += h;
            bottom = top + h;
            if (top <= y && y < bottom) {
                y -= top;
                return i;  // TODO: check left, right as well.
            }
        }
        return 0;
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

    void resolveAbsoluteWidth(const ContainingBlock* containingBlock);
    void layOutAbsolute(ViewCSSImp* view, Node node);  // 2nd pass

    // Gets the first child box which is an anonymous box. Creates one if there's none
    // even if there's no children; if so, the existing texts are moved to the
    // new anonymous box.
    BlockLevelBox* getAnonymousBox();

    void resolveWidth(ViewCSSImp* view, const ContainingBlock* containingBlock, float available = 0);
    virtual void layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent = "");
};

typedef boost::intrusive_ptr<BlockLevelBox> BlockLevelBoxPtr;

// line of text
// ‘display’ of ‘inline’, ‘inline-block’, ‘inline-table’ or ‘ruby’.
class LineBox : public Box
{
    friend class BlockLevelBox ;

    float baseline;
public:
    virtual unsigned getBoxType() const {
        return LINE_BOX;
    }

    virtual void toViewPort(const Box* box, float& x, float& y) const {
        for (auto i = box->getPreviousSibling(); i; i = i->getPreviousSibling()) {
            if (!i->isAbsolutelyPositioned())
                x += i->getTotalWidth();
        }
        if (box = getParentBox())
            box->toViewPort(this, x, y);
    }

    virtual Box* toBox(int& x, int& y) const {
        int cx = 0;
        int left = 0;
        int right = 0;
        for (auto i = getFirstChild(); i; i = i->getNextSibling()) {
            if (i->isAbsolutelyPositioned())  // TODO: support absolutely positioned boxes
                continue;
            int w = i->getTotalWidth();
            left = cx + i->offsetH;
            cx += w;
            right = left + w;
            if (left <= x && x < right) {
                x -= left;
                return i;  // TODO: check top, bottom as well.
            }
        }
        return 0;

    }

    float getBaseline() const {
        return baseline;
    }
    virtual void layOut(ViewCSSImp* view, FormattingContext* context);
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent);
    LineBox() :
        Box(0),
        baseline(0.0f) {
    }
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
    virtual unsigned getBoxType() const {
        return INLINE_LEVEL_BOX;
    }

    virtual void toViewPort(const Box* box, float& x, float& y) const;

    virtual Box* toBox(int& x, int& y) const {
        return 0;
    }

    virtual bool isAnonymous() const;

    float getBaseline() const {
        return baseline;
    }
    void resolveWidth();
    virtual void resolveOffset(ViewCSSImp* view);
    void setData(FontTexture* font, float point, std::u16string data);
    std::u16string getData() const {
        return data;
    }
    virtual void render(ViewCSSImp* view);
    virtual void dump(ViewCSSImp* view, std::string indent);
    InlineLevelBox(Node node, CSSStyleDeclarationImp* style) :
        Box(node),
        font(0),
        point(0.0f),
        baseline(0.0f) {
        this->style = style;
    }
};

typedef boost::intrusive_ptr<InlineLevelBox> InlineLevelBoxPtr;

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_CSSBOX_H
