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

#ifndef CSSSTYLEDECLARATION_IMP_H
#define CSSSTYLEDECLARATION_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSStyleDeclaration.h>

#include <org/w3c/dom/css/CSSRule.h>
#include <org/w3c/dom/css/CSSStyleDeclarationValue.h>

#include <bitset>
#include <map>
#include <boost/intrusive_ptr.hpp>

#include "CSSParser.h"
#include "CSSPropertyValueImp.h"
#include "CSSRuleImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class CSSStyleDeclarationImp;

typedef boost::intrusive_ptr<CSSStyleDeclarationImp> CSSStyleDeclarationPtr;

class CSSStyleDeclarationImp : public ObjectMixin<CSSStyleDeclarationImp>
{
    friend class CSSBackgroundShorthandImp;
    friend class CSSBorderColorShorthandImp;
    friend class CSSBorderStyleShorthandImp;
    friend class CSSBorderWidthShorthandImp;
    friend class CSSBorderShorthandImp;
    friend class CSSDisplayValueImp;
    friend class CSSMarginShorthandImp;
    friend class CSSPaddingShorthandImp;
    friend class ViewCSSImp;

    friend class BlockLevelBox;

    static const size_t PropertyCount = 122 + 1;  // including Unknown
    static const char16_t* PropertyNames[PropertyCount];
public:
    enum
    {
        Unknown = 0,
        Azimuth,    // "azimuth"
        Background, // "background"
        BackgroundAttachment,   // "background-attachment"
        BackgroundColor,    // "background-color"
        BackgroundImage,    // "background-image"
        BackgroundPosition, // "background-position"
        BackgroundRepeat,   // "background-repeat"
        Border, // "border"
        BorderCollapse, // "border-collapse"
        BorderColor,    // "border-color"
        BorderSpacing,  // "border-spacing"
        BorderStyle,    // "border-style"
        BorderTop,  // "border-top"
        BorderRight,    // "border-right"
        BorderBottom,   // "border-bottom"
        BorderLeft, // "border-left"
        BorderTopColor, // "border-top-color"
        BorderRightColor,   // "border-right-color"
        BorderBottomColor,  // "border-bottom-color"
        BorderLeftColor,    // "border-left-color"
        BorderTopStyle, // "border-top-style"
        BorderRightStyle,   // "border-right-style"
        BorderBottomStyle,  // "border-bottom-style"
        BorderLeftStyle,    // "border-left-style"
        BorderTopWidth, // "border-top-width"
        BorderRightWidth,   // "border-right-width"
        BorderBottomWidth,  // "border-bottom-width"
        BorderLeftWidth,    // "border-left-width"
        BorderWidth,    // "border-width"
        Bottom, // "bottom"
        CaptionSide,    // "caption-side"
        Clear,  // "clear"
        Clip,   // "clip"
        Color,  // "color"
        Content,    // "content"
        CounterIncrement,   // "counter-increment"
        CounterReset,   // "counter-reset"
        Cue,    // "cue"
        CueAfter,   // "cue-after"
        CueBefore,  // "cue-before"
        Cursor, // "cursor"
        Direction,  // "direction"
        Display,    // "display"
        Elevation,  // "elevation"
        EmptyCells, // "empty-cells"
        Float,   // "float"
        Font,   // "font"
        FontFamily, // "font-family"
        FontSize,   // "font-size"
        FontSizeAdjust, // "font-size-adjust"
        FontStretch,    // "font-stretch"
        FontStyle,  // "font-style"
        FontVariant,    // "font-variant"
        FontWeight, // "font-weight"
        Height, // "height"
        Left,   // "left"
        LetterSpacing,  // "letter-spacing"
        LineHeight, // "line-height"
        ListStyle,  // "list-style"
        ListStyleImage, // "list-style-image"
        ListStylePosition,  // "list-style-position"
        ListStyleType,  // "list-style-type"
        Margin, // "margin"
        MarginTop,  // "margin-top"
        MarginRight,    // "margin-right"
        MarginBottom,   // "margin-bottom"
        MarginLeft, // "margin-left"
        MarkerOffset,   // "marker-offset"
        Marks,  // "marks"
        MaxHeight,  // "max-height"
        MaxWidth,   // "max-width"
        MinHeight,  // "min-height"
        MinWidth,   // "min-width"
        Orphans,    // "orphans"
        Outline,    // "outline"
        OutlineColor,   // "outline-color"
        OutlineStyle,   // "outline-style"
        OutlineWidth,   // "outline-width"
        Overflow,   // "overflow"
        Padding,    // "padding"
        PaddingTop, // "padding-top"
        PaddingRight,   // "padding-right"
        PaddingBottom,  // "padding-bottom"
        PaddingLeft,    // "padding-left"
        Page,   // "page"
        PageBreakAfter, // "page-break-after"
        PageBreakBefore,    // "page-break-before"
        PageBreakInside,    // "page-break-inside"
        Pause,  // "pause"
        PauseAfter, // "pause-after"
        PauseBefore,    // "pause-before"
        Pitch,  // "pitch"
        PitchRange, // "pitch-range"
        PlayDuring, // "play-during"
        Position,   // "position"
        Quotes, // "quotes"
        Richness,   // "richness"
        Right,  // "right"
        Size,   // "size"
        Speak,  // "speak"
        SpeakHeader,    // "speak-header"
        SpeakNumeral,   // "speak-numeral"
        SpeakPunctuation,   // "speak-punctuation"
        SpeechRate, // "speech-rate"
        Stress, // "stress"
        TableLayout,    // "table-layout"
        TextAlign,  // "text-align"
        TextDecoration, // "text-decoration"
        TextIndent, // "text-indent"
        TextShadow, // "text-shadow"
        TextTransform,  // "text-transform"
        Top,    // "top"
        UnicodeBidi,    // "unicode-bidi"
        VerticalAlign,  // "vertical-align"
        Visibility, // "visibility"
        VoiceFamily,    // "voice-family"
        Volume, // "volume"
        WhiteSpace, // "white-space"
        Widows, // "widows"
        Width,  // "width"
        WordSpacing,    // "word-spacing"
        ZIndex, // "z-index"

