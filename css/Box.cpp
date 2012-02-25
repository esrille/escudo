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

#include <algorithm>
#include <new>
#include <iostream>

#include <Object.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/html/HTMLIFrameElement.h>
#include <org/w3c/dom/html/HTMLImageElement.h>
#include <org/w3c/dom/html/HTMLDivElement.h>

#include "CSSSerialize.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSTokenizer.h"
#include "StackingContext.h"
#include "ViewCSSImp.h"
#include "WindowImp.h"

#include "Table.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

Box::Box(Node node) :
    node(node),
    parentBox(0),
    firstChild(0),
    lastChild(0),
    previousSibling(0),
    nextSibling(0),
    childCount(0),
    clearance(NAN),
    marginTop(0.0f),
    marginBottom(0.0f),
    marginLeft(0.0f),
    marginRight(0.0f),
    paddingTop(0.0f),
    paddingBottom(0.0f),
    paddingLeft(0.0f),
    paddingRight(0.0f),
    borderTop(0.0f),
    borderBottom(0.0f),
    borderLeft(0.0f),
    borderRight(0.0f),
    offsetH(0.0f),
    offsetV(0.0f),
    stackingContext(0),
    nextBase(0),
    intrinsic(false),
    x(0.0f),
    y(0.0f),
    clipBox(0),
    backgroundColor(0x00000000),
    backgroundImage(0),
    backgroundLeft(0.0f),
    backgroundTop(0.0f),
    style(0),
    formattingContext(0),
    flags(0),
    shadow(0)
{
}

Box::~Box()
{
    while (0 < childCount) {
        Box* child = removeChild(firstChild);
        child->release_();
    }
    delete backgroundImage;
}

Box* Box::removeChild(Box* item)
{
    Box* next = item->nextSibling;
    Box* prev = item->previousSibling;
    if (!next)
        lastChild = prev;
    else
        next->previousSibling = prev;
    if (!prev)
        firstChild = next;
    else
        prev->nextSibling = next;
    item->parentBox = 0;
    --childCount;
    return item;
}

Box* Box::insertBefore(Box* item, Box* after)
{
    if (!after)
        return appendChild(item);
    item->previousSibling = after->previousSibling;
    item->nextSibling = after;
    after->previousSibling = item;
    if (!item->previousSibling)
        firstChild = item;
    else
        item->previousSibling->nextSibling = item;
    item->parentBox = this;
    item->retain_();
    ++childCount;
    return item;
}

Box* Box::appendChild(Box* item)
{
    Box* prev = lastChild;
    if (!prev)
        firstChild = item;
    else
        prev->nextSibling = item;
    item->previousSibling = prev;
    item->nextSibling = 0;
    lastChild = item;
    item->parentBox = this;
    item->retain_();
    ++childCount;
    return item;
}

Box* Box::getParentBox() const
{
    return parentBox;
}

bool Box::hasChildBoxes() const
{
    return firstChild;
}

Box* Box::getFirstChild() const
{
    return firstChild;
}

Box* Box::getLastChild() const
{
    return lastChild;
}

Box* Box::getPreviousSibling() const
{
    return previousSibling;
}

Box* Box::getNextSibling() const
{
    return nextSibling;
}

float Box::getEffectiveTotalWidth() const
{
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/clear-float-002.htm
    float c = getClearance();
    float h = getTotalHeight();
    if (LineBox* lineBox = dynamic_cast<LineBox*>(parentBox))
        c += lineBox->getClearance();
    if (0.0f < c)
        h += c;
    return (h <= 0.0f) ? 0.0f : getTotalWidth();
}

void Box::updatePadding()
{
    paddingTop = style->paddingTop.getPx();
    paddingRight = style->paddingRight.getPx();
    paddingBottom = style->paddingBottom.getPx();
    paddingLeft = style->paddingLeft.getPx();
}

void Box::updateBorderWidth()
{
    borderTop = style->borderTopWidth.getPx();
    borderRight = style->borderRightWidth.getPx();
    borderBottom = style->borderBottomWidth.getPx();
    borderLeft = style->borderLeftWidth.getPx();
}

const ContainingBlock* Box::getContainingBlock(ViewCSSImp* view) const
{
    const Box* box = this;
    do {
        const Box* parent = box->getParentBox();
        if (!parent)
            return view->getInitialContainingBlock();
        box = parent;
    } while (box->getBoxType() != BLOCK_LEVEL_BOX);
    return box;
}

const ContainingBlock* BlockLevelBox::getContainingBlock(ViewCSSImp* view) const
{
    if (isAbsolutelyPositioned())
        return &absoluteBlock;
    return Box::getContainingBlock(view);
}

// We also calculate offsetH and offsetV here.
// TODO: Maybe it's better to visit ancestors via the box tree rather than the node tree.
//       cf. CSSContentValueImp::eval()
void BlockLevelBox::setContainingBlock(ViewCSSImp* view)
{
    assert(isAbsolutelyPositioned());
    if (!isFixed()) {
        assert(node);
        for (auto ancestor = node.getParentElement(); ancestor; ancestor = ancestor.getParentElement()) {
            CSSStyleDeclarationImp* style = view->getStyle(ancestor);
            if (!style)
                break;
            if (style->isPositioned()) {
                // Now we need to find the corresponding box for this ancestor.
                Box* box = style->box;
                assert(box);    // TODO: check NULL case
                offsetH = box->x + box->marginLeft + box->borderLeft - x;
                offsetV = box->y + box->marginTop + box->borderTop - y;
                clipBox = box->clipBox;
                if (BlockLevelBox* block = dynamic_cast<BlockLevelBox*>(box)) {
                    offsetV += block->topBorderEdge;
                    absoluteBlock.width = box->getPaddingWidth();
                    absoluteBlock.height = box->getPaddingHeight();
                    if (style->overflow.isClipped())
                        clipBox = block;
                } else {
                    assert(box->getBoxType() == INLINE_LEVEL_BOX);
                    if (Box* inlineBlock = box->getFirstChild()) {
                        absoluteBlock.width = inlineBlock->getPaddingWidth();
                        absoluteBlock.height = inlineBlock->getPaddingHeight();
                    } else {
                        Box* p = box->getParentBox();
                        float t = box->y - box->paddingTop;
                        float l = box->x - box->paddingLeft;
                        box = style->lastBox;
                        assert(box);
                        float b = box->y + box->height + box->paddingBottom;
                        float r = box->x + box->width + box->paddingRight;
                        absoluteBlock.width = r - l;
                        absoluteBlock.height = b - t;
                    }
                }
                return;
            }
        }
    }
    offsetH = -x;
    offsetV = -y;
    clipBox = 0;
    absoluteBlock.width = view->getInitialContainingBlock()->width;
    absoluteBlock.height = view->getInitialContainingBlock()->height;
}

