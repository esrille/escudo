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

#ifndef CSSSPROPERTYVALUE_IMP_H
#define CSSSPROPERTYVALUE_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSPropertyValue.h>

#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/Element.h>

#include <assert.h>  // TODO
#include <math.h>

#include "CSSValueParser.h"
#include "CSSSerialize.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class ContainingBlock;
class BoxImage;
class LineBox;
class InlineLevelBox;
class CSSStyleDeclarationImp;
class ViewCSSImp;

class CSSFontSizeValueImp;

struct CSSNumericValue
{
    static const char16_t* Units[];
    unsigned short unit;  // cf. CSSPrimitiveValue
    short index;  // non-negative value if this value has a keyword index value rather than number
    float number;
    CSSNumericValue(int index = 0) :
        unit(CSSParserTerm::CSS_TERM_INDEX),
        index(index),
        number(0.0f) {
        assert(0 <= index);
    }
    CSSNumericValue(float number, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) :
        unit(unit),
        index(-1),
        number(number) {
    }
    std::u16string getCssText(const char16_t* options[] = 0) {
        if (unit == CSSParserTerm::CSS_TERM_INDEX)
            return options[index];
        std::u16string cssText = CSSSerializeNumber(number);
        if (number != 0.0f && css::CSSPrimitiveValue::CSS_PERCENTAGE <= unit && unit <= css::CSSPrimitiveValue::CSS_KHZ)
            cssText += Units[unit - css::CSSPrimitiveValue::CSS_PERCENTAGE];
        return cssText;
    }
    CSSNumericValue& setValue(CSSParserTerm* term) {
        assert(term);
        unit = term->unit;
        index = (unit == CSSParserTerm::CSS_TERM_INDEX) ? term->getIndex() : -1;
        number = static_cast<float>(term->number);
        return *this;
    }
    CSSNumericValue& setValue(float number, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) {
        assert(!isnan(number));
        this->unit = unit;
        this->index = -1;
        this->number = number;
        return *this;
    }
    float getPx() const {
        assert(unit == css::CSSPrimitiveValue::CSS_PX || number == 0.0f);
        return number;
    }
    bool isNegative() const {
        return !isIndex() && number < 0.0;
    }
    bool isIndex() const {
        return unit == CSSParserTerm::CSS_TERM_INDEX;
    }
    short getIndex() const {
        if (unit == CSSParserTerm::CSS_TERM_INDEX)
            return index;
        return -1;
    }
    CSSNumericValue& setIndex(short value) {
        this->unit = CSSParserTerm::CSS_TERM_INDEX;
        this->index = value;
        return *this;
    }
    bool isPercentage() const {
        return unit == css::CSSPrimitiveValue::CSS_PERCENTAGE;
    }
    bool operator==(const CSSNumericValue& value) const {
        if (unit == CSSParserTerm::CSS_TERM_INDEX && index == value.index)
            return true;
        if (number == value.number) {
            if (unit == value.unit)
                return true;
            if (number == 0.0f &&
                css::CSSPrimitiveValue::CSS_PERCENTAGE <= unit && unit <= css::CSSPrimitiveValue::CSS_KHZ &&
                css::CSSPrimitiveValue::CSS_PERCENTAGE <= value.unit && value.unit <= css::CSSPrimitiveValue::CSS_KHZ)
                return true;
        }
        return false;
    }
    bool operator!=(const CSSNumericValue& value) const {
        return !(*this == value);
    }
    void specify(const CSSNumericValue& value) {
        unit = value.unit;
        index = value.index;
        number = value.number;
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize);
};

class CSSPropertyValueImp
{
public:
    virtual CSSPropertyValueImp& setValue(CSSParserTerm* term) {
        return *this;
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser) {
        CSSParserTerm* term = parser->getStack().back();
        setValue(term);
        return true;
    }
    // CSSPropertyValue
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return u"";
    }
    virtual void setCssText(std::u16string cssText) {}
};

class CSSListStyleTypeValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Disc,
        Circle,
        Square,
        Decimal,
        DecimalLeadingZero,
        LowerRoman,
        UpperRoman,
        LowerGreek,
        LowerLatin,
        UpperLatin,
        Armenian,
        Georgian,
        LowerAlpha,
        UpperAlpha
    };
    unsigned getValue() const {
        return value;
    }
    CSSListStyleTypeValueImp& setValue(unsigned value = Disc) {
        this->value = value;
        return *this;
    }
    CSSListStyleTypeValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl = 0) {
        return Options[value];
    }
    bool operator==(const CSSListStyleTypeValueImp& style) const {
        return value == style.value;
    }
    bool operator!=(const CSSListStyleTypeValueImp& style) const {
        return value != style.value;
    }
    void specify(const CSSListStyleTypeValueImp& specified) {
        value = specified.value;
    }
    CSSListStyleTypeValueImp(unsigned initial = Disc) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSNumericValueImp : public CSSPropertyValueImp
{
protected:
    CSSNumericValue value;
public:
    CSSNumericValueImp& setValue(float number = 0.0f, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) {
        value.setValue(number, unit);
        return *this;
    }
    CSSNumericValueImp& setValue(CSSParserTerm* term) {
        value.setValue(term);
        return *this;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return value.getCssText();
    }
    bool operator==(const CSSNumericValueImp& n) {
        return value == n.value;
    }
    bool operator!=(const CSSNumericValueImp& n) {
        return value != n.value;
    }
    void specify(const CSSNumericValueImp& specified) {
        value.specify(specified.value);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize);
    float getPx() const {
        return value.getPx();
    }
    CSSNumericValueImp(float number = 0.0f, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) :
        value(number, unit) {
    }
};

