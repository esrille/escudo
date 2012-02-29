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

#ifndef CSSSTYLEDECLARATION_IMP_H
#define CSSSTYLEDECLARATION_IMP_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSStyleDeclaration.h>

#include <org/w3c/dom/css/CSSRule.h>
#include <org/w3c/dom/css/CSSStyleDeclarationValue.h>

#include <bitset>
#include <list>
#include <map>
#include <boost/intrusive_ptr.hpp>

#include "CSSParser.h"
#include "CSSPropertyValueImp.h"
#include "CSSSelector.h"
#include "CSSRuleImp.h"

class FontTexture;  // TODO: define namespace

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class StackingContext;
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

        Binding, // "binding" - Behavioral Extensions to CSS

        MaxCSSProperties,

        HtmlAlign = MaxCSSProperties, // "align"

        MaxProperties  // including Unknown
    };

    struct TextDecorationContext
    {
        unsigned color;      // from CSSColor::getARGB()
        unsigned decoration; // from CSSTextDecorationValueImp::getValue()

        TextDecorationContext() :
            color(0),
            decoration(CSSTextDecorationValueImp::None)
        {
        }
        TextDecorationContext(const TextDecorationContext& other) :
            color(other.color),
            decoration(other.decoration)
        {
        }
        bool hasDecoration() const {
            return decoration != CSSTextDecorationValueImp::None;
        }
        void update(CSSStyleDeclarationImp* style) {
            color = style->color.getARGB();
            decoration = style->textDecoration.getValue();
        }
    };

private:
    static const size_t PropertyCount = MaxProperties;
    static const char16_t* PropertyNames[PropertyCount];

    Object* owner;
    css::CSSRule parentRule;
    std::bitset<PropertyCount> propertySet;
    std::bitset<PropertyCount> importantSet;
    std::bitset<PropertyCount> inheritSet;
    bool resolved;

    CSSStyleDeclarationImp* parentStyle;
    Box* box;
    Box* lastBox;  // for inline
    StackingContext* stackingContext;
    FontTexture* fontTexture;

    int propertyID;
    CSSParserExpr* expression;
    std::u16string priority;

    int pseudoElementSelectorType;
    CSSStyleDeclarationPtr pseudoElements[CSSPseudoElementSelector::MaxPseudoElements];

    void specify(const CSSStyleDeclarationImp* decl, unsigned id);
    void specify(const CSSStyleDeclarationImp* decl, const std::bitset<PropertyCount>& set);

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
    CSSBorderValueImp borderTop;
    CSSBorderValueImp borderRight;
    CSSBorderValueImp borderBottom;
    CSSBorderValueImp borderLeft;
    CSSBorderColorValueImp borderTopColor;
    CSSBorderColorValueImp borderRightColor;
    CSSBorderColorValueImp borderBottomColor;
    CSSBorderColorValueImp borderLeftColor;
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
    CSSCaptionSideValueImp captionSide;
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
    CSSFontVariantValueImp fontVariant;
    CSSFontWeightValueImp fontWeight;
    CSSFontShorthandImp font;
    CSSAutoLengthValueImp height;
    CSSAutoLengthValueImp left;
    CSSLetterSpacingValueImp letterSpacing;
    CSSLineHeightValueImp lineHeight;
    CSSListStyleImageValueImp listStyleImage;
    CSSListStylePositionValueImp listStylePosition;
    CSSListStyleTypeValueImp listStyleType;
    CSSListStyleShorthandImp listStyle;
    CSSAutoLengthValueImp marginTop;
    CSSAutoLengthValueImp marginRight;
    CSSAutoLengthValueImp marginBottom;
    CSSAutoLengthValueImp marginLeft;
    CSSMarginShorthandImp margin;
    CSSNoneLengthValueImp maxHeight;
    CSSNoneLengthValueImp maxWidth;
    CSSNonNegativeValueImp minHeight;
    CSSNonNegativeValueImp minWidth;

    CSSOverflowValueImp overflow;
    CSSNonNegativeValueImp paddingTop;
    CSSNonNegativeValueImp paddingRight;
    CSSNonNegativeValueImp paddingBottom;
    CSSNonNegativeValueImp paddingLeft;
    CSSPaddingShorthandImp padding;
    CSSPageBreakValueImp pageBreakAfter;
    CSSPageBreakValueImp pageBreakBefore;
    CSSPageBreakValueImp pageBreakInside;

    CSSPositionValueImp position;

    CSSAutoLengthValueImp right;

    CSSTableLayoutValueImp tableLayout;
    CSSTextAlignValueImp textAlign;
    CSSTextDecorationValueImp textDecoration;
    CSSNumericValueImp textIndent;
    CSSTextTransformValueImp textTransform;
    CSSAutoLengthValueImp top;
    CSSUnicodeBidiValueImp unicodeBidi;
    CSSVerticalAlignValueImp verticalAlign;
    CSSVisibilityValueImp visibility;

    CSSWhiteSpaceValueImp whiteSpace;
    CSSWordSpacingValueImp wordSpacing;
    CSSAutoLengthValueImp width;

    CSSZIndexValueImp zIndex;
    CSSBindingValueImp binding;

    HTMLAlignValueImp htmlAlign;

    TextDecorationContext textDecorationContext;

    CSSStyleDeclarationImp(const CSSStyleDeclarationImp&);