FormattingContext* Box::updateFormattingContext(FormattingContext* context)
{
    if (isFlowRoot()) {
        assert(formattingContext);
        return formattingContext;
    } else {
        // adjust left and right blanks.
        context->marginLeft += marginLeft;
        context->marginRight += marginRight;
    }
    return context;
}

FormattingContext* Box::restoreFormattingContext(FormattingContext* context)
{
    if (isFlowRoot())
        return formattingContext;
    else {
        context->marginLeft -= marginLeft;
        context->marginRight -= marginRight;
    }
    return context;
}

bool Box::isFlowOf(const Box* flowRoot) const
{
    assert(flowRoot->isFlowRoot());
    for (const Box* box = this; box; box = box->getParentBox()) {
        if (box == flowRoot)
            return true;
    }
    return false;
}

// Calculate left, right, top, bottom for a 'relative' element.
// TODO: rtl
void Box::resolveOffset(CSSStyleDeclarationImp* style)
{
    if (style->position.isStatic())
        return;
    assert(style->position.isRelative());

    float h = 0.0f;
    if (!style->left.isAuto())
        h = style->left.getPx();
    else if (!style->right.isAuto())
        h = -style->right.getPx();
    offsetH += h;

    float v = 0.0f;
    if (!style->top.isAuto())
        v = style->top.getPx();
    else if (!style->bottom.isAuto())
        v = -style->bottom.getPx();
    offsetV += v;
}

void Box::resolveOffset(ViewCSSImp* view)
{
    if (isAnonymous())
        return;
    resolveOffset(getStyle());
}

BlockLevelBox::BlockLevelBox(Node node, CSSStyleDeclarationImp* style) :
    Box(node),
    textAlign(CSSTextAlignValueImp::Default),
    topBorderEdge(0.0f),
    consumed(0.0f),
    inserted(false),
    edge(0.0f),
    remainingHeight(0.0f)
{
    setStyle(style);
}

bool BlockLevelBox::isAbsolutelyPositioned() const
{
    return !isAnonymous() && style && style->isAbsolutelyPositioned();
}

bool BlockLevelBox::isFloat() const
{
    return !isAnonymous() && style && style->isFloat();
}

bool BlockLevelBox::isFixed() const
{
    return !isAnonymous() && style && style->position.getValue() == CSSPositionValueImp::Fixed;
}

BlockLevelBox* BlockLevelBox::getAnonymousBox()
{
    BlockLevelBox* anonymousBox;
    if (hasAnonymousBox()) {
        anonymousBox = dynamic_cast<BlockLevelBox*>(lastChild);
        if (anonymousBox)
            return anonymousBox;
    }
    anonymousBox = new(std::nothrow) BlockLevelBox;
    if (anonymousBox) {
        anonymousBox->spliceInline(this);
        appendChild(anonymousBox);
    }
    return anonymousBox;
}

void BlockLevelBox::resolveWidth(ViewCSSImp* view, const ContainingBlock* containingBlock, float available)
{
    assert(style);
    resolveBackground(view);
    updatePadding();
    updateBorderWidth();
    resolveMargin(view, containingBlock, available);
}

void BlockLevelBox::resolveBackground(ViewCSSImp* view)
{
    assert(style);
    backgroundColor = style->backgroundColor.getARGB();
    if (!style->backgroundImage.isNone()) {
        view->preload(view->getDocument().getDocumentURI(), style->backgroundImage.getValue());
        backgroundImage = new(std::nothrow) BoxImage(this, view->getDocument().getDocumentURI(), style->backgroundImage.getValue(), style->backgroundRepeat.getValue());
    }
}

void BlockLevelBox::resolveWidth(float w)
{
    resolveNormalWidth(w);
    applyMinMaxWidth(w);
}

void BlockLevelBox::applyMinMaxWidth(float w)
{
    if (!style->maxWidth.isNone()) {
        float maxWidth = style->maxWidth.getPx();
        if (maxWidth < width)
            resolveNormalWidth(w, maxWidth);
    }
    float minWidth = style->minWidth.getPx();
    if (width < minWidth)
        resolveNormalWidth(w, minWidth);
}

// Calculate width
//
// marginLeft + borderLeftWidth + paddingLeft + width + paddingRight + borderRightWidth + marginRight
// == containingBlock->width (- scrollbar width, if any)
void BlockLevelBox::resolveNormalWidth(float w, float r)
{
    if (isAnonymous()) {
        if (!isnan(r))
            width = r;
        else
            width = w;
        return;
    }

    int autoCount = 3;
    unsigned autoMask = Left | Width | Right;
    if (style) {
        if (style->isFloat() || style->isInlineBlock())
            return resolveFloatWidth(w, r);
        if (intrinsic) {
            --autoCount;
            autoMask &= ~Width;
            w -= width;
        } else if (!isnan(r)) {
            width = r;
            --autoCount;
            autoMask &= ~Width;
            w -= width;
        } else if (!style->width.isAuto()) {
            width = style->width.getPx();
            --autoCount;
            autoMask &= ~Width;
            w -= width;
        }
        if (!style->marginLeft.isAuto()) {
            marginLeft = style->marginLeft.getPx();
            --autoCount;
            autoMask &= ~Left;
            w -= marginLeft;
        }
        if (!style->marginRight.isAuto()) {
            marginRight = style->marginRight.getPx();
            --autoCount;
            autoMask &= ~Right;
            w -= marginRight;
        }
    }
    w -= borderLeft + paddingLeft + paddingRight + borderRight;
    if (w < 0.0f && !(autoMask & Width))
        w = 0.0f;
    switch (autoMask) {
    case Left | Width | Right:
        width = w;
        marginLeft = marginRight = 0.0f;
        break;
    case Left | Width:
        width = w;
        marginLeft = 0.0f;
        break;
    case Width | Right:
        width = w;
        marginRight = 0.0f;
        break;
    case Left | Right:
        marginLeft = marginRight = w / 2.0f;
        break;
    case Left:
        marginLeft = w;
        break;
    case Width:
        width = w;
        break;
    case Right:
        marginRight = w;
        break;
    default:  // over-constrained
        marginRight += w;   // TODO: assuming LTR
        break;
    }
}

