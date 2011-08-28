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
#include <org/w3c/dom/html/HTMLDivElement.h>
#include <org/w3c/dom/html/HTMLIFrameElement.h>
#include <org/w3c/dom/html/HTMLImageElement.h>
#include <org/w3c/dom/html/HTMLInputElement.h>

#include "CSSSerialize.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSTokenizer.h"
#include "ViewCSSImp.h"
#include "WindowImp.h"

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
void BlockLevelBox::setContainingBlock(ViewCSSImp* view)
{
    assert(isAbsolutelyPositioned());
    float x = 0.0f;
    float y = 0.0f;
    Box::toViewPort(x, y);  // TODO: Why do we need to say 'Box::' here?
    offsetH = -x;
    offsetV = -y;
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
                Box* box = style->box;
                assert(box);
                float t = -box->paddingTop;
                float l = -box->paddingLeft;
                box->toViewPort(l, t);
                if (box->getBoxType() == BLOCK_LEVEL_BOX) {
                    absoluteBlock.width = box->getPaddingWidth();
                    absoluteBlock.height = box->getPaddingHeight();
                } else {
                    assert(box->getBoxType() == INLINE_LEVEL_BOX);
                    if (Box* inlineBlock = box->getFirstChild()) {
                        absoluteBlock.width = inlineBlock->getPaddingWidth();
                        absoluteBlock.height = inlineBlock->getPaddingHeight();
                    } else {
                        float b = style->lastBox->height + style->lastBox->paddingBottom;
                        float r = style->lastBox->width + style->lastBox->paddingRight;
                        style->lastBox->toViewPort(r, b);
                        absoluteBlock.width = r - l;
                        absoluteBlock.height = t - b;
                    }
                }

                // TODO: overflow
                absoluteBlock.width = view->getInitialContainingBlock()->width - x;

                offsetH += l;
                offsetV += t;
                return;
            }
            default:
                break;
            }
        }
    }
    absoluteBlock.width = view->getInitialContainingBlock()->width;
    absoluteBlock.height = view->getInitialContainingBlock()->height;
}