        MaxProperties
    };

    // Pseudo-element index
    enum
    {
        NonPseudo,
        FirstLine,
        FirstLetter,
        Before,
        After,

        MaxPseudoElements
    };

    static int getPseudoElementID(const std::u16string& name);
    static int getPseudoClassID(const std::u16string& name);
    static const char16_t* getPseudoClassName(int id);
    static const char16_t* getPseudoElementName(int id);

private:
    css::CSSRule parentRule;
    std::bitset<PropertyCount> propertySet;
    std::bitset<PropertyCount> importantSet;
    std::bitset<PropertyCount> inheritSet;
    bool resolved;

    Box* box;
    Box* lastBox;  // for inline

    CSSStyleDeclarationPtr pseudoElements[MaxPseudoElements];

    void specify(CSSStyleDeclarationImp* decl, unsigned id);
    void specify(CSSStyleDeclarationImp* decl, const std::bitset<PropertyCount>& set);

    void setInherit(unsigned id);
    void resetInherit(unsigned id);
    void setImportant(unsigned id);
    void resetImportant(unsigned id);
    void setProperty(unsigned id);
    void resetProperty(unsigned id);

public:
    // property values
    CSSBackgroundAttachmentValueImp backgroundAttachment;
    CSSColorValueImp backgroundColor;
    CSSBackgroundImageValueImp backgroundImage;
    CSSBackgroundPositionValueImp backgroundPosition;
    CSSBackgroundRepeatValueImp backgroundRepeat;
    CSSBackgroundShorthandImp background;
    CSSBorderCollapseValueImp borderCollapse;
    CSSBorderColorShorthandImp borderColor;
    CSSBorderSpacingValueImp borderSpacing;
    CSSBorderStyleShorthandImp borderStyle;

    CSSColorValueImp borderTopColor;
    CSSColorValueImp borderRightColor;
    CSSColorValueImp borderBottomColor;
    CSSColorValueImp borderLeftColor;
    CSSBorderStyleValueImp borderTopStyle;
    CSSBorderStyleValueImp borderRightStyle;
    CSSBorderStyleValueImp borderBottomStyle;
    CSSBorderStyleValueImp borderLeftStyle;
    CSSBorderWidthValueImp borderTopWidth;
    CSSBorderWidthValueImp borderRightWidth;
    CSSBorderWidthValueImp borderBottomWidth;
    CSSBorderWidthValueImp borderLeftWidth;
    CSSBorderWidthShorthandImp borderWidth;
    CSSBorderShorthandImp border;
    CSSAutoLengthValueImp bottom;

