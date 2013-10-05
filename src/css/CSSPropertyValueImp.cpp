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

#include "CSSPropertyValueImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/HTMLImageElement.h>

#include "CounterImp.h"
#include "CSSStyleDeclarationImp.h"
#include "DocumentImp.h"
#include "ViewCSSImp.h"
#include "Box.h"
#include "BoxImage.h"

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

const char16_t* CSSNumericValue::ResolutionUnits[] = {
    u"dppx",
    u"dpi",
    u"dpcm",
};

const char16_t* CSSAutoLengthValueImp::Options[] = {
    u"auto",
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

const char16_t* CSSCursorValueImp::Options[] = {
    u"auto",
    u"crosshair",
    u"default",
    u"pointer",
    u"move",
    u"e-resize",
    u"ne-resize",
    u"nw-resize",
    u"n-resize",
    u"se-resize",
    u"sw-resize",
    u"s-resize",
    u"w-resize",
    u"text",
    u"wait",
    u"help",
    u"progress"
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

const char16_t* CSSEmptyCellsValueImp::Options[] = {
    u"hide",
    u"show",
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

const char16_t* CSSLineHeightValueImp::Options[] = {
    u"normal",
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

const char16_t* CSSNormalLengthValueImp::Options[] = {
    u"normal",
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

void CSSNumericValue::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (isIndex() || isPercentage()) {
        resolved = NAN;
        return;
    }
    switch (unit) {
    case css::CSSPrimitiveValue::CSS_EMS:
        resolved = view->getPx(*this, self->fontSize.getPx());
        break;
    case css::CSSPrimitiveValue::CSS_EXS:
        if (FontTexture* font = self->getFontTexture())
            resolved = view->getPx(*this, font->getXHeight(view->getPointFromPx(self->fontSize.getPx())));
        else
            resolved = view->getPx(*this, self->fontSize.getPx() * 0.5f);
        break;
    default:
        resolved = view->getPx(*this);
        break;
    }
}

void CSSNumericValue::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self, float fullSize)
{
    if (isIndex())
        return;
    switch (unit) {
    case css::CSSPrimitiveValue::CSS_PERCENTAGE:
        // In the 2nd layout pass, 'fullSize' can be changed.
        // In such a case, 'resolved' is not a NaN, but it needs to be computed.
        resolved = view->getPx(*this, fullSize);
        break;
    case css::CSSPrimitiveValue::CSS_EMS:
        if (isnan(resolved))
            resolved = view->getPx(*this, self->fontSize.getPx());
        break;
    case css::CSSPrimitiveValue::CSS_EXS:
        if (isnan(resolved)) {
            if (FontTexture* font = self->getFontTexture())
                resolved = view->getPx(*this, font->getXHeight(view->getPointFromPx(self->fontSize.getPx())));
            else
                resolved = view->getPx(*this, self->fontSize.getPx() * 0.5f);
        }
        break;
    default:
        if (isnan(resolved))
            resolved = view->getPx(*this);
        break;
    }
}

void CSSNumericValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    value.compute(view, self);
}

void CSSNumericValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self, float fullSize)
{
    value.resolve(view, self, fullSize);
}

void CSSNumericValueImp::clip(float min, float max)
{
    value.clip(min, max);
}

void CSSAutoLengthValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (isAuto() || isPercentage())
        return;  // leave as it is
    length.compute(view, self);
}

void CSSAutoLengthValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self, float fullSize)
{
    if (isAuto())
        return;  // leave length as auto
    length.resolve(view, self, fullSize);
}

void CSSNoneLengthValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (isNone() || isPercentage())
        return;  // leave as it is
    length.compute(view, self);
}

void CSSNoneLengthValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self, float fullSize)
{
    if (isNone())
        return;  // leave as it is
    length.resolve(view, self, fullSize);
}

void CSSNormalLengthValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    assert(!length.isPercentage());
    if (isNormal())
        return;  // leave as it is
    length.compute(view, self);
}

void CSSWordSpacingValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (isNormal()) {
        length.setPx(0.0f);
        return;  // leave as it is
    }
    length.compute(view, self);
}

bool CSSAutoNumberingValueImp::operator==(const CSSAutoNumberingValueImp& other) const
{
    if (defaultNumber != other.defaultNumber)
        return false;
    if (contents.size() != other.contents.size())
        return false;
    return getCssText() == other.getCssText();    // TODO: Refine
}

bool CSSAutoNumberingValueImp::CounterContext::hasCounter(const std::u16string& name) const
{
    for (auto i = counters.begin(); i != counters.end(); ++i) {
        if ((*i)->getIdentifier() == name)
            return true;
    }
    return false;
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

bool CSSAutoNumberingValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    reset();
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

std::u16string CSSAutoNumberingValueImp::getCssText(CSSStyleDeclarationImp* self) const
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

void CSSAutoNumberingValueImp::incrementCounter(ViewCSSImp* view, CounterContext* context)
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

void CSSBackgroundImageValueImp::compute(ViewCSSImp* view)
{
    if (isNone())
        return;
    if (view->getDocument())
        view->preload(view->getDocument()->getDocumentURI(), getValue());
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

bool CSSBackgroundPositionValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    setValue(stack, stack.begin());
    return true;
}

void CSSBackgroundPositionValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    horizontal.compute(view, self);
    vertical.compute(view, self);
}