void BlockLevelBox::resolveFloatWidth(float w, float r)
{
    assert(style);
    marginLeft = style->marginLeft.isAuto() ? 0.0f : style->marginLeft.getPx();
    marginRight = style->marginRight.isAuto() ? 0.0f : style->marginRight.getPx();
    if (!isnan(r))
        width = r;
    else if (!style->width.isAuto())
        width = style->width.getPx();
    else
        width = w - getBlankLeft() - getBlankRight();
}

void BlockLevelBox::resolveMargin(ViewCSSImp* view, const ContainingBlock* containingBlock, float available)
{
    resolveWidth((available != 0.0f) ? available : containingBlock->width);
    if (!style->marginTop.isAuto())
        marginTop = style->marginTop.getPx();
    else
        marginTop = 0;
    if (!style->marginBottom.isAuto())
        marginBottom = style->marginBottom.getPx();
    else
        marginBottom = 0;
    if (!style->height.isAuto())
        height = style->height.getPx();
}

void BlockLevelBox::layOutInlineLevelBox(ViewCSSImp* view, Node node, FormattingContext* context,
                                         Element element, CSSStyleDeclarationImp* style)
{
    assert(element);
    assert(style);

    if (!context->lineBox) {
        if (!context->addLineBox(view, this))
            return;  // TODO error
    }
    InlineLevelBox* inlineLevelBox = new(std::nothrow) InlineLevelBox(node, style);
    if (!inlineLevelBox)
        return;  // TODO error
    style->addBox(inlineLevelBox);

    inlineLevelBox->parentBox = context->lineBox;  // for getContainingBlock
    context->prevChar = 0;

    if (layOutReplacedElement(view, inlineLevelBox, element, style)) {
        inlineLevelBox->resolveWidth();
        inlineLevelBox->baseline = inlineLevelBox->getTotalHeight();
    } else {
        BlockLevelBox* inlineBlock = view->layOutBlockBoxes(element, 0, 0, 0, true);
        if (!inlineBlock)
            return;  // TODO error
        inlineLevelBox->appendChild(inlineBlock);
        inlineBlock->layOut(view, context);
        inlineLevelBox->width = inlineBlock->getTotalWidth();
        inlineLevelBox->height = inlineBlock->getTotalHeight();
        if (inlineLevelBox->height == 0.0f)
            inlineLevelBox->width = 0.0f;
        inlineLevelBox->baseline = inlineLevelBox->height;
        if (!style->overflow.isClipped()) {
            if (LineBox* lineBox = dynamic_cast<LineBox*>(inlineBlock->getLastChild()))
                inlineLevelBox->baseline = inlineLevelBox->height - inlineBlock->getBlankBottom() -
                                           lineBox->getTotalHeight() + lineBox->getBlankTop() + lineBox->getBaseline();
        }
    }

    while (context->leftover < inlineLevelBox->getTotalWidth()) {
        if (context->lineBox->hasChildBoxes() || context->hasNewFloats()) {
            context->nextLine(view, this, false);
            if (!context->addLineBox(view, this))
                return;  // TODO error
            continue;
        }
        if (!context->shiftDownLineBox(view))
            break;
    }

    context->x += inlineLevelBox->getTotalWidth();
    context->leftover -= inlineLevelBox->getTotalWidth();
    context->appendInlineBox(inlineLevelBox, style);
}

void BlockLevelBox::layOutFloat(ViewCSSImp* view, Node node, BlockLevelBox* floatBox, FormattingContext* context)
{
    assert(floatBox->style);
    floatBox->layOut(view, context);
    floatBox->remainingHeight = floatBox->getTotalHeight();
    if (!context->floatNodes.empty()) {
        // Floats are not allowed to reorder. Process this float box later in the other line box.
        context->floatNodes.push_back(node);
        return;
    }
    unsigned clear = floatBox->style->clear.getValue();
    if ((clear & CSSClearValueImp::Left) && context->getLeftEdge() ||
        (clear & CSSClearValueImp::Right) && context->getRightEdge()) {
        context->floatNodes.push_back(node);
        return;
    }
    if (!context->lineBox) {
        if (!context->addLineBox(view, this))
            return;   // TODO error
    }
    float w = floatBox->getEffectiveTotalWidth();
    float l = context->getLeftoverForFloat(floatBox->style->float_.getValue());
    // If both w and l are zero, move this floating box to the next line;
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/stack-floats-003.htm
    if ((l < w || l == 0.0f && w == 0.0f) &&
        (context->lineBox->hasChildBoxes() || context->hasLeft() || context->hasRight())) {
        // Process this float box later in the other line box.
        context->floatNodes.push_back(node);
        return;
    }
    context->addFloat(floatBox, w);
}

void BlockLevelBox::layOutAbsolute(ViewCSSImp* view, Node node, BlockLevelBox* absBox, FormattingContext* context)
{
    // Just insert this absolute box into a line box now.
    // Absolute boxes will be processed later in ViewCSSImp::layOut().
    if (!context->lineBox) {
        if (!context->addLineBox(view, this))
            return;  // TODO error
    }
    context->lineBox->appendChild(absBox);
}

void BlockLevelBox::layOutAnonymousInlineTable(ViewCSSImp* view, FormattingContext* context, std::list<Node>::iterator& i)
{
    if (!context->lineBox) {
        if (!context->addLineBox(view, this))
            return;  // TODO error
    }
    InlineLevelBox* inlineLevelBox = new(std::nothrow) InlineLevelBox(node, 0);
    if (!inlineLevelBox)
        return;  // TODO error

    inlineLevelBox->parentBox = context->lineBox;  // for getContainingBlock
    context->prevChar = 0;

    Element element = interface_cast<Element>(*i);
    assert(element);
    CSSStyleDeclarationImp* style = view->getStyle(element);
    assert(style);
    TableWrapperBox* table = new(std::nothrow) TableWrapperBox(view, element, style);
    if (!table)
        return;
    bool done = false;
    bool ws = false;
    while (!done && ++i != inlines.end()) {
        Node node = *i;
        switch (node.getNodeType()) {
        case Node::TEXT_NODE: {
            std::u16string data = interface_cast<Text>(node).getData();
            for (size_t j = 0; j < data.length(); ++j) {
                if (!isSpace(data[j])) {
                    done = true;
                    if (ws)
                        --i;
                    break;
                }
            }
            ws = true;
            break;
        }
        case Node::ELEMENT_NODE:
            element = interface_cast<Element>(node);
            if (CSSStyleDeclarationImp* elementStyle = view->getStyle(element)) {
                if (!elementStyle->display.isTableParts()) {
                    done = true;
                    if (ws)
                        --i;
                    break;
                }
            }
            ws = false;
            table->processTableChild(node, style);
            break;
        default:
            done = true;
            break;
        }
    }
    --i;

    inlineLevelBox->appendChild(table);
    table->layOutBlockBoxes();
    table->layOut(view, context);
    inlineLevelBox->width = table->getTotalWidth();
    inlineLevelBox->height = table->getTotalHeight();
    inlineLevelBox->baseline = table->getBaseline();

    while (context->leftover < inlineLevelBox->getTotalWidth()) {
        if (context->lineBox->hasChildBoxes() || context->hasNewFloats()) {
            context->nextLine(view, this, false);
            if (!context->addLineBox(view, this))
                return;  // TODO error
            continue;
        }
        if (!context->shiftDownLineBox(view))
            break;
    }

    context->x += inlineLevelBox->getTotalWidth();
    context->leftover -= inlineLevelBox->getTotalWidth();
    context->appendInlineBox(inlineLevelBox, style);

    context->prevChar = 0;
}