    CSSClearValueImp clear;

    CSSColorValueImp color;
    CSSContentValueImp content;
    CSSAutoNumberingValueImp counterIncrement;
    CSSAutoNumberingValueImp counterReset;

    CSSDirectionValueImp direction;
    CSSDisplayValueImp display;

    CSSFloatValueImp float_;
    CSSFontFamilyValueImp fontFamily;
    CSSFontSizeValueImp fontSize;
    CSSFontStyleValueImp fontStyle;
    CSSFontWeightValueImp fontWeight;

    CSSAutoLengthValueImp height;
    CSSAutoLengthValueImp left;

    CSSLineHeightValueImp lineHeight;

    CSSListStyleTypeValueImp listStyleType;
    CSSAutoLengthValueImp marginRight;
    CSSAutoLengthValueImp marginLeft;
    CSSAutoLengthValueImp marginTop;
    CSSAutoLengthValueImp marginBottom;
    CSSMarginShorthandImp margin;
    CSSNoneLengthValueImp maxHeight;
    CSSNoneLengthValueImp maxWidth;
    CSSNumericValueImp minHeight;
    CSSNumericValueImp minWidth;

    CSSOverflowValueImp overflow;
    CSSNumericValueImp paddingTop;
    CSSNumericValueImp paddingRight;
    CSSNumericValueImp paddingBottom;
    CSSNumericValueImp paddingLeft;
    CSSPaddingShorthandImp padding;
    CSSPageBreakValueImp pageBreakAfter;
    CSSPageBreakValueImp pageBreakBefore;
    CSSPageBreakValueImp pageBreakInside;

    CSSPositionValueImp position;

    CSSAutoLengthValueImp right;

    CSSTextAlignValueImp textAlign;
    CSSTextDecorationValueImp textDecoration;
    CSSNumericValueImp textIndent;

    CSSAutoLengthValueImp top;
    CSSUnicodeBidiValueImp unicodeBidi;
    CSSVerticalAlignValueImp verticalAlign;
    CSSVisibilityValueImp visibility;

    CSSWhiteSpaceValueImp whiteSpace;

    CSSAutoLengthValueImp width;

    CSSZIndexValueImp zIndex;

public:
    CSSStyleDeclarationImp();
    ~CSSStyleDeclarationImp()
    {
        ;  //
    }

    void setParentRule(css::CSSRule parentRule) {
        this->parentRule = parentRule;
    }
    bool setProperty(std::u16string property, CSSParserExpr* expr, const std::u16string& prio = u"");
    void setColor(unsigned color) {
        this->color = color;
    }

    CSSPropertyValueImp* getProperty(unsigned id);

    CSSStyleDeclarationImp* getPseudoElementStyle(int id) {
        assert(0 <= id && id < MaxPseudoElements);
        return pseudoElements[id].get();
    }
    CSSStyleDeclarationImp* getPseudoElementStyle(const std::u16string& name) {
        int id = getPseudoElementID(name);
        if (0 <= id)
            return pseudoElements[id].get();
        return 0;
    }

    CSSStyleDeclarationImp* createPseudoElementStyle(int id);

    void specify(CSSStyleDeclarationImp* style);
    void specifyImportant(CSSStyleDeclarationImp* style);

    void reset(unsigned id);
    void resetInheritedProperties();