void CSSBackgroundPositionValueImp::resolve(ViewCSSImp* view, BoxImage* image, CSSStyleDeclarationImp* self, float width, float height)
{
    assert(image);
    horizontal.resolve(view, self, width - image->getNaturalWidth());  // TODO: negative width case
    vertical.resolve(view, self, height - image->getNaturalHeight());  // TODO: negative height case
}

bool CSSBackgroundShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
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
            self->backgroundColor.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundAttachment) {
            attachment = true;
            self->backgroundAttachment.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundRepeat) {
            repeat = true;
            self->backgroundRepeat.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundImage) {
            image = true;
            self->backgroundImage.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BackgroundPosition) {
            position = true;
            i = self->backgroundPosition.setValue(stack, i);
        }
    }
    if (!color)
        self->backgroundColor.setValue(CSSColorValueImp::Transparent);
    if (!attachment)
        self->backgroundAttachment.setValue();
    if (!repeat)
        self->backgroundRepeat.setValue();
    if (!image)
        self->backgroundImage.setValue();
    if (!position)
        self->backgroundPosition.setValue();
    return true;
}

std::u16string CSSBackgroundShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    return self->backgroundColor.getCssText(self) + u' ' +
           self->backgroundImage.getCssText(self) + u' ' +
           self->backgroundRepeat.getCssText(self) + u' ' +
           self->backgroundAttachment.getCssText(self) + u' ' +
           self->backgroundPosition.getCssText(self);
}

void CSSBackgroundShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
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

void CSSBorderColorValueImp::compute(CSSStyleDeclarationImp* self)
{
    assert(self);
    if (!hasValue)
        resolved = self->color.getARGB();
    else
        resolved = value;
}

void CSSBorderSpacingValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    horizontal.compute(view, self);
    vertical.compute(view, self);
}

bool CSSBorderColorShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        self->borderBottomColor = self->borderLeftColor = self->borderRightColor = self->borderTopColor.setValue(stack[0]);
        break;
    case 2:
        self->borderBottomColor = self->borderTopColor.setValue(stack[0]);
        self->borderLeftColor = self->borderRightColor.setValue(stack[1]);
        break;
    case 3:
        self->borderTopColor.setValue(stack[0]);
        self->borderLeftColor = self->borderRightColor.setValue(stack[1]);
        self->borderBottomColor.setValue(stack[2]);
        break;
    case 4:
        self->borderTopColor.setValue(stack[0]);
        self->borderRightColor.setValue(stack[1]);
        self->borderBottomColor.setValue(stack[2]);
        self->borderLeftColor.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderColorShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    if (self->borderLeftColor != self->borderRightColor)
        return self->borderTopColor.getCssText(self) + u' ' +
               self->borderRightColor.getCssText(self) + u' ' +
               self->borderBottomColor.getCssText(self) + u' ' +
               self->borderLeftColor.getCssText(self);
    if (self->borderTopColor != self->borderBottomColor)
        return self->borderTopColor.getCssText(self) + u' ' +
               self->borderRightColor.getCssText(self) + u' ' +
               self->borderBottomColor.getCssText(self);
    if (self->borderTopColor != self->borderRightColor)
        return self->borderTopColor.getCssText(self) + u' ' +
               self->borderRightColor.getCssText(self);
    return self->borderTopColor.getCssText(self);
}

void CSSBorderColorShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->borderTopColor.specify(decl->borderTopColor);
    self->borderRightColor.specify(decl->borderRightColor);
    self->borderBottomColor.specify(decl->borderBottomColor);
    self->borderLeftColor.specify(decl->borderLeftColor);
}

bool CSSBorderStyleShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        self->borderBottomStyle = self->borderLeftStyle = self->borderRightStyle = self->borderTopStyle.setValue(stack[0]);
        break;
    case 2:
        self->borderBottomStyle = self->borderTopStyle.setValue(stack[0]);
        self->borderLeftStyle = self->borderRightStyle.setValue(stack[1]);
        break;
    case 3:
        self->borderTopStyle.setValue(stack[0]);
        self->borderLeftStyle = self->borderRightStyle.setValue(stack[1]);
        self->borderBottomStyle.setValue(stack[2]);
        break;
    case 4:
        self->borderTopStyle.setValue(stack[0]);
        self->borderRightStyle.setValue(stack[1]);
        self->borderBottomStyle.setValue(stack[2]);
        self->borderLeftStyle.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderStyleShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    if (self->borderLeftStyle != self->borderRightStyle)
        return self->borderTopStyle.getCssText(self) + u' ' +
               self->borderRightStyle.getCssText(self) + u' ' +
               self->borderBottomStyle.getCssText(self) + u' ' +
               self->borderLeftStyle.getCssText(self);
    if (self->borderTopStyle != self->borderBottomStyle)
        return self->borderTopStyle.getCssText(self) + u' ' +
               self->borderRightStyle.getCssText(self) + u' ' +
               self->borderBottomStyle.getCssText(self);
    if (self->borderTopStyle != self->borderRightStyle)
        return self->borderTopStyle.getCssText(self) + u' ' +
               self->borderRightStyle.getCssText(self);
    return self->borderTopStyle.getCssText(self);
}

void CSSBorderStyleShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->borderTopStyle.specify(decl->borderTopStyle);
    self->borderRightStyle.specify(decl->borderRightStyle);
    self->borderBottomStyle.specify(decl->borderBottomStyle);
    self->borderLeftStyle.specify(decl->borderLeftStyle);
}

void CSSBorderWidthValueImp::compute(ViewCSSImp* view, const CSSBorderStyleValueImp& borderStyle, CSSStyleDeclarationImp* self)
{
    switch (borderStyle.getValue()) {
    case CSSBorderStyleValueImp::None:
    case CSSBorderStyleValueImp::Hidden:
        width.setPx(0.0f);
        return;
    default:
        break;
    }

    switch (width.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        switch (width.getIndex()) {
        case Thin:
            width.setPx(1.0f);  // TODO * view->getMediumFontSize() ...
            break;
        case Medium:
            width.setPx(3.0f);
            break;
        case Thick:
            width.setPx(5.0f);
            break;
        default:
            width.setPx(1.0f);  // TODO: error
            break;
        }
        break;
    default:
        // TODO use height in the vertical writing mode
        width.compute(view, self);
        break;
    }
}

bool CSSBorderWidthShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        self->borderBottomWidth = self->borderLeftWidth = self->borderRightWidth = self->borderTopWidth.setValue(stack[0]);
        break;
    case 2:
        self->borderBottomWidth = self->borderTopWidth.setValue(stack[0]);
        self->borderLeftWidth = self->borderRightWidth.setValue(stack[1]);
        break;
    case 3:
        self->borderTopWidth.setValue(stack[0]);
        self->borderLeftWidth = self->borderRightWidth.setValue(stack[1]);
        self->borderBottomWidth.setValue(stack[2]);
        break;
    case 4:
        self->borderTopWidth.setValue(stack[0]);
        self->borderRightWidth.setValue(stack[1]);
        self->borderBottomWidth.setValue(stack[2]);
        self->borderLeftWidth.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSBorderWidthShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    if (self->borderLeftWidth != self->borderRightWidth)
        return self->borderTopWidth.getCssText(self) + u' ' +
               self->borderRightWidth.getCssText(self) + u' ' +
               self->borderBottomWidth.getCssText(self) + u' ' +
               self->borderLeftWidth.getCssText(self);
    if (self->borderTopWidth != self->borderBottomWidth)
        return self->borderTopWidth.getCssText(self) + u' ' +
               self->borderRightWidth.getCssText(self) + u' ' +
               self->borderBottomWidth.getCssText(self);
    if (self->borderTopWidth != self->borderRightWidth)
        return self->borderTopWidth.getCssText(self) + u' ' +
               self->borderRightWidth.getCssText(self);
    return self->borderTopWidth.getCssText(self);
}

void CSSBorderWidthShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->borderTopWidth.specify(decl->borderTopWidth);
    self->borderRightWidth.specify(decl->borderRightWidth);
    self->borderBottomWidth.specify(decl->borderBottomWidth);
    self->borderLeftWidth.specify(decl->borderLeftWidth);
}

bool CSSBorderValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
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
                self->borderTopStyle.setValue(term);
                break;
            case 1:
                self->borderRightStyle.setValue(term);
                break;
            case 2:
                self->borderBottomStyle.setValue(term);
                break;
            case 3:
                self->borderLeftStyle.setValue(term);
                break;
            default:
                break;
            }
        } else if (term->propertyID == CSSStyleDeclarationImp::BorderWidth) {
            width = true;
            switch (index) {
            case 0:
                self->borderTopWidth.setValue(term);
                break;
            case 1:
                self->borderRightWidth.setValue(term);
                break;
            case 2:
                self->borderBottomWidth.setValue(term);
                break;
            case 3:
                self->borderLeftWidth.setValue(term);
                break;
            default:
                break;
            }
        } else {
            color = true;
            switch (index) {
            case 0:
                self->borderTopColor.setValue(term);
                break;
            case 1:
                self->borderRightColor.setValue(term);
                break;
            case 2:
                self->borderBottomColor.setValue(term);
                break;
            case 3:
                self->borderLeftColor.setValue(term);
                break;
            default:
                break;
            }
        }
    }
    if (!style) {
        switch (index) {
        case 0:
            self->borderTopStyle.setValue();
            break;
        case 1:
            self->borderRightStyle.setValue();
            break;
        case 2:
            self->borderBottomStyle.setValue();
            break;
        case 3:
            self->borderLeftStyle.setValue();
            break;
        default:
            break;
        }
    }
    if (!width) {
        switch (index) {
        case 0:
            self->borderTopWidth.setValue();
            break;
        case 1:
            self->borderRightWidth.setValue();
            break;
        case 2:
            self->borderBottomWidth.setValue();
            break;
        case 3:
            self->borderLeftWidth.setValue();
            break;
        default:
            break;
        }
    }
    if (!color) {
        switch (index) {
        case 0:
            self->borderTopColor.reset();
            break;
        case 1:
            self->borderRightColor.reset();
            break;
        case 2:
            self->borderBottomColor.reset();
            break;
        case 3:
            self->borderLeftColor.reset();
            break;
        default:
            break;
        }
    }
    return true;
}