FormattingContext* Box::updateFormattingContext(FormattingContext* context)
{
    if (isFlowRoot())
        return formattingContext;
    // TODO: adjust left and right blanks.
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

BlockLevelBox* Box::expandBinding(ViewCSSImp* view, Element element, CSSStyleDeclarationImp* style)
{
    if (!style->isInlineBlock()) {
        // TODO: This line should not be reached but currently it is as absolute boxes are created lazily.
        return 0;
    }

    BlockLevelBox* inlineBlock = 0;
    switch (style->binding.getValue()) {
    case CSSBindingValueImp::InputTextfield: {
        html::HTMLInputElement input = interface_cast<html::HTMLInputElement>(element);
        html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(view->getDocument().createElement(u"div"));
        Text text = view->getDocument().createTextNode(input.getValue());
        if (div && text) {
            div.appendChild(text);
            css::CSSStyleDeclaration divStyle = div.getStyle();
            divStyle.setCssText(u"display: block; border-style: solid; border-width: thin; height: 1.2em");
            CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(divStyle.self());
            if (imp) {
                imp->specify(style);
                imp->specifyImportant(style);
            }
            divStyle.setDisplay(u"block");
            view->cascade(div, style);
            inlineBlock = view->layOutBlockBoxes(div, 0, 0, 0);
        }
        break;
    }
    case CSSBindingValueImp::InputButton: {
        html::HTMLInputElement input = interface_cast<html::HTMLInputElement>(element);
        html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(view->getDocument().createElement(u"div"));
        Text text = view->getDocument().createTextNode(input.getValue());
        if (div && text) {
            div.appendChild(text);
            css::CSSStyleDeclaration divStyle = div.getStyle();
            divStyle.setCssText(u"float: left; border-style: outset; border-width: thin; height: 1.2em; padding: 0 0.5em");
            CSSStyleDeclarationImp* imp = dynamic_cast<CSSStyleDeclarationImp*>(divStyle.self());
            if (imp) {
                imp->specify(style);
                imp->specifyImportant(style);
            }
            divStyle.setLineHeight(utfconv(std::to_string(style->height.getPx())) + u"px");  // TODO:
            divStyle.setDisplay(u"block");
            view->cascade(div, style);
            inlineBlock = view->layOutBlockBoxes(div, 0, 0, 0);
        }
        break;
    }
    default:
        // Create a BlockLevelBox and make it a child of inlineLevelBox.
        inlineBlock = view->layOutBlockBoxes(element, 0, 0, 0, true);
        break;
    }
    return inlineBlock;
}

bool BlockLevelBox::isFloat() const
{
    return style && style->isFloat();
}

bool BlockLevelBox::isFixed() const
{
    return style && style->position.getValue() == CSSPositionValueImp::Fixed;
}

bool BlockLevelBox::isAbsolutelyPositioned() const
{
    return style && style->isAbsolutelyPositioned();
}

void BlockLevelBox::dump(ViewCSSImp* view, std::string indent)
{
    std::cout << indent << "* block-level box (" << static_cast<void*>(this);
    if (!node)
        std::cout << ") [anonymous]";
    else {
        std::cout << ':' << node.self() << ')';
        std::cout << " [" << node.getNodeName() << ']';
    }
    std::cout << " (" << x << ", " << y << "), (" << getTotalWidth() << ", " << getTotalHeight() << ") " <<
    "m:" << marginTop << '-' << marginRight << '-' << marginBottom << '-' << marginLeft << ' ' <<
    "p:" << paddingTop << '-' <<  paddingRight << '-'<< paddingBottom<< '-' << paddingLeft << ' ' <<
    "b:" << borderTop << '-' <<  borderRight << '-' << borderBottom<< '-' << borderLeft << '\n';
    indent += "    ";
    if (!getFirstChild() && hasInline()) {
        for (auto i = inlines.begin(); i != inlines.end(); ++i) {
            if ((*i).getNodeType() == Node::TEXT_NODE) {
                Text text = interface_cast<Text>(*i);
                std::cout << indent << "* inline-level box: \"" << text.getData() << "\"\n";
            } else if (Box* box = view->getFloatBox(*i))
                box->dump(view, indent);
        }
        return;
    }
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(view, indent);
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
    backgroundColor = style->backgroundColor.getARGB();
    if (!style->backgroundImage.isNone()) {
        view->preload(view->getDocument().getDocumentURI(), style->backgroundImage.getValue());
        backgroundImage = new(std::nothrow) BoxImage(this, view->getDocument().getDocumentURI(), style->backgroundImage.getValue(), style->backgroundRepeat.getValue());
    }

    //
    // Calculate width
    //
    // marginLeft + borderLeftWidth + paddingLeft + width + paddingRight + borderRightWidth + marginRight
    // == containingBlock->width (- scrollbar width, if any)
    //
    updatePadding();
    updateBorderWidth();

    int autoCount = 3;  // properties which can be auto are margins and width
    if (!style->marginLeft.isAuto()) {
        marginLeft = style->marginLeft.getPx();
        --autoCount;
    }
    if (!style->marginRight.isAuto()) {
        marginRight = style->marginRight.getPx();
        --autoCount;
    }
    if (!style->width.isAuto()) {
        width = style->width.getPx();
        --autoCount;
    } else if (available) {
        available -= getBlankLeft() + getBlankRight();
        width = available;
        --autoCount;
    }
    // if 0 == autoCount, the values are over-constrained; ignore marginRight if 'ltr'
    float leftover = 0.0f;
    if (!available)
        leftover = containingBlock->width - getTotalWidth();
    // if leftover < 0, follow overflow
    if (autoCount == 1) {
        if (style->marginLeft.isAuto())
            marginLeft = leftover;
        else if (style->marginRight.isAuto())
            marginRight = leftover;
        else
            width = leftover;
    } else if (style->width.isAuto() && available == 0.0f) {
        if (style->marginLeft.isAuto())
            marginLeft = 0.0f;
        if (style->marginRight.isAuto())
            marginRight = 0.0f;
        width = leftover;
    } else if (style->marginLeft.isAuto() && style->marginRight.isAuto())
        marginLeft = marginRight = leftover / 2.0f;

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

void BlockLevelBox::layOutText(ViewCSSImp* view, Text text, FormattingContext* context)
{
    CSSStyleDeclarationImp* style = 0;
    Element element = getContainingElement(text);
    if (!element)
        return;  // TODO error

    style = view->getStyle(element);
    if (!style)
        return;  // TODO error
    style->resolve(view, this, element);
    std::u16string data = text.getData();
    if (style->processWhiteSpace(data, context->prevChar) == 0)
        return;

    bool psuedoChecked = false;
    CSSStyleDeclarationImp* firstLineStyle = 0;
    CSSStyleDeclarationImp* firstLetterStyle = 0;
    CSSStyleDeclarationImp* activeStyle = style;

    FontTexture* font = view->selectFont(activeStyle);
    float point = view->getPointFromPx(activeStyle->fontSize.getPx());
    size_t position = 0;
    for (;;) {
        if (!context->lineBox) {
            if (style->processLineHeadWhiteSpace(data) == 0)
                return;
            if (!context->addLineBox(view, this))
                return;  // TODO error
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
                    CSSStyleDeclarationImp* parentStyle = style;
                    for (auto i = firstLineStyles.begin(); i != firstLineStyles.end(); ++i) {
                        (*i)->compute(view, parentStyle, 0);
                        parentStyle = *i;
                    }
                    firstLineStyle = firstLineStyles.back();
                    CSSStyleDeclarationImp* p = style->createPseudoElementStyle(CSSPseudoElementSelector::FirstLine);
                    assert(p);  // TODO
                    p->specify(firstLineStyle);
                    firstLineStyle = p;
                }
                if (!firstLetterStyles.empty()) {
                    CSSStyleDeclarationImp* parentStyle = style;
                    for (auto i = firstLetterStyles.begin(); i != firstLetterStyles.end(); ++i) {
                        (*i)->compute(view, parentStyle, 0);
                        parentStyle = *i;
                    }
                    firstLetterStyle = firstLetterStyles.back();
                    CSSStyleDeclarationImp* p = style->createPseudoElementStyle(CSSPseudoElementSelector::FirstLetter);
                    assert(p);  // TODO
                    p->specify(firstLetterStyle);
                    firstLetterStyle = p;
                }
                if (firstLetterStyle) {
                    activeStyle = firstLetterStyle;
                    font = view->selectFont(activeStyle);
                    point = view->getPointFromPx(activeStyle->fontSize.getPx());
                } else if (firstLineStyle) {
                    activeStyle = firstLineStyle;
                    font = view->selectFont(activeStyle);
                    point = view->getPointFromPx(activeStyle->fontSize.getPx());
                }
            }
        }

        InlineLevelBox* inlineLevelBox = new(std::nothrow) InlineLevelBox(text, activeStyle);
        if (!inlineLevelBox)
            return;  // TODO error
        style->addBox(inlineLevelBox);  // activeStyle? maybe not...
        inlineLevelBox->resolveWidth();
        float blankLeft = inlineLevelBox->getBlankLeft();
        if (0 < position)  // TODO: this is a bit awkward...
            inlineLevelBox->marginLeft = inlineLevelBox->paddingLeft = inlineLevelBox->borderLeft = blankLeft = 0;
        float blankRight = inlineLevelBox->getBlankRight();
        context->x += blankLeft;
        context->leftover -= blankLeft + blankRight;

        size_t fitLength = firstLetterStyle ? 1 : data.length();  // TODO: 1 is absolutely wrong...

        // We are still not sure if there's a room for text in context->lineBox.
        // If there's no room due to float box(es), move the linebox down to
        // the closest bottom of float box.
        // And repeat this process until there's no more float box in the context.
        float advanced;
        size_t length;
        bool linefeed;
        linefeed = false;
        size_t next = 1;
        float required = 0.0f;
        do {
            advanced = context->leftover;
            if (data[0] == '\n') {
                linefeed = true;
                length = 1;
                advanced = 0.0f;
                break;
            }
            length = font->fitText(data.c_str(), fitLength, point, context->leftover, &next, &required);
            if (0 < length) {
                advanced -= context->leftover;
                break;
            }
        } while (context->shiftDownLineBox());
        if (length == 0) {
            context->leftover -= required;
            advanced -= context->leftover;
            length = next;
        }

        if (!linefeed) {
            inlineLevelBox->setData(font, point, data.substr(0, length));
            inlineLevelBox->width = advanced;
            if (!firstLetterStyle && length < data.length()) {  // TODO: firstLetterStyle : actually we are not sure if the following characters would fit in the same line box...
                inlineLevelBox->marginRight = inlineLevelBox->paddingRight = inlineLevelBox->borderRight = blankRight = 0;
            }
        }
        inlineLevelBox->height = font->getHeight(point);
        inlineLevelBox->baseline += (activeStyle->lineHeight.getPx() - inlineLevelBox->height) / 2.0f;
        context->x += advanced + blankRight;
        LineBox* lineBox = context->lineBox;
        lineBox->appendChild(inlineLevelBox);
        lineBox->height = std::max(lineBox->height, std::max(activeStyle->lineHeight.getPx(), inlineLevelBox->height));
        lineBox->baseline = std::max(lineBox->baseline, inlineLevelBox->baseline);
        lineBox->width += blankLeft + advanced + blankRight;
        lineBox->underlinePosition = std::max(lineBox->underlinePosition, font->getUnderlinePosition(point));
        lineBox->underlineThickness = std::max(lineBox->underlineThickness, font->getUnderlineThickness(point));
        position += length;
        data.erase(0, length);
        if (data.length() == 0) {  // layout done?
            if (linefeed)
                context->nextLine(this);
            break;
        }

        if (firstLetterStyle) {
            firstLetterStyle = 0;
            if (firstLineStyle) {
                activeStyle = firstLineStyle;
                font = view->selectFont(activeStyle);
                point = view->getPointFromPx(activeStyle->fontSize.getPx());
            } else {
                activeStyle = style;
                font = view->selectFont(activeStyle);
                point = view->getPointFromPx(activeStyle->fontSize.getPx());
            }
        } else {
            context->nextLine(this);
            if (firstLineStyle) {
                firstLineStyle = 0;
                activeStyle = style;
                font = view->selectFont(activeStyle);
                point = view->getPointFromPx(activeStyle->fontSize.getPx());
            }
        }
    }
}

