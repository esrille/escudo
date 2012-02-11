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

#include "CSSPropertyValueImp.h"

#include <org/w3c/dom/Element.h>

#include "CounterImp.h"
#include "CSSStyleDeclarationImp.h"
#include "ViewCSSImp.h"
#include "Box.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

const char16_t* CSSNumericValue::Units[] = {
    u"%",
    u"em",
    u"ex",
    u"px",
    u"cm",
    u"mm",
    u"in",
    u"pt",
    u"pc",
    u"deg",
    u"rad",
    u"grad",
    u"ms",
    u"s",
    u"Hz",
    u"kHz"
};

const char16_t* CSSBackgroundAttachmentValueImp::Options[] = {
    u"scroll",
    u"fixed"
};

const char16_t* CSSBackgroundRepeatValueImp::Options[] = {
    u"no-repeat",
    u"repeat-x",
    u"repeat-y",
    u"repeat"
};

const char16_t* CSSBorderCollapseValueImp::Options[] = {
    u"collapse",
    u"separate"
};

const char16_t* CSSBorderStyleValueImp::Options[] = {
    u"none",
    u"inset",
    u"groove",
    u"outset",
    u"ridge",
    u"dotted",
    u"dashed",
    u"solid",
    u"double",
    u"hidden"
};

const char16_t* CSSBorderWidthValueImp::Options[] = {
    u"thin",
    u"medium",
    u"thick"
};

const char16_t* CSSCaptionSideValueImp::Options[] = {
    u"top",
    u"bottom"
};

const char16_t* CSSClearValueImp::Options[] = {
    u"none",
    u"left",
    u"right",
    u"both"
};

const char16_t* CSSContentValueImp::Options[] = {
    u"normal",
    u"none",
    u"open-quote",
    u"close-quote",
    u"no-open-quote",
    u"no-close-quote",
    u"attr",
};

const char16_t* CSSDirectionValueImp::Options[] = {
    u"ltr",
    u"rtl"
};

const char16_t* CSSDisplayValueImp::Options[] = {
    u"inline",
    u"block",
    u"list-item",
    u"run-in",
    u"inline-block",
    u"table",
    u"inline-table",
    u"table-row-group",
    u"table-header-group",
    u"table-footer-group",
    u"table-row",
    u"table-column-group",
    u"table-column",
    u"table-cell",
    u"table-caption",
    u"none"
};

const char16_t* CSSFloatValueImp::Options[] = {
    u"none",
    u"left",
    u"right"
};

const char16_t* CSSFontFamilyValueImp::Options[] = {
    u"serif",
    u"sans-serif",
    u"cursive",
    u"fantasy",
    u"monospace"
};

const char16_t* CSSFontSizeValueImp::Options[] = {
    u"xx-small",
    u"x-small",
    u"small",
    u"medium",
    u"large",
    u"x-large",
    u"xx-large",
    u"larger",
    u"smaller"
};

const char16_t* CSSFontStyleValueImp::Options[] = {
    u"normal",
    u"italic",
    u"oblique"
};

const char16_t* CSSFontVariantValueImp::Options[] = {
    u"normal",
    u"small-caps"
};

const char16_t* CSSFontWeightValueImp::Options[] = {
    u"normal",
    u"bold",
    u"bolder",
    u"lighter",
};

const char16_t* CSSFontShorthandImp::Options[] = {
    u"normal",
    u"caption",
    u"icon",
    u"menu",
    u"message-box",
    u"small-caption"
    u"status-bar"
};

const char16_t* CSSListStylePositionValueImp::Options[] = {
    u"inside",
    u"outside"
};

const char16_t* CSSListStyleTypeValueImp::Options[] = {
    u"none",
    u"disc",
    u"circle",
    u"square",
    u"decimal",
    u"decimal-leading-zero",
    u"lower-roman",
    u"upper-roman",
    u"lower-greek",
    u"lower-latin",
    u"upper-latin",
    u"armenian",
    u"georgian",
    u"lower-alpha",
    u"upper-alpha"
};

const char16_t* CSSOverflowValueImp::Options[] = {
    u"visible",
    u"hidden",
    u"scroll",
    u"auto"
};

const char16_t* CSSPageBreakValueImp::Options[] = {
    u"auto",
    u"always",
    u"avoid",
    u"left",
    u"right"
};

const char16_t* CSSPositionValueImp::Options[] = {
    u"static",
    u"relative",
    u"absolute",
    u"fixed"
};

const char16_t* CSSTableLayoutValueImp::Options[] = {
    u"auto",
    u"fixed",
};

const char16_t* CSSTextAlignValueImp::Options[] = {
    u"left",
    u"right",
    u"center",
    u"justify",
    u""
};

const char16_t* CSSTextDecorationValueImp::Options[] = {
    u"none",
    u"underline",
    u"overline",
    u"line-through",
    u"blink"
};

const char16_t* CSSTextTransformValueImp::Options[] = {
    u"none",
    u"capitalize",
    u"uppercase",
    u"lowercase"
};

const char16_t* CSSVerticalAlignValueImp::Options[] = {
    u"baseline",
    u"sub",
    u"super",
    u"top",
    u"text-top",
    u"middle",
    u"bottom",
    u"text-bottom"
};

const char16_t* CSSUnicodeBidiValueImp::Options[] = {
    u"normal",
    u"embed",
    u"bidi-override"
};

const char16_t* CSSVisibilityValueImp::Options[] = {
    u"visible",
    u"hidden",
    u"collapse"
};

const char16_t* CSSWhiteSpaceValueImp::Options[] = {
    u"normal",
    u"pre",
    u"nowrap",
    u"pre-wrap",
    u"pre-line"
};

const char16_t* CSSBindingValueImp::Options[] = {
    u"none",
    u"button",
    u"details",
    u"input-textfield",
    u"input-password",
    u"input-datetime",
    u"input-date",
    u"input-month",
    u"input-week",
    u"input-time",
    u"input-datetime-local",
    u"input-number",
    u"input-range",
    u"input-color",
    u"input-checkbox",
    u"input-radio",
    u"input-file",
    u"input-button",
    u"marquee",
    u"meter",
    u"progress",
    u"select",
    u"textarea",
    u"keygen",
    u"time",
};