std::u16string CSSBorderValueImp::getCssText(CSSStyleDeclarationImp* self) const
{
    switch (index) {
    case 0:
        return self->borderTopWidth.getCssText(self) + u' ' + self->borderTopStyle.getCssText(self) + u' ' + self->borderTopColor.getCssText(self);
    case 1:
        return self->borderRightWidth.getCssText(self) + u' ' + self->borderRightStyle.getCssText(self) + u' ' + self->borderRightColor.getCssText(self);
    case 2:
        return self->borderBottomWidth.getCssText(self) + u' ' + self->borderBottomStyle.getCssText(self) + u' ' + self->borderBottomColor.getCssText(self);
    case 3:
        return self->borderLeftWidth.getCssText(self) + u' ' + self->borderLeftStyle.getCssText(self) + u' ' + self->borderLeftColor.getCssText(self);
    default:
        return u"";
    }
}

void CSSBorderValueImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
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

bool CSSBorderShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    bool style = false;
    bool width = false;
    bool color = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID == CSSStyleDeclarationImp::BorderStyle) {
            style = true;
            self->borderBottomStyle = self->borderLeftStyle = self->borderRightStyle = self->borderTopStyle.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BorderWidth) {
            width = true;
            self->borderBottomWidth = self->borderLeftWidth = self->borderRightWidth = self->borderTopWidth.setValue(term);
        } else {
            color = true;
            self->borderBottomColor = self->borderLeftColor = self->borderRightColor = self->borderTopColor.setValue(term);
        }
    }
    if (!style)
        self->borderBottomStyle = self->borderLeftStyle = self->borderRightStyle = self->borderTopStyle.setValue();
    if (!width)
        self->borderBottomWidth = self->borderLeftWidth = self->borderRightWidth = self->borderTopWidth.setValue();
    if (!color)
        self->borderBottomColor = self->borderLeftColor = self->borderRightColor = self->borderTopColor.reset();
    return true;
}

std::u16string CSSBorderShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    return self->borderWidth.getCssText(self) + u' ' +
           self->borderStyle.getCssText(self) + u' ' +
           self->borderColor.getCssText(self);
}

void CSSBorderShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->borderWidth.specify(self, decl);
    self->borderStyle.specify(self, decl);
    self->borderColor.specify(self, decl);
}

bool CSSContentValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    reset();
    original = None;
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
                original = value = index;
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
                    content = new(std::nothrow) URIContent(term->getURL());
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

std::u16string CSSContentValueImp::getCssText(CSSStyleDeclarationImp* self) const
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

void CSSContentValueImp::specify(const CSSContentValueImp& specified)
{
    reset();
    original = specified.original;
    value = specified.value;
    for (auto i = specified.contents.begin(); i != specified.contents.end(); ++i) {
        if (Content* clone = (*i)->clone())
            contents.push_back(clone);
    }
}

bool CSSContentValueImp::operator==(const CSSContentValueImp& content) const
{
    if (wasNormal() && content.wasNormal())
        return true;
    if (wasNormal() || content.wasNormal())
        return false;
    if (isNone() && content.isNone())
        return true;
    if (isNone() || content.isNone())
        return false;
    assert(!contents.empty() && !content.contents.empty());
    if (contents.size() != content.contents.size())
        return false;
    return getCssText() == content.getCssText();    // TODO: Refine
}

void CSSContentValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    switch (self->getPseudoElementSelectorType()) {
    case CSSPseudoElementSelector::Before:
    case CSSPseudoElementSelector::After:
        if (wasNormal()) {
            value = None;
            clearContents();
        }
        break;
    case CSSPseudoElementSelector::Marker:
        if (wasNormal()) {
            value = None;
            clearContents();
            // If the image is not valid, use the 'list-style-type' instead.
            if (self->listStyleImage.hasImage()) {
                if (URIContent* content = new(std::nothrow) URIContent(self->listStyleImage.getValue()))
                    contents.push_back(content);
            } else {
                switch (self->listStyleType.getValue()) {
                case CSSListStyleTypeValueImp::None:
                    break;
                case CSSListStyleTypeValueImp::Disc:
                case CSSListStyleTypeValueImp::Circle:
                case CSSListStyleTypeValueImp::Square:
                    if (CounterContent* content = new CounterContent(u"list-item", self->listStyleType.getValue()))
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
                    if (CounterContent* content = new CounterContent(u"list-item", self->listStyleType.getValue()))
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

std::u16string CSSContentValueImp::CounterContent::eval(ViewCSSImp* view, Element element, CounterContext* context)
{
    if (CounterImpPtr counter = view->getCounter(identifier)) {
        if (nested)
            return counter->eval(string, listStyleType.getValue(), context);
        else
            return counter->eval(listStyleType.getValue(), context);
    }
    return u"";
}

std::u16string CSSContentValueImp::AttrContent::eval(ViewCSSImp* view, Element element, CounterContext* context)
{
    return element.getAttribute(identifier);
}

std::u16string CSSContentValueImp::QuoteContent::eval(ViewCSSImp* view, Element element, CounterContext* context)
{
    int depth;
    switch (value) {
    case OpenQuote:
        depth = view->incrementQuotingDepth();
        if (0 <= depth) {
            if (CSSStyleDeclarationPtr style = view->getStyle(element))
                return style->quotes.getOpenQuote(depth);
        }
        break;
    case CloseQuote:
        depth = view->decrementQuotingDepth();
        if (0 <= depth) {
            if (CSSStyleDeclarationPtr style = view->getStyle(element))
                return style->quotes.getCloseQuote(depth);
        }
        break;
    case NoOpenQuote:
        view->incrementQuotingDepth();
        break;
    case NoCloseQuote:
        view->decrementQuotingDepth();
        break;
    default:
        break;
    }
    return u"";
}

std::u16string CSSContentValueImp::evalText(ViewCSSImp* view, Element element, CounterContext* context)
{
    if (!contents.empty() && dynamic_cast<URIContent*>(contents.front()))
        return u"";

    std::u16string data;
    for (auto i = contents.begin(); i != contents.end(); ++i)
        data += (*i)->eval(view, element, context);
    return data;
}

Element CSSContentValueImp::eval(ViewCSSImp* view, Element element, CounterContext* context)
{
    if (contents.empty())
        return nullptr;

    if (URIContent* content = dynamic_cast<URIContent*>(contents.front())) {
        html::HTMLImageElement img = interface_cast<html::HTMLImageElement>(view->getDocument()->createElement(u"img"));
        if (img) {
            img.setSrc(content->value);
            if (auto imp = std::dynamic_pointer_cast<NodeImp>(img.self()))
                imp->setParent(std::static_pointer_cast<NodeImp>(element.self()));
        }
        return img;
    }

    Element span = view->getDocument()->createElement(u"span");
    if (!span)
        return nullptr;
    std::u16string data;
    for (auto i = contents.begin(); i != contents.end(); ++i)
        data += (*i)->eval(view, element, context);
    if (org::w3c::dom::Text text = view->getDocument()->createTextNode(data)) {
        if (auto imp = std::dynamic_pointer_cast<NodeImp>(span.self()))
            imp->appendChild(text, true);   // TODO: Find a better way. We don't want to fire any events.
    }

    // Set the pseudo parentNode of the new span element so that
    // setContainingBlock() works even for the positioned :before and
    // :after pseudo-elements.
    // cf. http://test.csswg.org/suites/css2.1/20110323/html4/containing-block-029.htm
    if (auto imp = std::dynamic_pointer_cast<NodeImp>(span.self()))
        imp->setParent(std::static_pointer_cast<NodeImp>(element.self()));

    return span;
}

bool CSSCursorValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    reset();
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->unit == CSSPrimitiveValue::CSS_URI)
            uriList.push_back(term->getURL());
        else if (0 <= term->getIndex())
            value = term->getIndex();
    }
    return true;
}

std::u16string CSSCursorValueImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    for (auto i = uriList.begin(); i != uriList.end(); ++i) {
        if (i != uriList.begin())
            cssText += u", ";
        cssText += u"url(" + CSSSerializeString(*i) + u')';
    }
    if (!uriList.empty())
        cssText += u", ";
    return cssText + Options[value];
}

void CSSDisplayValueImp::compute(CSSStyleDeclarationImp* self, Element element)
{
    if (original == None)
        return;
    if (self->isAbsolutelyPositioned())
        self->float_.setValue(CSSFloatValueImp::None);  // TOOD: keep original?
    else if (self->float_.getValue() == CSSFloatValueImp::None && (!element || element.getParentElement()))
        return;
    switch (original) {
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
    reset();
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
            // The keywords 'initial' and 'default' are reserved for future use.
            if (compareIgnoreCase(family, u"initial") && compareIgnoreCase(family, u"default"))
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
    if (!family.empty()) {
        // The keywords 'initial' and 'default' are reserved for future use.
        if (compareIgnoreCase(family, u"initial") && compareIgnoreCase(family, u"default"))
            familyNames.push_back(family);
    }
    return --i;
}

bool CSSFontFamilyValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    setValue(stack, stack.begin());
    return true;
}

std::u16string CSSFontFamilyValueImp::getCssText(CSSStyleDeclarationImp* self) const
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

void CSSFontSizeValueImp::compute(ViewCSSImp* view, const CSSStyleDeclarationPtr& parentStyle)
{
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
    size.setPx(w);
}