void BlockLevelBox::layOutInlineReplaced(ViewCSSImp* view, Node node, FormattingContext* context)
{
    CSSStyleDeclarationImp* style = 0;
    Element element = 0;
    if (element = getContainingElement(node)) {
        style = view->getStyle(element);
        if (!style)
            return;  // TODO error
        style->resolve(view, this, element);
    } else
        return;  // TODO error

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
    } else if (BlockLevelBox* inlineBlock = expandBinding(view, element, style)) {
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
    lineBox->height = std::max(lineBox->height, inlineLevelBox->height);  // TODO: marginTop, etc.???
    lineBox->baseline = std::max(lineBox->baseline, inlineLevelBox->baseline);
    lineBox->width += blankLeft + inlineLevelBox->width + blankRight;
}

void BlockLevelBox::layOutFloat(ViewCSSImp* view, Node node, BlockLevelBox* floatBox, FormattingContext* context)
{
    assert(floatBox->style);
    bool first = false;
    if (!context->lineBox) {
        first = true;
        if (!context->addLineBox(view, this))
            return;   // TODO error
    }
    floatBox->layOut(view, context);
    floatBox->remainingHeight = floatBox->getTotalHeight();
    float w = floatBox->getTotalWidth();
    if (context->leftover < w && !first) {
        // Process this float box later in the other line box.
        context->floatNodes.push_back(node);
        return;
    }
    context->addFloat(floatBox, w);
}

