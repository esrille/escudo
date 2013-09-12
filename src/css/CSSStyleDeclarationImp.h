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

#ifndef ORG_W3C_DOM_BOOTSTRAP_CSSSTYLEDECLARATIONIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_CSSSTYLEDECLARATIONIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/css/CSSStyleDeclaration.h>

#include <org/w3c/dom/css/CSSRule.h>

#include <bitset>
#include <list>
#include <map>
#include <boost/intrusive_ptr.hpp>

#include "CSSParser.h"
#include "CSSPropertyValueImp.h"
#include "CSSSelector.h"
#include "CSSRuleImp.h"
#include "CSSRuleListImp.h"
#include "StackingContext.h"

class FontTexture;  // TODO: define namespace

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class Box;
class Block;
class CSSStyleDeclarationImp;

struct CSSStyleDeclarationBoard
{
    // property values                                         Block/  | need
    //                                                         reFlow/ | Resolve
    //                                                         rePaint |
    CSSBorderCollapseValueImp borderCollapse;               // F
    CSSBorderSpacingValueImp borderSpacing;                 // F
    CSSBorderWidthValueImp borderTopWidth;                  // F
    CSSBorderWidthValueImp borderRightWidth;                // F
    CSSBorderWidthValueImp borderBottomWidth;               // F
    CSSBorderWidthValueImp borderLeftWidth;                 // F
    CSSAutoLengthValueImp bottom;                           // TBD       R
    CSSCaptionSideValueImp captionSide;                     // B
    CSSClearValueImp clear;                                 // F
    CSSContentValueImp content;                             // B
    CSSAutoNumberingValueImp counterIncrement;              // F
    CSSAutoNumberingValueImp counterReset;                  // F
    CSSDirectionValueImp direction;                         // F
    CSSDisplayValueImp display;                             // B
    CSSFloatValueImp float_;                                // B
    CSSFontFamilyValueImp fontFamily;                       // F
    CSSFontSizeValueImp fontSize;                           // F
    CSSFontStyleValueImp fontStyle;                         // F
    CSSFontVariantValueImp fontVariant;                     // F
    CSSFontWeightValueImp fontWeight;                       // F
    CSSAutoLengthValueImp height;                           // F         R
    CSSAutoLengthValueImp left;                             // TBD       R
    CSSLetterSpacingValueImp letterSpacing;                 // F
    CSSLineHeightValueImp lineHeight;                       // F         R
    CSSListStyleImageValueImp listStyleImage;               // B
    CSSListStylePositionValueImp listStylePosition;         // B
    CSSListStyleTypeValueImp listStyleType;                 // B
    CSSAutoLengthValueImp marginTop;                        // F         R
    CSSAutoLengthValueImp marginRight;                      // F         R
    CSSAutoLengthValueImp marginBottom;                     // F         R
    CSSAutoLengthValueImp marginLeft;                       // F         R
    CSSNoneLengthValueImp maxHeight;                        // F         R
    CSSNoneLengthValueImp maxWidth;                         // F         R
    CSSNonNegativeLengthImp minHeight;                      // F         R
    CSSNonNegativeLengthImp minWidth;                       // F         R
    CSSOverflowValueImp overflow;                           // F
    CSSPaddingWidthValueImp paddingTop;                     // F         R
    CSSPaddingWidthValueImp paddingRight;                   // F         R
    CSSPaddingWidthValueImp paddingBottom;                  // F         R
    CSSPaddingWidthValueImp paddingLeft;                    // F         R
    CSSPositionValueImp position;                           // B
    CSSQuotesValueImp quotes;                               // F
    CSSAutoLengthValueImp right;                            // TBD       R
    CSSTableLayoutValueImp tableLayout;                     // F
    CSSTextAlignValueImp textAlign;                         // F
    CSSTextDecorationValueImp textDecoration;               // F
    CSSNumericValueImp textIndent;                          // F         R
    CSSTextTransformValueImp textTransform;                 // F
    CSSAutoLengthValueImp top;                              // TBD       R
    CSSUnicodeBidiValueImp unicodeBidi;                     // F
    CSSVerticalAlignValueImp verticalAlign;                 // F         R
    CSSWhiteSpaceValueImp whiteSpace;                       // F
    CSSWordSpacingValueImp wordSpacing;                     // F
    CSSAutoLengthValueImp width;                            // F         R
    CSSZIndexValueImp zIndex;                               // B
    CSSBindingValueImp binding;                             // B
    HTMLAlignValueImp htmlAlign;                            // F         R