    void copy(CSSStyleDeclarationImp* parentStyle, unsigned id);
    void copyInheritedProperties(CSSStyleDeclarationImp* parentStyle);

    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle, Element element);

    void resolve(ViewCSSImp* view, const ContainingBlock* containingBlock, Element element);

    size_t processWhiteSpace(std::u16string& data, char16_t& prevChar);
    size_t processLineHeadWhiteSpace(std::u16string& data);

    bool isFlowRoot() const;

    bool isFloat() const {
        return float_.getValue() != CSSFloatValueImp::None;
    }
    bool isAbsolutelyPositioned() const {
        return position.getValue() == CSSPositionValueImp::Absolute ||
               position.getValue() == CSSPositionValueImp::Fixed;
    }

    void addBox(Box* box) {
        if (!this->box)
            this->box = lastBox = box;
        else
            lastBox = box;
    }

    static int getPropertyID(const std::u16string& ident);
    static const char16_t* getPropertyName(int propertyID);

    // CSSStyleDeclaration
    virtual std::u16string getCssText() __attribute__((weak));
    virtual void setCssText(std::u16string cssText) __attribute__((weak));
    virtual unsigned int getLength() __attribute__((weak));
    virtual std::u16string item(unsigned int index) __attribute__((weak));
    virtual std::u16string getPropertyValue(std::u16string property) __attribute__((weak));
    virtual std::u16string getPropertyPriority(std::u16string property) __attribute__((weak));
    virtual void setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value) __attribute__((weak));
    virtual void setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value, Nullable<std::u16string> priority) __attribute__((weak));
    virtual std::u16string removeProperty(std::u16string property) __attribute__((weak));
    virtual css::CSSStyleDeclarationValue getValues() __attribute__((weak));
    virtual css::CSSRule getParentRule() __attribute__((weak));
    // CSS2Properties
    virtual Nullable<std::u16string> getAzimuth() __attribute__((weak));
    virtual void setAzimuth(Nullable<std::u16string> azimuth) __attribute__((weak));
    virtual Nullable<std::u16string> getBackground() __attribute__((weak));
    virtual void setBackground(Nullable<std::u16string> background) __attribute__((weak));
    virtual Nullable<std::u16string> getBackgroundAttachment() __attribute__((weak));
    virtual void setBackgroundAttachment(Nullable<std::u16string> backgroundAttachment) __attribute__((weak));
    virtual Nullable<std::u16string> getBackgroundColor() __attribute__((weak));
    virtual void setBackgroundColor(Nullable<std::u16string> backgroundColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBackgroundImage() __attribute__((weak));
    virtual void setBackgroundImage(Nullable<std::u16string> backgroundImage) __attribute__((weak));
    virtual Nullable<std::u16string> getBackgroundPosition() __attribute__((weak));
    virtual void setBackgroundPosition(Nullable<std::u16string> backgroundPosition) __attribute__((weak));
    virtual Nullable<std::u16string> getBackgroundRepeat() __attribute__((weak));
    virtual void setBackgroundRepeat(Nullable<std::u16string> backgroundRepeat) __attribute__((weak));
    virtual Nullable<std::u16string> getBorder() __attribute__((weak));
    virtual void setBorder(Nullable<std::u16string> border) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderCollapse() __attribute__((weak));
    virtual void setBorderCollapse(Nullable<std::u16string> borderCollapse) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderColor() __attribute__((weak));
    virtual void setBorderColor(Nullable<std::u16string> borderColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderSpacing() __attribute__((weak));
    virtual void setBorderSpacing(Nullable<std::u16string> borderSpacing) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderStyle() __attribute__((weak));
    virtual void setBorderStyle(Nullable<std::u16string> borderStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderTop() __attribute__((weak));
    virtual void setBorderTop(Nullable<std::u16string> borderTop) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderRight() __attribute__((weak));
    virtual void setBorderRight(Nullable<std::u16string> borderRight) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderBottom() __attribute__((weak));
    virtual void setBorderBottom(Nullable<std::u16string> borderBottom) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderLeft() __attribute__((weak));
    virtual void setBorderLeft(Nullable<std::u16string> borderLeft) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderTopColor() __attribute__((weak));
    virtual void setBorderTopColor(Nullable<std::u16string> borderTopColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderRightColor() __attribute__((weak));
    virtual void setBorderRightColor(Nullable<std::u16string> borderRightColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderBottomColor() __attribute__((weak));
    virtual void setBorderBottomColor(Nullable<std::u16string> borderBottomColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderLeftColor() __attribute__((weak));
    virtual void setBorderLeftColor(Nullable<std::u16string> borderLeftColor) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderTopStyle() __attribute__((weak));
    virtual void setBorderTopStyle(Nullable<std::u16string> borderTopStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderRightStyle() __attribute__((weak));
    virtual void setBorderRightStyle(Nullable<std::u16string> borderRightStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderBottomStyle() __attribute__((weak));
    virtual void setBorderBottomStyle(Nullable<std::u16string> borderBottomStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderLeftStyle() __attribute__((weak));
    virtual void setBorderLeftStyle(Nullable<std::u16string> borderLeftStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderTopWidth() __attribute__((weak));
    virtual void setBorderTopWidth(Nullable<std::u16string> borderTopWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderRightWidth() __attribute__((weak));
    virtual void setBorderRightWidth(Nullable<std::u16string> borderRightWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderBottomWidth() __attribute__((weak));
    virtual void setBorderBottomWidth(Nullable<std::u16string> borderBottomWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderLeftWidth() __attribute__((weak));
    virtual void setBorderLeftWidth(Nullable<std::u16string> borderLeftWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getBorderWidth() __attribute__((weak));
    virtual void setBorderWidth(Nullable<std::u16string> borderWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getBottom() __attribute__((weak));
    virtual void setBottom(Nullable<std::u16string> bottom) __attribute__((weak));
    virtual Nullable<std::u16string> getCaptionSide() __attribute__((weak));
    virtual void setCaptionSide(Nullable<std::u16string> captionSide) __attribute__((weak));
    virtual Nullable<std::u16string> getClear() __attribute__((weak));
    virtual void setClear(Nullable<std::u16string> clear) __attribute__((weak));
    virtual Nullable<std::u16string> getClip() __attribute__((weak));
    virtual void setClip(Nullable<std::u16string> clip) __attribute__((weak));
    virtual Nullable<std::u16string> getColor() __attribute__((weak));
    virtual void setColor(Nullable<std::u16string> color) __attribute__((weak));
    virtual Nullable<std::u16string> getContent() __attribute__((weak));
    virtual void setContent(Nullable<std::u16string> content) __attribute__((weak));
    virtual Nullable<std::u16string> getCounterIncrement() __attribute__((weak));
    virtual void setCounterIncrement(Nullable<std::u16string> counterIncrement) __attribute__((weak));
    virtual Nullable<std::u16string> getCounterReset() __attribute__((weak));
    virtual void setCounterReset(Nullable<std::u16string> counterReset) __attribute__((weak));
    virtual Nullable<std::u16string> getCue() __attribute__((weak));
    virtual void setCue(Nullable<std::u16string> cue) __attribute__((weak));
    virtual Nullable<std::u16string> getCueAfter() __attribute__((weak));
    virtual void setCueAfter(Nullable<std::u16string> cueAfter) __attribute__((weak));
    virtual Nullable<std::u16string> getCueBefore() __attribute__((weak));
    virtual void setCueBefore(Nullable<std::u16string> cueBefore) __attribute__((weak));
    virtual Nullable<std::u16string> getCursor() __attribute__((weak));
    virtual void setCursor(Nullable<std::u16string> cursor) __attribute__((weak));
    virtual Nullable<std::u16string> getDirection() __attribute__((weak));
    virtual void setDirection(Nullable<std::u16string> direction) __attribute__((weak));
    virtual Nullable<std::u16string> getDisplay() __attribute__((weak));
    virtual void setDisplay(Nullable<std::u16string> display) __attribute__((weak));
    virtual Nullable<std::u16string> getElevation() __attribute__((weak));
    virtual void setElevation(Nullable<std::u16string> elevation) __attribute__((weak));
    virtual Nullable<std::u16string> getEmptyCells() __attribute__((weak));
    virtual void setEmptyCells(Nullable<std::u16string> emptyCells) __attribute__((weak));
    virtual Nullable<std::u16string> getCssFloat() __attribute__((weak));
    virtual void setCssFloat(Nullable<std::u16string> cssFloat) __attribute__((weak));
    virtual Nullable<std::u16string> getFont() __attribute__((weak));
    virtual void setFont(Nullable<std::u16string> font) __attribute__((weak));
    virtual Nullable<std::u16string> getFontFamily() __attribute__((weak));
    virtual void setFontFamily(Nullable<std::u16string> fontFamily) __attribute__((weak));
    virtual Nullable<std::u16string> getFontSize() __attribute__((weak));
    virtual void setFontSize(Nullable<std::u16string> fontSize) __attribute__((weak));
    virtual Nullable<std::u16string> getFontSizeAdjust() __attribute__((weak));
    virtual void setFontSizeAdjust(Nullable<std::u16string> fontSizeAdjust) __attribute__((weak));
    virtual Nullable<std::u16string> getFontStretch() __attribute__((weak));
    virtual void setFontStretch(Nullable<std::u16string> fontStretch) __attribute__((weak));
    virtual Nullable<std::u16string> getFontStyle() __attribute__((weak));
    virtual void setFontStyle(Nullable<std::u16string> fontStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getFontVariant() __attribute__((weak));
    virtual void setFontVariant(Nullable<std::u16string> fontVariant) __attribute__((weak));
    virtual Nullable<std::u16string> getFontWeight() __attribute__((weak));
    virtual void setFontWeight(Nullable<std::u16string> fontWeight) __attribute__((weak));
    virtual Nullable<std::u16string> getHeight() __attribute__((weak));
    virtual void setHeight(Nullable<std::u16string> height) __attribute__((weak));
    virtual Nullable<std::u16string> getLeft() __attribute__((weak));
    virtual void setLeft(Nullable<std::u16string> left) __attribute__((weak));
    virtual Nullable<std::u16string> getLetterSpacing() __attribute__((weak));
    virtual void setLetterSpacing(Nullable<std::u16string> letterSpacing) __attribute__((weak));
    virtual Nullable<std::u16string> getLineHeight() __attribute__((weak));
    virtual void setLineHeight(Nullable<std::u16string> lineHeight) __attribute__((weak));
    virtual Nullable<std::u16string> getListStyle() __attribute__((weak));
    virtual void setListStyle(Nullable<std::u16string> listStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getListStyleImage() __attribute__((weak));
    virtual void setListStyleImage(Nullable<std::u16string> listStyleImage) __attribute__((weak));
    virtual Nullable<std::u16string> getListStylePosition() __attribute__((weak));
    virtual void setListStylePosition(Nullable<std::u16string> listStylePosition) __attribute__((weak));
    virtual Nullable<std::u16string> getListStyleType() __attribute__((weak));
    virtual void setListStyleType(Nullable<std::u16string> listStyleType) __attribute__((weak));
    virtual Nullable<std::u16string> getMargin() __attribute__((weak));
    virtual void setMargin(Nullable<std::u16string> margin) __attribute__((weak));
    virtual Nullable<std::u16string> getMarginTop() __attribute__((weak));
    virtual void setMarginTop(Nullable<std::u16string> marginTop) __attribute__((weak));
    virtual Nullable<std::u16string> getMarginRight() __attribute__((weak));
    virtual void setMarginRight(Nullable<std::u16string> marginRight) __attribute__((weak));
    virtual Nullable<std::u16string> getMarginBottom() __attribute__((weak));
    virtual void setMarginBottom(Nullable<std::u16string> marginBottom) __attribute__((weak));
    virtual Nullable<std::u16string> getMarginLeft() __attribute__((weak));
    virtual void setMarginLeft(Nullable<std::u16string> marginLeft) __attribute__((weak));
    virtual Nullable<std::u16string> getMarkerOffset() __attribute__((weak));
    virtual void setMarkerOffset(Nullable<std::u16string> markerOffset) __attribute__((weak));
    virtual Nullable<std::u16string> getMarks() __attribute__((weak));
    virtual void setMarks(Nullable<std::u16string> marks) __attribute__((weak));
    virtual Nullable<std::u16string> getMaxHeight() __attribute__((weak));
    virtual void setMaxHeight(Nullable<std::u16string> maxHeight) __attribute__((weak));
    virtual Nullable<std::u16string> getMaxWidth() __attribute__((weak));
    virtual void setMaxWidth(Nullable<std::u16string> maxWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getMinHeight() __attribute__((weak));
    virtual void setMinHeight(Nullable<std::u16string> minHeight) __attribute__((weak));
    virtual Nullable<std::u16string> getMinWidth() __attribute__((weak));
    virtual void setMinWidth(Nullable<std::u16string> minWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getOrphans() __attribute__((weak));
    virtual void setOrphans(Nullable<std::u16string> orphans) __attribute__((weak));
    virtual Nullable<std::u16string> getOutline() __attribute__((weak));
    virtual void setOutline(Nullable<std::u16string> outline) __attribute__((weak));
    virtual Nullable<std::u16string> getOutlineColor() __attribute__((weak));
    virtual void setOutlineColor(Nullable<std::u16string> outlineColor) __attribute__((weak));
    virtual Nullable<std::u16string> getOutlineStyle() __attribute__((weak));
    virtual void setOutlineStyle(Nullable<std::u16string> outlineStyle) __attribute__((weak));
    virtual Nullable<std::u16string> getOutlineWidth() __attribute__((weak));
    virtual void setOutlineWidth(Nullable<std::u16string> outlineWidth) __attribute__((weak));
    virtual Nullable<std::u16string> getOverflow() __attribute__((weak));
    virtual void setOverflow(Nullable<std::u16string> overflow) __attribute__((weak));
    virtual Nullable<std::u16string> getPadding() __attribute__((weak));
    virtual void setPadding(Nullable<std::u16string> padding) __attribute__((weak));
    virtual Nullable<std::u16string> getPaddingTop() __attribute__((weak));
    virtual void setPaddingTop(Nullable<std::u16string> paddingTop) __attribute__((weak));
    virtual Nullable<std::u16string> getPaddingRight() __attribute__((weak));
    virtual void setPaddingRight(Nullable<std::u16string> paddingRight) __attribute__((weak));
    virtual Nullable<std::u16string> getPaddingBottom() __attribute__((weak));
    virtual void setPaddingBottom(Nullable<std::u16string> paddingBottom) __attribute__((weak));
    virtual Nullable<std::u16string> getPaddingLeft() __attribute__((weak));
    virtual void setPaddingLeft(Nullable<std::u16string> paddingLeft) __attribute__((weak));
    virtual Nullable<std::u16string> getPage() __attribute__((weak));
    virtual void setPage(Nullable<std::u16string> page) __attribute__((weak));
    virtual Nullable<std::u16string> getPageBreakAfter() __attribute__((weak));
    virtual void setPageBreakAfter(Nullable<std::u16string> pageBreakAfter) __attribute__((weak));
    virtual Nullable<std::u16string> getPageBreakBefore() __attribute__((weak));
    virtual void setPageBreakBefore(Nullable<std::u16string> pageBreakBefore) __attribute__((weak));
    virtual Nullable<std::u16string> getPageBreakInside() __attribute__((weak));
    virtual void setPageBreakInside(Nullable<std::u16string> pageBreakInside) __attribute__((weak));
    virtual Nullable<std::u16string> getPause() __attribute__((weak));
    virtual void setPause(Nullable<std::u16string> pause) __attribute__((weak));
    virtual Nullable<std::u16string> getPauseAfter() __attribute__((weak));
    virtual void setPauseAfter(Nullable<std::u16string> pauseAfter) __attribute__((weak));
    virtual Nullable<std::u16string> getPauseBefore() __attribute__((weak));
    virtual void setPauseBefore(Nullable<std::u16string> pauseBefore) __attribute__((weak));
    virtual Nullable<std::u16string> getPitch() __attribute__((weak));
    virtual void setPitch(Nullable<std::u16string> pitch) __attribute__((weak));
    virtual Nullable<std::u16string> getPitchRange() __attribute__((weak));
    virtual void setPitchRange(Nullable<std::u16string> pitchRange) __attribute__((weak));
    virtual Nullable<std::u16string> getPlayDuring() __attribute__((weak));
    virtual void setPlayDuring(Nullable<std::u16string> playDuring) __attribute__((weak));
    virtual Nullable<std::u16string> getPosition() __attribute__((weak));
    virtual void setPosition(Nullable<std::u16string> position) __attribute__((weak));
    virtual Nullable<std::u16string> getQuotes() __attribute__((weak));
    virtual void setQuotes(Nullable<std::u16string> quotes) __attribute__((weak));
    virtual Nullable<std::u16string> getRichness() __attribute__((weak));
    virtual void setRichness(Nullable<std::u16string> richness) __attribute__((weak));
    virtual Nullable<std::u16string> getRight() __attribute__((weak));
    virtual void setRight(Nullable<std::u16string> right) __attribute__((weak));
    virtual Nullable<std::u16string> getSize() __attribute__((weak));
    virtual void setSize(Nullable<std::u16string> size) __attribute__((weak));
    virtual Nullable<std::u16string> getSpeak() __attribute__((weak));
    virtual void setSpeak(Nullable<std::u16string> speak) __attribute__((weak));
    virtual Nullable<std::u16string> getSpeakHeader() __attribute__((weak));
    virtual void setSpeakHeader(Nullable<std::u16string> speakHeader) __attribute__((weak));
    virtual Nullable<std::u16string> getSpeakNumeral() __attribute__((weak));
    virtual void setSpeakNumeral(Nullable<std::u16string> speakNumeral) __attribute__((weak));
    virtual Nullable<std::u16string> getSpeakPunctuation() __attribute__((weak));
    virtual void setSpeakPunctuation(Nullable<std::u16string> speakPunctuation) __attribute__((weak));
    virtual Nullable<std::u16string> getSpeechRate() __attribute__((weak));
    virtual void setSpeechRate(Nullable<std::u16string> speechRate) __attribute__((weak));
    virtual Nullable<std::u16string> getStress() __attribute__((weak));
    virtual void setStress(Nullable<std::u16string> stress) __attribute__((weak));
    virtual Nullable<std::u16string> getTableLayout() __attribute__((weak));
    virtual void setTableLayout(Nullable<std::u16string> tableLayout) __attribute__((weak));
    virtual Nullable<std::u16string> getTextAlign() __attribute__((weak));
    virtual void setTextAlign(Nullable<std::u16string> textAlign) __attribute__((weak));
    virtual Nullable<std::u16string> getTextDecoration() __attribute__((weak));
    virtual void setTextDecoration(Nullable<std::u16string> textDecoration) __attribute__((weak));
    virtual Nullable<std::u16string> getTextIndent() __attribute__((weak));
    virtual void setTextIndent(Nullable<std::u16string> textIndent) __attribute__((weak));
    virtual Nullable<std::u16string> getTextShadow() __attribute__((weak));
    virtual void setTextShadow(Nullable<std::u16string> textShadow) __attribute__((weak));
    virtual Nullable<std::u16string> getTextTransform() __attribute__((weak));
    virtual void setTextTransform(Nullable<std::u16string> textTransform) __attribute__((weak));
    virtual Nullable<std::u16string> getTop() __attribute__((weak));
    virtual void setTop(Nullable<std::u16string> top) __attribute__((weak));
    virtual Nullable<std::u16string> getUnicodeBidi() __attribute__((weak));
    virtual void setUnicodeBidi(Nullable<std::u16string> unicodeBidi) __attribute__((weak));
    virtual Nullable<std::u16string> getVerticalAlign() __attribute__((weak));
    virtual void setVerticalAlign(Nullable<std::u16string> verticalAlign) __attribute__((weak));
    virtual Nullable<std::u16string> getVisibility() __attribute__((weak));
    virtual void setVisibility(Nullable<std::u16string> visibility) __attribute__((weak));
    virtual Nullable<std::u16string> getVoiceFamily() __attribute__((weak));
    virtual void setVoiceFamily(Nullable<std::u16string> voiceFamily) __attribute__((weak));
    virtual Nullable<std::u16string> getVolume() __attribute__((weak));
    virtual void setVolume(Nullable<std::u16string> volume) __attribute__((weak));
    virtual Nullable<std::u16string> getWhiteSpace() __attribute__((weak));
    virtual void setWhiteSpace(Nullable<std::u16string> whiteSpace) __attribute__((weak));
    virtual Nullable<std::u16string> getWidows() __attribute__((weak));
    virtual void setWidows(Nullable<std::u16string> widows) __attribute__((weak));
    virtual Nullable<std::u16string> getWidth() __attribute__((weak));
    virtual void setWidth(Nullable<std::u16string> width) __attribute__((weak));
    virtual Nullable<std::u16string> getWordSpacing() __attribute__((weak));
    virtual void setWordSpacing(Nullable<std::u16string> wordSpacing) __attribute__((weak));
    virtual Nullable<std::u16string> getZIndex() __attribute__((weak));
    virtual void setZIndex(Nullable<std::u16string> zIndex) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSStyleDeclaration::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSStyleDeclaration::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CSSSTYLEDECLARATION_IMP_H