void CSSFontWeightValueImp::compute(ViewCSSImp* view, const CSSStyleDeclarationPtr& parentStyle)
{
    unsigned inherited = parentStyle ? parentStyle->fontWeight.getWeight() : 400;
    unsigned w;
    switch (value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        switch (value.getIndex()) {
        case Normal:
            w = 400;
            break;
        case Bold:
            w = 700;
            break;
        case Bolder:
            if (inherited < 400)
                w = 400;
            else if (inherited < 600)
                w = 700;
            else
                w = 900;
            break;
        case Lighter:
            if (inherited < 600)
                w = 100;
            else if (inherited < 800)
                w = 400;
            else
                w = 700;
            break;
        default:  // TODO: error
            w = inherited;
            break;
        }
        break;
    default:
        w = value.number;
        break;
    }
    value.setPx(w);
}

bool CSSFontShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    reset(self);
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        switch (term->propertyID) {
        case CSSStyleDeclarationImp::FontStyle:
            self->fontStyle.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontVariant:
            self->fontVariant.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontWeight:
            self->fontWeight.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontSize:
            self->fontSize.setValue(term);
            break;
        case CSSStyleDeclarationImp::LineHeight:
            self->lineHeight.setValue(term);
            break;
        case CSSStyleDeclarationImp::FontFamily:
            i = self->fontFamily.setValue(stack, i);
            break;
        default:
            if (term->unit == CSSParserTerm::CSS_TERM_INDEX)
                index = term->getIndex();
            break;
        }
    }
    return true;
}

std::u16string CSSFontShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    if (index != Normal)
        return Options[index];

    std::u16string text;
    if (!self->fontStyle.isNormal())
        text += self->fontStyle.getCssText(self);
    if (!self->fontVariant.isNormal()) {
        if (!text.empty())
            text += u" ";
        text += self->fontVariant.getCssText(self);
    }
    if (!self->fontWeight.isNormal()) {
        if (!text.empty())
            text += u" ";
        text += self->fontWeight.getCssText(self);
    }
    if (!text.empty())
        text += u" ";
    text += self->fontSize.getCssText(self);
    if (!self->lineHeight.isNormal())
        text += u"/" + self->lineHeight.getCssText(self);
    text += u" " + self->fontFamily.getCssText(self);
    return text;
}

void CSSFontShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
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

void CSSLineHeightValueImp::inherit(const CSSLineHeightValueImp& parent)
{
    switch (parent.value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
    case css::CSSPrimitiveValue::CSS_NUMBER:
        value.specify(parent.value);
        break;
    default:
        value.inherit(parent.value);
        break;
    }
}

void CSSLineHeightValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    switch (value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
    case css::CSSPrimitiveValue::CSS_NUMBER:
        value.resolved = NAN;
        break;
    default:
        value.resolve(view, self, self->fontSize.getPx());
        break;
    }
}

void CSSLineHeightValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    float w;
    switch (value.isNegative() ? CSSParserTerm::CSS_TERM_INDEX : value.unit) {
    case CSSParserTerm::CSS_TERM_INDEX:
        if (FontTexture* font = self->getFontTexture())
            w = font->getLineHeight(view->getPointFromPx(self->fontSize.getPx()));
        else
            w = self->fontSize.getPx() * 1.2;
        break;
    case css::CSSPrimitiveValue::CSS_NUMBER:
        w = self->fontSize.getPx() * value.number;
        break;
    default:
        return;
    }
    value.setPx(w);
}

void CSSListStyleImageValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (isNone() || self->getPseudoElementSelectorType() != CSSPseudoElementSelector::Marker || !self->content.wasNormal())
        return;
    if (view->getDocument()) {
        HttpRequestPtr prev = request;
        request = view->preload(view->getDocument()->getDocumentURI(), getValue());
        if (request) {
            if (prev && prev != request)
                prev ->clearCallback(requestID);
            requestID = request->addCallback(boost::bind(&CSSListStyleImageValueImp::notify, this, self), requestID);
            status = request->getStatus();
            // TODO: load count should be incremented
        }
    }
}

void CSSListStyleImageValueImp::notify(CSSStyleDeclarationImp* self)
{
    assert(request);
    if (status != request->getStatus()) {
        self->requestReconstruct(Box::NEED_STYLE_RECALCULATION);
        self->clearFlags(CSSStyleDeclarationImp::Computed);
    }
}

void CSSListStylePositionValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (self->getPseudoElementSelectorType() != CSSPseudoElementSelector::Marker)
        return;

    switch (value) {
    case Inside:
        self->display.setValue(CSSDisplayValueImp::InlineBlock);
        self->position.setValue(CSSPositionValueImp::Static);
        break;
    case Outside:
        self->display.setValue(CSSDisplayValueImp::Block);
        self->position.setValue(CSSPositionValueImp::Absolute);
        break;
    default:
        break;
    }
}

bool CSSListStyleShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
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
            self->listStyleType.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::ListStylePosition) {
            position = true;
            self->listStylePosition.setValue(term);
        } else if (term->unit == css::CSSPrimitiveValue::CSS_URI) {
            image = true;
            self->listStyleImage.setValue(term);
        } else if (term->getIndex() == CSSListStyleShorthandImp::None)
            none = true;
    }
    if (!type) {
        if (none)
            self->listStyleType.setValue(CSSListStyleTypeValueImp::None);
        else
            self->listStyleType.setValue();
    }
    if (!position)
        self->listStylePosition.setValue();
    if (!image)
        self->listStyleImage.setValue();
    if (none && type && image) {
        reset(self);
        return false;
    }
    return true;
}