// Generate line boxes
bool BlockLevelBox::layOutInline(ViewCSSImp* view, FormattingContext* context, float originalMargin)
{
    // Use the positive margin stored in context to consume the remaining height of floating boxes.
    bool keepConsumed = false;
    consumed = context->useMargin();

    context->atLineHead = true;

    assert(!hasChildBoxes());
    bool collapsed = true;
    for (auto i = inlines.begin(); i != inlines.end(); ++i) {
        Node node = *i;
        if (BlockLevelBox* box = view->getFloatBox(node)) {
            if (box->isFloat()) {
                if (box->style->clear.getValue())
                    keepConsumed = true;
                layOutFloat(view, node, box, context);
            } else if (box->isAbsolutelyPositioned())
                layOutAbsolute(view, node, box, context);
            collapsed = false;
        } else {
            CSSStyleDeclarationImp* style = 0;
            Element element = getContainingElement(node);
            if (!element)
                continue;
            style = view->getStyle(element);
            if (!style)
                continue;
            style->resolve(view, this);
            if (node.getNodeType() == Node::TEXT_NODE) {
                Text text = interface_cast<Text>(node);
                if (layOutText(view, node, context, text.getData(), element, style))
                    collapsed = false;
            } else if (style->display.isTableParts()) {
                layOutAnonymousInlineTable(view, context, i);
                collapsed = false;
            } else if (!isReplacedElement(element) && style->display.isInline()) {
                // empty inline element
                assert(!element.hasChildNodes());
                if (layOutText(view, node, context, u"", element, style))
                    collapsed = false;
            } else {
                // At this point, node is an inline block element or a replaced element.
                layOutInlineLevelBox(view, node, context, element, style);
                collapsed = false;
            }
        }
    }
    if (context->lineBox)
        context->nextLine(view, this, false);

    // Layout remaining floating boxes in context
    while (!context->floatNodes.empty()) {
        BlockLevelBox* floatBox = view->getFloatBox(context->floatNodes.front());
        float clearance = 0.0f;
        if (unsigned clear = floatBox->style->clear.getValue()) {
            keepConsumed = true;
            clearance = -context->usedMargin;
            clearance += context->clear(clear);
        } else {
            context->leftover = width - context->getLeftEdge() - context->getRightEdge();
            while (context->getLeftoverForFloat(floatBox->style->float_.getValue()) < floatBox->getEffectiveTotalWidth()) {
                float h = context->shiftDown(width);
                if (h <= 0.0f)
                    break;
                clearance += h;
                context->clearance += h;
                context->adjustRemainingHeight(clearance);
            }
        }
        LineBox* nextLine = context->addLineBox(view, this);
        context->nextLine(view, this, false);
        if (nextLine && clearance != 0.0f)
            nextLine->clearance = clearance;
    }
    if (!keepConsumed)
        consumed = 0.0f;
    if (collapsed && isAnonymous()) {
        undoCollapseMarginTop(context, originalMargin);
        return false;
    }
    return true;
}

// TODO for a more complete implementation, see,
//      http://groups.google.com/group/netscape.public.mozilla.layout/msg/0455a21b048ffac3?pli=1

void BlockLevelBox::shrinkToFit()
{
    fit(shrinkTo());
}

// returns the minimum total width
float Box::shrinkTo()
{
    return getTotalWidth();
}

float BlockLevelBox::shrinkTo()
{
    int autoCount = 3;
    float min = 0.0f;
    if (style && !style->width.isAuto()) {
        --autoCount;
        min = style->width.getPx();
    } else {
        for (Box* child = getFirstChild(); child; child = child->getNextSibling())
            min = std::max(min, child->shrinkTo());
    }
    min += borderLeft + paddingLeft + paddingRight + borderRight;
    if (style) {
        if (!style->marginLeft.isAuto()) {
            --autoCount;
            min += style->marginLeft.getPx();
        }
        if (!style->marginRight.isAuto()) {
            --autoCount;
            float m  = style->marginRight.getPx();
            if (0.0f < m)
                min += m;
        }
    }
    return min;
}

void BlockLevelBox::fit(float w)
{
    if (getBlockWidth() == w)
        return;
    resolveWidth(w);
    if (!isAnonymous() && !style->width.isAuto())
        return;
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->fit(width);
}

bool BlockLevelBox::isCollapsableInside() const
{
    return !isFlowRoot();
}

bool BlockLevelBox::isCollapsableOutside() const
{
    if (!isInFlow())
        return false;
    if (!isAnonymous() && style) {
        if (style->isInlineBlock() || style->display.getValue() == CSSDisplayValueImp::TableCell)
            return false;
    }
    return true;
}

bool BlockLevelBox::isCollapsedThrough() const
{
    if (height != 0.0f || isFlowRoot() ||
        borderTop != 0.0f || paddingTop != 0.0f || paddingBottom != 0.0f || borderBottom != 0.0f)
        return false;
    for (LineBox* lineBox = dynamic_cast<LineBox*>(getFirstChild());
         lineBox;
         lineBox = dynamic_cast<LineBox*>(lineBox->getNextSibling())) {
        if (lineBox->getTotalHeight() != 0.0f)
            return false;
        for (auto i = lineBox->getFirstChild(); i; i = i->getNextSibling()) {
            if (dynamic_cast<InlineLevelBox*>(i))
                return false;
        }
    }
    return true;
}