void CSSNumericValue::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (isIndex() || isPercentage())
        return;
    resolve(view, style, 0.0f);
}

void CSSNumericValue::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize)
{
    if (isIndex())
        return;
    float w;
    switch (unit) {
    case css::CSSPrimitiveValue::CSS_PERCENTAGE:
        w = view->getPx(*this, fullSize);
        break;
    case css::CSSPrimitiveValue::CSS_EMS:
        w = view->getPx(*this, style->fontSize.getPx());
        break;
    case css::CSSPrimitiveValue::CSS_EXS:
        if (FontTexture* font = style->getFontTexture())
            w = view->getPx(*this, font->getXHeight(view->getPointFromPx(style->fontSize.getPx())));
        else
            w = view->getPx(*this, style->fontSize.getPx() * 0.5f);
        break;
    default:
        w = view->getPx(*this);
        break;
    }
    setValue(w, css::CSSPrimitiveValue::CSS_PX);
}

void CSSNumericValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    value.compute(view, style);
}

void CSSNumericValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize)
{
    value.resolve(view, style, fullSize);
}

void CSSAutoLengthValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (isAuto() || length.isPercentage())
        return;  // leave as it is
    length.compute(view, style);
}

void CSSAutoLengthValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize)
{
    if (isAuto())
        return;  // leave length as auto
    length.resolve(view, style, fullSize);
}

void CSSNoneLengthValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize)
{
    if (isNone())
        return;  // leave length as none
    length.resolve(view, style, fullSize);
}

void CSSLetterSpacingValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (isNormal())
        return;  // leave as it is
    length.compute(view, style);
}

void CSSLetterSpacingValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize)
{
    if (isNormal())
        length.setValue(0.0f, CSSPrimitiveValue::CSS_PX);
}

void CSSWordSpacingValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (isNormal()) {
        length.setValue(0.0f, CSSPrimitiveValue::CSS_PX);
        return;  // leave as it is
    }
    length.compute(view, style);
}

bool CSSAutoNumberingValueImp::CounterContext::hasCounter(const std::u16string& name) const
{
    for (auto i = counters.begin(); i != counters.end(); ++i) {
        if ((*i)->getIdentifier() == name)
            return true;
    }
    return false;
}

void CSSAutoNumberingValueImp::CounterContext::update(CSSStyleDeclarationImp* style)
{
    style->updateCounters(view, this);
}

CSSAutoNumberingValueImp::CounterContext::~CounterContext()
{
    if (!view)
        return;
    for (auto i = counters.begin(); i != counters.end(); ++i) {
        if (CounterImpPtr counter = view->getCounter((*i)->getIdentifier()))
            counter->restore();
    }
}

bool CSSAutoNumberingValueImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->getIndex() == 0)  // 'none'
            return true;
        assert(term->unit == CSSPrimitiveValue::CSS_IDENT);
        std::u16string name = term->text.toString(true);
        int number = defaultNumber;
        ++i;
        if (i != stack.end() && (*i)->unit == CSSPrimitiveValue::CSS_NUMBER)
            number = static_cast<int>((*i)->number);
        else
            --i;
        if (Content* content = new(std::nothrow) Content(name, number))
            contents.push_back(content);
    }
    return true;
}

std::u16string CSSAutoNumberingValueImp::getCssText(CSSStyleDeclarationImp* decl)
{
    if (contents.empty())
        return u"none";
    std::u16string cssText;
    for (auto i = contents.begin(); i != contents.end(); ++i) {
        if (i != contents.begin())
            cssText += u' ';
        cssText += (*i)->getCssText(defaultNumber);
    }
    return cssText;
}

void CSSAutoNumberingValueImp::incrementCounter(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context)
{
    for (auto i = contents.begin(); i != contents.end(); ++i) {
        if (CounterImpPtr counter = view->getCounter((*i)->name)) {
            if (counter->empty()) {
                counter->nest(0);
                context->addCounter(counter.get());
            }
            counter->increment((*i)->number);
        }
    }
}

void CSSAutoNumberingValueImp::resetCounter(ViewCSSImp* view, CounterContext* context)
{
    for (auto i = contents.begin(); i != contents.end(); ++i) {
        if (CounterImpPtr counter = view->getCounter((*i)->name)) {
            if (context->hasCounter((*i)->name))
                counter->reset((*i)->number);
            else {
                counter->nest((*i)->number);
                context->addCounter(counter.get());
            }
        }
    }
}

std::deque<CSSParserTerm*>::iterator CSSBackgroundPositionValueImp::setValue(std::deque<CSSParserTerm*>& stack, std::deque<CSSParserTerm*>::iterator i)
{
    bool h = false;
    bool v = false;
    while (i != stack.end()) {
        CSSParserTerm* term = *i;
        if (term->propertyID != CSSStyleDeclarationImp::BackgroundPosition)
            break;
        switch (term->getIndex()) {
        case Top:
            vertical.setValue(0, CSSPrimitiveValue::CSS_PERCENTAGE);
            v = true;
            break;
        case Bottom:
            vertical.setValue(100, CSSPrimitiveValue::CSS_PERCENTAGE);
            v = true;
            break;
        case Left:
            horizontal.setValue(0, CSSPrimitiveValue::CSS_PERCENTAGE);
            h = true;
            break;
        case Right:
            horizontal.setValue(100, CSSPrimitiveValue::CSS_PERCENTAGE);
            h = true;
            break;
        case Center:
            if (!h) {
                horizontal.setValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);
                h = true;
            } else {
                vertical.setValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);
                v = true;
            }
            break;
        default:
            if (!h) {
                horizontal.setValue(term->number, term->unit);
                h = true;
            } else {
                vertical.setValue(term->number, term->unit);
                v = true;
            }
            break;
        }
        ++i;
    }
    if (!v)
        vertical.setValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);
    if (!h)
        horizontal.setValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);
    return --i;
}

bool CSSBackgroundPositionValueImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    setValue(stack, stack.begin());
    return true;
}

void CSSBackgroundPositionValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    horizontal.compute(view, style);
    vertical.compute(view, style);
}

void CSSBackgroundPositionValueImp::resolve(ViewCSSImp* view, BoxImage* image, CSSStyleDeclarationImp* style, float width, float height)
{
    assert(image);
    horizontal.resolve(view, style, width - image->getWidth());  // TODO: negative width case
    vertical.resolve(view, style, height - image->getHeight());  // TODO: negative height case
}

bool CSSBackgroundShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    bool color = false;
    bool attachment = false;
    bool repeat = false;
    bool image = false;
    bool position = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->unit == CSSPrimitiveValue::CSS_RGBCOLOR) {
            color = true;
            decl->backgroundColor.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundAttachment) {
            attachment = true;
            decl->backgroundAttachment.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundRepeat) {
            repeat = true;
            decl->backgroundRepeat.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundImage) {
            image = true;
            decl->backgroundImage.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundPosition) {
            position = true;
            i = decl->backgroundPosition.setValue(stack, i);
        }
    }
    if (!color)
        decl->backgroundColor.setValue(CSSColorValueImp::Transparent);
    if (!attachment)
        decl->backgroundAttachment.setValue();
    if (!repeat)
        decl->backgroundRepeat.setValue();
    if (!image)
        decl->backgroundImage.setValue();
    if (!position)
        decl->backgroundPosition.setValue();
    return true;
}

std::u16string CSSBackgroundShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    return decl->backgroundColor.getCssText(decl) + u' ' +
           decl->backgroundImage.getCssText(decl) + u' ' +
           decl->backgroundRepeat.getCssText(decl) + u' ' +
           decl->backgroundAttachment.getCssText(decl) + u' ' +
           decl->backgroundPosition.getCssText(decl);
}

void CSSBackgroundShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->backgroundColor.specify(decl->backgroundColor);
    self->backgroundImage.specify(decl->backgroundImage);
    self->backgroundRepeat.specify(decl->backgroundRepeat);
    self->backgroundAttachment.specify(decl->backgroundAttachment);
    self->backgroundPosition.specify(decl->backgroundPosition);
}

void CSSBackgroundShorthandImp::reset(CSSStyleDeclarationImp* self)
{
    self->backgroundColor.setValue(CSSColorValueImp::Transparent);
    self->backgroundImage.setValue();
    self->backgroundRepeat.setValue();
    self->backgroundAttachment.setValue();
    self->backgroundPosition.setValue();
}

void CSSBorderColorValueImp::compute(CSSStyleDeclarationImp* decl)
{
    assert(decl);
    if (!hasValue)
        value = decl->color.getARGB();
}

void CSSBorderSpacingValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    horizontal.compute(view, style);
    vertical.compute(view, style);
}

bool CSSBorderColorShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        decl->borderBottomColor = decl->borderLeftColor = decl->borderRightColor = decl->borderTopColor.setValue(stack[0]);
        break;
    case 2:
        decl->borderBottomColor = decl->borderTopColor.setValue(stack[0]);
        decl->borderLeftColor = decl->borderRightColor.setValue(stack[1]);
        break;
    case 3:
        decl->borderTopColor.setValue(stack[0]);
        decl->borderLeftColor = decl->borderRightColor.setValue(stack[1]);
        decl->borderBottomColor.setValue(stack[2]);
        break;
    case 4:
        decl->borderTopColor.setValue(stack[0]);
        decl->borderRightColor.setValue(stack[1]);
        decl->borderBottomColor.setValue(stack[2]);
        decl->borderLeftColor.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderColorShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (decl->borderLeftColor != decl->borderRightColor)
        return decl->borderTopColor.getCssText(decl) + u' ' +
               decl->borderRightColor.getCssText(decl) + u' ' +
               decl->borderBottomColor.getCssText(decl) + u' ' +
               decl->borderLeftColor.getCssText(decl);
    if (decl->borderTopColor != decl->borderBottomColor)
        return decl->borderTopColor.getCssText(decl) + u' ' +
               decl->borderRightColor.getCssText(decl) + u' ' +
               decl->borderBottomColor.getCssText(decl);
    if (decl->borderTopColor != decl->borderRightColor)
        return decl->borderTopColor.getCssText(decl) + u' ' +
               decl->borderRightColor.getCssText(decl);
    return decl->borderTopColor.getCssText(decl);
}

void CSSBorderColorShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->borderTopColor.specify(decl->borderTopColor);
    self->borderRightColor.specify(decl->borderRightColor);
    self->borderBottomColor.specify(decl->borderBottomColor);
    self->borderLeftColor.specify(decl->borderLeftColor);
}

bool CSSBorderStyleShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        decl->borderBottomStyle = decl->borderLeftStyle = decl->borderRightStyle = decl->borderTopStyle.setValue(stack[0]);
        break;
    case 2:
        decl->borderBottomStyle = decl->borderTopStyle.setValue(stack[0]);
        decl->borderLeftStyle = decl->borderRightStyle.setValue(stack[1]);
        break;
    case 3:
        decl->borderTopStyle.setValue(stack[0]);
        decl->borderLeftStyle = decl->borderRightStyle.setValue(stack[1]);
        decl->borderBottomStyle.setValue(stack[2]);
        break;
    case 4:
        decl->borderTopStyle.setValue(stack[0]);
        decl->borderRightStyle.setValue(stack[1]);
        decl->borderBottomStyle.setValue(stack[2]);
        decl->borderLeftStyle.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderStyleShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (decl->borderLeftStyle != decl->borderRightStyle)
        return decl->borderTopStyle.getCssText(decl) + u' ' +
               decl->borderRightStyle.getCssText(decl) + u' ' +
               decl->borderBottomStyle.getCssText(decl) + u' ' +
               decl->borderLeftStyle.getCssText(decl);
    if (decl->borderTopStyle != decl->borderBottomStyle)
        return decl->borderTopStyle.getCssText(decl) + u' ' +
               decl->borderRightStyle.getCssText(decl) + u' ' +
               decl->borderBottomStyle.getCssText(decl);
    if (decl->borderTopStyle != decl->borderRightStyle)
        return decl->borderTopStyle.getCssText(decl) + u' ' +
               decl->borderRightStyle.getCssText(decl);
    return decl->borderTopStyle.getCssText(decl);
}

void CSSBorderStyleShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->borderTopStyle.specify(decl->borderTopStyle);
    self->borderRightStyle.specify(decl->borderRightStyle);
    self->borderBottomStyle.specify(decl->borderBottomStyle);
    self->borderLeftStyle.specify(decl->borderLeftStyle);
}

void CSSBorderWidthValueImp::compute(ViewCSSImp* view, const CSSBorderStyleValueImp& borderStyle, CSSStyleDeclarationImp* style)
{
    switch (borderStyle.getValue()) {
    case CSSBorderStyleValueImp::None:
    case CSSBorderStyleValueImp::Hidden:
        width.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        return;
    default:
        break;
    }

    float w;
    switch (width.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        switch (width.getIndex()) {
        case Thin:
            w = 1.0f;  // TODO * view->getMediumFontSize() ...
            break;
        case Medium:
            w = 3.0f;
            break;
        case Thick:
            w = 5.0f;
            break;
        default:
            w = 1.0f;   // TODO: error
            break;
        }
        width.setValue(w, css::CSSPrimitiveValue::CSS_PX);
        break;
    default:
        // TODO use height in the vertical writing mode
        width.compute(view, style);
        break;
    }
}

bool CSSBorderWidthShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        decl->borderBottomWidth = decl->borderLeftWidth = decl->borderRightWidth = decl->borderTopWidth.setValue(stack[0]);
        break;
    case 2:
        decl->borderBottomWidth = decl->borderTopWidth.setValue(stack[0]);
        decl->borderLeftWidth = decl->borderRightWidth.setValue(stack[1]);
        break;
    case 3:
        decl->borderTopWidth.setValue(stack[0]);
        decl->borderLeftWidth = decl->borderRightWidth.setValue(stack[1]);
        decl->borderBottomWidth.setValue(stack[2]);
        break;
    case 4:
        decl->borderTopWidth.setValue(stack[0]);
        decl->borderRightWidth.setValue(stack[1]);
        decl->borderBottomWidth.setValue(stack[2]);
        decl->borderLeftWidth.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderWidthShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (decl->borderLeftWidth != decl->borderRightWidth)
        return decl->borderTopWidth.getCssText(decl) + u' ' +
               decl->borderRightWidth.getCssText(decl) + u' ' +
               decl->borderBottomWidth.getCssText(decl) + u' ' +
               decl->borderLeftWidth.getCssText(decl);
    if (decl->borderTopWidth != decl->borderBottomWidth)
        return decl->borderTopWidth.getCssText(decl) + u' ' +
               decl->borderRightWidth.getCssText(decl) + u' ' +
               decl->borderBottomWidth.getCssText(decl);
    if (decl->borderTopWidth != decl->borderRightWidth)
        return decl->borderTopWidth.getCssText(decl) + u' ' +
               decl->borderRightWidth.getCssText(decl);
    return decl->borderTopWidth.getCssText(decl);
}

void CSSBorderWidthShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->borderTopWidth.specify(decl->borderTopWidth);
    self->borderRightWidth.specify(decl->borderRightWidth);
    self->borderBottomWidth.specify(decl->borderBottomWidth);
    self->borderLeftWidth.specify(decl->borderLeftWidth);
}

bool CSSBorderValueImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    bool style = false;
    bool width = false;
    bool color = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID == CSSStyleDeclarationImp::BorderStyle) {
            style = true;
            switch (index) {
            case 0:
                decl->borderTopStyle.setValue(term);
                break;
            case 1:
                decl->borderRightStyle.setValue(term);
                break;
            case 2:
                decl->borderBottomStyle.setValue(term);
                break;
            case 3:
                decl->borderLeftStyle.setValue(term);
                break;
            default:
                break;
            }
        } else if (term->propertyID == CSSStyleDeclarationImp::BorderWidth) {
            width = true;
            switch (index) {
            case 0:
                decl->borderTopWidth.setValue(term);
                break;
            case 1:
                decl->borderRightWidth.setValue(term);
                break;
            case 2:
                decl->borderBottomWidth.setValue(term);
                break;
            case 3:
                decl->borderLeftWidth.setValue(term);
                break;
            default:
                break;
            }
        } else {
            color = true;
            switch (index) {
            case 0:
                decl->borderTopColor.setValue(term);
                break;
            case 1:
                decl->borderRightColor.setValue(term);
                break;
            case 2:
                decl->borderBottomColor.setValue(term);
                break;
            case 3:
                decl->borderLeftColor.setValue(term);
                break;
            default:
                break;
            }
        }
    }
    if (!style) {
        switch (index) {
        case 0:
            decl->borderTopStyle.setValue();
            break;
        case 1:
            decl->borderRightStyle.setValue();
            break;
        case 2:
            decl->borderBottomStyle.setValue();
            break;
        case 3:
            decl->borderLeftStyle.setValue();
            break;
        default:
            break;
        }
    }
    if (!width) {
        switch (index) {
        case 0:
            decl->borderTopWidth.setValue();
            break;
        case 1:
            decl->borderRightWidth.setValue();
            break;
        case 2:
            decl->borderBottomWidth.setValue();
            break;
        case 3:
            decl->borderLeftWidth.setValue();
            break;
        default:
            break;
        }
    }
    if (!color) {
        switch (index) {
        case 0:
            decl->borderTopColor.reset();
            break;
        case 1:
            decl->borderRightColor.reset();
            break;
        case 2:
            decl->borderBottomColor.reset();
            break;
        case 3:
            decl->borderLeftColor.reset();
            break;
        default:
            break;
        }
    }
    return true;
}