    CSSStyleDeclarationBoard(CSSStyleDeclarationImp* style);
    unsigned compare(CSSStyleDeclarationImp* style);
};

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

    friend unsigned CSSStyleDeclarationBoard::compare(CSSStyleDeclarationImp* style);

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

        Opacity,  // "opacity"

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

    enum flags {
        Computed = 0x01,
        Resolved = 0x02,
        MediaDependent =        0x1000000,  // This style declaration depends on media queries.
        ComputedStyle =         0x2000000,
        Mutated =               0x4000000,
        NeedSelectorMatching =  0x8000000
    };

private:
    static const char16_t* PropertyNames[MaxProperties];

    Object* owner;
    mutable css::CSSRule parentRule;
    std::bitset<MaxProperties> propertySet;
    std::bitset<MaxProperties> importantSet;
    std::bitset<MaxProperties> inheritSet;

    //
    // Data members for parsing a CSS text
    //
    int propertyID;
    CSSParserExpr* expression;
    std::u16string priority;

    //
    // Data members for the computed style
    //
    unsigned flags;

    CSSRuleListImp::RuleSet ruleSet;
    unsigned affectedBits;  // 1u << CSSPseudoClassSelector::Hover, etc.
    CSSStyleDeclarationImp* parentStyle;
    CSSStyleDeclarationImp* bodyStyle;
    int emptyInline;    // 0: none, 1: first, 2: last, 3: both, 4: empty
    StackingContextPtr stackingContext;
    FontTexture* fontTexture;

    int pseudoElementSelectorType;
    CSSStyleDeclarationPtr pseudoElements[CSSPseudoElementSelector::MaxPseudoElements];

    // pseudo-elements
    Element marker;
    Element before;
    Element after;

    float containingBlockWidth;
    float containingBlockHeight;
    Box* box;
    Box* lastBox;   // for inline

    void initialize();

    void specify(const CSSStyleDeclarationImp* decl, unsigned id);
    void specify(const CSSStyleDeclarationImp* decl, const std::bitset<MaxProperties>& set);

    void setInherit(unsigned id);
    void resetInherit(unsigned id);
    void setImportant(unsigned id);
    void resetImportant(unsigned id);
    void setProperty(unsigned id);
    void resetProperty(unsigned id);