class CSSNonNegativeValueImp : public CSSNumericValueImp
{
public:
    CSSNonNegativeValueImp& setValue(float number = 0.0f, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) {
        value.setValue(number, unit);
        return *this;
    }
    CSSNonNegativeValueImp& setValue(CSSParserTerm* term) {
        value.setValue(term);
        return *this;
    }
    bool isPercentage() const {
        return value.unit == css::CSSPrimitiveValue::CSS_PERCENTAGE;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return value.getCssText();
    }
    bool operator==(const CSSNonNegativeValueImp& n) {
        return value == n.value;
    }
    bool operator!=(const CSSNonNegativeValueImp& n) {
        return value != n.value;
    }
    void specify(const CSSNonNegativeValueImp& specified) {
        value.specify(specified.value);
    }
    float getPx() const {
        float px = value.getPx();
        return (px < 0.0f) ? 0.0f : px;
    }
    CSSNonNegativeValueImp(float number = 0.0f, unsigned short unit = css::CSSPrimitiveValue::CSS_NUMBER) :
        CSSNumericValueImp(number, unit) {
    }
};

// <length>, <percentage>, auto
class CSSAutoLengthValueImp : public CSSPropertyValueImp
{
protected:
    CSSNumericValue length;
public:
    CSSAutoLengthValueImp& setValue(float number, unsigned short unit) {
        length.setValue(number, unit);
        return *this;
    }
    CSSAutoLengthValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            length.setValue(term);
        else
            length.setIndex(0);
        return *this;
    }
    bool isAuto() const {
        return length.getIndex() == 0;
    }
    bool isPercentage() const {
        return length.unit == css::CSSPrimitiveValue::CSS_PERCENTAGE;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (isAuto())
            return u"auto";
        return length.getCssText();
    }
    bool operator==(const CSSAutoLengthValueImp& value) {
        return length == value.length;
    }
    bool operator!=(const CSSAutoLengthValueImp& value) {
        return length != value.length;
    }
    void specify(const CSSAutoLengthValueImp& specified) {
        length.specify(specified.length);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize);
    float getPx() const {
        return length.getPx();
    }
    CSSAutoLengthValueImp() :
        length(0) {
    }
    CSSAutoLengthValueImp(float number, unsigned short unit) :
        length(number, unit) {
    }
};

// <length>, <percentage>, none
// Negative values are illegal.
class CSSNoneLengthValueImp : public CSSPropertyValueImp
{
protected:
    CSSNumericValue length;
public:
    CSSNoneLengthValueImp& setValue(float number, unsigned short unit) {
        length.setValue(number, unit);
        if (length.isNegative())
            length.setIndex(0);
        return *this;
    }
    CSSNoneLengthValueImp& setValue(CSSParserTerm* term = 0) {
        if (!term)
            length.setIndex(0);
        else {
            length.setValue(term);
            if (length.isNegative())
                length.setIndex(0);
        }
        return *this;
    }
    bool isNone() const {
        return length.getIndex() == 0;
    }
    bool isPercentage() const {
        return length.unit == css::CSSPrimitiveValue::CSS_PERCENTAGE;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (isNone())
            return u"none";
        return length.getCssText();
    }
    bool operator==(const CSSNoneLengthValueImp& value) {
        return length == value.length;
    }
    bool operator!=(const CSSNoneLengthValueImp& value) {
        return length != value.length;
    }
    void specify(const CSSNoneLengthValueImp& specified) {
        length.specify(specified.length);
    }
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style, float fullSize);
    float getPx() const {
        float px = length.getPx();
        return (px < 0.0f) ? 0.0f : px;
    }
    CSSNoneLengthValueImp() :
        length(0) {
    }
    CSSNoneLengthValueImp(float number, unsigned short unit) :
        length(number, unit) {
    }
};

class CounterImp;

class CSSAutoNumberingValueImp : public CSSPropertyValueImp
{
public:
    struct Content {
        std::u16string name;
        int number;
        Content(const std::u16string& name, int number) :
            name(name),
            number(number) {
        }
        virtual std::u16string getCssText(int defaultNumber) {
            if (number == defaultNumber)
                return CSSSerializeIdentifier(name);
            return CSSSerializeIdentifier(name) + u' ' + CSSSerializeInteger(number);
        }
    };

    struct CounterContext
    {
        ViewCSSImp* view;
        std::list<CounterImp*> counters;
    public:
        CounterContext(ViewCSSImp* view) :
            view(view)
        {}
        ~CounterContext();
        bool hasCounter() const {
            return !counters.empty();
        }
        bool hasCounter(const std::u16string& name) const;
        void addCounter(CounterImp* counter) {
            counters.push_front(counter);
        }
        void update(CSSStyleDeclarationImp* style);
    };


private:
    std::list<Content*> contents;
    int defaultNumber;

    void clearContents() {
        while (!contents.empty()) {
            Content* content = contents.front();
            delete content;
            contents.pop_front();
        }
    }

public:
    virtual ~CSSAutoNumberingValueImp() {
        clearContents();
    }
    void reset() {
        clearContents();
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(const CSSAutoNumberingValueImp& specified) {
        contents = specified.contents;
    }
    bool hasCounter() const {
        return !contents.empty();
    }
    void incrementCounter(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context);
    void resetCounter(ViewCSSImp* view, CounterContext* context);

    CSSAutoNumberingValueImp(int defaultNumber) :
        defaultNumber(defaultNumber) {
    }
};

class CSSBackgroundAttachmentValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Scroll,
        Fixed
    };
    CSSBackgroundAttachmentValueImp& setValue(unsigned value = Scroll) {
        this->value = value;
        return *this;
    }
    CSSBackgroundAttachmentValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSBackgroundAttachmentValueImp& specified) {
        value = specified.value;
    }
    CSSBackgroundAttachmentValueImp(unsigned initial = Scroll) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSBackgroundImageValueImp : public CSSPropertyValueImp
{
    std::u16string uri;
public:
    enum {
        None
    };
    CSSBackgroundImageValueImp& setValue(const std::u16string uri = u"") {
        this->uri = uri;
        return *this;
    }
    CSSBackgroundImageValueImp& setValue(CSSParserTerm* term) {
        if (0 <= term->getIndex())
            return setValue();
        else
            return setValue(term->getString());
    }
    std::u16string getValue() const {
        return uri;
    }
    bool isNone() const {
        return uri.length() == 0;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (isNone())
            return u"none";
        return u"url(" + CSSSerializeString(uri) + u')';
    }
    void specify(const CSSBackgroundImageValueImp& specified) {
        uri = specified.uri;
    }
    CSSBackgroundImageValueImp() {
    }
};