std::u16string CSSBorderValueImp::getCssText(CSSStyleDeclarationImp* decl)
{
    switch (index) {
    case 0:
        return decl->borderTopWidth.getCssText(decl) + u' ' + decl->borderTopStyle.getCssText(decl) + u' ' + decl->borderTopColor.getCssText(decl);
    case 1:
        return decl->borderRightWidth.getCssText(decl) + u' ' + decl->borderRightStyle.getCssText(decl) + u' ' + decl->borderRightColor.getCssText(decl);
    case 2:
        return decl->borderBottomWidth.getCssText(decl) + u' ' + decl->borderBottomStyle.getCssText(decl) + u' ' + decl->borderBottomColor.getCssText(decl);
    case 3:
        return decl->borderLeftWidth.getCssText(decl) + u' ' + decl->borderLeftStyle.getCssText(decl) + u' ' + decl->borderLeftColor.getCssText(decl);
    default:
        return u"";
    }
}

void CSSBorderValueImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    switch (index) {
    case 0:
        self->borderTopWidth.specify(decl->borderTopWidth);
        self->borderTopStyle.specify(decl->borderTopStyle);
        self->borderTopColor.specify(decl->borderTopColor);
        break;
    case 1:
        self->borderRightWidth.specify(decl->borderRightWidth);
        self->borderRightStyle.specify(decl->borderRightStyle);
        self->borderRightColor.specify(decl->borderRightColor);
        break;
    case 2:
        self->borderBottomWidth.specify(decl->borderBottomWidth);
        self->borderBottomStyle.specify(decl->borderBottomStyle);
        self->borderBottomColor.specify(decl->borderBottomColor);
        break;
    case 3:
        self->borderLeftWidth.specify(decl->borderLeftWidth);
        self->borderLeftStyle.specify(decl->borderLeftStyle);
        self->borderLeftColor.specify(decl->borderLeftColor);
        break;
    default:
        break;
    }
}

bool CSSBorderShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    bool style = false;
    bool width = false;
    bool color = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID == CSSStyleDeclarationImp::BorderStyle) {
            style = true;
            decl->borderBottomStyle = decl->borderLeftStyle = decl->borderRightStyle = decl->borderTopStyle.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BorderWidth) {
            width = true;
            decl->borderBottomWidth = decl->borderLeftWidth = decl->borderRightWidth = decl->borderTopWidth.setValue(term);
        } else {
            color = true;
            decl->borderBottomColor = decl->borderLeftColor = decl->borderRightColor = decl->borderTopColor.setValue(term);
        }
    }
    if (!style)
        decl->borderBottomStyle = decl->borderLeftStyle = decl->borderRightStyle = decl->borderTopStyle.setValue();
    if (!width)
        decl->borderBottomWidth = decl->borderLeftWidth = decl->borderRightWidth = decl->borderTopWidth.setValue();
    if (!color)
        decl->borderBottomColor = decl->borderLeftColor = decl->borderRightColor = decl->borderTopColor.reset();
    return true;
}

std::u16string CSSBorderShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    return decl->borderWidth.getCssText(decl) + u' ' +
           decl->borderStyle.getCssText(decl) + u' ' +
           decl->borderColor.getCssText(decl);
}

void CSSBorderShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->borderWidth.specify(self, decl);
    self->borderStyle.specify(self, decl);
    self->borderColor.specify(self, decl);
}

bool CSSContentValueImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        Content* content = 0;
        int index = -1;
        switch (term->propertyID) {
        case Counter: {
            std::u16string name;
            std::u16string string;
            bool nested = false;
            unsigned listStyleType = CSSListStyleTypeValueImp::Decimal;
            do {
                switch (term->unit) {
                case CSSPrimitiveValue::CSS_IDENT:
                    name = term->getString();
                    break;
                case CSSPrimitiveValue::CSS_STRING:
                    nested = true;
                    string = term->getString();
                    break;
                case CSSParserTerm::CSS_TERM_INDEX:
                    listStyleType = term->getIndex();
                    break;
                default:
                    break;
                }
                ++i;
                term = *i;
            } while (term->unit != CSSParserTerm::CSS_TERM_FUNCTION);
            if (nested)
                content = new CounterContent(name, string, listStyleType);
            else
                content = new CounterContent(name, listStyleType);
            break;
        }
        case Attr:
            if (term->unit == CSSPrimitiveValue::CSS_IDENT)
                content = new(std::nothrow) AttrContent(term->getString());
            break;
        default:
            index = term->getIndex();
            switch (index) {
            case Normal:
            case None:
                value = index;
                return true;
            case OpenQuote:
            case CloseQuote:
            case NoOpenQuote:
            case NoCloseQuote:
                content = new(std::nothrow) QuoteContent(index);
                break;
            default:
                assert(index == -1);
                switch (term->unit) {
                case CSSPrimitiveValue::CSS_STRING:
                    content = new(std::nothrow) StringContent(term->getString());
                    break;
                case CSSPrimitiveValue::CSS_URI:
                    content = new(std::nothrow) URIContent(term->getString());
                    break;
                }
                break;
            }
        }
        if (content)
            contents.push_back(content);
    }
    return true;
}

std::u16string CSSContentValueImp::getCssText(CSSStyleDeclarationImp* decl)
{
    if (contents.empty())
        return Options[value];
    std::u16string cssText;
    for (auto i = contents.begin(); i != contents.end(); ++i) {
        if (i != contents.begin())
            cssText += u' ';
        cssText += (*i)->getCssText();
    }
    return cssText;
}

void CSSContentValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    switch (style->getPseudoElementSelectorType()) {
    case CSSPseudoElementSelector::Before:
    case CSSPseudoElementSelector::After:
        if (isNormal()) {
            value = None;
            clearContents();
        }
        break;
    case CSSPseudoElementSelector::Marker:
        if (isNormal()) {
            value = None;
            if (!style->listStyleImage.isNone()) {
                if (URIContent* content = new(std::nothrow) URIContent(style->listStyleImage.getValue()))
                    contents.push_back(content);
            } else {
                switch (style->listStyleType.getValue()) {
                case CSSListStyleTypeValueImp::None:
                case CSSListStyleTypeValueImp::Disc:
                case CSSListStyleTypeValueImp::Circle:
                case CSSListStyleTypeValueImp::Square:
                    if (CounterContent* content = new CounterContent(u"list-item", style->listStyleType.getValue()))
                        contents.push_back(content);
                    if (Content* content = new(std::nothrow) StringContent(u"\u00A0"))
                        contents.push_back(content);
                    break;
                case CSSListStyleTypeValueImp::Decimal:
                case CSSListStyleTypeValueImp::DecimalLeadingZero:
                case CSSListStyleTypeValueImp::LowerRoman:
                case CSSListStyleTypeValueImp::UpperRoman:
                case CSSListStyleTypeValueImp::LowerGreek:
                case CSSListStyleTypeValueImp::LowerLatin:
                case CSSListStyleTypeValueImp::UpperLatin:
                case CSSListStyleTypeValueImp::Armenian:
                case CSSListStyleTypeValueImp::Georgian:
                case CSSListStyleTypeValueImp::LowerAlpha:
                case CSSListStyleTypeValueImp::UpperAlpha:
                    if (CounterContent* content = new CounterContent(u"list-item", style->listStyleType.getValue()))
                        contents.push_back(content);
                    if (Content* content = new(std::nothrow) StringContent(u".\u00A0"))
                        contents.push_back(content);
                    break;
                default:
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}

std::u16string CSSContentValueImp::CounterContent::eval(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context)
{
    if (CounterImpPtr counter = view->getCounter(identifier)) {
        if (nested)
            return counter->eval(string, listStyleType.getValue(), context);
        else
            return counter->eval(listStyleType.getValue(), context);
    }
    return u"";
}

Element CSSContentValueImp::eval(ViewCSSImp* view, Element element, CSSAutoNumberingValueImp::CounterContext* context)
{
    if (contents.empty())
        return 0;

    if (URIContent* content = dynamic_cast<URIContent*>(contents.front())) {
        // TODO: if the image is not valid, use the 'list-style-type' instead.
        html::HTMLImageElement img = interface_cast<html::HTMLImageElement>(view->getDocument().createElement(u"img"));
        if (img) {
            img.setSrc(content->value);
            if (NodeImp* imp = dynamic_cast<NodeImp*>(img.self()))
                imp->setParentNode(static_cast<NodeImp*>(element.self()));
        }
        return img;
    }

    Element span = view->getDocument().createElement(u"span");
    if (!span)
        return 0;
    std::u16string data;
    for (auto i = contents.begin(); i != contents.end(); ++i)
        data += (*i)->eval(view, context);
    if (org::w3c::dom::Text text = view->getDocument().createTextNode(data))
        span.appendChild(text);

    // Set the pseudo parentNode of the new span element so that
    // setContainingBlock() works even for the positioned :before and
    // :after pseudo-elements.
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/containing-block-029.htm
    if (NodeImp* imp = dynamic_cast<NodeImp*>(span.self()))
        imp->setParentNode(static_cast<NodeImp*>(element.self()));

    return span;
}

void CSSDisplayValueImp::compute(CSSStyleDeclarationImp* decl, Element element)
{
    if (value == None)
        return;
    if (decl->position.isAbsolute() || decl->position.isFixed())
        decl->float_.setValue(CSSFloatValueImp::None);
    else if (decl->float_.getValue() == CSSFloatValueImp::None && (!element || element.getParentElement()))
        return;
    switch (value) {
    case InlineTable:
        value = Table;
        break;
    case Inline:
    case RunIn:
    case TableRowGroup:
    case TableColumn:
    case TableColumnGroup:
    case TableHeaderGroup:
    case TableFooterGroup:
    case TableRow:
    case TableCell:
    case TableCaption:
    case InlineBlock:
        value = Block;
        break;
    default:
        break;
    }
}

std::deque<CSSParserTerm*>::iterator CSSFontFamilyValueImp::setValue(std::deque<CSSParserTerm*>& stack, std::deque<CSSParserTerm*>::iterator i)
{
    bool hasGeneric = false;
    generic = None;
    std::u16string family;
    for (; i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID != CSSStyleDeclarationImp::FontFamily)
            break;
        if (term->unit == CSSPrimitiveValue::CSS_IDENT) {
            if (!family.empty())
                family += u' ';
            family += term->text;
            continue;
        }
        if (!family.empty()) {
            familyNames.push_back(family);
            family.clear();
        }
        switch (term->unit) {
        case CSSParserTerm::CSS_TERM_INDEX:
            if (!hasGeneric) {
                generic = term->getIndex();
                hasGeneric = true;
            }
            break;
        case CSSPrimitiveValue::CSS_STRING:
            familyNames.push_back(term->text);
            break;
        default:
            break;
        }
    }
    if (!family.empty())
        familyNames.push_back(family);
    return --i;
}

bool CSSFontFamilyValueImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    setValue(stack, stack.begin());
    return true;
}

std::u16string CSSFontFamilyValueImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (0 < familyNames.size()) {
        for (auto i = familyNames.begin(); i != familyNames.end(); ++i) {
            if (i != familyNames.begin())
                cssText += u", ";
            cssText += CSSSerializeString(*i);
        }
    }
    if (generic != None) {
        if (!cssText.empty())
            cssText += u", ";
        cssText += Options[generic - 1];
    }
    return cssText;
}

void CSSFontSizeValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle) {
    float w;
    float parentSize = parentStyle ? parentStyle->fontSize.getPx() : view->getMediumFontSize();
    unsigned i;
    switch (size.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        i = size.getIndex();
        switch (i) {
        case XxSmall:
        case XSmall:
        case Small:
        case Medium:
        case Large:
        case XLarge:
        case XxLarge:
            w = view->getFontSize(i);
            break;
        case Larger:
            w = view->getLargerFontSize(parentSize);
            break;
        case Smaller:
            w = view->getSmallerFontSize(parentSize);
            break;
        default:
            // TODO: error
            w = view->getFontSize(Medium);
            break;
        }
        break;
    case css::CSSPrimitiveValue::CSS_PERCENTAGE:
    case css::CSSPrimitiveValue::CSS_EMS:
        w = view->getPx(size, parentSize);
        break;
    case css::CSSPrimitiveValue::CSS_EXS:
        if (parentStyle && parentStyle->getFontTexture())
            w = view->getPx(size, parentStyle->getFontTexture()->getXHeight(view->getPointFromPx(parentSize)));
        else
            w = view->getPx(size, parentSize * 0.5f);
        break;
    default:
        w = view->getPx(size);
        break;
    }
    size.setValue(w, css::CSSPrimitiveValue::CSS_PX);
}

void CSSFontWeightValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle)
{
    float inherited = parentStyle ? parentStyle->fontWeight.value.number : 400.0f;
    float w;
    switch (value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        switch (value.getIndex()) {
        case Normal:
            w = 400.0f;
            break;
        case Bold:
            w = 700.0f;
            break;
        case Bolder:
            if (inherited < 400.0f)
                w = 400.0f;
            else if (inherited < 700.0f)
                w = 700.0f;
            else
                w = 900.0f;
            break;
        case Lighter:
            if (inherited < 600.0f)
                w = 100.0f;
            else if (inherited < 800.0f)
                w = 400.0f;
            else
                w = 700.0f;
            break;
        default:  // TODO: error
            w = inherited;
            break;
        }
        break;
    default:
        return;
    }
    value.setValue(w, css::CSSPrimitiveValue::CSS_NUMBER);
}

bool CSSFontShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    reset(decl);
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        switch (term->propertyID) {
        case CSSStyleDeclarationImp::FontStyle:
            decl->fontStyle.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontVariant:
            decl->fontVariant.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontWeight:
            decl->fontWeight.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontSize:
            decl->fontSize.setValue(term);
            break;
        case CSSStyleDeclarationImp::LineHeight:
            decl->lineHeight.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontFamily:
            i = decl->fontFamily.setValue(stack, i);
            break;
        default:
            if (term->unit == CSSParserTerm::CSS_TERM_INDEX)
                index = term->getIndex();
            break;
        }
    }
    return true;
}

std::u16string CSSFontShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    if (index != Normal)
        return Options[index];

    std::u16string text;
    if (!decl->fontStyle.isNormal())
        text += decl->fontStyle.getCssText(decl);
    if (!decl->fontVariant.isNormal()) {
        if (!text.empty())
            text += u" ";
        text += decl->fontVariant.getCssText(decl);
    }
    if (!decl->fontWeight.isNormal()) {
        if (!text.empty())
            text += u" ";
        text += decl->fontWeight.getCssText(decl);
    }
    if (!text.empty())
        text += u" ";
    text += decl->fontSize.getCssText(decl);
    if (!decl->lineHeight.isNormal())
        text += u"/" + decl->lineHeight.getCssText(decl);
    text += u" " + decl->fontFamily.getCssText(decl);
    return text;
}

void CSSFontShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    if (decl->font.index != Normal) {
        reset(self);
        index = decl->font.index;
    } else {
        index = Normal;
        self->fontStyle.specify(decl->fontStyle);
        self->fontVariant.specify(decl->fontVariant);
        self->fontWeight.specify(decl->fontWeight);
        self->fontSize.specify(decl->fontSize);
        self->lineHeight.specify(decl->lineHeight);
        self->fontFamily.specify(decl->fontFamily);
    }
}

void CSSFontShorthandImp::reset(CSSStyleDeclarationImp* self)
{
    index = Normal;
    self->fontStyle.setValue();
    self->fontVariant.setValue();
    self->fontWeight.setValue();
    self->fontSize.setValue();
    self->lineHeight.setValue();
    self->fontFamily.reset();
}

void CSSLineHeightValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style) {
    switch (value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
    case css::CSSPrimitiveValue::CSS_NUMBER:
        break;
    default:
        value.resolve(view, style, style->fontSize.getPx());
        computed = value;
        break;
    }
}

void CSSLineHeightValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style) {
    float w;
    switch (value.isNegative() ? CSSParserTerm::CSS_TERM_INDEX : value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        if (FontTexture* font = style->getFontTexture())
            w = font->getLineHeight(view->getPointFromPx(style->fontSize.getPx()));
        else
            w = style->fontSize.getPx() * 1.2;
        break;
    case css::CSSPrimitiveValue::CSS_NUMBER:
        w = style->fontSize.getPx() * value.number;
        break;
    default:
        return;
    }
    value.setValue(w, css::CSSPrimitiveValue::CSS_PX);
}

void CSSListStylePositionValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (style->getPseudoElementSelectorType() != CSSPseudoElementSelector::Marker)
        return;

    if (value == Outside) {
        style->display.setValue(CSSDisplayValueImp::Block);
        style->position.setValue(CSSPositionValueImp::Absolute);
    }
}