void BlockLevelBox::layOutAbsolute(ViewCSSImp* view, Node node, BlockLevelBox* absBox, FormattingContext* context)
{
    // Just insert this absolute box into a line box. We will the abs. box later. TODO
    if (!context->lineBox) {
        if (!context->addLineBox(view, this))
            return;  // TODO error
    }
    context->lineBox->appendChild(absBox);
}

// Generate line boxes
void BlockLevelBox::layOutInline(ViewCSSImp* view, FormattingContext* context)
{
    assert(!hasChildBoxes());
    for (auto i = inlines.begin(); i != inlines.end(); ++i) {
        if (BlockLevelBox* box = view->getFloatBox(*i)) {
            if (box->isFloat())
                layOutFloat(view, *i, box, context);
            else if (box->isAbsolutelyPositioned())
                layOutAbsolute(view, *i, box, context);
        } else if ((*i).getNodeType() == Node::TEXT_NODE) {
            Text text = interface_cast<Text>(*i);
            layOutText(view, text, context);
        } else {
            // At this point, *i should be a replaced element or an inline block element.
            // TODO: it could be of other types as we develop...
            layOutInlineReplaced(view, *i, context);
        }
    }
    if (context->lineBox)
        context->nextLine(this);
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
            if (0 < autoCount)
                min += style->marginRight.getPx();
        }
    }
    return min;
}