class CSSBackgroundPositionValueImp : public CSSPropertyValueImp
{
    CSSNumericValue horizontal;
    CSSNumericValue vertical;
public:
    enum {
        Top,
        Right,
        Bottom,
        Left,
        Center
    };
    CSSBackgroundPositionValueImp& setValue(float h = 0.0f, short horizontalUnit = css::CSSPrimitiveValue::CSS_PERCENTAGE,
                                            float v = 0.0f, short verticalUnit = css::CSSPrimitiveValue::CSS_PERCENTAGE) {
        horizontal.setValue(h, horizontalUnit);
        vertical.setValue(v, verticalUnit);
        return *this;
    }
    std::deque<CSSParserTerm*>::iterator setValue(std::deque<CSSParserTerm*>& stack, std::deque<CSSParserTerm*>::iterator i);
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return horizontal.getCssText() + u' ' + vertical.getCssText();
    }
    void specify(const CSSBackgroundPositionValueImp& specified) {
        horizontal.specify(specified.horizontal);
        vertical.specify(specified.vertical);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, BoxImage* image, CSSStyleDeclarationImp* style, float width, float height);
    float getLeftPx() const {
        return horizontal.getPx();
    }
    float getTopPx() const {
        return vertical.getPx();
    }
    CSSBackgroundPositionValueImp() :
        horizontal(0.0, css::CSSPrimitiveValue::CSS_PERCENTAGE),
        vertical(0.0, css::CSSPrimitiveValue::CSS_PERCENTAGE) {
    }
};

class CSSBackgroundRepeatValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        NoRepeat,
        RepeatX,
        RepeatY,
        Repeat,
    };
    CSSBackgroundRepeatValueImp& setValue(unsigned value = Repeat) {
        this->value = value;
        return *this;
    }
    CSSBackgroundRepeatValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSBackgroundRepeatValueImp& specified) {
        value = specified.value;
    }
    CSSBackgroundRepeatValueImp (unsigned initial = Repeat) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSBackgroundShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    void reset(CSSStyleDeclarationImp* self);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSBorderColorValueImp : public CSSPropertyValueImp
{
    unsigned value;
    bool hasValue;
public:
    CSSBorderColorValueImp& setValue(unsigned color = 0xff000000) {
        value = color;
        hasValue = true;
        return *this;
    }
    CSSBorderColorValueImp& setValue(CSSParserTerm* term) {
        assert(term->unit == css::CSSPrimitiveValue::CSS_RGBCOLOR);
        hasValue = true;
        return setValue(term->rgb);
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return CSSSerializeRGB(value);
    }
    bool operator==(const CSSBorderColorValueImp& color) const {
        if (!hasValue && !color.hasValue)
            return true;
        if (hasValue != color.hasValue)
            return false;
        return value == color.value;
    }
    bool operator!=(const CSSBorderColorValueImp& color) const {
        return !(*this == color);
    }
    void specify(const CSSBorderColorValueImp& specified) {
        value = specified.value;
        hasValue = specified.hasValue;
    }
    CSSBorderColorValueImp& reset() {
        value = 0xff000000;
        hasValue = false;
        return *this;
    }
    unsigned getARGB() {
        return value;
    }
    void compute(CSSStyleDeclarationImp* decl);
    CSSBorderColorValueImp(unsigned argb = 0xff000000) :
        value(0xff000000),
        hasValue(false)
    {
    }
};