public:
    // property values                                         Block/reFlow/rePaint
    CSSBackgroundAttachmentValueImp backgroundAttachment;   // P
    CSSColorValueImp backgroundColor;                       // P
    CSSBackgroundImageValueImp backgroundImage;             // P
    CSSBackgroundPositionValueImp backgroundPosition;       // P
    CSSBackgroundRepeatValueImp backgroundRepeat;           // P
    CSSBackgroundShorthandImp background;                   //
    CSSBorderCollapseValueImp borderCollapse;               // F
    CSSBorderColorShorthandImp borderColor;                 // P
    CSSBorderSpacingValueImp borderSpacing;                 // F
    CSSBorderStyleShorthandImp borderStyle;                 //
    CSSBorderValueImp borderTop;                            //
    CSSBorderValueImp borderRight;                          //
    CSSBorderValueImp borderBottom;                         //
    CSSBorderValueImp borderLeft;                           //
    CSSBorderColorValueImp borderTopColor;                  // P
    CSSBorderColorValueImp borderRightColor;                // P
    CSSBorderColorValueImp borderBottomColor;               // P
    CSSBorderColorValueImp borderLeftColor;                 // P
    CSSBorderStyleValueImp borderTopStyle;                  // P
    CSSBorderStyleValueImp borderRightStyle;                // P
    CSSBorderStyleValueImp borderBottomStyle;               // P
    CSSBorderStyleValueImp borderLeftStyle;                 // P
    CSSBorderWidthValueImp borderTopWidth;                  // F
    CSSBorderWidthValueImp borderRightWidth;                // F
    CSSBorderWidthValueImp borderBottomWidth;               // F
    CSSBorderWidthValueImp borderLeftWidth;                 // F
    CSSBorderWidthShorthandImp borderWidth;                 //
    CSSBorderShorthandImp border;                           //
    CSSAutoLengthValueImp bottom;                           // TBD
    CSSCaptionSideValueImp captionSide;                     // B
    CSSClearValueImp clear;                                 // F

    CSSColorValueImp color;                                 // P
    CSSContentValueImp content;                             // B
    CSSAutoNumberingValueImp counterIncrement;              // F
    CSSAutoNumberingValueImp counterReset;                  // F

    CSSCursorValueImp cursor;                               // P
    CSSDirectionValueImp direction;                         // F
    CSSDisplayValueImp display;                             // B

    CSSEmptyCellsValueImp emptyCells;                       // P
    CSSFloatValueImp float_;                                // B
    CSSFontFamilyValueImp fontFamily;                       // F
    CSSFontSizeValueImp fontSize;                           // F
    CSSFontStyleValueImp fontStyle;                         // F
    CSSFontVariantValueImp fontVariant;                     // F
    CSSFontWeightValueImp fontWeight;                       // F
    CSSFontShorthandImp font;                               //
    CSSAutoLengthValueImp height;                           // F
    CSSAutoLengthValueImp left;                             // TBD
    CSSLetterSpacingValueImp letterSpacing;                 // F
    CSSLineHeightValueImp lineHeight;                       // F
    CSSListStyleImageValueImp listStyleImage;               // B
    CSSListStylePositionValueImp listStylePosition;         // B
    CSSListStyleTypeValueImp listStyleType;                 // B
    CSSListStyleShorthandImp listStyle;                     //
    CSSAutoLengthValueImp marginTop;                        // F
    CSSAutoLengthValueImp marginRight;                      // F
    CSSAutoLengthValueImp marginBottom;                     // F
    CSSAutoLengthValueImp marginLeft;                       // F
    CSSMarginShorthandImp margin;                           //
    CSSNoneLengthValueImp maxHeight;                        // F
    CSSNoneLengthValueImp maxWidth;                         // F
    CSSNonNegativeLengthImp minHeight;                      // F
    CSSNonNegativeLengthImp minWidth;                       // F

    CSSOutlineColorValueImp outlineColor;                   // P
    CSSBorderStyleValueImp outlineStyle;                    // P
    CSSBorderWidthValueImp outlineWidth;                    // P
    CSSOutlineShorthandImp outline;                         //
    CSSOverflowValueImp overflow;                           // F
    CSSPaddingWidthValueImp paddingTop;                     // F
    CSSPaddingWidthValueImp paddingRight;                   // F
    CSSPaddingWidthValueImp paddingBottom;                  // F
    CSSPaddingWidthValueImp paddingLeft;                    // F
    CSSPaddingShorthandImp padding;                         //
    CSSPageBreakValueImp pageBreakAfter;                    // TBD
    CSSPageBreakValueImp pageBreakBefore;                   // TBD
    CSSPageBreakValueImp pageBreakInside;                   // TBD

    CSSPositionValueImp position;                           // B
    CSSQuotesValueImp quotes;                               // F

    CSSAutoLengthValueImp right;                            // TBD

    CSSTableLayoutValueImp tableLayout;                     // F
    CSSTextAlignValueImp textAlign;                         // F
    CSSTextDecorationValueImp textDecoration;               // F
    CSSNumericValueImp textIndent;                          // F
    CSSTextTransformValueImp textTransform;                 // F
    CSSAutoLengthValueImp top;                              // TBD
    CSSUnicodeBidiValueImp unicodeBidi;                     // F
    CSSVerticalAlignValueImp verticalAlign;                 // F
    CSSVisibilityValueImp visibility;                       // P

    CSSWhiteSpaceValueImp whiteSpace;                       // F
    CSSWordSpacingValueImp wordSpacing;                     // F
    CSSAutoLengthValueImp width;                            // F

    CSSZIndexValueImp zIndex;                               // B

    CSSBindingValueImp binding;                             // B
    CSSNumericValueImp opacity;                             // P

    HTMLAlignValueImp htmlAlign;                            // F

    TextDecorationContext textDecorationContext;

    CSSStyleDeclarationImp(const CSSStyleDeclarationImp&);