void Box::fit(float w)
{
}

void BlockLevelBox::fit(float w)
{
    if (getTotalWidth() <= w)
        return;
    int autoCount = 3;
    float newWidth = w;

    if (style) {
        if (!style->width.isAuto()) {
            --autoCount;
            newWidth = style->width.getPx();
        }
        if (!style->marginLeft.isAuto()) {
            --autoCount;
            newWidth -= style->marginLeft.getPx();
        }
        if (!style->marginRight.isAuto()) {
            --autoCount;
            if (0 < autoCount)
                newWidth -= style->marginRight.getPx();
        }
    }
    newWidth -= borderLeft + paddingLeft + paddingRight + borderRight;
    float shrink = width - newWidth;
    width = newWidth;
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        if (LineBox* line = dynamic_cast<LineBox*>(child))
            line->realignText(this, shrink);
        else
            child->fit(width);
    }
}

void BlockLevelBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    const ContainingBlock* containingBlock = getContainingBlock(view);

    Element element = 0;
    if (!isAnonymous())
        element = getContainingElement(node);
    else if (const Box* box = dynamic_cast<const Box*>(containingBlock))
        element = getContainingElement(box->node);
    if (!element)
        return;  // TODO error

    style = view->getStyle(element);
    if (!style)
        return;  // TODO error

    if (!isAnonymous()) {
        style->resolve(view, containingBlock, element);
        resolveWidth(view, containingBlock,
                     (style->isInlineBlock() || style->isFloat()) ? context->leftover : 0.0f);
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

    // Collapse margins
    if (Box* parent = getParentBox()) {  // TODO: root exception
        if (parent->getFirstChild() == this &&
            parent->borderTop == 0 && parent->paddingTop == 0 /* && TODO: check clearance */) {
            marginTop = std::max(marginTop, parent->marginTop);  // TODO: negative case
            parent->marginTop = 0.0f;
        } else if (Box* prev = getPreviousSibling()) {
            marginTop = std::max(prev->marginBottom, marginTop);  // TODO: negative case
            prev->marginBottom = 0.0f;
        }
    }

    context = updateFormattingContext(context);
    assert(context);
    context->updateRemainingHeight(getBlankTop());
    this->marginTop += context->clear(style->clear.getValue());

    if (hasInline())
        layOutInline(view, context);
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->layOut(view, context);
        // TODO: overflow
        width = std::max(width, child->getTotalWidth());
    }

    if ((style->isInlineBlock() || style->isFloat()) && style->width.isAuto())
        shrinkToFit();

    // Collapse marginBottom  // TODO: root exception
    if (height == 0 && borderBottom == 0 && paddingBottom == 0 /* && TODO: check clearance */) {
        if (Box* child = getLastChild()) {
            marginBottom = std::max(marginBottom, child->marginBottom);  // TODO: negative case
            child->marginBottom = 0;
        }
    }

    if (height == 0) {
        for (Box* child = getFirstChild(); child; child = child->getNextSibling())
            height += child->getTotalHeight();
    }

    if (isFlowRoot()) {
        this->height += context->clear(3);
        // Layout remaining float boxes in context
        while (!context->floatNodes.empty()) {
            context->addLineBox(view, this);
            context->nextLine(this, !context->floatNodes.empty());
        }
    }

    if (!isAnonymous()) {
        width = std::max(width, style->minWidth.getPx());
        height = std::max(height, style->minHeight.getPx());
    }

    // Now that 'height' is fixed, calculate 'left', 'right', 'top', and 'bottom'.
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->resolveOffset(view);
        child->adjustWidth();
    }

    if (backgroundImage && backgroundImage->getState() == BoxImage::CompletelyAvailable) {
        style->backgroundPosition.compute(view, backgroundImage, style->fontSize, getPaddingWidth(), getPaddingHeight());
        backgroundLeft = style->backgroundPosition.getLeftPx();
        backgroundTop = style->backgroundPosition.getTopPx();
    }
}