class CSSBorderSpacingValueImp : public CSSPropertyValueImp
{
    CSSNumericValue horizontal;
    CSSNumericValue vertical;
public:
    CSSBorderSpacingValueImp& setValue(float h = 0.0f, short horizontalUnit = css::CSSPrimitiveValue::CSS_PX,
                                       float v = 0.0f, short verticalUnit = css::CSSPrimitiveValue::CSS_PX) {
        horizontal.setValue(h, horizontalUnit);
        vertical.setValue(v, verticalUnit);
        return *this;
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser) {
        std::deque<CSSParserTerm*>& stack = parser->getStack();
        if (stack.size() == 1)
            setValue(stack[0]->number, stack[0]->unit, stack[0]->number, stack[0]->unit);
        else
            setValue(stack[0]->number, stack[0]->unit, stack[1]->number, stack[1]->unit);
        return true;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (horizontal == vertical)
            return horizontal.getCssText();
        return horizontal.getCssText() + u' ' + vertical.getCssText();
    }
    void specify(const CSSBorderSpacingValueImp& specified) {
        horizontal.specify(specified.horizontal);
        vertical.specify(specified.vertical);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    float getHorizontalSpacing() const {
        // Note in HTML, the default value is 2.
        float v = horizontal.getPx();
        return (0.0f <= v) ? v : 2.0f;
    }
    float getVerticalSpacing() const {
        // Note in HTML, the default value is 2.
        float v = vertical.getPx();
        return (0.0f <= v) ? v : 2.0f;
    }
    CSSBorderSpacingValueImp() :
        horizontal(0, css::CSSPrimitiveValue::CSS_PX),
        vertical(0, css::CSSPrimitiveValue::CSS_PX) {
    }
};

class CSSBorderCollapseValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Collapse,
        Separate
    };
    CSSBorderCollapseValueImp& setValue(unsigned value = Separate) {
        this->value = value;
        return *this;
    }
    CSSBorderCollapseValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSBorderCollapseValueImp& specified) {
        value = specified.value;
    }
    CSSBorderCollapseValueImp(unsigned initial = Separate) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSBorderColorShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSBorderStyleValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        // In the preferred order for the border conflict resolution
        None,
        Inset,
        Groove,
        Outset,
        Ridge,
        Dotted,
        Dashed,
        Solid,
        Double,
        Hidden
    };
    CSSBorderStyleValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    CSSBorderStyleValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    bool operator==(const CSSBorderStyleValueImp& style) const {
        return value == style.value;
    }
    bool operator!=(const CSSBorderStyleValueImp& style) const {
        return value != style.value;
    }
    bool operator<(const CSSBorderStyleValueImp& style) const {
        return value < style.value;
    }
    void specify(const CSSBorderStyleValueImp& specified) {
        value = specified.value;
    }
    CSSBorderStyleValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSBorderStyleShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSBorderWidthValueImp : public CSSPropertyValueImp
{
    CSSNumericValue width;
public:
    enum {
         Thin,
         Medium,
         Thick
    };
    CSSBorderWidthValueImp& setValue(float number, unsigned short unit) {
        width.setValue(number, unit);
        return *this;
    }
    CSSBorderWidthValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            width.setValue(term);
        else
            width.setIndex(Medium);
        return *this;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return width.getCssText(Options);
    }
    bool operator==(const CSSBorderWidthValueImp& value) const {
        return width == value.width;
    }
    bool operator!=(const CSSBorderWidthValueImp& value) const {
        return width != value.width;
    }
    bool operator<(const CSSBorderWidthValueImp& value) const {
        return getPx() < value.getPx();
    }
    void specify(const CSSBorderWidthValueImp& specified) {
        width.specify(specified.width);
    }
    void compute(ViewCSSImp* view, const CSSBorderStyleValueImp& borderStyle, CSSStyleDeclarationImp* style);
    float getPx() const {
        float px = width.getPx();
        return (px < 0.0f) ? 3.0f : px;  // returns the default 'medium' width in px if negative.
    }
    CSSBorderWidthValueImp() :
        width(Medium) {
    }
    CSSBorderWidthValueImp(float number) :
        width(number) {
    }
    static const char16_t* Options[];
};

class CSSBorderWidthShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSBorderValueImp : public CSSPropertyValueImp
{
    unsigned index;
public:
    CSSBorderValueImp(unsigned index) :
        index(index)
    {
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSBorderShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSCaptionSideValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Top,
        Bottom
    };
    CSSCaptionSideValueImp& setValue(unsigned value = Top) {
        this->value = value;
        return *this;
    }
    CSSCaptionSideValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSCaptionSideValueImp& specified) {
        value = specified.value;
    }
    CSSCaptionSideValueImp(unsigned initial = Top) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSClearValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Left,
        Right,
        Both
    };
    CSSClearValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    CSSClearValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSClearValueImp& specified) {
        value = specified.value;
    }
    CSSClearValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSColorValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    static const unsigned Transparent = 0x00000000u;
    CSSColorValueImp& setValue(unsigned color = 0xff000000) {
        value = color;
        return *this;
    }
    CSSColorValueImp& setValue(CSSParserTerm* term) {
        assert(term->unit == css::CSSPrimitiveValue::CSS_RGBCOLOR);
        return setValue(term->rgb);
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return CSSSerializeRGB(value);
    }
    bool operator==(const CSSColorValueImp& color) const {
        return value == color.value;
    }
    bool operator!=(const CSSColorValueImp& color) const {
        return value != color.value;
    }
    void specify(const CSSColorValueImp& specified) {
        value = specified.value;
    }
    unsigned getARGB() {
        return value;
    }
    CSSColorValueImp(unsigned argb = 0xff000000) :
        value(argb) {
    }
};

class CSSContentValueImp : public CSSPropertyValueImp
{
public:
    enum {
        Normal,
        None,
        // QuoteContent
        OpenQuote,
        CloseQuote,
        NoOpenQuote,
        NoCloseQuote,
        // Attr
        Attr,
        Counter // pseudo
    };
    struct Content {
        virtual ~Content() {}
        virtual std::u16string getCssText() = 0;
        virtual std::u16string eval(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context) {
            return u"";
        }
    };
    struct StringContent : public Content {
        std::u16string value;
        StringContent(const std::u16string& value) :
            value(value) {
        }
        virtual std::u16string getCssText() {
            return CSSSerializeString(value);
        }
        virtual std::u16string eval(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context) {
            return value;
        }
    };
    struct URIContent : public Content {
        std::u16string value;
        URIContent(const std::u16string& value) :
            value(value) {
        }
        virtual std::u16string getCssText() {
            return u"url(" + CSSSerializeString(value) + u')';
        }
    };
    struct CounterContent : public Content {
        std::u16string identifier;
        std::u16string string;
        bool nested;
        CSSListStyleTypeValueImp listStyleType;
        CounterContent(const std::u16string& identifier, unsigned listStyleType) :
            identifier(identifier),
            nested(false),
            listStyleType(listStyleType) {
        }
        CounterContent(const std::u16string& identifier, const std::u16string& string, unsigned listStyleType) :
            identifier(identifier),
            string(string),
            nested(true),
            listStyleType(listStyleType) {
        }
        virtual std::u16string getCssText() {
            if (nested)
                return u"counters(" +  CSSSerializeIdentifier(identifier) + u", " + CSSSerializeString(string) + u", " + listStyleType.getCssText() + u')';
            return u"counter(" +  CSSSerializeIdentifier(identifier) + u", " + listStyleType.getCssText() + u')';
        }
        virtual std::u16string eval(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context);
    };
    struct AttrContent : public Content {
        std::u16string identifier;
        AttrContent(const std::u16string& value) :
            identifier(value) {
        }
        virtual std::u16string getCssText() {
            return u"attr(" + CSSSerializeIdentifier(identifier) + u')';
        }
    };
    struct QuoteContent : public Content {
        unsigned value;
        QuoteContent(unsigned value) :
            value(value) {
        }
        virtual std::u16string getCssText() {
            return CSSContentValueImp::Options[value];
        }
    };

