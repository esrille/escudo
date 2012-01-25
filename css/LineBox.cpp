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

#include <unicode/uchar.h>

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

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

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

void setActiveStyle(ViewCSSImp* view, CSSStyleDeclarationImp*& activeStyle, CSSStyleDeclarationImp* style, FontTexture*& font, float& point)
{
    activeStyle = style;
    font = activeStyle->getFontTexture();
    point = view->getPointFromPx(activeStyle->fontSize.getPx());
}

size_t getfirstLetterLength(const std::u16string& data)
{
    size_t fitLength = data.size();
    if (0 < fitLength) {
        size_t pos = 0;
        fitLength = 0;
        while (u_ispunct(nextChar(data, pos)))
            fitLength = pos;
        nextChar(data, fitLength);
        while (u_ispunct(nextChar(data, pos)))
            fitLength = pos;
    }
    return fitLength;
}

}

void BlockLevelBox::nextLine(ViewCSSImp* view, FormattingContext* context, CSSStyleDeclarationImp*& activeStyle,
                             CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle,
                             CSSStyleDeclarationImp* style, FontTexture*& font, float& point)
{
    if (firstLetterStyle) {
        firstLetterStyle = 0;
        if (firstLineStyle)
            setActiveStyle(view, activeStyle, firstLineStyle.get(), font, point);
        else
            setActiveStyle(view, activeStyle, style, font, point);
    } else {
        context->nextLine(view, this);
        if (firstLineStyle) {
            firstLineStyle = 0;
            setActiveStyle(view, activeStyle, style, font, point);
        }
    }
}

void BlockLevelBox::getPsuedoStyles(ViewCSSImp* view, FormattingContext* context, CSSStyleDeclarationImp* style,
                                    CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle)
{
    bool isFirstLetter = true;
    for (Box* i = context->lineBox->getFirstChild(); i; i = i->getNextSibling()) {
        if (dynamic_cast<InlineLevelBox*>(i)) {
            isFirstLetter = false;
            break;
        }
    }

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
            if (isFirstLetter) {
                if (CSSStyleDeclarationImp* p = s->getPseudoElementStyle(CSSPseudoElementSelector::FirstLetter))
                    firstLetterStyles.push_front(p);
                if (s->getPseudoElementSelectorType() == CSSPseudoElementSelector::Marker)
                    isFirstLetter = false;
            }
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
            if (style->display.isInline()) {
                // 'style' should inherit properties from 'firstLineStyle'.
                // cf. 7.1.1. First formatted line definition in CSS - Selectors Level 3
                // cf. http://test.csswg.org/suites/css2.1/20110323/html4/first-line-pseudo-021.htm
                firstLineStyle->specifyWithoutInherited(style);
            }
            firstLineStyle->compute(view, getStyle(), 0);
            firstLineStyle->resolve(view, this);
        }
    }
    if (!firstLetterStyles.empty()) {
        firstLetterStyle = new(std::nothrow) CSSStyleDeclarationImp;
        if (firstLetterStyle) {
            for (auto i = firstLetterStyles.begin(); i != firstLetterStyles.end(); ++i)
                firstLetterStyle->specify(*i);
            if (style->display.isInline() && style->getPseudoElementSelectorType() == CSSPseudoElementSelector::NonPseudo)
                firstLetterStyle->specify(style);
            firstLetterStyle->compute(view, firstLineStyle.get() ? firstLineStyle.get() : style, 0);
            firstLetterStyle->resolve(view, this);
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

    bool psuedoChecked = isAnonymous() && getParentBox()->getFirstChild() != this;
    CSSStyleDeclarationPtr firstLineStyle;
    CSSStyleDeclarationPtr firstLetterStyle;
    CSSStyleDeclarationImp* activeStyle;
    FontTexture* font;
    float point;
    setActiveStyle(view, activeStyle, style, font, point);

    size_t position = 0;
    for (;;) {
        if (context->atLineHead && discardable && style->processLineHeadWhiteSpace(data) == 0)
            return !isAnonymous();
        if (!context->lineBox) {
            if (!context->addLineBox(view, this))
                return false;  // TODO error
        }
        if (!psuedoChecked && getFirstChild() == context->lineBox) {
            psuedoChecked  = true;
            getPsuedoStyles(view, context, style, firstLetterStyle, firstLineStyle);
            if (firstLetterStyle) {
                setActiveStyle(view, activeStyle, firstLetterStyle.get(), font, point);
                if (firstLetterStyle->isFloat()) {
                    size_t length = getfirstLetterLength(data);
                    Document document = view->getDocument();
                    html::HTMLDivElement div = interface_cast<html::HTMLDivElement>(document.createElement(u"div"));
                    Text text = document.createTextNode(data.substr(0, length));
                    div.appendChild(text);
                    BlockLevelBox* floatingBox = view->createBlockLevelBox(div, firstLetterStyle.get(), true);
                    floatingBox->insertInline(text);
                    view->addFloatBox(div, floatingBox, firstLetterStyle.get());
                    inlines.push_front(div);
                    layOutFloat(view, div, floatingBox, context);
                    data.erase(0, length);
                    if (data.length() == 0)
                        break;
                    nextLine(view, context, activeStyle, firstLetterStyle, firstLineStyle, style, font, point);
                    continue;
                }
            } else if (firstLineStyle)
                setActiveStyle(view, activeStyle, firstLineStyle.get(), font, point);
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
            size_t fitLength = data.length();
            if (firstLetterStyle)
                fitLength = getfirstLetterLength(data);
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
    float leftover = std::max(0.0f, w - shrinkTo());
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
    assert(data[0] != 0 || data.empty());
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