float BlockLevelBox::collapseMarginTop(FormattingContext* context)
{
    if (!isCollapsableOutside()) {
        assert(!isAnonymous());
        if (isFloat() || isAbsolutelyPositioned() || !getParentBox())
            return NAN;
        assert(context);
        BlockLevelBox* prev = dynamic_cast<BlockLevelBox*>(getPreviousSibling());
        if (prev && prev->isCollapsableOutside())
            context->collapseMargins(prev->marginBottom);
        context->fixMargin();

        float clearance = context->clear(style->clear.getValue());
        if (clearance == 0.0f)
            clearance = NAN;
        else {
            if (clearance < marginTop)
                clearance = marginTop;
            clearance -= marginTop;
            context->collapseMargins(marginTop);
            context->setClearance();
        }
        return NAN;
    }

    float original = marginTop;
    float before = NAN;
    if (BlockLevelBox* parent = dynamic_cast<BlockLevelBox*>(getParentBox())) {
        if (parent->getFirstChild() == this) {
            if (parent->isCollapsableInside() && parent->borderTop == 0 && parent->paddingTop == 0 && !hasClearance()) {
                before = parent->marginTop;
                parent->marginTop = 0.0f;
            }
        } else {
            BlockLevelBox* prev = dynamic_cast<BlockLevelBox*>(getPreviousSibling());
            assert(prev);
            if (prev->isCollapsableOutside()) {
                before = context->collapseMargins(prev->marginBottom);
                prev->marginBottom = 0.0f;
                if (prev->isCollapsedThrough())
                    prev->marginTop = 0.0f;
            }
        }
    }
    marginTop = context->collapseMargins(marginTop);

    if (!isAnonymous()) {
        unsigned clearValue = style->clear.getValue();
        if (isFlowRoot())
            clearValue = CSSFloatValueImp::Left | CSSFloatValueImp::Right;

        clearance = context->clear(clearValue);
        BlockLevelBox* prev = dynamic_cast<BlockLevelBox*>(getPreviousSibling());
        if (clearance == 0.0f)
            clearance = NAN;
        else if (prev && prev->isCollapsedThrough()) {
            if (clearance < marginTop)
                clearance = marginTop;
            prev->marginBottom = before;
            clearance -= original + before;
            marginTop = original;
            before = NAN;
            context->collapseMargins(marginTop);
            context->setClearance();
        } else if (clearance < marginTop) {
            clearance = NAN;
            before = NAN;
            context->collapseMargins(marginTop);
        } else {
            if (prev) {
                prev->marginBottom = context->undoCollapseMargins();
                clearance -= original + before;
            } else
                clearance -= original;
            marginTop = original;
            before = NAN;
            context->collapseMargins(marginTop);
            context->setClearance();
        }
    }

    if (0.0f < context->clearance) {
        if (isnan(clearance))
            clearance = context->clearance;
        else
            clearance += context->clearance;
        context->setClearance();
    }

    return before;
}

void BlockLevelBox::collapseMarginBottom(FormattingContext* context)
{
    BlockLevelBox* last = dynamic_cast<BlockLevelBox*>(getLastChild());
    if (last && last->isCollapsableOutside()) {
        float lm = context->collapseMargins(last->marginBottom);
        if (last->isCollapsedThrough()) {
            lm = context->collapseMargins(last->marginTop);
            last->marginTop = 0.0f;
            if (isCollapsableInside() && borderBottom == 0 && paddingBottom == 0 && style->height.isAuto() &&
                !context->hasClearance())
            {
                last->marginBottom = 0.0f;
                marginBottom = context->collapseMargins(marginBottom);
            } else {
                last->marginBottom = lm;
                context->fixMargin();
                if (!last->hasClearance())
                    last->moveUpCollapsedThroughMargins(context);
            }
        } else if (isCollapsableInside() && borderBottom == 0 && paddingBottom == 0 && style->height.isAuto()) {
            last->marginBottom = 0.0f;
            marginBottom = context->collapseMargins(marginBottom);
        } else {
            last->marginBottom = lm;
            context->fixMargin();
        }
    }

    BlockLevelBox* first = dynamic_cast<BlockLevelBox*>(getFirstChild());
    if (first && first->isCollapsableOutside() && !first->hasClearance() && isCollapsableInside() && borderTop == 0 && paddingTop == 0) {
        if (hasClearance()) {
            // The following algorithm is deduced from the following tests:
            //   http://test.csswg.org/suites/css2.1/20110323/html4/margin-collapse-157.htm
            //   http://test.csswg.org/suites/css2.1/20110323/html4/margin-collapse-clear-015.htm
            //   http://hixie.ch/tests/evil/acid/002-no-data/#top (clearance < 0.0f)
            float original = style->marginTop.isAuto() ? 0 : style->marginTop.getPx();
            if (clearance <= 0.0f)
                marginTop = std::max(original, first->marginTop);
            else if (original < first->marginTop - clearance)
                marginTop = first->marginTop - clearance;
            else
                marginTop = original;
            first->marginTop = 0.0f;
        } else {
            // Note even if first->marginTop is zero, first->topBorderEdge
            // still needs to be cleared; cf. margin-bottom-103.
            std::swap(first->marginTop, marginTop);
            while (first && first->isCollapsedThrough()) {
                // The top border edge must not be cleared if the next adjacent sibling has a clearance;
                // cf. clear-001.
                BlockLevelBox* next = dynamic_cast<BlockLevelBox*>(first->getNextSibling());
                if (!next || (!next->hasClearance() && next->consumed <= 0.0f))
                    first->topBorderEdge = 0.0f;
                else
                    break;
                first = next;
            }
        }
    }
}

bool BlockLevelBox::undoCollapseMarginTop(FormattingContext* context, float before)
{
    if (isnan(before))
        return false;
    if (BlockLevelBox* prev = dynamic_cast<BlockLevelBox*>(getPreviousSibling()))
        prev->marginBottom = context->undoCollapseMargins();
    else {
        Box* parent = getParentBox();
        assert(parent);
        parent->marginTop = context->undoCollapseMargins();
    }
    return true;
}

// Adjust marginTop of the 1st, collapsed through child box.
void BlockLevelBox::adjustCollapsedThroughMargins(FormattingContext* context)
{
    if (isCollapsedThrough()) {
        topBorderEdge = marginTop;
        if (hasClearance())
            moveUpCollapsedThroughMargins(context);
        context->adjustRemainingFloatingBoxes(topBorderEdge);
    } else if (isCollapsableOutside()) {
        assert(topBorderEdge == 0.0f);
        context->fixMargin();
        moveUpCollapsedThroughMargins(context);
    }
}