    void clearContents() {
        while (!contents.empty()) {
            Content* content = contents.front();
            delete content;
            contents.pop_front();
        }
    }

protected:
    unsigned value;  // Normal or None; ignore this value if contents is not empty.
    std::list<Content*> contents;

public:
    ~CSSContentValueImp() {
        clearContents();
    }
    void reset() {
        value = Normal;
        clearContents();
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);

    bool isNone() const {
        return contents.empty() && value == None;
    }
    bool isNormal() const {
        return contents.empty() && value == Normal;
    }

    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(const CSSContentValueImp& specified) {
        value = specified.value;
        contents = specified.contents;
    }

    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    Element eval(ViewCSSImp* view, Element element, CSSAutoNumberingValueImp::CounterContext* context);

    CSSContentValueImp(unsigned initial = Normal) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSDirectionValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Ltr,
        Rtl
    };
    CSSDirectionValueImp& setValue(unsigned value = Ltr) {
        this->value = value;
        return *this;
    }
    CSSDirectionValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSDirectionValueImp& specified) {
        value = specified.value;
    }
    CSSDirectionValueImp(unsigned initial = Ltr) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSDisplayValueImp : public CSSPropertyValueImp
{
    unsigned original;  // retains the original value for calculating the static position of an absolutely positioned box.
    unsigned value;
public:
    enum {
        Inline,
        Block,
        ListItem,
        RunIn,
        InlineBlock,
        Table,
        InlineTable,
        TableRowGroup,
        TableHeaderGroup,
        TableFooterGroup,
        TableRow,
        TableColumnGroup,
        TableColumn,
        TableCell,
        TableCaption,
        None
    };
    CSSDisplayValueImp& setValue(unsigned value = Inline) {
        original = this->value = value;
        return *this;
    }
    CSSDisplayValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    unsigned getOriginalValue() const {
        return original;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    bool operator==(const CSSDisplayValueImp& display) const {
        return value == display.value;
    }
    bool operator!=(const CSSDisplayValueImp& display) const {
        return value != display.value;
    }
    void specify(const CSSDisplayValueImp& specified) {
        setValue(specified.value);
    }
    void compute(CSSStyleDeclarationImp* decl, Element element);
    CSSDisplayValueImp(unsigned initial = Inline) :
        original(initial),
        value(initial)
    {
    }
    bool isBlockLevel() const {
        return isBlockLevel(value);
    }
    bool isInlineBlock() const {
        switch (value) {
        case InlineBlock:
        case InlineTable:
            return true;
        default:
            return false;
        }
    }
    bool isInlineLevel() const {
        switch (value) {
        case Inline:
        case InlineBlock:
        case InlineTable:
        // Ruby:
            return true;
        default:
            return false;
        }
    }
    bool isInline() const {
        return value == Inline;
    }
    bool isRunIn() const {
        return value == RunIn;
    }
    bool isNone() const {
        return value == None;
    }
    bool isListItem() const {
        return value == ListItem;
    }
    bool isTableParts() const {
        switch (value) {
        case Table:
        case InlineTable:
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
        case TableRow:
        case TableColumnGroup:
        case TableColumn:
        case TableCell:
        case TableCaption:
            return true;
        default:
            return false;
        }
    }
    bool isRowGoupBox() const {
        switch (value) {
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
            return true;
        default:
            return false;
        }
    }
    bool isProperTableChild() const {
        switch (value) {
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
        case TableRow:
        case TableColumnGroup:
        case TableColumn:
        case TableCaption:
            return true;
        default:
            return false;
        }
    }
    bool isProperTableRowParent() const {
        switch (value) {
        case Table:
        case InlineTable:
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
            return true;
        default:
            return false;
        }
    }
    bool isInternalTableBox() const {
        switch (value) {
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
        case TableRow:
        case TableColumnGroup:
        case TableColumn:
        case TableCell:
            return true;
        default:
            return false;
        }
    }
    bool isTabularContainer() const {
        switch (value) {
        case Table:
        case InlineTable:
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
        case TableRow:
            return true;
        default:
            return false;
        }
    }
    bool isFlowRoot() const {
        switch (value) {
        case TableCell:
        case TableCaption:
        case InlineBlock:
        case InlineTable:
            return true;
        default:
            return false;
        }
    }

    static const char16_t* Options[];

    static bool isBlockLevel(unsigned value) {
        switch (value) {
        case Block:
        case ListItem:
        case Table:
        case TableRowGroup:
        case TableHeaderGroup:
        case TableFooterGroup:
        case TableRow:
        case TableColumnGroup:
        case TableColumn:
        case TableCell:
        case TableCaption:
        // <template>:
            return true;
        default:
            return false;
        }
    }
};

class CSSFloatValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Left,
        Right
    };
    CSSFloatValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    virtual CSSFloatValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    bool operator==(const CSSFloatValueImp& n) {
        return value == n.value;
    }
    bool operator!=(const CSSFloatValueImp& n) {
        return value != n.value;
    }
    void specify(const CSSFloatValueImp& specified) {
        value = specified.value;
    }
    CSSFloatValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSFontFamilyValueImp : public CSSPropertyValueImp
{
    unsigned generic;
    std::list<std::u16string> familyNames;
public:
    enum {
        None,
        Serif,
        SansSerif,
        Cursive,
        Fantasy,
        Monospace
    };
    void reset() {
        generic = None;
        familyNames.clear();
    }
    void setGeneric(unsigned generic) {
        this->generic = generic;
    }
    void addFamily(const std::u16string name) {
        familyNames.push_back(name);
    }
    std::deque<CSSParserTerm*>::iterator setValue(std::deque<CSSParserTerm*>& stack, std::deque<CSSParserTerm*>::iterator i);
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(const CSSFontFamilyValueImp& specified) {
        generic = specified.generic;
        familyNames = specified.familyNames;
    }
    unsigned getGeneric() const {
        return generic;
    }
    const std::list<std::u16string>& getFamilyNames() const {
        return familyNames;
    }
    CSSFontFamilyValueImp() :
        generic(None) {
    }
    static const char16_t* Options[];
};