bool CSSListStyleShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    bool none = false;
    bool type = false;
    bool position = false;
    bool image = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID == CSSStyleDeclarationImp::ListStyleType) {
            type = true;
            decl->listStyleType.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::ListStylePosition) {
            position = true;
            decl->listStylePosition.setValue(term);
        } else if (term->unit == css::CSSPrimitiveValue::CSS_URI) {
            image = true;
            decl->listStyleImage.setValue(term);
        } else if (term->getIndex() == CSSListStyleShorthandImp::None)
            none = true;
    }
    if (!type) {
        if (none)
            decl->listStyleType.setValue(CSSListStyleTypeValueImp::None);
        else
            decl->listStyleType.setValue();
    }
    if (!position)
        decl->listStylePosition.setValue();
    if (!image)
        decl->listStyleImage.setValue();
    if (none && type && image) {
        reset(decl);
        return false;
    }
    return true;
}

std::u16string CSSListStyleShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    return decl->listStyleType.getCssText(decl) + u' ' +
           decl->listStylePosition.getCssText(decl) + u' ' +
           decl->listStyleImage.getCssText(decl);
}

void CSSListStyleShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->listStyleImage.specify(decl->listStyleImage);
    self->listStylePosition.specify(decl->listStylePosition);
    self->listStyleType.specify(decl->listStyleType);
}

void CSSListStyleShorthandImp::reset(CSSStyleDeclarationImp* self)
{
    self->listStyleImage.setValue();
    self->listStylePosition.setValue();
    self->listStyleType.setValue();
}

bool CSSMarginShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        decl->marginBottom = decl->marginLeft = decl->marginRight = decl->marginTop.setValue(stack[0]);
        break;
    case 2:
        decl->marginBottom = decl->marginTop.setValue(stack[0]);
        decl->marginLeft = decl->marginRight.setValue(stack[1]);
        break;
    case 3:
        decl->marginTop.setValue(stack[0]);
        decl->marginLeft = decl->marginRight.setValue(stack[1]);
        decl->marginBottom.setValue(stack[2]);
        break;
    case 4:
        decl->marginTop.setValue(stack[0]);
        decl->marginRight.setValue(stack[1]);
        decl->marginBottom.setValue(stack[2]);
        decl->marginLeft.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSMarginShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (decl->marginLeft != decl->marginRight)
        return decl->marginTop.getCssText(decl) + u' ' +
               decl->marginRight.getCssText(decl) + u' ' +
               decl->marginBottom.getCssText(decl) + u' ' +
               decl->marginLeft.getCssText(decl);
    if (decl->marginTop != decl->marginBottom)
        return decl->marginTop.getCssText(decl) + u' ' +
               decl->marginRight.getCssText(decl) + u' ' +
               decl->marginBottom.getCssText(decl);
    if (decl->marginTop != decl->marginRight)
        return decl->marginTop.getCssText(decl) + u' ' +
               decl->marginRight.getCssText(decl);
    return decl->marginTop.getCssText(decl);
}

void CSSMarginShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->marginTop.specify(decl->marginTop);
    self->marginRight.specify(decl->marginRight);
    self->marginBottom.specify(decl->marginBottom);
    self->marginLeft.specify(decl->marginLeft);
}

bool CSSPaddingShorthandImp::setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        decl->paddingBottom = decl->paddingLeft = decl->paddingRight = decl->paddingTop.setValue(stack[0]);
        break;
    case 2:
        decl->paddingBottom = decl->paddingTop.setValue(stack[0]);
        decl->paddingLeft = decl->paddingRight.setValue(stack[1]);
        break;
    case 3:
        decl->paddingTop.setValue(stack[0]);
        decl->paddingLeft = decl->paddingRight.setValue(stack[1]);
        decl->paddingBottom.setValue(stack[2]);
        break;
    case 4:
        decl->paddingTop.setValue(stack[0]);
        decl->paddingRight.setValue(stack[1]);
        decl->paddingBottom.setValue(stack[2]);
        decl->paddingLeft.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSPaddingShorthandImp::getCssText(CSSStyleDeclarationImp* decl)
{
    std::u16string cssText;
    if (decl->paddingLeft != decl->paddingRight)
        return decl->paddingTop.getCssText(decl) + u' ' +
               decl->paddingRight.getCssText(decl) + u' ' +
               decl->paddingBottom.getCssText(decl) + u' ' +
               decl->paddingLeft.getCssText(decl);
    if (decl->paddingTop != decl->paddingBottom)
        return decl->paddingTop.getCssText(decl) + u' ' +
               decl->paddingRight.getCssText(decl) + u' ' +
               decl->paddingBottom.getCssText(decl);
    if (decl->paddingTop != decl->paddingRight)
        return decl->paddingTop.getCssText(decl) + u' ' +
               decl->paddingRight.getCssText(decl);
    return decl->paddingTop.getCssText(decl);
}

void CSSPaddingShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl)
{
    self->paddingTop.specify(decl->paddingTop);
    self->paddingRight.specify(decl->paddingRight);
    self->paddingBottom.specify(decl->paddingBottom);
    self->paddingLeft.specify(decl->paddingLeft);
}

void CSSVerticalAlignValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (value.isIndex() || value.isPercentage())
        return;
    value.compute(view, style);
}

void CSSVerticalAlignValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style)
{
    if (value.isIndex())
        return;
    value.resolve(view, style, style->lineHeight.getPx());
}

float CSSVerticalAlignValueImp::getOffset(LineBox* line, InlineLevelBox* text) const
{
    float leading = text->getLeading() / 2.0f;
    // TODO: Check if there is a parent inline element firstly.
    switch (value.getIndex()) {
    case Top:
        return 0.0f;
    case Baseline:
        return line->getBaseline() - (leading + text->getBaseline());
    case Middle:
        return line->getBaseline() - (text->getHeight() / 2.0f);
    case Bottom:
        return line->getHeight() - text->getHeight();
    case Sub:
    case Super:
    case TextTop:
    case TextBottom:
        // TODO: implement me!
        return line->getBaseline() - (leading + text->getBaseline());
    default:
        assert(value.unit == css::CSSPrimitiveValue::CSS_PX);
        return line->getBaseline() - (leading + text->getBaseline()) - value.getPx();
    }
}

}}}}  // org::w3c::bootstrap
