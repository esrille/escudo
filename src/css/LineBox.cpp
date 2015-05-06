/*
 * Copyright 2010-2015 Esrille Inc.
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
#include "DocumentImp.h"
#include "FormattingContext.h"
#include "StackingContext.h"
#include "ViewCSSImp.h"
#include "WindowProxy.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

namespace {

CSSStyleDeclarationPtr setActiveStyle(ViewCSSImp* view, const CSSStyleDeclarationPtr& style, FontTexture*& font, float& point)
{
    font = style->getFontTexture();
    point = view->getPointFromPx(style->fontSize.getPx());
    return style;
}

}

void Block::getPsuedoStyles(ViewCSSImp* view, FormattingContext* context, const CSSStyleDeclarationPtr& style,
                            CSSStyleDeclarationPtr& firstLetterStyle, CSSStyleDeclarationPtr& firstLineStyle)
{
    bool isFirstLetter = true;
    if (context->lineBox) {
        for (BoxPtr i = context->lineBox->getFirstChild(); i; i = i->getNextSibling()) {
            if (std::dynamic_pointer_cast<InlineBox>(i)) {
                isFirstLetter = false;
                break;
            }
        }
    }

    // The current line box is the 1st line of this block box.
    // style to use can be a pseudo element styles from any ancestor elements.
    // Note the :first-line, first-letter pseudo-elements can only be attached to a block container element.
    std::list<CSSStyleDeclarationPtr> firstLineStyles;
    std::list<CSSStyleDeclarationPtr> firstLetterStyles;
    BoxPtr box = self();
    for (;;) {
        if (CSSStyleDeclarationPtr s = box->getStyle()) {
            if (CSSStyleDeclarationPtr p = s->getPseudoElementStyle(CSSPseudoElementSelector::FirstLine))
                firstLineStyles.push_front(p);
            if (isFirstLetter) {
                if (CSSStyleDeclarationPtr p = s->getPseudoElementStyle(CSSPseudoElementSelector::FirstLetter))
                    firstLetterStyles.push_front(p);
                if (s->getPseudoElementSelectorType() == CSSPseudoElementSelector::Marker)
                    isFirstLetter = false;
            }
        }
        BoxPtr parent = box->getParentBox();
        if (!parent || parent->getFirstChild() != box)
            break;
        box = parent;
    }
    if (!firstLineStyles.empty()) {
        firstLineStyle = std::make_shared<CSSStyleDeclarationImp>(CSSPseudoElementSelector::FirstLine);
        if (firstLineStyle) {
            for (auto i = firstLineStyles.begin(); i != firstLineStyles.end(); ++i)
                firstLineStyle->specify(*i);
            if (style->display.isInline()) {
                // 'style' should inherit properties from 'firstLineStyle'.
                // cf. 7.1.1. First formatted line definition in CSS - Selectors Level 3
                // cf. http://test.csswg.org/suites/css2.1/20110323/html4/first-line-pseudo-021.htm
                firstLineStyle->specifyWithoutInherited(style);
            }
            firstLineStyle->compute(view, getStyle(), nullptr);
            firstLineStyle->resolve(view, self());
        }
    }
    if (!firstLetterStyles.empty()) {
        firstLetterStyle = std::make_shared<CSSStyleDeclarationImp>(CSSPseudoElementSelector::FirstLetter);
        if (firstLetterStyle) {
            for (auto i = firstLetterStyles.begin(); i != firstLetterStyles.end(); ++i)
                firstLetterStyle->specify(*i);
            if (style->display.isInline() && style->getPseudoElementSelectorType() == CSSPseudoElementSelector::NonPseudo)
                firstLetterStyle->specify(style);
            firstLetterStyle->compute(view, firstLineStyle ? firstLineStyle : style, nullptr);
            firstLetterStyle->resolve(view, self());
        }
    }
}

size_t Block::layOutFloatingFirstLetter(ViewCSSImp* view, FormattingContext* context, const std::u16string& data, const CSSStyleDeclarationPtr& firstLetterStyle)
{
    DocumentPtr document = view->getDocument();
    if (!floatingFirstLetter)
        floatingFirstLetter = document->createElement(u"div");
    else {
        while (Node child = floatingFirstLetter.getFirstChild())
            floatingFirstLetter.removeChild(child);
    }
    size_t length = firstLetterStyle->getfirstLetterLength(data, 0);  // TODO: position?
    Text text = document->createTextNode(data.substr(0, length));
    floatingFirstLetter.appendChild(text);
    BlockPtr floatingBox = view->createBlock(floatingFirstLetter, self(), firstLetterStyle, true);
    floatingBox->insertInline(text);
    floatingBox->flags &= ~(Box::NEED_EXPANSION | Box::NEED_CHILD_EXPANSION);
    addBlock(floatingFirstLetter, floatingBox);
    view->addStyle(floatingFirstLetter, firstLetterStyle);
    inlines.push_front(floatingFirstLetter);
    floatingBox->layOut(view, context);
    layOutFloat(view, floatingFirstLetter, floatingBox, context);
    return length;
}

namespace {

size_t trimSpacesAtBack(const std::u16string& data, size_t offset, size_t length, bool preLine)
{
    size_t len;
    for (len = length; 0 < len; --len) {
        char16_t u = data[offset + len - 1];
        switch (u) {
        case '\t':
            u = ' ';
            break;
        case '\r':
            u = '\n';
            // FALL THROUGH
        case '\n':
            if (!preLine)
                u = ' ';
            break;
        default:
            break;
        }
        if (u != ' ')
            break;
    }
    return len;
}

}

bool Block::layOutText(ViewCSSImp* view, Node text, FormattingContext* context,
                       std::u16string data, Element element, const CSSStyleDeclarationPtr& style,
                       WrapControl& wrapControl)
{
    assert(element);
    assert(style);

    // An empty inline element should pass 'data' as an empty string. In this case,
    // the inline box to be generated must not be collapsed away by returning false.
    // cf. 10.8 Line height calculations: the ’line-height’ and ’vertical-align’ properties
    bool discardable = !data.empty();
    if (discardable && style->display.isInline()) {
        // TODO: Check emptyInline
        if (element.getFirstChild() == text && (style->marginLeft.getPx() || style->borderLeftWidth.getPx() || style->paddingLeft.getPx()) ||
            element.getLastChild() == text && (style->marginRight.getPx() || style->borderRightWidth.getPx() || style->paddingRight.getPx()))
            discardable = false;
    }

    FontTexture* font;
    float point;
    CSSStyleDeclarationPtr activeStyle = setActiveStyle(view, style, font, point);
    CSSStyleDeclarationPtr firstLineStyle;
    CSSStyleDeclarationPtr firstLetterStyle;

    if (!isAnonymous() || getParentBox()->getFirstChild() == self()) {
        if (!getFirstChild())
            context->isFirstLetter = true;
        if (!getFirstChild() || childCount == 1 && getFirstChild()->getBoxType() == LINE_BOX && context->lineBox)
            getPsuedoStyles(view, context, style, firstLetterStyle, firstLineStyle);
    }

    size_t base(0);
    size_t position(0);   // within data
    if (wrapControl.isRestart()) {
        position = wrapControl.getOffset();
        wrapControl.clearRestart();
    }
    size_t wrap(position);
    float wrapWidth(0.0f);
    InlineBoxPtr inlineBox;
    FontGlyph* glyph(nullptr);
    float blankLeft(0.0f);
    float blankRight(0.0f);

    // The outer loop processes line boxes and finished inline boxes:
    do {
        bool linefeed(false);
        LineBoxPtr lineBox;

        // Check pseudo
        if (firstLetterStyle)
            activeStyle = setActiveStyle(view, firstLetterStyle, font, point);
        else if (firstLineStyle)
            activeStyle = setActiveStyle(view, firstLineStyle, font, point);

        if (context->atLineHead || context->prevChar == ' ') {
            context->prevChar = '\n';
            size_t skipped = activeStyle->skipWhiteSpace(data, position);
            if (position < skipped) {
                if (position == 0)
                    base = skipped;
                position = skipped;
                if (!context->atLineHead && activeStyle->whiteSpace.isBreakingLines())
                    wrapControl.markBreakable();
            }
        }

        // The inner loop processes the data for completing an inline box:
        do {
            size_t offset(position);
            bool isFirstLetter(context->isFirstLetter);
            char32_t prevChar(context->prevChar);
            std::u16string word;
            size_t length(0);
            bool breakable(false);

            if (activeStyle != firstLetterStyle)
                length = activeStyle->getNextWord(data, position, context->isFirstLetter, context->prevChar, word);
            else
                length = activeStyle->getFirstLetter(data, position, context->isFirstLetter, context->prevChar, word);
            breakable = (position < data.length() && activeStyle->whiteSpace.isBreakingLines());
            if (word.empty()) {
                if (!context->atLineHead && activeStyle->whiteSpace.isBreakingLines() && offset < data.length())
                    wrapControl.markBreakable();    // cf. html4/white-space-007.htm
                if (discardable)
                    return !isAnonymous();
            } else {
                if (offset == 0 && activeStyle->whiteSpace.isBreakingLines() && !context->atLineHead && prevChar) {
                    TextIterator ti;
                    std::u16string test;
                    append(test, prevChar);
                    size_t testLength = test.length();
                    test += word;
                    ti.setText(test.c_str(), test.length());
                    if (ti.next() && *ti == testLength)
                        wrapControl.markBreakable();
                }
                if (word.front() == u'\n' || word.back() == '\n') {
                    linefeed = true;
                    breakable= true;
                }
            }

            // Prepare a new line box.
            discardable = false;
            context->useMargin(self());
            if (!context->lineBox) {
                if (!context->addLineBox(view, self()))
                    return false;  // TODO error
            }
            lineBox = context->lineBox;

#if 0
            // XXX Use word for the inline box created inside the floating box.
            if (firstLetterStyle && firstLetterStyle->isFloat()) {
                position += layOutFloatingFirstLetter(view, context, data, firstLetterStyle);
                if (data.length() <= position)
                    break;
                nextLine(view, context, activeStyle, firstLetterStyle, firstLineStyle, style, false, font, point);
                continue;
            }
#endif

            if (!inlineBox) {
                try {
                    inlineBox = std::make_shared<InlineBox>(text, activeStyle);
                } catch (...) {
                    return false;
                }
                inlineBox->resolveWidth();
                if (base < offset || !inlineBox->isEmptyInlineAtFirst(style, element, text))
                    inlineBox->clearBlankLeft();
                blankLeft = inlineBox->getBlankLeft();
                blankRight = inlineBox->getBlankRight();
                context->x += blankLeft;
                context->leftover -= blankLeft;
            }

            float w = activeStyle->measureText(view, data, offset, length, point, isFirstLetter, prevChar, glyph);
            if (firstLetterStyle || data.length() <= position && inlineBox->isEmptyInlineAtLast(style, element, text))
                w += blankRight;    // BWBAL: blankRight will be adjusted later

            while (context->leftover < w && (wrapControl.isBreakable() || activeStyle->whiteSpace.isBreakingLines())) {
                if (activeStyle->whiteSpace.isCollapsingSpace() && !word.empty() && word.back() == u' ') {
                    float lineEnd = w - glyph->advance * font->getScale(point) - activeStyle->wordSpacing.getPx();
                    if (!activeStyle->letterSpacing.isNormal())
                        lineEnd -= activeStyle->letterSpacing.getPx();
                    if (lineEnd <= context->leftover || lineEnd <= 0.0f) {
                        breakable = true;
                        context->dontWrap();
                        w = lineEnd;
                        length = trimSpacesAtBack(data, offset, length, activeStyle->whiteSpace.getValue() == CSSWhiteSpaceValueImp::PreLine);
                        linefeed = true;
                        break;
                    }
                }
                // This text segment doesn't fit in the current line.
                if (inlineBox->isBreakable()) {
                    position = offset;
                    length = 0;
                    w = 0.0f;
                    context->isFirstLetter = isFirstLetter;
                    context->prevChar = prevChar;
                    linefeed = true;
                    break;
                }
                if (wrapControl.isBreakable()) {
                    context->removeWrapBox(wrapControl);
                    wrapControl.clearMCW();
                    wrapControl.restart();
                    // if (lineBox->hasChildBoxes() || context->hasNewFloats()) {
                    position = offset;
                    length = 0;
                    context->isFirstLetter = isFirstLetter;
                    context->prevChar = prevChar;
                    linefeed = true;
                    inlineBox = 0;
                    break;
                }
                if (!context->shiftDownLineBox(view))
                    break;
            }
            if (!inlineBox)
                break;

            if (offset < position) {
                context->x += w;
                context->leftover -= w;
                inlineBox->width += w;
                if (firstLetterStyle || data.length() <= position && inlineBox->isEmptyInlineAtLast(style, element, text))
                    inlineBox->width -= blankRight;     // Amend BWBAL.
            }

            // cf. html4/white-space-normal-001.htm
            if (activeStyle->whiteSpace.isCollapsingSpace() && !word.empty() && word.back() == u' ')
                w -= glyph->advance * font->getScale(point) + activeStyle->wordSpacing.getPx();

            wrapControl.extendMCW(w);
            updateMCW(wrapControl.getMCW());
            if (breakable) {
                wrap = position;
                wrapWidth = inlineBox->width;
                wrapControl.clearMCW();
            }

            inlineBox->setData(font, point, offset, length, wrap, wrapWidth);
        } while (activeStyle != firstLetterStyle && !linefeed && position < data.length());

        if (inlineBox) {
            if (!firstLetterStyle && (position < data.length() || !inlineBox->isEmptyInlineAtLast(style, element, text))) {
                inlineBox->clearBlankRight();
                blankRight = 0.0f;
            }

            if (inlineBox->hasHeight()) {   // TODO: This can be done in LineBox::layOut().
                // Switch height from 'line-height' to the content height.
                inlineBox->height = font->getLineHeight(point);
                inlineBox->leading = activeStyle->lineHeight.getPx() - inlineBox->height;
                lineBox->underlinePosition = std::max(lineBox->underlinePosition, font->getUnderlinePosition(point));
                lineBox->underlineThickness = std::max(lineBox->underlineThickness, font->getUnderlineThickness(point));
                lineBox->lineThroughPosition = std::max(lineBox->lineThroughPosition, font->getLineThroughPosition(point));
                lineBox->lineThroughThickness = std::max(lineBox->lineThroughThickness, font->getLineThroughThickness(point));
            }
            context->appendInlineBox(view, inlineBox, activeStyle);
            style->addBox(inlineBox);  // activeStyle? maybe not...
            if (inlineBox->isBreakable()) {
                if (!linefeed)
                    wrapControl.set(text, wrap);
                else
                    wrapControl.clear();
            }
            inlineBox = 0;
        }

        if (firstLetterStyle) {
            firstLetterStyle = nullptr;
            if (firstLineStyle)
                activeStyle = setActiveStyle(view, firstLineStyle, font, point);
            else
                activeStyle = setActiveStyle(view, style, font, point);
        }
        if (linefeed) {
            context->nextLine(view, self(), linefeed);
            if (firstLineStyle) {
                firstLineStyle = nullptr;
                activeStyle = setActiveStyle(view, style, font, point);
            }
        }

        if (wrapControl.isRestart())
            break;
    } while (position < data.length());
    return true;
}

LineBox::LineBox(const CSSStyleDeclarationPtr& style) :
    Box(nullptr),
    baseline(0.0f),
    underlinePosition(0.0f),
    underlineThickness(1.0f),
    lineThroughPosition(0.0f),
    lineThroughThickness(1.0f),
    leftGap(0.0f),
    rightGap(0.0f)
{
    setStyle(style);
    // Keep 'height' 0.0f here since float and positioned elements are
    // also placed in line boxes in this implementation.
}

bool LineBox::layOut(ViewCSSImp* view, FormattingContext* context)
{
    for (BoxPtr box = getFirstChild(); box; box = box->getNextSibling()) {
        if (box->isAbsolutelyPositioned())
            continue;
        if (auto inlineBox = std::dynamic_pointer_cast<InlineBox>(box)) {
            const CSSStyleDeclarationPtr& style = box->getStyle();
            if (style && style->display.isInlineLevel())
                inlineBox->offsetV = style->verticalAlign.getOffset(view, style.get(), self(), inlineBox);
            else {
                float leading = inlineBox->getLeading() / 2.0f;
                inlineBox->offsetV = getBaseline() - (leading + inlineBox->getBaseline());
            }
        }
    }
    return true;
}

float LineBox::shrinkTo()
{
    float w = Box::shrinkTo();

    float wl = 0.0f;
    float l = 0.0f;
    for (auto child = getFirstChild(); child; child = child->getNextSibling()) {
        if (child->isAnonymous() || !child->style)
            break;
        if (child->style->float_.getValue() == CSSFloatValueImp::Left) {
            float e = child->getEffectiveTotalWidth();
            wl += e;
            l += e;
            if (e == 0.0f)
                l = std::max(wl + child->getTotalWidth(), l);
        } else
            break;
    }

    float wr = 0.0f;
    float r = 0.0f;
    for (auto child = getLastChild(); child; child = child->getPreviousSibling()) {
        if (child->isAnonymous() || !child->style)
            break;
        if (child->style->float_.getValue() == CSSFloatValueImp::Right) {
            float e = child->getEffectiveTotalWidth();
            wr += e;
            r += e;
            if (e == 0.0f)
                r = std::max(wr + child->getTotalWidth(), r);
        } else
            break;
    }

    w += wl + wr;
    return std::max(w, std::max(l, r));
}

void LineBox::fit(float w, FormattingContext* context)
{
    assert(getParentBox());
    assert(std::dynamic_pointer_cast<Block>(getParentBox()));
    float leftover = std::max(0.0f, w - shrinkTo());
    switch (std::dynamic_pointer_cast<Block>(getParentBox())->getTextAlign()) {
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

void LineBox::resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr& clip)
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
        BlockPtr floatingBox;
        next = left;
        if (!child->isAbsolutelyPositioned()) {
            if (!child->isFloat())
                next += child->getTotalWidth();
            else {
                floatingBox = std::dynamic_pointer_cast<Block>(child);
                assert(floatingBox);
                if (floatingBox == getRightBox())
                    break;
                next = left + floatingBox->getEffectiveTotalWidth();
            }
        }
        if (!usedLeftGap && !floatingBox) {
            left += leftGap;
            next += leftGap;
            usedLeftGap = true;
        }
        child->resolveXY(view, left, top, clip);
        left = next;
    }
    if (getRightBox()) {
        float right = x + getParentBox()->width - getBlankRight();
        for (auto child = getLastChild(); child; child = child->getPreviousSibling()) {
            BlockPtr floatingBox = std::dynamic_pointer_cast<Block>(child);
            right -= floatingBox->getEffectiveTotalWidth();
            child->resolveXY(view, right, top, clip);
            if (floatingBox == getRightBox())
                break;
        }
    }
}

void LineBox::dump(std::string indent)
{
    float relativeX = getParentBox()->stackingContext->getRelativeX();
    float relativeY = getParentBox()->stackingContext->getRelativeY();
    std::cout << indent << "* line box";
    if (3 <= getLogLevel())
        std::cout << " [" << uid << '|' << std::hex << flags << std::dec << '(' << count_() << ")]";
    std::cout << " (" << x + relativeX << ", " << y + relativeY << ") " <<
        "w:" << width << " h:" << height << " (" << relativeX << ", " << relativeY <<") ";
    if (hasClearance())
        std::cout << "c:" << clearance << ' ';
    std::cout << "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << '\n';
    indent += "  ";
    for (BoxPtr child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

InlineBox::InlineBox(Node node, const CSSStyleDeclarationPtr& style) :
    Box(node),
    font(0),
    point(0.0f),
    baseline(0.0f),
    leading(0.0f),
    offset(0),
    length(0),
    wrap(0),
    wrapWidth(0.0f),
    emptyInline(0)
{
    if (style) {
        setStyle(style);
        visibility = style->visibility.getValue();
    }
}

bool InlineBox::isEmptyInlineAtFirst(const CSSStyleDeclarationPtr& style, Element& element, Node& node)
{
    if (element != node)
        return (element.getFirstChild() == node) && !(style->getEmptyInline() & 1);
    if (!emptyInline)
        emptyInline = style->checkEmptyInline();
    return (emptyInline & 1) || emptyInline == 4;
}

bool InlineBox::isEmptyInlineAtLast(const CSSStyleDeclarationPtr& style, Element& element, Node& node)
{
    if (element != node)
        return (element.getLastChild() == node) && !(style->getEmptyInline() & 2);
    if (!emptyInline)
        emptyInline = style->checkEmptyInline();
    return !(emptyInline & 1) && ((emptyInline & 2) || emptyInline == 4);
}

void InlineBox::setData(FontTexture* font, float point, size_t offset, size_t length, size_t wrap, float wrapWidth)
{
    this->font = font;
    this->point = point;
    baseline = font->getAscender(point);

    if (this->length == 0) {
        this->offset = offset;
        this->length = length;
        this->wrap = wrap;
        this->wrapWidth = wrapWidth;
    } else {
        this->length = offset + length - this->offset;
        if (this->offset < wrap) {
            this->wrap = wrap;
            this->wrapWidth = wrapWidth;
        }
    }
    if (wrap < this->offset)
        wrap = this->offset;
}

// Trim this inline box at the wrap position.
void InlineBox::trim()
{
    clearBlankRight();
    length = wrap - offset;
    width = wrapWidth;
}

float InlineBox::atEndOfLine()
{
    if (length == 0)
        return 0.0f;
    if (style->whiteSpace.isCollapsingSpace()) {
        std::u16string data;
        if (node.getNodeType() == Node::TEXT_NODE) {
            Text text = interface_cast<Text>(node);
            data = text.substringData(offset, length);
            size_t len = trimSpacesAtBack(data, 0, length, getStyle()->whiteSpace.getValue() == CSSWhiteSpaceValueImp::PreLine);
            if (len < length) {
                length = len;
                // TODO: Deal with the errors in floating point operations.
                float w = -font->measureText(u" ", point) - getStyle()->wordSpacing.getPx();
                if (!getStyle()->letterSpacing.isNormal())
                    w -= getStyle()->letterSpacing.getPx();
                width += w;
                return w;
            }
        }
    }
    return 0.0f;
}

float InlineBox::getSub() const
{
    if (!font)
        return 0.0f;
    return font->getSub(point);
}

float InlineBox::getSuper() const
{
    if (!font)
        return 0.0f;
    return font->getSuper(point);
}

void InlineBox::resolveWidth()
{
    // The ‘width’ and ‘height’ properties do not apply.
    if (isInline()) {
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

void InlineBox::resolveXY(ViewCSSImp* view, float left, float top, const BlockPtr& clip)
{
    left += offsetH;
    top += offsetV + leading / 2.0f;
    if (!childWindow && getFirstChild())
        getFirstChild()->resolveXY(view, left + getBlankLeft(), top + getBlankTop(), clip);
    x = left;
    y = top;
    clipBox = clip;

    if (isPositioned()) {
        assert(getStyle());
        getStyle()->getStackingContext()->setClipBox(clip);
    }
}

void InlineBox::dump(std::string indent)
{
    const CSSStyleDeclarationPtr& activeStyle = getStyle();
    float relativeX = stackingContext->getRelativeX();
    float relativeY = stackingContext->getRelativeY();
    std::cout << indent << "* inline-level box";
    if (3 <= getLogLevel())
        std::cout << " [" << uid << '|' << std::hex << flags << std::dec << '(' << count_() << ")]";

    std::u16string data;
    std::u16string transformed;
    if (node.getNodeType() == Node::TEXT_NODE) {
        Text text = interface_cast<Text>(node);
        data = text.substringData(offset, length);
        for (size_t position = 0; position < length; ) {
            char32_t u = activeStyle->nextChar(data, position, dumpIsFirstLetter, dumpPrevChar);
            dumpPrevChar = u;
            if (u == '\n' || u == u'\u200B')
                continue;
            append(transformed, u);
        }
    }

    std::cout << " (" << x + relativeX << ", " << y + relativeY << ") " <<
        "w:" << width << " h:" << height << ' ' <<
        "m:" << marginTop << ':' << marginRight << ':' << marginBottom << ':' << marginLeft << ' ' <<
        "p:" << paddingTop << ':' <<  paddingRight << ':'<< paddingBottom<< ':' << paddingLeft << ' ' <<
        "b:" << borderTop << ':' <<  borderRight << ':' << borderBottom<< ':' << borderLeft << ' ' <<
        '"' << transformed << "\" ";
    if (activeStyle)
        std::cout << std::hex << CSSSerializeRGB(activeStyle->color.getARGB()) << std::dec;
    std::cout << '\n';
    indent += "  ";
    for (BoxPtr child = getFirstChild(); child; child = child->getNextSibling())
        child->dump(indent);
}

}}}}  // org::w3c::dom::bootstrap