void BlockLevelBox::adjustWidth()
{
    if (parentBox) {
        float diff = parentBox->width - getTotalWidth();
        if (0.0f < diff) {
            width += diff;
            for (Box* child = getFirstChild(); child; child = child->getNextSibling())
                child->adjustWidth();
        }
    }
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

void BlockLevelBox::layOutAbsolute(ViewCSSImp* view, Node node)
{
    assert(isAbsolutelyPositioned());
    Element element = getContainingElement(node);
    if (!element)
        return;  // TODO error
    if (!style)
        return;  // TODO error

    setContainingBlock(view);
    const ContainingBlock* containingBlock = &absoluteBlock;

    style->resolve(view, containingBlock, element);
    float right;
    float bottom;
    unsigned autoMask = resolveAbsoluteWidth(containingBlock, right, bottom);

    FormattingContext* context = updateFormattingContext(context);
    assert(context);

    if (hasInline())
        layOutInline(view, context);
    for (Box* child = getFirstChild(); child; child = child->getNextSibling()) {
        child->layOut(view, context);
        // TODO: overflow
        width = std::max(width, child->getTotalWidth());
    }

    if (autoMask & Width) {
        shrinkToFit();
        if (autoMask & Left) {
            float left = containingBlock->width - getTotalWidth() - right;
            offsetH += left;
        }
    }
    if ((autoMask & Height) || height == 0) {
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
        child->adjustWidth();
    }
}

void LineBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    // Process 'vertical-align'
    // TODO: assume baseline for now
    for (Box* box = getFirstChild(); box; box = box->getNextSibling()) {
        if (box->isAbsolutelyPositioned())
            continue;
        box->resolveOffset(view);
        if (InlineLevelBox* inlineBox = dynamic_cast<InlineLevelBox*>(box))
            inlineBox->marginTop += getBaseline() - inlineBox->getBaseline();
    }
}

void LineBox::dump(ViewCSSImp* view, std::string indent)
{
    std::cout << indent << "* line box: (" << x << ", " << y << "), (" << getTotalWidth() << ", " << getTotalHeight() << ")\n";
    indent += "    ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(view, indent);
}

void LineBox::realignText(BlockLevelBox* parentBox, float shrink)
{
    shrink -= marginRight;
    if (shrink <= 0.0f)
        return;
    switch (parentBox->getTextAlign()) {
    case CSSTextAlignValueImp::Left:
        break;
    case CSSTextAlignValueImp::Right:
        offsetH -= shrink;
        break;
    case CSSTextAlignValueImp::Center:
        offsetH -= shrink / 2.0f;
        break;
    default:  // TODO: support Justify and Default
        break;
    }
}

void InlineLevelBox::toViewPort(const Box* box, float& x, float& y) const
{
    // In the case of the inline-block, InlineLevelBox holds a block-level box
    // as its only child.
    if (Box* box = getParentBox())
        box->toViewPort(this, x, y);
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

void InlineLevelBox::dump(ViewCSSImp* view, std::string indent)
{
    std::cout << indent << "* inline-level box (" << static_cast<void*>(this) << "): (" <<
        x << ", " << y << "), (" << getTotalWidth() << ", " << getTotalHeight() << ") \"" <<
        data << "\"\n";
    indent += "    ";
    for (Box* child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(view, indent);
}

}}}}  // org::w3c::dom::bootstrap