public:
    CSSStyleDeclarationImp(int pseudoElementSelectorType = CSSPseudoElementSelector::NonPseudo);
    ~CSSStyleDeclarationImp();

    void setOwner(Object* owner) {
        this->owner = owner;
    }

    void setParentRule(css::CSSRule parentRule) {
        this->parentRule = parentRule;
    }

    int appendProperty(std::u16string property, CSSParserExpr* expr, const std::u16string& prio = u"");
    int commitAppend();
    int cancelAppend();

    int setProperty(int id, CSSParserExpr* expr, const std::u16string& prio = u"");
    int setProperty(std::u16string property, CSSParserExpr* expr, const std::u16string& prio = u"");

    void setColor(unsigned color) {
        this->color = color;
    }

    CSSPropertyValueImp* getProperty(unsigned id);

    int getPseudoElementSelectorType() const {
        return pseudoElementSelectorType;
    }
    CSSStyleDeclarationImp* getPseudoElementStyle(int id);
    CSSStyleDeclarationImp* getPseudoElementStyle(const std::u16string& name);
    CSSStyleDeclarationImp* createPseudoElementStyle(int id);

    void specifyWithoutInherited(const CSSStyleDeclarationImp* style);
    void specify(const CSSStyleDeclarationImp* style);
    void specifyImportant(const CSSStyleDeclarationImp* style);

    void reset(unsigned id);
    void resetInheritedProperties();

    void copy(const CSSStyleDeclarationImp* parentStyle, unsigned id);
    void copyInheritedProperties(const CSSStyleDeclarationImp* parentStyle);

    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle, Element element);

    void resolve(ViewCSSImp* view, const ContainingBlock* containingBlock);

    void updateCounters(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context) {
        counterReset.resetCounter(view, context);
        counterIncrement.incrementCounter(view, context);
    }

    size_t processWhiteSpace(std::u16string& data, char16_t& prevChar);
    size_t processLineHeadWhiteSpace(const std::u16string& data, size_t position);

    bool isFlowRoot() const;

    bool isBlockLevel() const {
        return display.isBlockLevel() || binding.isBlockLevel();
    }
    bool isInlineBlock() const {
        return display.isInlineBlock() || binding.isInlineBlock();
    }

    bool isFloat() const {
        return float_.getValue() != CSSFloatValueImp::None;
    }
    bool isAbsolutelyPositioned() const {
        return position.getValue() == CSSPositionValueImp::Absolute ||
               position.getValue() == CSSPositionValueImp::Fixed;
    }
    bool isPositioned() const {
        return position.getValue() != CSSPositionValueImp::Static;
    }

    CSSStyleDeclarationImp* getParentStyle() const {
        return parentStyle;
    }

    Box* getBox() const {
        return box;
    }
    void addBox(Box* box);

    StackingContext* getStackingContext() const {
        return stackingContext;
    }

    FontTexture* getFontTexture() const {
        return fontTexture;
    }
    FontTexture* getAltFontTexture(ViewCSSImp* view, FontTexture* current, char32_t u);

    static int getPropertyID(const std::u16string& ident);
    static const char16_t* getPropertyName(int propertyID);

    void setProperty(int id, Nullable<std::u16string> value, const std::u16string& prio = u"");
    std::u16string removeProperty(int id);

    // CSSStyleDeclaration
    virtual std::u16string getCssText();
    virtual void setCssText(std::u16string cssText);
    virtual unsigned int getLength();
    virtual std::u16string item(unsigned int index);
    virtual std::u16string getPropertyValue(std::u16string property);
    virtual std::u16string getPropertyPriority(std::u16string property);
    virtual void setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value);
    virtual void setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value, Nullable<std::u16string> priority);
    virtual std::u16string removeProperty(std::u16string property);
    virtual css::CSSStyleDeclarationValue getValues();
    virtual css::CSSRule getParentRule();
    // CSS2Properties
    virtual Nullable<std::u16string> getAzimuth();
    virtual void setAzimuth(Nullable<std::u16string> azimuth);
    virtual Nullable<std::u16string> getBackground();
    virtual void setBackground(Nullable<std::u16string> background);
    virtual Nullable<std::u16string> getBackgroundAttachment();
    virtual void setBackgroundAttachment(Nullable<std::u16string> backgroundAttachment);
    virtual Nullable<std::u16string> getBackgroundColor();
    virtual void setBackgroundColor(Nullable<std::u16string> backgroundColor);
    virtual Nullable<std::u16string> getBackgroundImage();
    virtual void setBackgroundImage(Nullable<std::u16string> backgroundImage);
    virtual Nullable<std::u16string> getBackgroundPosition();
    virtual void setBackgroundPosition(Nullable<std::u16string> backgroundPosition);
    virtual Nullable<std::u16string> getBackgroundRepeat();
    virtual void setBackgroundRepeat(Nullable<std::u16string> backgroundRepeat);
    virtual Nullable<std::u16string> getBorder();
    virtual void setBorder(Nullable<std::u16string> border);
    virtual Nullable<std::u16string> getBorderCollapse();
    virtual void setBorderCollapse(Nullable<std::u16string> borderCollapse);
    virtual Nullable<std::u16string> getBorderColor();
    virtual void setBorderColor(Nullable<std::u16string> borderColor);
    virtual Nullable<std::u16string> getBorderSpacing();
    virtual void setBorderSpacing(Nullable<std::u16string> borderSpacing);
    virtual Nullable<std::u16string> getBorderStyle();
    virtual void setBorderStyle(Nullable<std::u16string> borderStyle);
    virtual Nullable<std::u16string> getBorderTop();
    virtual void setBorderTop(Nullable<std::u16string> borderTop);
    virtual Nullable<std::u16string> getBorderRight();
    virtual void setBorderRight(Nullable<std::u16string> borderRight);
    virtual Nullable<std::u16string> getBorderBottom();
    virtual void setBorderBottom(Nullable<std::u16string> borderBottom);
    virtual Nullable<std::u16string> getBorderLeft();
    virtual void setBorderLeft(Nullable<std::u16string> borderLeft);
    virtual Nullable<std::u16string> getBorderTopColor();
    virtual void setBorderTopColor(Nullable<std::u16string> borderTopColor);
    virtual Nullable<std::u16string> getBorderRightColor();
    virtual void setBorderRightColor(Nullable<std::u16string> borderRightColor);
    virtual Nullable<std::u16string> getBorderBottomColor();
    virtual void setBorderBottomColor(Nullable<std::u16string> borderBottomColor);
    virtual Nullable<std::u16string> getBorderLeftColor();
    virtual void setBorderLeftColor(Nullable<std::u16string> borderLeftColor);
    virtual Nullable<std::u16string> getBorderTopStyle();
    virtual void setBorderTopStyle(Nullable<std::u16string> borderTopStyle);
    virtual Nullable<std::u16string> getBorderRightStyle();
    virtual void setBorderRightStyle(Nullable<std::u16string> borderRightStyle);
    virtual Nullable<std::u16string> getBorderBottomStyle();
    virtual void setBorderBottomStyle(Nullable<std::u16string> borderBottomStyle);
    virtual Nullable<std::u16string> getBorderLeftStyle();
    virtual void setBorderLeftStyle(Nullable<std::u16string> borderLeftStyle);
    virtual Nullable<std::u16string> getBorderTopWidth();
    virtual void setBorderTopWidth(Nullable<std::u16string> borderTopWidth);
    virtual Nullable<std::u16string> getBorderRightWidth();
    virtual void setBorderRightWidth(Nullable<std::u16string> borderRightWidth);
    virtual Nullable<std::u16string> getBorderBottomWidth();
    virtual void setBorderBottomWidth(Nullable<std::u16string> borderBottomWidth);
    virtual Nullable<std::u16string> getBorderLeftWidth();
    virtual void setBorderLeftWidth(Nullable<std::u16string> borderLeftWidth);
    virtual Nullable<std::u16string> getBorderWidth();
    virtual void setBorderWidth(Nullable<std::u16string> borderWidth);
    virtual Nullable<std::u16string> getBottom();
    virtual void setBottom(Nullable<std::u16string> bottom);
    virtual Nullable<std::u16string> getCaptionSide();
    virtual void setCaptionSide(Nullable<std::u16string> captionSide);
    virtual Nullable<std::u16string> getClear();
    virtual void setClear(Nullable<std::u16string> clear);
    virtual Nullable<std::u16string> getClip();
    virtual void setClip(Nullable<std::u16string> clip);
    virtual Nullable<std::u16string> getColor();
    virtual void setColor(Nullable<std::u16string> color);
    virtual Nullable<std::u16string> getContent();
    virtual void setContent(Nullable<std::u16string> content);
    virtual Nullable<std::u16string> getCounterIncrement();
    virtual void setCounterIncrement(Nullable<std::u16string> counterIncrement);
    virtual Nullable<std::u16string> getCounterReset();
    virtual void setCounterReset(Nullable<std::u16string> counterReset);
    virtual Nullable<std::u16string> getCue();
    virtual void setCue(Nullable<std::u16string> cue);
    virtual Nullable<std::u16string> getCueAfter();
    virtual void setCueAfter(Nullable<std::u16string> cueAfter);
    virtual Nullable<std::u16string> getCueBefore();
    virtual void setCueBefore(Nullable<std::u16string> cueBefore);
    virtual Nullable<std::u16string> getCursor();
    virtual void setCursor(Nullable<std::u16string> cursor);
    virtual Nullable<std::u16string> getDirection();
    virtual void setDirection(Nullable<std::u16string> direction);
    virtual Nullable<std::u16string> getDisplay();
    virtual void setDisplay(Nullable<std::u16string> display);
    virtual Nullable<std::u16string> getElevation();
    virtual void setElevation(Nullable<std::u16string> elevation);
    virtual Nullable<std::u16string> getEmptyCells();
    virtual void setEmptyCells(Nullable<std::u16string> emptyCells);
    virtual Nullable<std::u16string> getCssFloat();
    virtual void setCssFloat(Nullable<std::u16string> cssFloat);
    virtual Nullable<std::u16string> getFont();
    virtual void setFont(Nullable<std::u16string> font);
    virtual Nullable<std::u16string> getFontFamily();
    virtual void setFontFamily(Nullable<std::u16string> fontFamily);
    virtual Nullable<std::u16string> getFontSize();
    virtual void setFontSize(Nullable<std::u16string> fontSize);
    virtual Nullable<std::u16string> getFontSizeAdjust();
    virtual void setFontSizeAdjust(Nullable<std::u16string> fontSizeAdjust);
    virtual Nullable<std::u16string> getFontStretch();
    virtual void setFontStretch(Nullable<std::u16string> fontStretch);
    virtual Nullable<std::u16string> getFontStyle();
    virtual void setFontStyle(Nullable<std::u16string> fontStyle);
    virtual Nullable<std::u16string> getFontVariant();
    virtual void setFontVariant(Nullable<std::u16string> fontVariant);
    virtual Nullable<std::u16string> getFontWeight();
    virtual void setFontWeight(Nullable<std::u16string> fontWeight);
    virtual Nullable<std::u16string> getHeight();
    virtual void setHeight(Nullable<std::u16string> height);
    virtual Nullable<std::u16string> getLeft();
    virtual void setLeft(Nullable<std::u16string> left);
    virtual Nullable<std::u16string> getLetterSpacing();
    virtual void setLetterSpacing(Nullable<std::u16string> letterSpacing);
    virtual Nullable<std::u16string> getLineHeight();
    virtual void setLineHeight(Nullable<std::u16string> lineHeight);
    virtual Nullable<std::u16string> getListStyle();
    virtual void setListStyle(Nullable<std::u16string> listStyle);
    virtual Nullable<std::u16string> getListStyleImage();
    virtual void setListStyleImage(Nullable<std::u16string> listStyleImage);
    virtual Nullable<std::u16string> getListStylePosition();
    virtual void setListStylePosition(Nullable<std::u16string> listStylePosition);
    virtual Nullable<std::u16string> getListStyleType();
    virtual void setListStyleType(Nullable<std::u16string> listStyleType);
    virtual Nullable<std::u16string> getMargin();
    virtual void setMargin(Nullable<std::u16string> margin);
    virtual Nullable<std::u16string> getMarginTop();
    virtual void setMarginTop(Nullable<std::u16string> marginTop);
    virtual Nullable<std::u16string> getMarginRight();
    virtual void setMarginRight(Nullable<std::u16string> marginRight);
    virtual Nullable<std::u16string> getMarginBottom();
    virtual void setMarginBottom(Nullable<std::u16string> marginBottom);
    virtual Nullable<std::u16string> getMarginLeft();
    virtual void setMarginLeft(Nullable<std::u16string> marginLeft);
    virtual Nullable<std::u16string> getMarkerOffset();
    virtual void setMarkerOffset(Nullable<std::u16string> markerOffset);
    virtual Nullable<std::u16string> getMarks();
    virtual void setMarks(Nullable<std::u16string> marks);
    virtual Nullable<std::u16string> getMaxHeight();
    virtual void setMaxHeight(Nullable<std::u16string> maxHeight);
    virtual Nullable<std::u16string> getMaxWidth();
    virtual void setMaxWidth(Nullable<std::u16string> maxWidth);
    virtual Nullable<std::u16string> getMinHeight();
    virtual void setMinHeight(Nullable<std::u16string> minHeight);
    virtual Nullable<std::u16string> getMinWidth();
    virtual void setMinWidth(Nullable<std::u16string> minWidth);
    virtual Nullable<std::u16string> getOrphans();
    virtual void setOrphans(Nullable<std::u16string> orphans);
    virtual Nullable<std::u16string> getOutline();
    virtual void setOutline(Nullable<std::u16string> outline);
    virtual Nullable<std::u16string> getOutlineColor();
    virtual void setOutlineColor(Nullable<std::u16string> outlineColor);
    virtual Nullable<std::u16string> getOutlineStyle();
    virtual void setOutlineStyle(Nullable<std::u16string> outlineStyle);
    virtual Nullable<std::u16string> getOutlineWidth();
    virtual void setOutlineWidth(Nullable<std::u16string> outlineWidth);
    virtual Nullable<std::u16string> getOverflow();
    virtual void setOverflow(Nullable<std::u16string> overflow);
    virtual Nullable<std::u16string> getPadding();
    virtual void setPadding(Nullable<std::u16string> padding);
    virtual Nullable<std::u16string> getPaddingTop();
    virtual void setPaddingTop(Nullable<std::u16string> paddingTop);
    virtual Nullable<std::u16string> getPaddingRight();
    virtual void setPaddingRight(Nullable<std::u16string> paddingRight);
    virtual Nullable<std::u16string> getPaddingBottom();
    virtual void setPaddingBottom(Nullable<std::u16string> paddingBottom);
    virtual Nullable<std::u16string> getPaddingLeft();
    virtual void setPaddingLeft(Nullable<std::u16string> paddingLeft);
    virtual Nullable<std::u16string> getPage();
    virtual void setPage(Nullable<std::u16string> page);
    virtual Nullable<std::u16string> getPageBreakAfter();
    virtual void setPageBreakAfter(Nullable<std::u16string> pageBreakAfter);
    virtual Nullable<std::u16string> getPageBreakBefore();
    virtual void setPageBreakBefore(Nullable<std::u16string> pageBreakBefore);
    virtual Nullable<std::u16string> getPageBreakInside();
    virtual void setPageBreakInside(Nullable<std::u16string> pageBreakInside);
    virtual Nullable<std::u16string> getPause();
    virtual void setPause(Nullable<std::u16string> pause);
    virtual Nullable<std::u16string> getPauseAfter();
    virtual void setPauseAfter(Nullable<std::u16string> pauseAfter);
    virtual Nullable<std::u16string> getPauseBefore();
    virtual void setPauseBefore(Nullable<std::u16string> pauseBefore);
    virtual Nullable<std::u16string> getPitch();
    virtual void setPitch(Nullable<std::u16string> pitch);
    virtual Nullable<std::u16string> getPitchRange();
    virtual void setPitchRange(Nullable<std::u16string> pitchRange);
    virtual Nullable<std::u16string> getPlayDuring();
    virtual void setPlayDuring(Nullable<std::u16string> playDuring);
    virtual Nullable<std::u16string> getPosition();
    virtual void setPosition(Nullable<std::u16string> position);
    virtual Nullable<std::u16string> getQuotes();
    virtual void setQuotes(Nullable<std::u16string> quotes);
    virtual Nullable<std::u16string> getRichness();
    virtual void setRichness(Nullable<std::u16string> richness);
    virtual Nullable<std::u16string> getRight();
    virtual void setRight(Nullable<std::u16string> right);
    virtual Nullable<std::u16string> getSize();
    virtual void setSize(Nullable<std::u16string> size);
    virtual Nullable<std::u16string> getSpeak();
    virtual void setSpeak(Nullable<std::u16string> speak);
    virtual Nullable<std::u16string> getSpeakHeader();
    virtual void setSpeakHeader(Nullable<std::u16string> speakHeader);
    virtual Nullable<std::u16string> getSpeakNumeral();
    virtual void setSpeakNumeral(Nullable<std::u16string> speakNumeral);
    virtual Nullable<std::u16string> getSpeakPunctuation();
    virtual void setSpeakPunctuation(Nullable<std::u16string> speakPunctuation);
    virtual Nullable<std::u16string> getSpeechRate();
    virtual void setSpeechRate(Nullable<std::u16string> speechRate);
    virtual Nullable<std::u16string> getStress();
    virtual void setStress(Nullable<std::u16string> stress);
    virtual Nullable<std::u16string> getTableLayout();
    virtual void setTableLayout(Nullable<std::u16string> tableLayout);
    virtual Nullable<std::u16string> getTextAlign();
    virtual void setTextAlign(Nullable<std::u16string> textAlign);
    virtual Nullable<std::u16string> getTextDecoration();
    virtual void setTextDecoration(Nullable<std::u16string> textDecoration);
    virtual Nullable<std::u16string> getTextIndent();
    virtual void setTextIndent(Nullable<std::u16string> textIndent);
    virtual Nullable<std::u16string> getTextShadow();
    virtual void setTextShadow(Nullable<std::u16string> textShadow);
    virtual Nullable<std::u16string> getTextTransform();
    virtual void setTextTransform(Nullable<std::u16string> textTransform);
    virtual Nullable<std::u16string> getTop();
    virtual void setTop(Nullable<std::u16string> top);
    virtual Nullable<std::u16string> getUnicodeBidi();
    virtual void setUnicodeBidi(Nullable<std::u16string> unicodeBidi);
    virtual Nullable<std::u16string> getVerticalAlign();
    virtual void setVerticalAlign(Nullable<std::u16string> verticalAlign);
    virtual Nullable<std::u16string> getVisibility();
    virtual void setVisibility(Nullable<std::u16string> visibility);
    virtual Nullable<std::u16string> getVoiceFamily();
    virtual void setVoiceFamily(Nullable<std::u16string> voiceFamily);
    virtual Nullable<std::u16string> getVolume();
    virtual void setVolume(Nullable<std::u16string> volume);
    virtual Nullable<std::u16string> getWhiteSpace();
    virtual void setWhiteSpace(Nullable<std::u16string> whiteSpace);
    virtual Nullable<std::u16string> getWidows();
    virtual void setWidows(Nullable<std::u16string> widows);
    virtual Nullable<std::u16string> getWidth();
    virtual void setWidth(Nullable<std::u16string> width);
    virtual Nullable<std::u16string> getWordSpacing();
    virtual void setWordSpacing(Nullable<std::u16string> wordSpacing);
    virtual Nullable<std::u16string> getZIndex();
    virtual void setZIndex(Nullable<std::u16string> zIndex);

    Nullable<std::u16string> getHTMLAlign();
    void setHTMLAlign(Nullable<std::u16string> align);

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