std::u16string CSSListStyleShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    return self->listStyleType.getCssText(self) + u' ' +
           self->listStylePosition.getCssText(self) + u' ' +
           self->listStyleImage.getCssText(self);
}

void CSSListStyleShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
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

bool CSSMarginShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        self->marginBottom = self->marginLeft = self->marginRight = self->marginTop.setValue(stack[0]);
        break;
    case 2:
        self->marginBottom = self->marginTop.setValue(stack[0]);
        self->marginLeft = self->marginRight.setValue(stack[1]);
        break;
    case 3:
        self->marginTop.setValue(stack[0]);
        self->marginLeft = self->marginRight.setValue(stack[1]);
        self->marginBottom.setValue(stack[2]);
        break;
    case 4:
        self->marginTop.setValue(stack[0]);
        self->marginRight.setValue(stack[1]);
        self->marginBottom.setValue(stack[2]);
        self->marginLeft.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSMarginShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    if (self->marginLeft != self->marginRight)
        return self->marginTop.getCssText(self) + u' ' +
               self->marginRight.getCssText(self) + u' ' +
               self->marginBottom.getCssText(self) + u' ' +
               self->marginLeft.getCssText(self);
    if (self->marginTop != self->marginBottom)
        return self->marginTop.getCssText(self) + u' ' +
               self->marginRight.getCssText(self) + u' ' +
               self->marginBottom.getCssText(self);
    if (self->marginTop != self->marginRight)
        return self->marginTop.getCssText(self) + u' ' +
               self->marginRight.getCssText(self);
    return self->marginTop.getCssText(self);
}

void CSSMarginShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->marginTop.specify(decl->marginTop);
    self->marginRight.specify(decl->marginRight);
    self->marginBottom.specify(decl->marginBottom);
    self->marginLeft.specify(decl->marginLeft);
}

bool CSSOutlineShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    bool style = false;
    bool width = false;
    bool color = false;
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        if (term->propertyID == CSSStyleDeclarationImp::BorderStyle) {
            style = true;
            self->outlineStyle.setValue(term);
        } else if (term->propertyID == CSSStyleDeclarationImp::BorderWidth) {
            width = true;
            self->outlineWidth.setValue(term);
        } else {
            color = true;
            self->outlineColor.setValue(term);
        }
    }
    if (!style)
        self->outlineStyle.setValue();
    if (!width)
        self->outlineWidth.setValue();
    if (!color)
        self->outlineColor.setValue();
    return true;
}

std::u16string CSSOutlineShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    return self->outlineWidth.getCssText(self) + u' ' + self->outlineStyle.getCssText(self) + u' ' + self->outlineColor.getCssText(self);
}

void CSSOutlineShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->outlineColor.specify(decl->outlineColor);
    self->outlineStyle.specify(decl->outlineStyle);
    self->outlineWidth.specify(decl->outlineWidth);
}

bool CSSPaddingShorthandImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    switch (stack.size()) {
    case 1:
        self->paddingBottom = self->paddingLeft = self->paddingRight = self->paddingTop.setValue(stack[0]);
        break;
    case 2:
        self->paddingBottom = self->paddingTop.setValue(stack[0]);
        self->paddingLeft = self->paddingRight.setValue(stack[1]);
        break;
    case 3:
        self->paddingTop.setValue(stack[0]);
        self->paddingLeft = self->paddingRight.setValue(stack[1]);
        self->paddingBottom.setValue(stack[2]);
        break;
    case 4:
        self->paddingTop.setValue(stack[0]);
        self->paddingRight.setValue(stack[1]);
        self->paddingBottom.setValue(stack[2]);
        self->paddingLeft.setValue(stack[3]);
        break;
    }
    return true;
}

std::u16string CSSPaddingShorthandImp::getCssText(CSSStyleDeclarationImp* self) const
{
    std::u16string cssText;
    if (self->paddingLeft != self->paddingRight)
        return self->paddingTop.getCssText(self) + u' ' +
               self->paddingRight.getCssText(self) + u' ' +
               self->paddingBottom.getCssText(self) + u' ' +
               self->paddingLeft.getCssText(self);
    if (self->paddingTop != self->paddingBottom)
        return self->paddingTop.getCssText(self) + u' ' +
               self->paddingRight.getCssText(self) + u' ' +
               self->paddingBottom.getCssText(self);
    if (self->paddingTop != self->paddingRight)
        return self->paddingTop.getCssText(self) + u' ' +
               self->paddingRight.getCssText(self);
    return self->paddingTop.getCssText(self);
}

void CSSPaddingShorthandImp::specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationPtr& decl)
{
    self->paddingTop.specify(decl->paddingTop);
    self->paddingRight.specify(decl->paddingRight);
    self->paddingBottom.specify(decl->paddingBottom);
    self->paddingLeft.specify(decl->paddingLeft);
}

void CSSQuotesValueImp::reset()
{
    quotes.clear();
}