class CSSFontSizeValueImp : public CSSPropertyValueImp
{
    CSSNumericValue size;
public:
    enum {
        XxSmall,
        XSmall,
        Small,
        Medium,
        Large,
        XLarge,
        XxLarge,
        Larger,
        Smaller
    };
    CSSFontSizeValueImp& setValue(float number, unsigned short unit) {
        size.setValue(number, unit);
        return *this;
    }
    virtual CSSFontSizeValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            size.setValue(term);
        else
            size.setIndex(Medium);
        return *this;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return size.getCssText(Options);
    }
    void specify(const CSSFontSizeValueImp& specified) {
        size.specify(specified.size);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle);
    float getPx() const {
        return size.getPx();
    }
    CSSFontSizeValueImp() :
        size(Medium) {
    }
    static const char16_t* Options[];
};

class CSSFontStyleValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Normal,
        Italic,
        Oblique
    };
    CSSFontStyleValueImp& setValue(unsigned value = Normal) {
        this->value = value;
        return *this;
    }
    CSSFontStyleValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSFontStyleValueImp& specified) {
        value = specified.value;
    }
    unsigned getStyle() const {
        return value;
    }
    bool isNormal() const {
        return value == Normal;
    }
    CSSFontStyleValueImp(unsigned initial = Normal) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSFontVariantValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Normal,
        SmallCaps
    };
    CSSFontVariantValueImp& setValue(unsigned value = Normal) {
        this->value = value;
        return *this;
    }
    CSSFontVariantValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSFontVariantValueImp& specified) {
        value = specified.value;
    }
    unsigned getStyle() const {
        return value;
    }
    bool isNormal() const {
        return value == Normal;
    }
    CSSFontVariantValueImp(unsigned initial = Normal) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSFontWeightValueImp : public CSSPropertyValueImp
{
    CSSNumericValue value;
public:
    enum {
        Normal,
        Bold,
        Bolder,
        Lighter
    };
    CSSFontWeightValueImp& setValue(unsigned weight) {
        value.setValue(static_cast<float>(weight), css::CSSPrimitiveValue::CSS_NUMBER);
        return *this;
    }
    CSSFontWeightValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            value.setValue(term);
        else
            value.setIndex(Normal);
        return *this;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return value.getCssText(Options);
    }
    void specify(const CSSFontWeightValueImp& specified) {
        value = specified.value;
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle);
    unsigned getWeight() const {
        assert(value.unit == css::CSSPrimitiveValue::CSS_NUMBER);
        return static_cast<unsigned>(value.number);
    }
    bool isNormal() const {
        if (value.getIndex() == Normal)
            return true;
        if (value.unit == css::CSSPrimitiveValue::CSS_NUMBER && value.number == 400.0f)
            return true;
        return false;
    }
    CSSFontWeightValueImp() :
        value(Normal) {
    }
    static const char16_t* Options[];
};

class CSSFontShorthandImp : public CSSPropertyValueImp
{
    unsigned index;
public:
    enum {
        Normal,
        Caption,
        Icon,
        Menu,
        MessageBox,
        SmallCaption,
        StatusBar
    };
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    void reset(CSSStyleDeclarationImp* self);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
    CSSFontShorthandImp() :
        index(Normal) {
    }
    static const char16_t* Options[];
};

// normal | <number> | <length> | <percentage>
class CSSLineHeightValueImp : public CSSPropertyValueImp
{
    CSSNumericValue computed;
protected:
    CSSNumericValue value;
public:
    enum {
        Normal
    };
    CSSLineHeightValueImp& setValue(float number, unsigned short unit) {
        value.setValue(number, unit);
        return *this;
    }
    CSSLineHeightValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            value.setValue(term);
        else
            value.setIndex(Normal);
        computed = value;
        return *this;
    }
    bool isNormal() const {
        return value.getIndex() == Normal;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (isNormal())
            return u"normal";
        return value.getCssText();
    }
    bool operator==(const CSSLineHeightValueImp& lineHeight) {
        return value == lineHeight.value;
    }
    bool operator!=(const CSSLineHeightValueImp& lineHeight) {
        return value != lineHeight.value;
    }
    void specify(const CSSLineHeightValueImp& specified) {
        value.specify(specified.computed);
        computed = value;
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    float getPx() const {
        return value.getPx();
    }
    CSSLineHeightValueImp() :
        value(Normal),
        computed(Normal)
    {
    }
};