void BlockLevelBox::moveUpCollapsedThroughMargins(FormattingContext* context)
{
    assert(isCollapsableOutside());
    float m;
    BlockLevelBox* from = this;
    BlockLevelBox* curr = this;
    BlockLevelBox* prev = dynamic_cast<BlockLevelBox*>(curr->getPreviousSibling());
    if (hasClearance()) {
        if (!prev)
            return;
        from = curr = prev;
        prev = dynamic_cast<BlockLevelBox*>(curr->getPreviousSibling());
        if (from->hasClearance() || !from->isCollapsedThrough())
            return;
        m = curr->marginTop;
    } else if (curr->isCollapsedThrough()) {
        assert(curr->marginTop == 0.0f);
        // cf. If previously a part of marginTop has been used for consuming some
        // floating box heights, leave it as marginTop; cf. clear-float-003.
        curr->marginTop = consumed;
        m = curr->marginBottom - consumed;
        curr->topBorderEdge = 0.0f;
        for (BlockLevelBox* last = dynamic_cast<BlockLevelBox*>(curr->getLastChild());
             last && last->isCollapsedThrough();
             last = dynamic_cast<BlockLevelBox*>(last->getPreviousSibling())) {
            last->topBorderEdge = 0.0f;
            if (last->hasClearance())
                break;
        }
    } else
        m = curr->marginTop - consumed;
    while (prev && prev->isCollapsedThrough() && !prev->hasClearance()) {
        prev->topBorderEdge -= m;
        curr = prev;
        prev = dynamic_cast<BlockLevelBox*>(curr->getPreviousSibling());
    }
    if (curr != from) {
        assert(curr->marginTop == 0.0f);
        assert(curr->marginBottom == 0.0f);
        curr->marginTop = m;
        if (!from->isCollapsedThrough() || hasClearance())
            from->marginTop -= m;
        else
            from->marginBottom = 0.0f;
    } else if (curr->isCollapsedThrough() && !hasClearance()) {
        curr->marginTop += m;
        curr->marginBottom = 0.0f;
        curr->topBorderEdge = 0.0f;
    }
}

void BlockLevelBox::layOutChildren(ViewCSSImp* view, FormattingContext* context)
{
    Box* next;
    for (Box* child = getFirstChild(); child; child = next) {
        next = child->getNextSibling();
        if (!child->layOut(view, context))
            removeChild(child);
    }
}

void BlockLevelBox::applyMinMaxHeight(FormattingContext* context)
{
    assert(!isAnonymous());
    if (!style->maxHeight.isNone()) {
        float maxHeight = style->maxHeight.getPx();
        if (maxHeight < height)
            height = maxHeight;
    }
    if (!hasChildBoxes() && 0.0f < height)
        context->updateRemainingHeight(height);
    float d = style->minHeight.getPx() - height;
    if (0.0f < d) {
        context->updateRemainingHeight(d);
        height = style->minHeight.getPx();
    }
}

bool BlockLevelBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    const ContainingBlock* containingBlock = getContainingBlock(view);

    Element element = 0;
    if (!isAnonymous())
        element = getContainingElement(node);
    else if (const Box* box = dynamic_cast<const Box*>(containingBlock))
        element = getContainingElement(box->node);
    if (!element)
        return false;  // TODO error

    style = view->getStyle(element);
    if (!style)
        return false;  // TODO error

    if (!isAnonymous()) {
        style->resolve(view, containingBlock);
        resolveWidth(view, containingBlock);
    } else {
        // The properties of anonymous boxes are inherited from the enclosing non-anonymous box.
        // Theoretically, we are supposed to create a new style for this anonymous box, but
        // of course we don't want to do so.
        backgroundColor = 0x00000000;
        paddingTop = paddingRight = paddingBottom = paddingLeft = 0.0f;
        borderTop = borderRight = borderBottom = borderLeft = 0.0f;
        marginTop = marginRight = marginLeft = marginBottom = 0.0f;
        width = containingBlock->width;
        height = 0.0f;
        stackingContext = style->getStackingContext();
    }

    textAlign = style->textAlign.getValue();

    float before = collapseMarginTop(context);
    if (isInFlow() && 0.0f < borderTop + paddingTop)
        context->updateRemainingHeight(borderTop + paddingTop);

    FormattingContext* parentContext = context;
    context = updateFormattingContext(context);

    if (!layOutReplacedElement(view, this, element, style.get()) && hasInline()) {
        if (!layOutInline(view, context, before))
            return false;
    }
    layOutChildren(view, context);

    if (!isAnonymous()) {
        if ((style->width.isAuto() || style->marginLeft.isAuto() || style->marginRight.isAuto()) &&
            (style->isInlineBlock() || style->isFloat() || style->display == CSSDisplayValueImp::TableCell) &&
            !intrinsic)
            shrinkToFit();
        applyMinMaxWidth(getTotalWidth());
    } else if (dynamic_cast<CellBox*>(this))
        shrinkToFit();

    // Collapse margins with the first and the last children before calculating the auto height.
    collapseMarginBottom(context);
    if (isFlowRoot()) {
        if (Box* last = getLastChild())
            last->marginBottom += context->clear(3);
    }

    if ((style->height.isAuto() && !intrinsic) || isAnonymous()) {
        float totalClearance = 0.0f;
        height = 0.0f;
        for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
            height += child->getTotalHeight();
            totalClearance += child->getClearance();
        }
        // If height is zero and this block-box contains only line-boxes,
        // clearances are used just to layout floating boxes, and thus
        // totalClearance should not be added to height.
        // TODO: test more conditions.
        if (height != 0.0f || !dynamic_cast<LineBox*>(getFirstChild()))
            height += totalClearance;
    }
    if (!isAnonymous()) {
        applyMinMaxHeight(context);
        // TODO: If min-height was applied, we might need to undo collapseMarginBottom().
    } else if (!hasChildBoxes() && 0.0f < height)
        context->updateRemainingHeight(height);

    // Now that 'height' is fixed, calculate 'left', 'right', 'top', and 'bottom'.
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->fit(width);
        child->resolveOffset(view);
    }

    if (backgroundImage && backgroundImage->getState() == BoxImage::CompletelyAvailable) {
        style->backgroundPosition.resolve(view, backgroundImage, style.get(), getPaddingWidth(), getPaddingHeight());
        backgroundLeft = style->backgroundPosition.getLeftPx();
        backgroundTop = style->backgroundPosition.getTopPx();
    }

    restoreFormattingContext(context);
    if (parentContext && parentContext != context) {
        if (isCollapsableOutside()) {
            // TODO: Review this logic; what's going to happen when collapse through, etc.
            parentContext->inheritMarginContext(context);
            if (0.0f < height)
                parentContext->updateRemainingHeight(height);
        }
        context = parentContext;
    }

    adjustCollapsedThroughMargins(context);
    if (isInFlow() && 0.0f < paddingBottom + borderBottom)
        context->updateRemainingHeight(paddingBottom + borderBottom);

    return true;
}