bool CSSQuotesValueImp::setValue(CSSStyleDeclarationImp* self, CSSValueParser* parser)
{
    reset();
    std::deque<CSSParserTerm*>& stack = parser->getStack();
    if (stack.size() % 2)
        return true;
    for (auto i = stack.begin(); i != stack.end(); ++i) {
        CSSParserTerm* term = *i;
        assert(term->unit == CSSPrimitiveValue::CSS_STRING);
        std::u16string open = term->getString();
        ++i;
        assert(i != stack.end());
        term = *i;
        std::u16string close = term->getString();
        quotes.push_back(std::make_pair(open, close));
    }
    return true;
}

std::u16string CSSQuotesValueImp::getCssText(CSSStyleDeclarationImp* self) const
{
    if (quotes.empty())
        return u"none";
    std::u16string cssText;
    for (auto i = quotes.begin(); i != quotes.end(); ++i) {
        if (i != quotes.begin())
            cssText += u' ';
        cssText += CSSSerializeString(i->first) + u' ' + CSSSerializeString(i->second);
    }
    return cssText;
}

void CSSQuotesValueImp::specify(const CSSQuotesValueImp& specified)
{
    quotes = specified.quotes;
}

void CSSVerticalAlignValueImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (value.isIndex() || value.isPercentage())
        return;
    value.compute(view, self);
}

void CSSVerticalAlignValueImp::resolve(ViewCSSImp* view, CSSStyleDeclarationImp* self)
{
    if (value.isIndex())
        return;
    value.resolve(view, self, self->lineHeight.getPx());
}

float CSSVerticalAlignValueImp::getOffset(ViewCSSImp* view, CSSStyleDeclarationImp* self, LineBox* line, FontTexture* font, float point, float leading) const
{
    assert(self->display.isInlineLevel());
    leading /= 2.0f;
    // TODO: Check if there is a parent inline element firstly.
    switch (value.getIndex()) {
    case Top:
        return 0.0f;
    case Baseline:
        return line->getBaseline() - (leading + font->getAscender(point));
    case Middle: {
        float offset = line->getBaseline() - (font->getLineHeight(point) / 2.0f);
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font)
                offset -= font->getXHeight(view->getPointFromPx(parent->fontSize.getPx())) / 2.0f;
        }
        return offset;
    }
    case Bottom:
        return line->getHeight() - font->getLineHeight(point);
    case Sub:
        return line->getBaseline() - (leading + font->getAscender(point)) + font->getSub(point);
    case Super:
        return line->getBaseline() - (leading + font->getAscender(point)) - font->getSuper(point);
    case TextTop: {
        float offset = 0.0f;
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font)
                offset = line->getBaseline() - font->getAscender(view->getPointFromPx(parent->fontSize.getPx()));
        }
        return offset;
    }
    case TextBottom: {
        float offset = line->getHeight();
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font) {
                float point = view->getPointFromPx(parent->fontSize.getPx());
                offset = line->getBaseline() - font->getAscender(point) + font->getLineHeight(point);
            }
        }
        return offset - font->getLineHeight(point);
    }
    default:
        assert(!value.isNaN());
        return line->getBaseline() - (leading + font->getAscender(point)) - value.getPx();
    }
}

float CSSVerticalAlignValueImp::getOffset(ViewCSSImp* view, CSSStyleDeclarationImp* self, LineBox* line, InlineBox* text) const
{
    assert(self->display.isInlineLevel());
    float leading = text->getLeading() / 2.0f;
    float h = text->getLeading() + text->getHeight();
    // TODO: Check if there is a parent inline element firstly.
    switch (value.getIndex()) {
    case Top:
        return 0.0f;
    case Baseline:
        return line->getBaseline() - (leading + text->getBaseline());
    case Middle: {
        float offset = line->getBaseline() - (h / 2.0f);
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font)
                offset -= font->getXHeight(view->getPointFromPx(parent->fontSize.getPx())) / 2.0f;
        }
        return offset;
    }
    case Bottom:
        return line->getHeight() - h;
    case Sub:
        return line->getBaseline() - (leading + text->getBaseline()) + text->getSub();
    case Super:
        return line->getBaseline() - (leading + text->getBaseline()) - text->getSuper();
    case TextTop: {
        float offset = 0.0f;
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font)
                offset = line->getBaseline() - font->getAscender(view->getPointFromPx(parent->fontSize.getPx()));
        }
        return offset;
    }
    case TextBottom: {
        float offset = line->getHeight();
        if (CSSStyleDeclarationPtr parent = self->getParentStyle()) {
            FontTexture* font = parent->getFontTexture();
            if (!font)
                font = view->selectFont(parent);
            if (font) {
                float point = view->getPointFromPx(parent->fontSize.getPx());
                offset = line->getBaseline() - font->getAscender(point) + font->getLineHeight(point);
            }
        }
        offset -= text->getTotalHeight();
        return offset;
    }
    default:
        assert(!value.isNaN());
        return line->getBaseline() - (leading + text->getBaseline()) - value.getPx();
    }
}

void CSSBindingValueImp::specify(const CSSStyleDeclarationPtr& decl)
{
    value = decl->binding.value;
    uri = decl->resolveRelativeURL(decl->binding.uri);
}

}}}}  // org::w3c::bootstrap