class CSSListStyleImageValueImp : public CSSPropertyValueImp
{
    std::u16string uri;
public:
    enum {
        None
    };
    CSSListStyleImageValueImp& setValue(const std::u16string uri = u"") {
        this->uri = uri;
        return *this;
    }
    CSSListStyleImageValueImp& setValue(CSSParserTerm* term) {
        if (0 <= term->getIndex())
            return setValue();
        else
            return setValue(term->getString());
    }
    std::u16string getValue() const {
        return uri;
    }
    bool isNone() const {
        return uri.length() == 0;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (isNone())
            return u"none";
        return u"url(" + CSSSerializeString(uri) + u')';
    }
    void specify(const CSSListStyleImageValueImp& specified) {
        uri = specified.uri;
    }
    CSSListStyleImageValueImp() {
    }
};

class CSSListStylePositionValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Inside,
        Outside,
    };
    unsigned getValue() const {
        return value;
    }
    CSSListStylePositionValueImp& setValue(unsigned value = Outside) {
        this->value = value;
        return *this;
    }
    CSSListStylePositionValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSListStylePositionValueImp& specified) {
        value = specified.value;
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    CSSListStylePositionValueImp(unsigned initial = Outside) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSListStyleShorthandImp : public CSSPropertyValueImp
{
public:
    enum {
        None
    };
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    void reset(CSSStyleDeclarationImp* self);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSOverflowValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Visible,
        Hidden,
        Scroll,
        Auto
    };
    CSSOverflowValueImp& setValue(unsigned value = Visible) {
        this->value = value;
        return *this;
    }
    CSSOverflowValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    bool isClipped() const {
        return value != Visible;
    }
    bool canScroll() const {
        return canScroll(value);
    }
    void specify(const CSSOverflowValueImp& specified) {
        value = specified.value;
    }
    CSSOverflowValueImp(unsigned initial = Visible) :
        value(initial) {
    }
    static const char16_t* Options[];

    static bool canScroll(unsigned value) {
        return value == Scroll || value == Auto;
    }
};

class CSSPageBreakValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Auto,
        Always,
        Avoid,
        Left,
        Right
    };
    CSSPageBreakValueImp& setValue(unsigned value = Auto) {
        this->value = value;
        return *this;
    }
    CSSPageBreakValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSPageBreakValueImp& specified) {
        value = specified.value;
    }
    CSSPageBreakValueImp(unsigned initial = Auto) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSMarginShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSPaddingShorthandImp : public CSSPropertyValueImp
{
public:
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser);
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl);
    void specify(CSSStyleDeclarationImp* self, const CSSStyleDeclarationImp* decl);
};

class CSSPositionValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Static,
        Relative,
        Absolute,
        Fixed
    };
    CSSPositionValueImp& setValue(unsigned value = Static) {
        this->value = value;
        return *this;
    }
    CSSPositionValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    bool operator==(const CSSPositionValueImp& positon) const {
        return value == positon.value;
    }
    bool operator!=(const CSSPositionValueImp& positon) const {
        return value != positon.value;
    }
    void specify(const CSSPositionValueImp& specified) {
        value = specified.value;
    }
    CSSPositionValueImp(unsigned initial = Static) :
        value(initial) {
    }
    bool isStatic() const {
        return value == Static;
    }
    bool isRelative() const {
        return value == Relative;
    }
    bool isAbsolute() const {
        return value == Absolute;
    }
    bool isFixed() const {
        return value == Fixed;
    }
    static const char16_t* Options[];
};

class CSSTableLayoutValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Auto,
        Fixed
    };
    unsigned getValue() const {
        return value;
    }
    CSSTableLayoutValueImp& setValue(unsigned value = Auto) {
        this->value = value;
        return *this;
    }
    CSSTableLayoutValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSTableLayoutValueImp& specified) {
        value = specified.value;
    }
    CSSTableLayoutValueImp(unsigned initial = Auto) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSTextAlignValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Left,
        Right,
        Center,
        Justify,
        Default  // Left if direction is 'ltr', Right if direction is 'rtl'
    };
    unsigned getValue() const {
        return value;
    }
    CSSTextAlignValueImp& setValue(unsigned value = Default) {
        this->value = value;
        return *this;
    }
    CSSTextAlignValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSTextAlignValueImp& specified) {
        value = specified.value;
    }
    CSSTextAlignValueImp(unsigned initial = Default) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSTextDecorationValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None = 0,
        Underline = 1,
        Overline = 2,
        LineThrough = 4,
        Blink = 8
    };
    CSSTextDecorationValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    virtual bool setValue(CSSStyleDeclarationImp* decl, CSSValueParser* parser) {
        unsigned decoration = None;
        std::deque<CSSParserTerm*>& stack = parser->getStack();
        for (auto i = stack.begin(); i != stack.end(); ++i)
            decoration |= (*i)->getIndex();
        setValue(decoration);
        return true;
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (value == 0)
            return Options[0];
        std::u16string cssText;
        for (int i = 0; i < 4; ++i) {
            if (value & (1u << i)) {
                if (0 < cssText.length())
                    cssText += u' ';
                cssText += Options[i + 1];
            }
        }
        return cssText;
    }
    void specify(const CSSTextDecorationValueImp& specified) {
        value = specified.value;
    }
    CSSTextDecorationValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSTextTransformValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Capitalize,
        Uppercase,
        Lowercase
    };
    CSSTextTransformValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    CSSTextTransformValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSTextTransformValueImp& specified) {
        value = specified.value;
    }
    CSSTextTransformValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSUnicodeBidiValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Normal,
        Embed,
        BidiOverride
    };
    CSSUnicodeBidiValueImp& setValue(unsigned value = Normal) {
        this->value = value;
        return *this;
    }
    CSSUnicodeBidiValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSUnicodeBidiValueImp& specified) {
        value = specified.value;
    }
    CSSUnicodeBidiValueImp(unsigned initial = Normal) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSVerticalAlignValueImp : public CSSPropertyValueImp
{
    CSSNumericValue value;
public:
    enum {
        Baseline,
        Sub,
        Super,
        Top,
        TextTop,
        Middle,
        Bottom,
        TextBottom
    };
    CSSVerticalAlignValueImp& setValue(float number, unsigned short unit) {
        value.setValue(number, unit);
        return *this;
    }
    CSSVerticalAlignValueImp& setValue(CSSParserTerm* term = 0) {
        if (term)
            value.setValue(term);
        else
            value.setIndex(Baseline);
        return *this;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return value.getCssText(Options);
    }
    void specify(const CSSVerticalAlignValueImp& specified) {
        value.specify(specified.value);
    }
    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    void resolve(ViewCSSImp* view, CSSStyleDeclarationImp* style);
    float getOffset(LineBox* line, InlineLevelBox* text) const;
    CSSVerticalAlignValueImp() :
        value(Baseline) {
    }
    static const char16_t* Options[];
};

class CSSVisibilityValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Visible,
        Hidden,
        Collapse
    };
    CSSVisibilityValueImp& setValue(unsigned value = Visible) {
        this->value = value;
        return *this;
    }
    CSSVisibilityValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSVisibilityValueImp& specified) {
        value = specified.value;
    }
    CSSVisibilityValueImp(unsigned initial = Visible) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSWhiteSpaceValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        Normal,
        Pre,
        Nowrap,
        PreWrap,
        PreLine
    };
    CSSWhiteSpaceValueImp& setValue(unsigned value = Normal) {
        this->value = value;
        return *this;
    }
    CSSWhiteSpaceValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSWhiteSpaceValueImp& specified) {
        value = specified.value;
    }
    bool isCollapsingSpace() const {
        switch (value) {
        case Normal:
        case Nowrap:
        case PreLine:
            return true;
        default:
            return false;
        }
    }
    bool isBreakingLines() const {
        switch (value) {
        case Normal:
        case PreWrap:
        case PreLine:
            return true;
        default:
            return false;
        }
    }
    CSSWhiteSpaceValueImp(unsigned initial = Normal) :
        value(initial) {
    }
    static const char16_t* Options[];
};

class CSSZIndexValueImp : public CSSPropertyValueImp
{
    bool auto_;
    int index;
public:
    CSSZIndexValueImp& setValue(bool auto_ = true, int index = 0) {
        this->auto_ = auto_;
        if (auto_)
            this->index = 0;
        else
            this->index = index;
        return *this;
    }
    CSSZIndexValueImp& setValue(CSSParserTerm* term) {
        if (0 <= term->getIndex())
            return setValue(true);
        double value = term->getNumber();
        if (fmod(value, 1.0) != 0.0)
            return setValue(true);
        return setValue(false, static_cast<int>(value));
    }
    bool isAuto() const {
        return auto_;
    }
    int getValue() const {
        return isAuto() ? 0 : index;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        if (auto_)
            return u"auto";
        return CSSSerializeInteger(index);
    }
    bool operator==(const CSSZIndexValueImp& value) {
        return auto_ == value.auto_ && index == value.index;
    }
    bool operator!=(const CSSZIndexValueImp& value) {
        return auto_ != value.auto_ || index != value.index;
    }
    void specify(const CSSZIndexValueImp& specified) {
        auto_ = specified.auto_;
        index = specified.index;
    }
    CSSZIndexValueImp() :
        auto_(true),
        index(0) {
    }
};

class CSSBindingValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Button,
        Details,  // block
        InputTextfield,
        InputPassword,
        InputDatetime,
        InputDate,
        InputMonth,
        InputWeek,
        InputTime,
        InputDatetimeLocal,
        InputNumber,
        InputRange,
        InputColor,
        InputCheckbox,
        InputRadio,
        InputFile,
        InputButton,
        Marquee,
        Meter,
        Progress,
        Select,  // multi-select list box
        Textarea,
        Keygen,
        Time,  // inline?
    };
    CSSBindingValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    CSSBindingValueImp& setValue(CSSParserTerm* term) {
        return setValue(term->getIndex());
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl) {
        return Options[value];
    }
    void specify(const CSSBindingValueImp& specified) {
        value = specified.value;
    }
    CSSBindingValueImp(unsigned initial = None) :
        value(initial) {
    }
    static const char16_t* Options[];

    bool isBlockLevel() const {
        switch (value) {
        case Details:
            return true;
        default:
            return false;
        }
    }
    bool isInlineBlock() const {
        switch (value) {
        case Button:
        case InputTextfield:
        case InputPassword:
        case InputDatetime:
        case InputDate:
        case InputMonth:
        case InputWeek:
        case InputTime:
        case InputDatetimeLocal:
        case InputNumber:
        case InputRange:
        case InputColor:
        case InputCheckbox:
        case InputRadio:
        case InputFile:
        case InputButton:
        case Marquee:
        case Meter:
        case Progress:
        case Select:  // multi-select list box
        case Textarea:
        case Keygen:
            return true;
            break;
        default:
            return false;
        }
    }
};

class HTMLAlignValueImp : public CSSPropertyValueImp
{
    unsigned value;
public:
    enum {
        None,
        Left,
        Center,
        Right
    };
    HTMLAlignValueImp& setValue(unsigned value = None) {
        this->value = value;
        return *this;
    }
    unsigned getValue() const {
        return value;
    }
    virtual std::u16string getCssText(CSSStyleDeclarationImp* decl = 0) {
        return u"";
    }
    bool operator==(const HTMLAlignValueImp& style) const {
        return value == style.value;
    }
    bool operator!=(const HTMLAlignValueImp& style) const {
        return value != style.value;
    }
    void specify(const HTMLAlignValueImp& specified) {
        value = specified.value;
    }
    HTMLAlignValueImp(unsigned initial = None) :
        value(initial)
    {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CSSSPROPERTYVALUE_IMP_H
