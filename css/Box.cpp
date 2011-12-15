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

#include "CSSSerialize.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSTokenizer.h"
#include "StackingContext.h"
#include "ViewCSSImp.h"
#include "WindowImp.h"

#include "Table.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

Element getContainingElement(Node node)
{
    for (; node; node = node.getParentNode()) {
        if (node.getNodeType() == Node::ELEMENT_NODE)
            return interface_cast<Element>(node);
    }
    return 0;
}

}

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
        anonymousBox = dynamic_cast<BlockLevelBox*>(firstChild);
        if (anonymousBox)
            return anonymousBox;
    }
    anonymousBox = new(std::nothrow) BlockLevelBox;
    if (anonymousBox) {
        anonymousBox->spliceInline(this);
        if (firstChild)
            insertBefore(anonymousBox, firstChild);
        else
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

namespace {

// TODO: there might not be such a text node that 'element.getFirstChild() == node'.
bool isAtLeftEdge(Element& element, Node& node)
{
    return element == node || element.getFirstChild() == node;
}

// TODO: there might not be such a text node that 'element.getLastNode() == node'.
bool isAtRightEdge(Element& element, Node& node)
{
    return element == node || element.getLastChild() == node;
}

}

void BlockLevelBox::nextLine(ViewCSSImp* view, FormattingContext* context, CSSStyleDeclarationImp*& activeStyle,
                             CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle,
                             CSSStyleDeclarationImp* style, FontTexture*& font, float& point)
{
    if (firstLetterStyle) {
        firstLetterStyle = 0;
        if (firstLineStyle)
            activeStyle = firstLineStyle.get();
        else
            activeStyle = style;
        font = activeStyle->getFontTexture();
        point = view->getPointFromPx(activeStyle->fontSize.getPx());
    } else {
        context->nextLine(view, this);
        if (firstLineStyle) {
            firstLineStyle = 0;
            activeStyle = style;
            font = activeStyle->getFontTexture();
            point = view->getPointFromPx(activeStyle->fontSize.getPx());
        }
    }
}

bool BlockLevelBox::layOutText(ViewCSSImp* view, Node text, FormattingContext* context,
                               std::u16string data, Element element, CSSStyleDeclarationImp* style)
{
    assert(element);
    assert(style);

    // An empty inline element should pass 'data' as an empty string. In this case,
    // the inline box to be generated must not be collapsed away by returning false.
    // cf. 10.8 Line height calculations: the ’line-height’ and ’vertical-align’ properties
    bool discardable = !data.empty();

    if (style->processWhiteSpace(data, context->prevChar) == 0 && discardable)
        return !isAnonymous();

    bool psuedoChecked = false;
    CSSStyleDeclarationPtr firstLineStyle;
    CSSStyleDeclarationPtr firstLetterStyle;
    CSSStyleDeclarationImp* activeStyle = style;

    FontTexture* font = activeStyle->getFontTexture();
    float point = view->getPointFromPx(activeStyle->fontSize.getPx());
    size_t position = 0;
    for (;;) {
        if (!context->lineBox) {
            if (style->processLineHeadWhiteSpace(data) == 0 && discardable)
                return !isAnonymous();
            discardable = false;
            if (!context->addLineBox(view, this))
                return false;  // TODO error
            if (!psuedoChecked && getFirstChild() == context->lineBox) {
                psuedoChecked  = true;
                // The current line box is the 1st line of this block box.
                // style to use can be a pseudo element styles from any ancestor elements.
                // Note the :first-line, first-letter pseudo-elements can only be attached to a block container element.
                std::list<CSSStyleDeclarationImp*> firstLineStyles;
                std::list<CSSStyleDeclarationImp*> firstLetterStyles;
                Box* box = this;
                for (;;) {
                    if (CSSStyleDeclarationImp* s = box->getStyle()) {
                        if (CSSStyleDeclarationImp* p = s->getPseudoElementStyle(CSSPseudoElementSelector::FirstLine))
                            firstLineStyles.push_front(p);
                        if (CSSStyleDeclarationImp* p = s->getPseudoElementStyle(CSSPseudoElementSelector::FirstLetter))
                            firstLetterStyles.push_front(p);
                    }
                    Box* parent = box->getParentBox();
                    if (!parent || parent->getFirstChild() != box)
                        break;
                    box = parent;
                }
                if (!firstLineStyles.empty()) {
                    firstLineStyle = new(std::nothrow) CSSStyleDeclarationImp;
                    if (firstLineStyle) {
                        for (auto i = firstLineStyles.begin(); i != firstLineStyles.end(); ++i)
                            firstLineStyle->specify(*i);
                        firstLineStyle->compute(view, style, 0);
                        // TODO: resolve?
                    }
                }
                if (!firstLetterStyles.empty()) {
                    firstLetterStyle = new(std::nothrow) CSSStyleDeclarationImp;
                    if (firstLetterStyle) {
                        for (auto i = firstLetterStyles.begin(); i != firstLetterStyles.end(); ++i)
                            firstLetterStyle->specify(*i);
                        firstLetterStyle->compute(view, style, 0);
                        // TODO: resolve?
                    }
                }
                if (firstLetterStyle) {
                    activeStyle = firstLetterStyle.get();
                    font = activeStyle->getFontTexture();
                    point = view->getPointFromPx(activeStyle->fontSize.getPx());
                } else if (firstLineStyle) {
                    activeStyle = firstLineStyle.get();
                    font = activeStyle->getFontTexture();
                    point = view->getPointFromPx(activeStyle->fontSize.getPx());
                }
            }
        }
        LineBox* lineBox = context->lineBox;

        InlineLevelBox* inlineLevelBox = new(std::nothrow) InlineLevelBox(text, activeStyle);
        if (!inlineLevelBox)
            return false;  // TODO error
        style->addBox(inlineLevelBox);  // activeStyle? maybe not...
        inlineLevelBox->resolveWidth();
        float blankLeft = inlineLevelBox->getBlankLeft();
        if (0 < position || !isAtLeftEdge(element, text))
            inlineLevelBox->marginLeft = inlineLevelBox->paddingLeft = inlineLevelBox->borderLeft = blankLeft = 0;
        context->x += blankLeft;
        context->leftover -= blankLeft;
        float blankRight = inlineLevelBox->getBlankRight();
        context->leftover -= blankRight;
        if (context->leftover < 0.0f && context->lineBox->hasChildBoxes()) {
            delete inlineLevelBox;
            nextLine(view, context, activeStyle, firstLetterStyle, firstLineStyle, style, font, point);
            continue;
        }

        size_t length = 0;
        bool linefeed = false;
        float advanced = 0.0f;

        if (data.empty())
            inlineLevelBox->setData(font, point, data);
        else if (data[0] == '\n') {
            linefeed = true;
            length = 1;
            advanced = 0.0f;
        } else {
            size_t fitLength = firstLetterStyle ? 1 : data.length();  // TODO: 1 is absolutely wrong...
            // We are still not sure if there's a room for text in context->lineBox.
            // If there's no room due to float box(es), move the linebox down to
            // the closest bottom of float box.
            // And repeat this process until there's no more float box in the context.
            size_t next = 1;
            float required = 0.0f;
            unsigned transform = activeStyle->textTransform.getValue();
            std::u16string transformed;
            size_t transformedLength = 0;
            for (;;) {
                advanced = context->leftover;
                if (!transform) {  // 'none'
                    length = font->fitText(data.c_str(), fitLength, point, context->leftover,
                                           activeStyle->whiteSpace.getValue(), &next, &required);
                } else {
                    transformed = font->fitTextWithTransformation(
                        data.c_str(), fitLength, point, transform, context->leftover,
                        &length, &transformedLength,
                        activeStyle->whiteSpace.getValue(), &next, &required);
                }
                if (0 < length) {
                    advanced -= context->leftover;
                    break;
                }
                if (context->lineBox->hasChildBoxes() || context->hasNewFloats()) {
                    delete inlineLevelBox;
                    goto NextLine;
                }
                if (!context->shiftDownLineBox(view)) {
                    context->leftover -= required;
                    advanced -= context->leftover;
                    length = next;
                    transformedLength = transformed.length();
                    break;
                }
            }
            assert(0 < length);
            if (!transform) // 'none'
                inlineLevelBox->setData(font, point, data.substr(0, length));
            else
                inlineLevelBox->setData(font, point, transformed.substr(0, transformedLength));
            inlineLevelBox->width = advanced;
        }
        if ((length < data.length() || !isAtRightEdge(element, text)) && !firstLetterStyle) {
            // TODO: firstLetterStyle: actually we are not sure if the following characters would fit in the same line box...
            inlineLevelBox->marginRight = inlineLevelBox->paddingRight = inlineLevelBox->borderRight = blankRight = 0;
        }
        if (inlineLevelBox->hasHeight()) {
            inlineLevelBox->height = activeStyle->lineHeight.getPx();
            inlineLevelBox->leading = std::max(inlineLevelBox->height, getStyle()->lineHeight.getPx()) - font->getLineHeight(point);

            // TODO: XXX
            lineBox->underlinePosition = std::max(lineBox->underlinePosition, font->getUnderlinePosition(point));
            lineBox->underlineThickness = std::max(lineBox->underlineThickness, font->getUnderlineThickness(point));
        }
        context->x += advanced + blankRight;
        context->appendInlineBox(inlineLevelBox, activeStyle);
        // Switch height from 'line-height' to the content height.
        if (inlineLevelBox->hasHeight())
            inlineLevelBox->height = font->getLineHeight(point);
        position += length;
        data.erase(0, length);
        if (data.length() == 0) {  // layout done?
            if (linefeed)
                context->nextLine(view, this);
            break;
        }
    NextLine:
        nextLine(view, context, activeStyle, firstLetterStyle, firstLineStyle, style, font, point);
    }
    return true;
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

    if (isReplacedElement(element)) {
        inlineLevelBox->resolveWidth();
        layOutReplacedElement(view, inlineLevelBox, element, style);
        inlineLevelBox->baseline = inlineLevelBox->getTotalHeight();
    } else {
        assert(style->isInlineBlock());
        BlockLevelBox* inlineBlock = view->layOutBlockBoxes(element, 0, 0, 0, true);
        if (!inlineBlock)
            return;  // TODO error
        inlineLevelBox->appendChild(inlineBlock);
        inlineBlock->layOut(view, context);
        inlineLevelBox->width = inlineBlock->getTotalWidth();
        inlineLevelBox->height = inlineBlock->getTotalHeight();

        inlineLevelBox->baseline = inlineLevelBox->height;
        if (!style->overflow.isClipped()) {
            if (LineBox* lineBox = dynamic_cast<LineBox*>(inlineBlock->getLastChild()))
                inlineLevelBox->baseline = inlineLevelBox->height - inlineBlock->getBlankBottom() -
                                           lineBox->getTotalHeight() + lineBox->getBlankTop() + lineBox->getBaseline();
        }
    }

    while (context->leftover < inlineLevelBox->getTotalWidth()) {
        if (context->lineBox->hasChildBoxes() || context->hasNewFloats()) {
            context->nextLine(view, this);
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
    if (context->getLeftoverForFloat(floatBox->style->float_.getValue()) < w &&
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

// Generate line boxes
bool BlockLevelBox::layOutInline(ViewCSSImp* view, FormattingContext* context, float originalMargin)
{
    // Use the positive margin stored in context to consume the remaining height of floating boxes.
    bool keepConsumed = false;
    consumed = context->useMargin();

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
            if (isReplacedElement(element)) {
                layOutInlineLevelBox(view, node, context, element, style);
                collapsed = false;
            } else if (node.getNodeType() == Node::TEXT_NODE) {
                Text text = interface_cast<Text>(node);
                if (layOutText(view, node, context, text.getData(), element, style))
                    collapsed = false;
            } else if (style->display.isInline()) {
                // empty inline element
                assert(!element.hasChildNodes());
                if (layOutText(view, node, context, u"", element, style))
                    collapsed = false;
            } else {
                // At this point, node should be an inline block element.
                layOutInlineLevelBox(view, node, context, element, style);
                collapsed = false;
            }
        }
    }
    if (context->lineBox)
        context->nextLine(view, this);

    // Layout remaining floats in context
    while (!context->floatNodes.empty()) {
        BlockLevelBox* floatBox = view->getFloatBox(context->floatNodes.front());
        float clearance = 0.0f;
        if (unsigned clear = floatBox->style->clear.getValue()) {
            keepConsumed = true;
            clearance = -context->usedMargin;
            clearance += context->clear(clear);
        } else {
            while (width - context->getLeftEdge() - context->getRightEdge() < floatBox->getEffectiveTotalWidth()) {
                float d = context->shiftDown(width);
                if (d <= 0.0f)
                    break;
                clearance += d;
                context->adjustRemainingHeight(clearance);
            }
        }
        LineBox* nextLine = context->addLineBox(view, this);
        context->nextLine(view, this);
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

    if (isAnonymous())
        return before;

    clearance = context->clear(style->clear.getValue());
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
            float original = style->marginTop.isAuto() ? 0 : style->marginTop.getPx();
            float d = first->marginTop - clearance;
            if (original < d)
                marginTop = d;
            else {
                // TODO: The expected behavior is not very clear.
                marginTop = original;
            }
            first->marginTop = 0.0f;
        } else if (first->marginTop != 0.0f) {
            std::swap(first->marginTop, marginTop);
            while (first->isCollapsedThrough()) {
                first->topBorderEdge = 0.0f;
                first = dynamic_cast<BlockLevelBox*>(first->getNextSibling());
                if (!first || first->hasClearance() || 0.0f < first->consumed)
                    break;
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
    if (isCollapsableOutside()) {
        if (context != parentContext)
            context->inheritMarginContext(parentContext);
    }

    if (isReplacedElement(element))
        layOutReplacedElement(view, this, element, style.get());
    else if (hasInline()) {
        if (!layOutInline(view, context, before))
            return false;
    }
    layOutChildren(view, context);

    if ((style->width.isAuto() || style->marginLeft.isAuto() || style->marginRight.isAuto()) &&
        (style->isInlineBlock() || style->isFloat() || style->display == CSSDisplayValueImp::TableCell) &&
        !intrinsic)
        shrinkToFit();

    if (!isAnonymous())
        applyMinMaxWidth(getTotalWidth());

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
        if (Box* last = getLastChild()) {
            if (last->marginBottom < 0.0f)
                height -= last->marginBottom;
        }
        // Note if height is zero, clearances are used only to layout floating boxes,
        // and thus totalClearance should not be added to height.
        if (height != 0.0f)
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

    backgroundColor = style->backgroundColor.getARGB();
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
            if (maskV == (Top | Height | Bottom) || maskV == (Top | Bottom))
                offsetV += lineBox->height + lineBox->getBlankBottom();
            if (maskH == (Left | Width | Right) || maskH == (Left | Right)) {
                for (const Box* box = getPreviousSibling(); box; box = box->getPreviousSibling()) {
                    if (!box->isAbsolutelyPositioned())
                        offsetH -= box->getTotalWidth();
                }
            }
        }
    }

    FormattingContext* context = updateFormattingContext(context);
    assert(context);

    if (isReplacedElement(element)) {
        layOutReplacedElement(view, this, element, style.get());
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
        if (Box* last = getLastChild()) {
            if (last->marginBottom < 0.0f)
                height -= last->marginBottom;
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

    restoreFormattingContext(context);
    adjustCollapsedThroughMargins(context);

    offsetH += left;
    offsetV += top;
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

bool LineBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    for (Box* box = getFirstChild(); box; box = box->getNextSibling()) {
        if (box->isAbsolutelyPositioned())
            continue;
        box->resolveOffset(view);
        if (InlineLevelBox* inlineBox = dynamic_cast<InlineLevelBox*>(box)) {
            CSSStyleDeclarationImp* style = box->getStyle();
            if (style && style->display.isInlineLevel())
                inlineBox->offsetV += style->verticalAlign.getOffset(this, inlineBox);
            else {
                float leading = inlineBox->getLeading() / 2.0f;
                inlineBox->offsetV += getBaseline() - (leading + inlineBox->getBaseline());
            }
        }
    }
    return true;
}

float LineBox::shrinkTo()
{
    float w = Box::shrinkTo();
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (child->isFloat())
            w += child->getEffectiveTotalWidth();
    }
    return w;
}

void LineBox::fit(float w)
{
    assert(parentBox);
    assert(dynamic_cast<BlockLevelBox*>(parentBox));
    float leftover = w - shrinkTo();
    switch (dynamic_cast<BlockLevelBox*>(parentBox)->getTextAlign()) {
    case CSSTextAlignValueImp::Left:
    case CSSTextAlignValueImp::Default: // TODO: rtl
        leftGap = 0.0f;
        rightGap = leftover;
        break;
    case CSSTextAlignValueImp::Right:
        leftGap = leftover;
        rightGap = 0.0f;
        break;
    case CSSTextAlignValueImp::Center:
        leftGap = rightGap = leftover / 2.0f;
        break;
    default:  // TODO: support Justify and Default
        break;
    }
}

void LineBox::resolveXY(ViewCSSImp* view, float left, float top, BlockLevelBox* clip)
{
    left += offsetH;
    top += offsetV + getClearance();
    x = left;
    y = top;
    clipBox = clip;
    left += getBlankLeft();  // Node floats are placed inside margins.
    top += getBlankTop();
    float next = 0.0f;
    bool usedLeftGap = false;
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        BlockLevelBox* floatingBox = 0;
        next = left;
        if (!child->isAbsolutelyPositioned()) {
            if (!child->isFloat())
                next += child->getTotalWidth();
            else {
                floatingBox = dynamic_cast<BlockLevelBox*>(child);
                assert(floatingBox);
                if (floatingBox == rightBox)
                    left += rightGap;
                next = left + floatingBox->getEffectiveTotalWidth();
            }
        }
        if (!usedLeftGap && (!floatingBox || floatingBox == rightBox)) {
            left += leftGap;
            next += leftGap;
            usedLeftGap = true;
        }
        child->resolveXY(view, left, top, clip);
        left = next;
    }

    view->updateScrollWidth(x + getTotalWidth() + getBlankRight());
}

void LineBox::dump(std::string indent)
{
    std::cout << indent << "* line box (" << x << ", " << y << ") " <<
        "w:" << width << " h:" << height << " (" << offsetH << ", " << offsetV <<") ";
    if (hasClearance())
        std::cout << "c:" << clearance << ' ';
    std::cout << "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << '\n';
    indent += "  ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

bool InlineLevelBox::isAnonymous() const
{
    return !style || !style->display.isInlineLevel();
}

void InlineLevelBox::setData(FontTexture* font, float point, std::u16string data)
{
    this->font = font;
    this->point = point;
    this->data = data;
    baseline = font->getAscender(point);
}

float InlineLevelBox::atEndOfLine()
{
    size_t length = data.length();
    if (length < 1)
        return 0.0f;
    if (style->whiteSpace.isCollapsingSpace() && data[length - 1] == u' ') {
        data.erase(length - 1);
        if (data.length() == 0) {
            // Deal with the errors in floating point operations.
            float w = -width;
            width = 0.0f;
            return w;
        } else {
            float w = -font->measureText(u" ", point);
            width += w;
            return w;
        }
    }
    return 0.0f;
}

void InlineLevelBox::resolveWidth()
{
    // The ‘width’ and ‘height’ properties do not apply.
    if (!isAnonymous() && style->display.getValue() == CSSDisplayValueImp::Inline) {
        backgroundColor = style->backgroundColor.getARGB();
        updatePadding();
        updateBorderWidth();
        marginTop = style->marginTop.isAuto() ? 0 : style->marginTop.getPx();
        marginRight = style->marginRight.isAuto() ? 0 : style->marginRight.getPx();
        marginLeft = style->marginLeft.isAuto() ? 0 : style->marginLeft.getPx();
        marginBottom = style->marginBottom.isAuto() ? 0 : style->marginBottom.getPx();
    } else {
        backgroundColor = 0x00000000;
        paddingTop = paddingRight = paddingBottom = paddingLeft = 0.0f;
        borderTop = borderRight = borderBottom = borderLeft = 0.0f;
        marginTop = marginRight = marginLeft = marginBottom = 0.0f;
    }
}

// To deal with nested inline level boxes in the document tree, resolveOffset
// is repeatedly applied to this inline level box up to the block-level box.
void InlineLevelBox::resolveOffset(ViewCSSImp* view)
{
    CSSStyleDeclarationImp* s = getStyle();
    Element element = getContainingElement(node);
    while (s && s->display.isInlineLevel()) {
        Box::resolveOffset(s);
        element = element.getParentElement();
        if (!element)
            break;
        s = view->getStyle(element);
    }
}

void InlineLevelBox::resolveXY(ViewCSSImp* view, float left, float top, BlockLevelBox* clip)
{
    left += offsetH;
    top += offsetV + leading / 2.0f;
    if (shadow)
        shadow->resolveXY(left, top);
    else if (getFirstChild())
        getFirstChild()->resolveXY(view, left, top, clip);
    x = left;
    y = top;
    clipBox = clip;
}

void InlineLevelBox::dump(std::string indent)
{
    std::cout << indent << "* inline-level box (" << x << ", " << y << ") " <<
        "w:" << width << " h:" << height << ' ' <<
        "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << ' ' <<
        "p:" << paddingTop << ':' <<  paddingRight << ':'<< paddingBottom<< ':' << paddingLeft << ' ' <<
        "b:" << borderTop << ':' <<  borderRight << ':' << borderBottom<< ':' << borderLeft << ' ' <<
        '"' << data << "\" " <<
        std::hex << CSSSerializeRGB(getStyle()->color.getARGB()) << std::dec << '\n';
    indent += "  ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap
