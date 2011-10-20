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
    positioned(false),
    stackingContext(0),
    nextBase(0),
    x(0.0f),
    y(0.0f),
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
            switch (style->position.getValue()) {
            case CSSPositionValueImp::Absolute:
            case CSSPositionValueImp::Relative:
            case CSSPositionValueImp::Fixed: {
                // Now we need to find the corresponding box for this ancestor.
                const Box* box = style->box;
                assert(box);    // TODO: check NULL case
                offsetH = box->x + box->marginLeft + box->borderLeft - x;
                offsetV = box->y + box->marginTop + box->borderTop - y;
                if (box->getBoxType() == BLOCK_LEVEL_BOX) {
                    absoluteBlock.width = box->getPaddingWidth();
                    absoluteBlock.height = box->getPaddingHeight();
                } else {
                    assert(box->getBoxType() == INLINE_LEVEL_BOX);
                    if (Box* inlineBlock = box->getFirstChild()) {
                        absoluteBlock.width = inlineBlock->getPaddingWidth();
                        absoluteBlock.height = inlineBlock->getPaddingHeight();
                    } else {
                        const Box* p = box->getParentBox();
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
            default:
                break;
            }
        }
    }
    offsetH = -x;
    offsetV = -y;
    absoluteBlock.width = view->getInitialContainingBlock()->width;
    absoluteBlock.height = view->getInitialContainingBlock()->height;
}