unsigned BlockLevelBox::resolveAbsoluteWidth(const ContainingBlock* containingBlock, float& left, float& right, float r)
{
    //
    // Calculate width
    //
    // left + marginLeft + borderLeftWidth + paddingLeft + width + paddingRight + borderRightWidth + marginRight + right
    // == containingBlock->width
    //
    marginLeft = style->marginLeft.isAuto() ? 0.0f : style->marginLeft.getPx();
    marginRight = style->marginRight.isAuto() ? 0.0f : style->marginRight.getPx();

    left = 0.0f;
    right = 0.0f;

    unsigned autoMask = Left | Width | Right;
    if (!style->left.isAuto()) {
        left = style->left.getPx();
        autoMask &= ~Left;
    }
    if (!isnan(r)) {
        width = r;
        autoMask &= ~Width;
    } else if (!style->width.isAuto()) {
        width = style->width.getPx();
        autoMask &= ~Width;
    }
    if (!style->right.isAuto()) {
        right = style->right.getPx();
        autoMask &= ~Right;
    }
    float leftover = containingBlock->width - getTotalWidth() - left - right;
    switch (autoMask) {
    case Left | Width | Right:
        left = -offsetH;
        autoMask &= ~Left;
        // FALL THROUGH
    case Width | Right:
        width += leftover;  // Set the max size and do shrink-to-fit later.
        break;
    case Left | Width:
        width += leftover;  // Set the max size and do shrink-to-fit later.
        break;
    case Left | Right:
        left = -offsetH;
        right += leftover - left;
        break;
    case Left:
        left += leftover;
        break;
    case Width:
        width += leftover;
        break;
    case Right:
        right += leftover;
        break;
    case 0:
        if (style->marginLeft.isAuto() && style->marginRight.isAuto()) {
            if (0.0f <= leftover)
                marginLeft = marginRight = leftover / 2.0f;
            else {  // TODO rtl
                marginLeft = 0.0f;
                marginRight = -leftover;
            }
        } else if (style->marginLeft.isAuto())
            marginLeft = leftover;
        else if (style->marginRight.isAuto())
            marginRight = leftover;
        else
            right += leftover;
        break;
    }
    return autoMask;
}

unsigned BlockLevelBox::applyAbsoluteMinMaxWidth(const ContainingBlock* containingBlock, float& left, float& right, unsigned autoMask)
{
    if (!style->maxWidth.isNone()) {
        float maxWidth = style->maxWidth.getPx();
        if (maxWidth < width)
            autoMask = resolveAbsoluteWidth(containingBlock, left, right, maxWidth);
    }
    float minWidth = style->minWidth.getPx();
    if (width < minWidth)
        autoMask = resolveAbsoluteWidth(containingBlock, left, right, minWidth);
    return autoMask;
}

unsigned BlockLevelBox::resolveAbsoluteHeight(const ContainingBlock* containingBlock, float& top, float& bottom, float r)
{
    //
    // Calculate height
    //
    // top + marginTop + borderTopWidth + paddingTop + height + paddingBottom + borderBottomWidth + marginBottom + bottom
    // == containingBlock->height
    //
    marginTop = style->marginTop.isAuto() ? 0.0f : style->marginTop.getPx();
    marginBottom = style->marginBottom.isAuto() ? 0.0f : style->marginBottom.getPx();

    top = 0.0f;
    bottom = 0.0f;

    unsigned autoMask = Top | Height | Bottom;
    if (!style->top.isAuto()) {
        top = style->top.getPx();
        autoMask &= ~Top;
    }
    if (!isnan(r)) {
        height = r;
        autoMask &= ~Height;
    } else if (!style->height.isAuto()) {
        height = style->height.getPx();
        autoMask &= ~Height;
    }
    if (!style->bottom.isAuto()) {
        bottom = style->bottom.getPx();
        autoMask &= ~Bottom;
    }
    float leftover = containingBlock->height - getTotalHeight() - top - bottom;
    switch (autoMask & (Top | Height | Bottom)) {
    case Top | Height | Bottom:
        top = -offsetV;
        autoMask &= ~Top;
        // FALL THROUGH
    case Height | Bottom:
        height += leftover;  // Set the max size and do shrink-to-fit later.
        break;
    case Top | Height:
        height += leftover;  // Set the max size and do shrink-to-fit later.
        break;
    case Top | Bottom:
        top = -offsetV;
        bottom += leftover - top;
        break;
    case Top:
        top += leftover;
        break;
    case Height:
        height += leftover;
        break;
    case Bottom:
        bottom += leftover;
        break;
    case 0:
        if (style->marginTop.isAuto() && style->marginBottom.isAuto()) {
            if (0.0f <= leftover)
                marginTop = marginBottom = leftover / 2.0f;
            else {
                marginTop = 0.0f;
                marginBottom = -leftover;
            }
        } else if (style->marginTop.isAuto())
            marginTop = leftover;
        else if (style->marginBottom.isAuto())
            marginBottom = leftover;
        else
            bottom += leftover;
        break;
    }
    return autoMask;
}

unsigned BlockLevelBox::applyAbsoluteMinMaxHeight(const ContainingBlock* containingBlock, float& top, float& bottom, unsigned autoMask)
{
    if (!style->maxHeight.isNone()) {
        float maxHeight = style->maxHeight.getPx();
        if (maxHeight < height)
            autoMask = resolveAbsoluteHeight(containingBlock, top, bottom, maxHeight);
    }
    float minHeight = style->minHeight.getPx();
    if (height < minHeight)
        autoMask = resolveAbsoluteHeight(containingBlock, top, bottom, minHeight);
    return autoMask;
}