public:
    CSSStyleDeclarationImp(int pseudoElementSelectorType = CSSPseudoElementSelector::NonPseudo);
    CSSStyleDeclarationImp(CSSStyleDeclarationImp* org);  // for cloneNode()
    ~CSSStyleDeclarationImp();

    void clearProperties();

    void resetComputedStyle();
    void restoreComputedValues(CSSStyleDeclarationBoard& board);

    void reset();

    void setOwner(Object* owner) {
        this->owner = owner;
    }

    void setParentRule(css::CSSRule parentRule) {
        this->parentRule = parentRule;
    }

    unsigned getFlags() const {
        return flags;
    }
    void clearFlags(unsigned f);
    void setFlags(unsigned f) {
        flags |= f;
    }
    bool isComputed() const {
        return flags & Computed;
    }
    bool isResolved() const {
        return flags & Resolved;
    }
    bool isMutated() const {
        return flags & Mutated;
    }

    int appendProperty(const std::u16string& property, CSSParserExpr* expr, const std::u16string& prio = u"");
    int commitAppend();
    int cancelAppend();

    int setProperty(int id, CSSParserExpr* expr, const std::u16string& prio = u"");
    int setProperty(const std::u16string& property, CSSParserExpr* expr, const std::u16string& prio = u"");

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

    CSSStyleDeclarationImp* getAffectedByHover() const;

    void specifyWithoutInherited(const CSSStyleDeclarationImp* style);
    void specify(const CSSStyleDeclarationImp* style);
    void specifyImportant(const CSSStyleDeclarationImp* style);

    void reset(unsigned id);
    void resetInheritedProperties();

    void inherit(const CSSStyleDeclarationImp* parentStyle, unsigned id);
    void inheritProperties(const CSSStyleDeclarationImp* parentStyle);

    void compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle, Element element);
    void computeStackingContext(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle, bool wasPositioned);
    unsigned resolve(ViewCSSImp* view, const ContainingBlock* containingBlock);
    void unresolve() {
        clearFlags(Resolved);
    }

    bool resolveRelativeOffset(float& x, float &y);
    bool setContainingBlockSize(float w, float h) {
        bool result = true;
        if (containingBlockWidth != w) {
            containingBlockWidth = w;
            result = false;
        }
        if (containingBlockHeight != h) {
            containingBlockHeight = h;
            result = false;
        }
        return result;
    }

    bool updateCounters(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context);

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

    unsigned getUsedPosition() const {
        unsigned value = position.getValue();
        if (value == CSSPositionValueImp::Static && opacity.getValue() < 1.0f)
            return CSSPositionValueImp::Relative;
        return value;
    }
    bool isAbsolutelyPositioned() const {
        unsigned value = position.getValue();
        return value == CSSPositionValueImp::Absolute || value == CSSPositionValueImp::Fixed;
    }
    bool isRelativelyPositioned() const {
        return getUsedPosition() == CSSPositionValueImp::Relative;
    }
    bool isPositioned() const {
        return getUsedPosition() != CSSPositionValueImp::Static;
    }

    CSSStyleDeclarationImp* getParentStyle() const {
        return parentStyle;
    }

    void clearBox();
    void addBox(Box* box);
    void removeBox(Box* box);

    Box* getBox() const {
        return box;
    }
    Box* getLastBox() const {
        return lastBox;
    }
    bool hasMultipleBoxes() const {
        return box && box != lastBox;
    }

    Block* updateInlines(Element element);
    Block* revert(Element element);
    void requestReconstruct(unsigned short flags);

    StackingContext* getStackingContext() const {
        return stackingContext.get();
    }
    void clearStackingContext() {
        stackingContext = 0;
    }

    FontTexture* getFontTexture() const {
        return fontTexture;
    }
    FontTexture* getAltFontTexture(ViewCSSImp* view, FontTexture* current, char32_t u);

    int getEmptyInline() const {
        return emptyInline;
    }

    int checkEmptyInline() {
        int code = emptyInline;
        emptyInline &= ~1;
        return code;
    }

    static int getPropertyID(const std::u16string& ident);
    static const char16_t* getPropertyName(int propertyID);

    void setProperty(int id, Nullable<std::u16string> value, const std::u16string& prio = u"");
    std::u16string removeProperty(int id);

    std::u16string resolveRelativeURL(const std::u16string& url) const;

    // CSSStyleDeclaration
    virtual std::u16string getCssText();
    virtual void setCssText(const std::u16string& cssText);
    virtual unsigned int getLength();
    virtual std::u16string item(unsigned int index);
    virtual std::u16string getPropertyValue(const std::u16string& property);
    virtual std::u16string getPropertyPriority(const std::u16string& property);
    void setProperty(const std::u16string& property, const std::u16string& value);
    void setProperty(const std::u16string& property, const std::u16string& value, const std::u16string& priority);
    std::u16string removeProperty(const std::u16string& property);
    css::CSSRule getParentRule();

    // CSS2Properties
    Nullable<std::u16string> getAzimuth();
    void setAzimuth(const Nullable<std::u16string>& azimuth);
    Nullable<std::u16string> getBackground();
    void setBackground(const Nullable<std::u16string>& background);
    Nullable<std::u16string> getBackgroundAttachment();
    void setBackgroundAttachment(const Nullable<std::u16string>& backgroundAttachment);
    Nullable<std::u16string> getBackgroundColor();
    void setBackgroundColor(const Nullable<std::u16string>& backgroundColor);
    Nullable<std::u16string> getBackgroundImage();
    void setBackgroundImage(const Nullable<std::u16string>& backgroundImage);
    Nullable<std::u16string> getBackgroundPosition();
    void setBackgroundPosition(const Nullable<std::u16string>& backgroundPosition);
    Nullable<std::u16string> getBackgroundRepeat();
    void setBackgroundRepeat(const Nullable<std::u16string>& backgroundRepeat);
    Nullable<std::u16string> getBorder();
    void setBorder(const Nullable<std::u16string>& border);
    Nullable<std::u16string> getBorderCollapse();
    void setBorderCollapse(const Nullable<std::u16string>& borderCollapse);
    Nullable<std::u16string> getBorderColor();
    void setBorderColor(const Nullable<std::u16string>& borderColor);
    Nullable<std::u16string> getBorderSpacing();
    void setBorderSpacing(const Nullable<std::u16string>& borderSpacing);
    Nullable<std::u16string> getBorderStyle();
    void setBorderStyle(const Nullable<std::u16string>& borderStyle);
    Nullable<std::u16string> getBorderTop();
    void setBorderTop(const Nullable<std::u16string>& borderTop);
    Nullable<std::u16string> getBorderRight();
    void setBorderRight(const Nullable<std::u16string>& borderRight);
    Nullable<std::u16string> getBorderBottom();
    void setBorderBottom(const Nullable<std::u16string>& borderBottom);
    Nullable<std::u16string> getBorderLeft();
    void setBorderLeft(const Nullable<std::u16string>& borderLeft);
    Nullable<std::u16string> getBorderTopColor();
    void setBorderTopColor(const Nullable<std::u16string>& borderTopColor);
    Nullable<std::u16string> getBorderRightColor();
    void setBorderRightColor(const Nullable<std::u16string>& borderRightColor);
    Nullable<std::u16string> getBorderBottomColor();
    void setBorderBottomColor(const Nullable<std::u16string>& borderBottomColor);
    Nullable<std::u16string> getBorderLeftColor();
    void setBorderLeftColor(const Nullable<std::u16string>& borderLeftColor);
    Nullable<std::u16string> getBorderTopStyle();
    void setBorderTopStyle(const Nullable<std::u16string>& borderTopStyle);
    Nullable<std::u16string> getBorderRightStyle();
    void setBorderRightStyle(const Nullable<std::u16string>& borderRightStyle);
    Nullable<std::u16string> getBorderBottomStyle();
    void setBorderBottomStyle(const Nullable<std::u16string>& borderBottomStyle);
    Nullable<std::u16string> getBorderLeftStyle();
    void setBorderLeftStyle(const Nullable<std::u16string>& borderLeftStyle);
    Nullable<std::u16string> getBorderTopWidth();
    void setBorderTopWidth(const Nullable<std::u16string>& borderTopWidth);
    Nullable<std::u16string> getBorderRightWidth();
    void setBorderRightWidth(const Nullable<std::u16string>& borderRightWidth);
    Nullable<std::u16string> getBorderBottomWidth();
    void setBorderBottomWidth(const Nullable<std::u16string>& borderBottomWidth);
    Nullable<std::u16string> getBorderLeftWidth();
    void setBorderLeftWidth(const Nullable<std::u16string>& borderLeftWidth);
    Nullable<std::u16string> getBorderWidth();
    void setBorderWidth(const Nullable<std::u16string>& borderWidth);
    Nullable<std::u16string> getBottom();
    void setBottom(const Nullable<std::u16string>& bottom);
    Nullable<std::u16string> getCaptionSide();
    void setCaptionSide(const Nullable<std::u16string>& captionSide);
    Nullable<std::u16string> getClear();
    void setClear(const Nullable<std::u16string>& clear);
    Nullable<std::u16string> getClip();
    void setClip(const Nullable<std::u16string>& clip);
    Nullable<std::u16string> getColor();
    void setColor(const Nullable<std::u16string>& color);
    Nullable<std::u16string> getContent();
    void setContent(const Nullable<std::u16string>& content);
    Nullable<std::u16string> getCounterIncrement();
    void setCounterIncrement(const Nullable<std::u16string>& counterIncrement);
    Nullable<std::u16string> getCounterReset();
    void setCounterReset(const Nullable<std::u16string>& counterReset);
    Nullable<std::u16string> getCue();
    void setCue(const Nullable<std::u16string>& cue);
    Nullable<std::u16string> getCueAfter();
    void setCueAfter(const Nullable<std::u16string>& cueAfter);
    Nullable<std::u16string> getCueBefore();
    void setCueBefore(const Nullable<std::u16string>& cueBefore);
    Nullable<std::u16string> getCursor();
    void setCursor(const Nullable<std::u16string>& cursor);
    Nullable<std::u16string> getDirection();
    void setDirection(const Nullable<std::u16string>& direction);
    Nullable<std::u16string> getDisplay();
    void setDisplay(const Nullable<std::u16string>& display);
    Nullable<std::u16string> getElevation();
    void setElevation(const Nullable<std::u16string>& elevation);
    Nullable<std::u16string> getEmptyCells();
    void setEmptyCells(const Nullable<std::u16string>& emptyCells);
    Nullable<std::u16string> getCssFloat();
    void setCssFloat(const Nullable<std::u16string>& cssFloat);
    Nullable<std::u16string> getFont();
    void setFont(const Nullable<std::u16string>& font);
    Nullable<std::u16string> getFontFamily();
    void setFontFamily(const Nullable<std::u16string>& fontFamily);
    Nullable<std::u16string> getFontSize();
    void setFontSize(const Nullable<std::u16string>& fontSize);
    Nullable<std::u16string> getFontSizeAdjust();
    void setFontSizeAdjust(const Nullable<std::u16string>& fontSizeAdjust);
    Nullable<std::u16string> getFontStretch();
    void setFontStretch(const Nullable<std::u16string>& fontStretch);
    Nullable<std::u16string> getFontStyle();
    void setFontStyle(const Nullable<std::u16string>& fontStyle);
    Nullable<std::u16string> getFontVariant();
    void setFontVariant(const Nullable<std::u16string>& fontVariant);
    Nullable<std::u16string> getFontWeight();
    void setFontWeight(const Nullable<std::u16string>& fontWeight);
    Nullable<std::u16string> getHeight();
    void setHeight(const Nullable<std::u16string>& height);
    Nullable<std::u16string> getLeft();
    void setLeft(const Nullable<std::u16string>& left);
    Nullable<std::u16string> getLetterSpacing();
    void setLetterSpacing(const Nullable<std::u16string>& letterSpacing);
    Nullable<std::u16string> getLineHeight();
    void setLineHeight(const Nullable<std::u16string>& lineHeight);
    Nullable<std::u16string> getListStyle();
    void setListStyle(const Nullable<std::u16string>& listStyle);
    Nullable<std::u16string> getListStyleImage();
    void setListStyleImage(const Nullable<std::u16string>& listStyleImage);
    Nullable<std::u16string> getListStylePosition();
    void setListStylePosition(const Nullable<std::u16string>& listStylePosition);
    Nullable<std::u16string> getListStyleType();
    void setListStyleType(const Nullable<std::u16string>& listStyleType);
    Nullable<std::u16string> getMargin();
    void setMargin(const Nullable<std::u16string>& margin);
    Nullable<std::u16string> getMarginTop();
    void setMarginTop(const Nullable<std::u16string>& marginTop);
    Nullable<std::u16string> getMarginRight();
    void setMarginRight(const Nullable<std::u16string>& marginRight);
    Nullable<std::u16string> getMarginBottom();
    void setMarginBottom(const Nullable<std::u16string>& marginBottom);
    Nullable<std::u16string> getMarginLeft();
    void setMarginLeft(const Nullable<std::u16string>& marginLeft);
    Nullable<std::u16string> getMarkerOffset();
    void setMarkerOffset(const Nullable<std::u16string>& markerOffset);
    Nullable<std::u16string> getMarks();
    void setMarks(const Nullable<std::u16string>& marks);
    Nullable<std::u16string> getMaxHeight();
    void setMaxHeight(const Nullable<std::u16string>& maxHeight);
    Nullable<std::u16string> getMaxWidth();
    void setMaxWidth(const Nullable<std::u16string>& maxWidth);
    Nullable<std::u16string> getMinHeight();
    void setMinHeight(const Nullable<std::u16string>& minHeight);
    Nullable<std::u16string> getMinWidth();
    void setMinWidth(const Nullable<std::u16string>& minWidth);
    Nullable<std::u16string> getOrphans();
    void setOrphans(const Nullable<std::u16string>& orphans);
    Nullable<std::u16string> getOutline();
    void setOutline(const Nullable<std::u16string>& outline);
    Nullable<std::u16string> getOutlineColor();
    void setOutlineColor(const Nullable<std::u16string>& outlineColor);
    Nullable<std::u16string> getOutlineStyle();
    void setOutlineStyle(const Nullable<std::u16string>& outlineStyle);
    Nullable<std::u16string> getOutlineWidth();
    void setOutlineWidth(const Nullable<std::u16string>& outlineWidth);
    Nullable<std::u16string> getOverflow();
    void setOverflow(const Nullable<std::u16string>& overflow);
    Nullable<std::u16string> getPadding();
    void setPadding(const Nullable<std::u16string>& padding);
    Nullable<std::u16string> getPaddingTop();
    void setPaddingTop(const Nullable<std::u16string>& paddingTop);
    Nullable<std::u16string> getPaddingRight();
    void setPaddingRight(const Nullable<std::u16string>& paddingRight);
    Nullable<std::u16string> getPaddingBottom();
    void setPaddingBottom(const Nullable<std::u16string>& paddingBottom);
    Nullable<std::u16string> getPaddingLeft();
    void setPaddingLeft(const Nullable<std::u16string>& paddingLeft);
    Nullable<std::u16string> getPage();
    void setPage(const Nullable<std::u16string>& page);
    Nullable<std::u16string> getPageBreakAfter();
    void setPageBreakAfter(const Nullable<std::u16string>& pageBreakAfter);
    Nullable<std::u16string> getPageBreakBefore();
    void setPageBreakBefore(const Nullable<std::u16string>& pageBreakBefore);
    Nullable<std::u16string> getPageBreakInside();
    void setPageBreakInside(const Nullable<std::u16string>& pageBreakInside);
    Nullable<std::u16string> getPause();
    void setPause(const Nullable<std::u16string>& pause);
    Nullable<std::u16string> getPauseAfter();
    void setPauseAfter(const Nullable<std::u16string>& pauseAfter);
    Nullable<std::u16string> getPauseBefore();
    void setPauseBefore(const Nullable<std::u16string>& pauseBefore);
    Nullable<std::u16string> getPitch();
    void setPitch(const Nullable<std::u16string>& pitch);
    Nullable<std::u16string> getPitchRange();
    void setPitchRange(const Nullable<std::u16string>& pitchRange);
    Nullable<std::u16string> getPlayDuring();
    void setPlayDuring(const Nullable<std::u16string>& playDuring);
    Nullable<std::u16string> getPosition();
    void setPosition(const Nullable<std::u16string>& position);
    Nullable<std::u16string> getQuotes();
    void setQuotes(const Nullable<std::u16string>& quotes);
    Nullable<std::u16string> getRichness();
    void setRichness(const Nullable<std::u16string>& richness);
    Nullable<std::u16string> getRight();
    void setRight(const Nullable<std::u16string>& right);
    Nullable<std::u16string> getSize();
    void setSize(const Nullable<std::u16string>& size);
    Nullable<std::u16string> getSpeak();
    void setSpeak(const Nullable<std::u16string>& speak);
    Nullable<std::u16string> getSpeakHeader();
    void setSpeakHeader(const Nullable<std::u16string>& speakHeader);
    Nullable<std::u16string> getSpeakNumeral();
    void setSpeakNumeral(const Nullable<std::u16string>& speakNumeral);
    Nullable<std::u16string> getSpeakPunctuation();
    void setSpeakPunctuation(const Nullable<std::u16string>& speakPunctuation);
    Nullable<std::u16string> getSpeechRate();
    void setSpeechRate(const Nullable<std::u16string>& speechRate);
    Nullable<std::u16string> getStress();
    void setStress(const Nullable<std::u16string>& stress);
    Nullable<std::u16string> getTableLayout();
    void setTableLayout(const Nullable<std::u16string>& tableLayout);
    Nullable<std::u16string> getTextAlign();
    void setTextAlign(const Nullable<std::u16string>& textAlign);
    Nullable<std::u16string> getTextDecoration();
    void setTextDecoration(const Nullable<std::u16string>& textDecoration);
    Nullable<std::u16string> getTextIndent();
    void setTextIndent(const Nullable<std::u16string>& textIndent);
    Nullable<std::u16string> getTextShadow();
    void setTextShadow(const Nullable<std::u16string>& textShadow);
    Nullable<std::u16string> getTextTransform();
    void setTextTransform(const Nullable<std::u16string>& textTransform);
    Nullable<std::u16string> getTop();
    void setTop(const Nullable<std::u16string>& top);
    Nullable<std::u16string> getUnicodeBidi();
    void setUnicodeBidi(const Nullable<std::u16string>& unicodeBidi);
    Nullable<std::u16string> getVerticalAlign();
    void setVerticalAlign(const Nullable<std::u16string>& verticalAlign);
    Nullable<std::u16string> getVisibility();
    void setVisibility(const Nullable<std::u16string>& visibility);
    Nullable<std::u16string> getVoiceFamily();
    void setVoiceFamily(const Nullable<std::u16string>& voiceFamily);
    Nullable<std::u16string> getVolume();
    void setVolume(const Nullable<std::u16string>& volume);
    Nullable<std::u16string> getWhiteSpace();
    void setWhiteSpace(const Nullable<std::u16string>& whiteSpace);
    Nullable<std::u16string> getWidows();
    void setWidows(const Nullable<std::u16string>& widows);
    Nullable<std::u16string> getWidth();
    void setWidth(const Nullable<std::u16string>& width);
    Nullable<std::u16string> getWordSpacing();
    void setWordSpacing(const Nullable<std::u16string>& wordSpacing);
    Nullable<std::u16string> getZIndex();
    void setZIndex(const Nullable<std::u16string>& zIndex);

    Nullable<std::u16string> getHTMLAlign();
    void setHTMLAlign(const Nullable<std::u16string>& align);

    // CSSStyleDeclaration-48
    Nullable<std::u16string> getOpacity();
    void setOpacity(const Nullable<std::u16string>& opacity);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return css::CSSStyleDeclaration::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return css::CSSStyleDeclaration::getMetaData();
    }

    // Returns true is the specified property does not change the positions of render boxes.
    static bool isPaintCategory(unsigned id);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_CSSSTYLEDECLARATIONIMP_H_INCLUDED