FormattingContext* Box::updateFormattingContext(FormattingContext* context)
{
    if (isFlowRoot()) {
        assert(formattingContext);
        // TODO: adjust left and right blanks.
        return formattingContext;
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

// Calculate left, right, top, bottom for 'static' or 'relative' element.
// TODO: rtl
void Box::resolveOffset(CSSStyleDeclarationImp* style)
{
    assert(style->position.getValue() == CSSPositionValueImp::Relative ||
           style->position.getValue() == CSSPositionValueImp::Static);

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
    int autoCount = 3;
    unsigned autoMask = Left | Width | Right;
    if (style) {
        if (style->isFloat() || style->isInlineBlock())
            return resolveFloatWidth(w);
        if (!isnan(r)) {
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
                                           activeStyle->whiteSpace.isCollapsingSpace(), &next, &required);
                } else {
                    transformed = font->fitTextWithTransformation(
                        data.c_str(), fitLength, point, transform, context->leftover,
                        &length, &transformedLength,
                        activeStyle->whiteSpace.isCollapsingSpace(), &next, &required);
                }
                if (0 < length) {
                    advanced -= context->leftover;
                    break;
                }
                if (context->lineBox->hasChildBoxes() || context->hasNewFloats()) {
                    delete inlineLevelBox;
                    goto NextLine;
                }
                if (!context->shiftDownLineBox()) {
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
        if (inlineLevelBox->getTotalWidth() || inlineLevelBox->getTotalHeight()) {  // have non-zero margins, padding, or borders?
            inlineLevelBox->height = font->getLineHeight(point);
            inlineLevelBox->baseline += (activeStyle->lineHeight.getPx() - inlineLevelBox->height) / 2.0f;
            lineBox->height = std::max(getStyle()->lineHeight.getPx(), std::max(lineBox->height, std::max(activeStyle->lineHeight.getPx(), inlineLevelBox->height)));
            lineBox->baseline = std::max(lineBox->baseline, inlineLevelBox->baseline);
            lineBox->underlinePosition = std::max(lineBox->underlinePosition, font->getUnderlinePosition(point));
            lineBox->underlineThickness = std::max(lineBox->underlineThickness, font->getUnderlineThickness(point));
        }
        context->x += advanced + blankRight;
        lineBox->appendChild(inlineLevelBox);
        lineBox->width += blankLeft + advanced + blankRight;
        if (activeStyle->isPositioned() && !inlineLevelBox->isAnonymous())
            activeStyle->getStackingContext()->addBase(inlineLevelBox);
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

void BlockLevelBox::layOutInlineReplaced(ViewCSSImp* view, Node node, FormattingContext* context,
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

    inlineLevelBox->resolveWidth();
    if (!style->width.isAuto())
        inlineLevelBox->width = style->width.getPx();
    else
        inlineLevelBox->width = 300;  // TODO
    if (!style->height.isAuto())
        inlineLevelBox->height = style->height.getPx();
    else
        inlineLevelBox->height = 24;  // TODO

    context->prevChar = 0;

    std::u16string tag = element.getLocalName();
    if (tag == u"img") {
        html::HTMLImageElement img = interface_cast<html::HTMLImageElement>(element);
        if (BoxImage* backgroundImage = new(std::nothrow) BoxImage(inlineLevelBox, view->getDocument().getDocumentURI(), img)) {
            inlineLevelBox->backgroundImage = backgroundImage;
            if (style->width.isAuto())
                inlineLevelBox->width = backgroundImage->getWidth();
            if (style->height.isAuto())
                inlineLevelBox->height = backgroundImage->getHeight();
        }
    } else if (tag == u"iframe") {
        html::HTMLIFrameElement iframe = interface_cast<html::HTMLIFrameElement>(element);
        inlineLevelBox->width = CSSTokenizer::parseInt(iframe.getWidth().c_str(), iframe.getWidth().size());
        inlineLevelBox->height = CSSTokenizer::parseInt(iframe.getHeight().c_str(), iframe.getHeight().size());
        html::Window contentWindow = iframe.getContentWindow();
        if (WindowImp* imp = dynamic_cast<WindowImp*>(contentWindow.self())) {
            imp->setSize(inlineLevelBox->width, inlineLevelBox->height);
            inlineLevelBox->shadow = imp->getView();
            std::u16string src = iframe.getSrc();
            if (!src.empty() && !imp->getDocument())
                iframe.setSrc(src);
        }
    } else if (!style->isInlineBlock()) {
        // TODO: This line should not be reached but currently it is as absolute boxes are created lazily.
        return;
    } else if (BlockLevelBox* inlineBlock = view->layOutBlockBoxes(element, 0, 0, 0, true)) {
        inlineLevelBox->appendChild(inlineBlock);
        inlineBlock->layOut(view, context);
        inlineLevelBox->width = inlineBlock->getTotalWidth();
        inlineLevelBox->marginLeft = inlineLevelBox->marginRight =
        inlineLevelBox->borderLeft = inlineLevelBox->borderRight =
        inlineLevelBox->paddingLeft = inlineLevelBox->paddingRight = 0.0f;

        inlineLevelBox->height = inlineBlock->getTotalHeight();
        inlineLevelBox->marginTop = inlineLevelBox->marginBottom =
        inlineLevelBox->borderTop = inlineLevelBox->borderBottom =
        inlineLevelBox->paddingTop = inlineLevelBox->paddingBottom = 0.0f;

        inlineLevelBox->baseline = inlineBlock->getBlankTop() + inlineBlock->height;
    } else
        return;  // TODO

    // TODO: calc inlineLevelBox->width and height with intrinsic values.
    if (inlineLevelBox->baseline == 0.0f)
        inlineLevelBox->baseline = inlineLevelBox->getBlankTop() + inlineLevelBox->height;  // TODO

    float blankLeft = inlineLevelBox->getBlankLeft();
    float blankRight = inlineLevelBox->getBlankRight();
    context->x += blankLeft;
    context->leftover -= blankLeft + blankRight;

    // We are still not sure if there's a room for text in context->lineBox.
    // If there's no room due to float box(es), move the linebox down to
    // the closest bottom of float box.
    // And repeat this process until there's no more float box in the context.
    do {
        if (inlineLevelBox->width <= context->leftover) {
            context->leftover -= inlineLevelBox->width;
            break;
        }
    } while (context->shiftDownLineBox());
    // TODO: deal with overflow

    context->x += inlineLevelBox->width + blankRight;
    LineBox* lineBox = context->lineBox;
    lineBox->appendChild(inlineLevelBox);
    lineBox->height = std::max(getStyle()->lineHeight.getPx(), std::max(lineBox->height, inlineLevelBox->height));  // TODO: marginTop, etc.???
    lineBox->baseline = std::max(lineBox->baseline, inlineLevelBox->baseline);
    lineBox->width += blankLeft + inlineLevelBox->width + blankRight;
    if (style->isPositioned() && !inlineLevelBox->isAnonymous())
        style->getStackingContext()->addBase(inlineLevelBox);
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
    if (context->leftover < w &&
        (context->lineBox->hasChildBoxes() ||
         0.0f < context->getLeftEdge() ||
         0.0f < context->getRightEdge())) {
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
    assert(!hasChildBoxes());
    bool collapsed = true;
    for (auto i = inlines.begin(); i != inlines.end(); ++i) {
        Node node = *i;
        if (BlockLevelBox* box = view->getFloatBox(node)) {
            if (box->isFloat())
                layOutFloat(view, node, box, context);
            else if (box->isAbsolutelyPositioned())
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
            style->resolve(view, this, element);
            if (isReplacedElement(element)) {
                layOutInlineReplaced(view, node, context, element, style);
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
                layOutInlineReplaced(view, node, context, element, style);
                collapsed = false;
            }
        }
    }

    if (context->lineBox) {
        // Layout remaining floats in context
        float clearance = 0.0f;
        while (!context->floatNodes.empty()) {
            LineBox* currentLine = context->lineBox;
            float saved = 0.0f;
            if (clearance != 0.0f) {
                Box* prevLine = currentLine->getPreviousSibling();
                saved = prevLine->marginBottom;
                prevLine->marginBottom = 0.0f;
            }
            BlockLevelBox* floatBox = view->getFloatBox(context->floatNodes.front());
            if (unsigned clear = floatBox->style->clear.getValue())
                context->nextLine(view, this, clear);
            else {
                clearance = context->shiftDown();
                if (0.0f < clearance) {
                    clearance -= currentLine->height;
                    if (clearance < 0.0f)
                        clearance = 0.0f;
                }
                currentLine->marginBottom += clearance;
                context->nextLine(view, this);
            }
            context->addLineBox(view, this);
            currentLine->marginTop += saved;
        }
        context->nextLine(view, this);
    }

    if (collapsed && isAnonymous()) {
        if (originalMargin != 0.0f) {
            // Undo collapseMarginTop
            if (Box* prev = getPreviousSibling())
                prev->marginBottom = originalMargin;
            else {
                Box* parent = getParentBox();
                assert(parent);
                parent->marginTop = originalMargin;
            }
        }
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
            min += style->marginRight.getPx();
        }
    }
    return min;
}

void BlockLevelBox::fit(float w)
{
    if (getTotalWidth() == w)
        return;
    resolveWidth(w);
    if (style && !style->width.isAuto())
        return;
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->fit(width);
}

namespace {

// TODO: Support when three or more margins collapse...
float collapseMargins(float a, float b)
{
    if (0.0f <= a && 0.0f <= b)
        return std::max(a, b);
    if (a < 0.0f && b < 0.0f)
        return std::min(a, b);
    return a + b;
}

}  // namespace

float BlockLevelBox::collapseMarginTop(FormattingContext* context)
{
    if (isFlowRoot())
        return 0.0f;
    assert(!isFlowRoot());
    float before = 0.0f;
    bool top = false;  // TODO: review this logic again for negative margins, etc.
    if (Box* parent = getParentBox()) {
        if (parent->getFirstChild() == this) {
            if (parent->getParentBox() && parent->borderTop == 0 && parent->paddingTop == 0) {
                top = true;
                before = parent->marginTop;
                marginTop = collapseMargins(marginTop, parent->marginTop);
                parent->marginTop = 0.0f;
            }
        } else {
            Box* prev = getPreviousSibling();
            assert(prev);
            before = prev->marginBottom;
            marginTop = collapseMargins(prev->marginBottom, marginTop);
            prev->marginBottom = 0.0f;
        }
    }
    context->updateRemainingHeight(top ? (getBlankTop() - before) : getBlankTop());
    return before;
}

void BlockLevelBox::collapseMarginBottom()
{
    if (Box* parent = getParentBox()) {
        if (parent->getFirstChild() == this) {
            if (parent->getParentBox() && parent->borderTop == 0 && parent->paddingTop == 0)
                std::swap(marginTop, parent->marginTop);
        }
    }
    if (getParentBox() && borderBottom == 0 && paddingBottom == 0) {
        if (Box* child = getLastChild()) {
            marginBottom = collapseMargins(marginBottom, child->marginBottom);
            child->marginBottom = 0;
        }
    }
}

void BlockLevelBox::layOutChildren(ViewCSSImp* view, FormattingContext* context)
{
    Box* next;
    for (Box* child = getFirstChild(); child; child = next) {
        next = child->getNextSibling();
        if (!child->layOut(view, context)) {
            removeChild(child);
            continue;
        }
        if (child->isFlowRoot())
            context->updateRemainingHeight(child->getTotalHeight());
        if (style->width.isAuto())
            width = std::max(width, child->getTotalWidth());
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
        style->resolve(view, containingBlock, element);
        resolveWidth(view, containingBlock, style->isInlineBlock() ? context->leftover : 0.0f);
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
    context = updateFormattingContext(context);
    float before = 0.0f;
    float clearance = 0.0f;
    if (!isFlowRoot()) {
        before = collapseMarginTop(context);
        if (!isAnonymous()) {
            clearance = context->clear(style->clear.getValue());
            marginTop += clearance;
        }
    }

    if (hasInline()) {
        if (!layOutInline(view, context, before))
            return false;
    }
    layOutChildren(view, context);

    if ((style->width.isAuto() || style->marginLeft.isAuto() || style->marginRight.isAuto()) &&
        (style->isInlineBlock() || style->isFloat() || style->display == CSSDisplayValueImp::TableCell))
        shrinkToFit();

    collapseMarginBottom();
    if (isFlowRoot()) {
        clearance = context->clear(3);
        if (Box* last = getLastChild())
            last->marginBottom += clearance;
    }

    if (style->height.isAuto() || isAnonymous()) {
        height = 0.0f;
        for (Box* child = getFirstChild(); child; child = child->getNextSibling())
            height += child->getTotalHeight();
    }
    if (!hasChildBoxes())
        context->updateRemainingHeight(height);
    if (!isAnonymous()) {
        float d = style->minHeight.getPx() - height;
        if (0.0f < d) {
            context->updateRemainingHeight(d);
            height = style->minHeight.getPx();
        }
    }

    // Collapse top and bottom margins.
    if (!isFlowRoot() && height == 0.0f &&
        borderTop == 0.0f && paddingTop == 0.0f && paddingBottom == 0.0f && borderBottom == 0.0f && clearance == 0.0f)
    {
        float offset = marginTop;
        marginBottom = collapseMargins(marginTop, marginBottom);
        marginTop = 0.0f;
        if (0.0f != offset) {
            if (Box* child = getFirstChild())
                child->marginTop += offset;
            context->updateRemainingHeight(-offset);
        }
    }

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

    return true;
}

unsigned BlockLevelBox::resolveAbsoluteWidth(const ContainingBlock* containingBlock, float& right, float& bottom)
{
    assert(style);
    backgroundColor = style->backgroundColor.getARGB();

    updatePadding();
    updateBorderWidth();

    //
    // Calculate width
    //
    // left + marginLeft + borderLeftWidth + paddingLeft + width + paddingRight + borderRightWidth + marginRight + right
    // == containingBlock->width
    //
    marginLeft = style->marginLeft.isAuto() ? 0.0f : style->marginLeft.getPx();
    marginRight = style->marginRight.isAuto() ? 0.0f : style->marginRight.getPx();

    float left = 0.0f;
    right = 0.0f;

    unsigned autoMask = Left | Width | Right;
    if (!style->left.isAuto()) {
        left = style->left.getPx();
        autoMask &= ~Left;
    }
    if (!style->width.isAuto()) {
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

    //
    // Calculate height
    //
    // top + marginTop + borderTopWidth + paddingTop + height + paddingBottom + borderBottomWidth + marginBottom + bottom
    // == containingBlock->height
    //
    marginTop = style->marginTop.isAuto() ? 0.0f : style->marginTop.getPx();
    marginBottom = style->marginBottom.isAuto() ? 0.0f : style->marginBottom.getPx();

    float top = 0.0f;
    bottom = 0.0f;

    autoMask |= Top | Height | Bottom;
    if (!style->top.isAuto()) {
        top = style->top.getPx();
        autoMask &= ~Top;
    }
    if (!style->height.isAuto()) {
        height = style->height.getPx();
        autoMask &= ~Height;
    }
    if (!style->bottom.isAuto()) {
        bottom = style->bottom.getPx();
        autoMask &= ~Bottom;
    }
    leftover = containingBlock->height - getTotalHeight() - top - bottom;
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
            else {  // TODO rtl
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

    offsetH += left;
    offsetV += top;

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

    std::u16string tag;

    setContainingBlock(view);
    const ContainingBlock* containingBlock = &absoluteBlock;

    style->resolve(view, containingBlock, element);
    float right;
    float bottom;
    unsigned autoMask = resolveAbsoluteWidth(containingBlock, right, bottom);
    unsigned maskH = autoMask & (Left | Width | Right);
    unsigned maskV = autoMask & (Top | Height | Bottom);

    if (CSSDisplayValueImp::isBlockLevel(style->display.getOriginalValue())) {
        Element parent = element.getParentElement();
        if (parent) {
            CSSStyleDeclarationImp* parentStyle = view->getStyle(parent);
            if (!CSSDisplayValueImp::isBlockLevel(parentStyle->display.getOriginalValue())) {
                // This box is originally a block-level box inside an inline context.
                // Set the static position to the beginning of the next line.
                const Box* lineBox = getParentBox();
                assert(lineBox);
                if (maskV == (Top | Height | Bottom) || maskV == (Top | Bottom))
                    offsetV += lineBox->getTotalHeight();
                if (maskH == (Left | Width | Right) || maskH == (Left | Right)) {
                    for (const Box* box = getPreviousSibling(); box; box = box->getPreviousSibling()) {
                        if (!box->isAbsolutelyPositioned())
                            offsetH -= box->getTotalWidth();
                    }
                }
            }
        }
    }

    FormattingContext* context = updateFormattingContext(context);
    assert(context);

    // TODO: Handle replaced elements in more smart way...
    tag = element.getLocalName();
    if (tag == u"img") {
        html::HTMLImageElement img = interface_cast<html::HTMLImageElement>(element);
        if (backgroundImage = new(std::nothrow) BoxImage(this, view->getDocument().getDocumentURI(), img)) {
            if (autoMask & Width) {
                width = backgroundImage->getWidth();
                autoMask &= ~Width;
                maskH = autoMask & (Left | Width | Right);
            }
            if (autoMask & Height) {
                height = backgroundImage->getHeight();
                autoMask &= ~Height;
                maskV = autoMask & (Top | Height | Bottom);
            }
        }
    } else if (hasInline())
        layOutInline(view, context);
    layOutChildren(view, context);

    if (maskH == (Left | Width) || maskH == (Width | Right)) {
        shrinkToFit();
        if (autoMask & Left) {
            float left = containingBlock->width - getTotalWidth() - right;
            offsetH += left;
        }
    }

    if (maskV == (Top | Height) || maskV == (Height | Bottom) || height == 0) {
        height = 0;
        for (Box* child = getFirstChild(); child; child = child->getNextSibling())
            height += child->getTotalHeight();
        if (autoMask & Top) {
            float top = containingBlock->height - getTotalHeight() - bottom;
            offsetV += top;
        }
    }

    // Now that 'height' is fixed, calculate 'left', 'right', 'top', and 'bottom'.
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->resolveOffset(view);
        child->fit(width);
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

void BlockLevelBox::resolveXY(ViewCSSImp* view, float left, float top)
{
    left += offsetH;
    top += offsetV;
    x = left;
    y = top;
    left += getBlankLeft();
    top += getBlankTop();
    if (shadow)
        shadow->resolveXY(left, top);
    else {
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            child->resolveXY(view, left, top);
            top += child->getTotalHeight();
        }
    }
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
        "(" << offsetH << ", " << offsetV <<") " <<
        "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << ' ' <<
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
            if (CSSStyleDeclarationImp* style = box->getStyle())
                inlineBox->offsetV += style->verticalAlign.getOffset(this, inlineBox);
        }
    }
    return true;
}

float LineBox::shrinkTo()
{
    float w = getTotalWidth();
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
    switch (dynamic_cast<BlockLevelBox*>(parentBox)->getTextAlign()) {
    case CSSTextAlignValueImp::Left:
        break;
    case CSSTextAlignValueImp::Right:
        offsetH = w - getTotalWidth();
        break;
    case CSSTextAlignValueImp::Center:
        offsetH = (w - getTotalWidth()) / 2.0f;
        break;
    default:  // TODO: support Justify and Default
        break;
    }

    // Adjust the gap between the last inline box and the leftmost float at the right side.
    if (rightBox) {
        gap = w - getTotalWidth();
        for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
            if (child->isFloat()) {
                BlockLevelBox* box = dynamic_cast<BlockLevelBox*>(child);
                assert(box);
                gap -= box->getEffectiveTotalWidth();
            }
        }
    }
}

void LineBox::resolveXY(ViewCSSImp* view, float left, float top)
{
    left += offsetH;
    top += offsetV;
    x = left;
    y = top;
    left += getBlankLeft();  // Node floats are placed inside margins.
    top += getBlankTop();
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        float next = left;
        if (!child->isAbsolutelyPositioned()) {
            if (!child->isFloat())
                next += child->getTotalWidth();
            else {
                BlockLevelBox* box = dynamic_cast<BlockLevelBox*>(child);
                assert(box);
                if (box == rightBox)
                    left += gap;
                next = left + box->getEffectiveTotalWidth();
            }
        }
        child->resolveXY(view, left, top);
        left = next;
    }
}

void LineBox::dump(std::string indent)
{
    std::cout << indent << "* line box (" << x << ", " << y << ") " <<
        "w:" << width << " h:" << height << " (" << offsetH << ", " << offsetV <<") " <<
        "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << '\n';
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
        float w = -font->measureText(u" ", point);
        width += w;
        return w;
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

void InlineLevelBox::resolveXY(ViewCSSImp* view, float left, float top)
{
    left += offsetH;
    top += offsetV;
    if (shadow)
        shadow->resolveXY(left, top);
    else if (getFirstChild())
        getFirstChild()->resolveXY(view, left, top);
    else if (font)
        top += baseline - font->getAscender(point);
    x = left;
    y = top;
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