void BlockLevelBox::layOutAbsolute(ViewCSSImp* view)
{
    assert(node);
    assert(isAbsolutelyPositioned());
    Element element = getContainingElement(node);
    if (!element)
        return;  // TODO error
    if (!style)
        return;  // TODO error

    setContainingBlock(view);
    const ContainingBlock* containingBlock = &absoluteBlock;

    style->resolve(view, containingBlock);

    resolveBackground(view);
    updatePadding();
    updateBorderWidth();

    float left;
    float right;
    unsigned maskH = resolveAbsoluteWidth(containingBlock, left, right);
    applyAbsoluteMinMaxWidth(containingBlock, left, right, maskH);
    float top;
    float bottom;
    unsigned maskV = resolveAbsoluteHeight(containingBlock, top, bottom);
    applyAbsoluteMinMaxHeight(containingBlock, top, bottom, maskV);

    if (CSSDisplayValueImp::isBlockLevel(style->display.getOriginalValue())) {
        // This box is originally a block-level box inside an inline context.
        // Set the static position to the beginning of the next line.
        if (const Box* lineBox = getParentBox()) {  // A root element can be absolutely positioned.
            for (const Box* box = getPreviousSibling(); box; box = box->getPreviousSibling()) {
                if (!box->isAbsolutelyPositioned()) {
                    if (maskV == (Top | Height | Bottom) || maskV == (Top | Bottom))
                        offsetV += lineBox->height + lineBox->getBlankBottom();
                    if (maskH == (Left | Width | Right) || maskH == (Left | Right))
                        offsetH -= box->getTotalWidth();
                }
            }
        }
    }

    FormattingContext* context = updateFormattingContext(context);
    assert(context);

    if (layOutReplacedElement(view, this, element, style.get())) {
        maskH &= ~Width;
        maskV &= ~Height;
        // TODO: more conditions...
    } else if (hasInline())
        layOutInline(view, context);
    layOutChildren(view, context);

    if (maskH == (Left | Width) || maskH == (Width | Right)) {
        shrinkToFit();
        if (maskH & Left) {
            float left = containingBlock->width - getTotalWidth() - right;
            offsetH += left;
        }
    }
    // Check 'max-width' and then 'min-width' again.
    maskH = applyAbsoluteMinMaxWidth(containingBlock, left, right, maskH);

    collapseMarginBottom(context);
    // An absolutely positioned box is a flow root.
    if (Box* last = getLastChild())
        last->marginBottom += context->clear(3);

    if (maskV == (Top | Height) || maskV == (Height | Bottom)) {
        float before = height;
        float totalClearance = 0.0f;
        height = 0;
        for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
            height += child->getTotalHeight();
            totalClearance += child->getClearance();
        }
        // Note if height is zero, clearances are used only to layout floating boxes,
        // and thus totalClearance should not be added to height.
        if (height != 0.0f)
            height += totalClearance;
        if (maskV == (Top | Height))
            top = before - height;
    }
    // Check 'max-height' and then 'min-height' again.
    maskV = applyAbsoluteMinMaxHeight(containingBlock, top, bottom, maskV);

    // Now that 'height' is fixed, calculate 'left', 'right', 'top', and 'bottom'.
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->resolveOffset(view);
        child->fit(width);
    }

    if (backgroundImage && backgroundImage->getState() == BoxImage::CompletelyAvailable) {
        style->backgroundPosition.resolve(view, backgroundImage, style.get(), getPaddingWidth(), getPaddingHeight());
        backgroundLeft = style->backgroundPosition.getLeftPx();
        backgroundTop = style->backgroundPosition.getTopPx();
    }

    restoreFormattingContext(context);
    adjustCollapsedThroughMargins(context);

    offsetH += left;
    offsetV += top;

    if (style->getPseudoElementSelectorType() == CSSPseudoElementSelector::Marker) {
        // The horizontal static position of the marker is such that the
        // marker's "end" edge is placed against the "start" edge of the
        // list item's parent.
        Box* list = getParentBox()->getParentBox();
        if (list->getParentBox() && list->getParentBox()->style->counterReset.hasCounter())
            list = list->getParentBox();
        offsetH = (list->x + list->getBlankLeft()) - x - getTotalWidth() + getMarginRight();
    }
}

void BlockLevelBox::resolveOffset(ViewCSSImp* view)
{
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/inline-box-002.htm
    Box::resolveOffset(view);
    if (isAnonymous())
        return;
    Element element = getContainingElement(node);
    element = element.getParentElement();
    if (!element)
        return;
    CSSStyleDeclarationImp* parentStyle = view->getStyle(element);
    if (!parentStyle)
        return;
    if (!parentStyle->display.isInline())
        return;
    Box::resolveOffset(parentStyle);
}

void BlockLevelBox::resolveXY(ViewCSSImp* view, float left, float top, BlockLevelBox* clip)
{
    if (!isAnonymous() && style && style->float_.getValue() == CSSFloatValueImp::Right) {
        // cf. http://www.webstandards.org/action/acid2/guide/#row-10-11
        if (getEffectiveTotalWidth() == 0.0f)
            left -= getTotalWidth();
    }

    left += offsetH;
    top += offsetV + getClearance();
    x = left;
    y = top;
    clipBox = clip;
    left += getBlankLeft();
    top += getBlankTop() + topBorderEdge;

    if (isClipped())
        clip = this;

    if (shadow)
        shadow->resolveXY(left, top);
    else {
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            child->resolveXY(view, left, top, clip);
            top += child->getTotalHeight() + child->getClearance();
        }
    }

    view->updateScrollWidth(x + getBlockWidth());
}

void BlockLevelBox::dump(std::string indent)
{
    std::cout << indent << "* block-level box";
    if (!node)
        std::cout << " [anonymous]";
    else
        std::cout << " [" << node.getNodeName() << ']';
    std::cout << " (" << x << ", " << y << ") " <<
        "w:" << width << " h:" << height << ' ' <<
        "(" << offsetH << ", " << offsetV <<") ";
    if (hasClearance())
        std::cout << "c:" << clearance << ' ';
    if (isCollapsedThrough())
        std::cout << "t:" << topBorderEdge << ' ';
    std::cout << "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << ' ' <<
        "p:" << paddingTop << ':' <<  paddingRight << ':'<< paddingBottom<< ':' << paddingLeft << ' ' <<
        "b:" << borderTop << ':' <<  borderRight << ':' << borderBottom<< ':' << borderLeft << ' ' <<
        std::hex << CSSSerializeRGB(backgroundColor) << std::dec << '\n';
    indent += "  ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap
