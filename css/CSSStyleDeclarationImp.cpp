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

#include "CSSStyleDeclarationImp.h"

#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/HTMLBodyElement.h>

#include "StackingContext.h"
#include "CSSValueParser.h"
#include "MutationEventImp.h"
#include "ViewCSSImp.h"

#include <iostream>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

const char16_t* CSSStyleDeclarationImp::PropertyNames[PropertyCount] = {
    u"",
    u"azimuth",
    u"background",
    u"background-attachment",
    u"background-color",
    u"background-image",
    u"background-position",
    u"background-repeat",
    u"border",
    u"border-collapse",
    u"border-color",
    u"border-spacing",
    u"border-style",
    u"border-top",
    u"border-right",
    u"border-bottom",
    u"border-left",
    u"border-top-color",
    u"border-right-color",
    u"border-bottom-color",
    u"border-left-color",
    u"border-top-style",
    u"border-right-style",
    u"border-bottom-style",
    u"border-left-style",
    u"border-top-width",
    u"border-right-width",
    u"border-bottom-width",
    u"border-left-width",
    u"border-width",
    u"bottom",
    u"caption-side",
    u"clear",
    u"clip",
    u"color",
    u"content",
    u"counter-increment",
    u"counter-reset",
    u"cue",
    u"cue-after",
    u"cue-before",
    u"cursor",
    u"direction",
    u"display",
    u"elevation",
    u"empty-cells",
    u"float",
    u"font",
    u"font-family",
    u"font-size",
    u"font-size-adjust",
    u"font-stretch",
    u"font-style",
    u"font-variant",
    u"font-weight",
    u"height",
    u"left",
    u"letter-spacing",
    u"line-height",
    u"list-style",
    u"list-style-image",
    u"list-style-position",
    u"list-style-type",
    u"margin",
    u"margin-top",
    u"margin-right",
    u"margin-bottom",
    u"margin-left",
    u"marker-offset",
    u"marks",
    u"max-height",
    u"max-width",
    u"min-height",
    u"min-width",
    u"orphans",
    u"outline",
    u"outline-color",
    u"outline-style",
    u"outline-width",
    u"overflow",
    u"padding",
    u"padding-top",
    u"padding-right",
    u"padding-bottom",
    u"padding-left",
    u"page",
    u"page-break-after",
    u"page-break-before",
    u"page-break-inside",
    u"pause",
    u"pause-after",
    u"pause-before",
    u"pitch",
    u"pitch-range",
    u"play-during",
    u"position",
    u"quotes",
    u"richness",
    u"right",
    u"size",
    u"speak",
    u"speak-header",
    u"speak-numeral",
    u"speak-punctuation",
    u"speech-rate",
    u"stress",
    u"table-layout",
    u"text-align",
    u"text-decoration",
    u"text-indent",
    u"text-shadow",
    u"text-transform",
    u"top",
    u"unicode-bidi",
    u"vertical-align",
    u"visibility",
    u"voice-family",
    u"volume",
    u"white-space",
    u"widows",
    u"width",
    u"word-spacing",
    u"z-index",

    u"binding",
};

CSSPropertyValueImp* CSSStyleDeclarationImp::getProperty(unsigned id)
{
    switch (id) {
    case Top:
        return &top;
    case Right:
        return &right;
    case Left:
        return &left;
    case Bottom:
        return &bottom;
    case Width:
        return &width;
    case Height:
        return &height;
    case BackgroundAttachment:
        return &backgroundAttachment;
    case BackgroundColor:
        return &backgroundColor;
    case BackgroundImage:
        return &backgroundImage;
    case BackgroundPosition:
        return &backgroundPosition;
    case BackgroundRepeat:
        return &backgroundRepeat;
    case Background:
        return &background;
    case BorderCollapse:
        return &borderCollapse;
    case BorderSpacing:
        return &borderSpacing;
    case BorderTopColor:
        return &borderTopColor;
    case BorderRightColor:
        return &borderRightColor;
    case BorderBottomColor:
        return &borderBottomColor;
    case BorderLeftColor:
        return &borderLeftColor;
    case BorderColor:
        return &borderColor;
    case BorderTopStyle:
        return &borderTopStyle;
    case BorderRightStyle:
        return &borderRightStyle;
    case BorderBottomStyle:
        return &borderBottomStyle;
    case BorderLeftStyle:
        return &borderLeftStyle;
    case BorderStyle:
        return &borderStyle;
    case BorderTopWidth:
        return &borderTopWidth;
    case BorderRightWidth:
        return &borderRightWidth;
    case BorderBottomWidth:
        return &borderBottomWidth;
    case BorderLeftWidth:
        return &borderLeftWidth;
    case BorderWidth:
        return &borderWidth;
    case BorderTop:
        return &borderTop;
    case BorderRight:
        return &borderRight;
    case BorderBottom:
        return &borderBottom;
    case BorderLeft:
        return &borderLeft;
    case Border:
        return &border;
    case CaptionSide:
        return &captionSide;
    case Clear:
        return &clear;
    case Color:
        return &color;
    case Content:
        return &content;
    case CounterIncrement:
        return &counterIncrement;
    case CounterReset:
        return &counterReset;
    case Direction:
        return &direction;
    case Display:
        return &display;
    case Float:
        return &float_;
    case FontFamily:
        return &fontFamily;
    case FontSize:
        return &fontSize;
    case FontStyle:
        return &fontStyle;
    case FontVariant:
        return &fontVariant;
    case FontWeight:
        return &fontWeight;
    case Font:
        return &font;
    case LineHeight:
        return &lineHeight;
    case ListStyleType:
        return &listStyleType;
    case Margin:
        return &margin;
    case MarginTop:
        return &marginTop;
    case MarginRight:
        return &marginRight;
    case MarginBottom:
        return &marginBottom;
    case MarginLeft:
        return &marginLeft;
    case MaxHeight:
        return &maxHeight;
    case MaxWidth:
        return &maxWidth;
    case MinHeight:
        return &minHeight;
    case MinWidth:
        return &minWidth;
    case Overflow:
        return &overflow;
    case PaddingTop:
        return &paddingTop;
    case PaddingRight:
        return &paddingRight;
    case PaddingBottom:
        return &paddingBottom;
    case PaddingLeft:
        return &paddingLeft;
    case Padding:
        return &padding;
    case PageBreakAfter:
        return &pageBreakAfter;
    case PageBreakBefore:
        return &pageBreakBefore;
    case PageBreakInside:
        return &pageBreakInside;
    case Position:
        return &position;
    case TableLayout:
        return &tableLayout;
    case TextAlign:
        return &textAlign;
    case TextDecoration:
        return &textDecoration;
    case TextIndent:
        return &textIndent;
    case TextTransform:
        return &textTransform;
    case UnicodeBidi:
        return &unicodeBidi;
    case VerticalAlign:
        return &verticalAlign;
    case Visibility:
        return &visibility;
    case WhiteSpace:
        return &whiteSpace;
    case ZIndex:
        return &zIndex;
    case Binding:
        return &binding;
    case HtmlAlign:
        return &htmlAlign;
    default:
        return 0;
    }
}

void CSSStyleDeclarationImp::setInherit(unsigned id)
{
    inheritSet.set(id);
    switch (id) {
    case Background:
        setInherit(BackgroundAttachment);
        setInherit(BackgroundColor);
        setInherit(BackgroundImage);
        setInherit(BackgroundPosition);
        setInherit(BackgroundRepeat);
        break;
    case BorderColor:
        setInherit(BorderTopColor);
        setInherit(BorderRightColor);
        setInherit(BorderBottomColor);
        setInherit(BorderLeftColor);
        break;
    case BorderStyle:
        setInherit(BorderTopStyle);
        setInherit(BorderRightStyle);
        setInherit(BorderBottomStyle);
        setInherit(BorderLeftStyle);
        break;
    case BorderWidth:
        setInherit(BorderTopWidth);
        setInherit(BorderRightWidth);
        setInherit(BorderBottomWidth);
        setInherit(BorderLeftWidth);
        break;
    case BorderTop:
        setInherit(BorderTopColor);
        setInherit(BorderTopStyle);
        setInherit(BorderTopWidth);
        break;
    case BorderRight:
        setInherit(BorderRightColor);
        setInherit(BorderRightStyle);
        setInherit(BorderRightWidth);
        break;
    case BorderBottom:
        setInherit(BorderBottomColor);
        setInherit(BorderBottomStyle);
        setInherit(BorderBottomWidth);
        break;
    case BorderLeft:
        setInherit(BorderLeftColor);
        setInherit(BorderLeftStyle);
        setInherit(BorderLeftWidth);
        break;
    case Border:
        setInherit(BorderColor);
        setInherit(BorderStyle);
        setInherit(BorderWidth);
        break;
    case Font:
        setInherit(FontStyle);
        setInherit(FontVariant);
        setInherit(FontWeight);
        setInherit(FontSize);
        setInherit(LineHeight);
        setInherit(FontFamily);
        break;
    case Margin:
        setInherit(MarginTop);
        setInherit(MarginRight);
        setInherit(MarginBottom);
        setInherit(MarginLeft);
        break;
    case Padding:
        setInherit(PaddingTop);
        setInherit(PaddingRight);
        setInherit(PaddingBottom);
        setInherit(PaddingLeft);
        break;
    default:
        break;
    }
}

void CSSStyleDeclarationImp::resetInherit(unsigned id)
{
    inheritSet.reset(id);
    switch (id) {
    case Background:
        resetInherit(BackgroundAttachment);
        resetInherit(BackgroundColor);
        resetInherit(BackgroundImage);
        resetInherit(BackgroundPosition);
        resetInherit(BackgroundRepeat);
        break;
    case BorderColor:
        resetInherit(BorderTopColor);
        resetInherit(BorderRightColor);
        resetInherit(BorderBottomColor);
        resetInherit(BorderLeftColor);
        break;
    case BorderStyle:
        resetInherit(BorderTopStyle);
        resetInherit(BorderRightStyle);
        resetInherit(BorderBottomStyle);
        resetInherit(BorderLeftStyle);
        break;
    case BorderWidth:
        resetInherit(BorderTopWidth);
        resetInherit(BorderRightWidth);
        resetInherit(BorderBottomWidth);
        resetInherit(BorderLeftWidth);
        break;
    case BorderTop:
        resetInherit(BorderTopColor);
        resetInherit(BorderTopStyle);
        resetInherit(BorderTopWidth);
        break;
    case BorderRight:
        resetInherit(BorderRightColor);
        resetInherit(BorderRightStyle);
        resetInherit(BorderRightWidth);
        break;
    case BorderBottom:
        resetInherit(BorderBottomColor);
        resetInherit(BorderBottomStyle);
        resetInherit(BorderBottomWidth);
        break;
    case BorderLeft:
        resetInherit(BorderLeftColor);
        resetInherit(BorderLeftStyle);
        resetInherit(BorderLeftWidth);
        break;
    case Border:
        resetInherit(BorderColor);
        resetInherit(BorderStyle);
        resetInherit(BorderWidth);
        break;
    case Font:
        resetInherit(FontStyle);
        resetInherit(FontVariant);
        resetInherit(FontWeight);
        resetInherit(FontSize);
        resetInherit(LineHeight);
        resetInherit(FontFamily);
        break;
    case Margin:
        resetInherit(MarginTop);
        resetInherit(MarginRight);
        resetInherit(MarginBottom);
        resetInherit(MarginLeft);
        break;
    case Padding:
        resetInherit(PaddingTop);
        resetInherit(PaddingRight);
        resetInherit(PaddingBottom);
        resetInherit(PaddingLeft);
        break;
    default:
        break;
    }
}

void CSSStyleDeclarationImp::setImportant(unsigned id)
{
    importantSet.set(id);
    propertySet.reset(id);
}

void CSSStyleDeclarationImp::resetImportant(unsigned id)
{
    importantSet.reset(id);
}

void CSSStyleDeclarationImp::setProperty(unsigned id)
{
    propertySet.set(id);
    importantSet.reset(id);
}

void CSSStyleDeclarationImp::resetProperty(unsigned id)
{
    propertySet.reset(id);
}

bool CSSStyleDeclarationImp::setProperty(int id, CSSParserExpr* expr, const std::u16string& prio)
{
    assert(expr);
    if (id == Unknown) {
        // TODO: delete expr; ?
        return false;
    }
    if (expr->isInherit())
        setInherit(id);
    else {
        CSSValueParser parser(id);
        if (!parser.isValid(expr)) {
            // TODO: delete expr; ?
            return false;
        }
        CSSPropertyValueImp* property = getProperty(id);
        if (!property) {
            // TODO: delete expr; ?
            return false;
        }
        property->setValue(this, &parser);
        resetInherit(id);
    }
    if (prio == u"!important")
        setImportant(id);
    else
        setProperty(id);
    return true;
}

bool CSSStyleDeclarationImp::setProperty(std::u16string property, CSSParserExpr* expr, const std::u16string& prio)
{
    if (!expr)
        return false;
    toLower(property);
    return setProperty(getPropertyID(property), expr, prio);
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationImp* decl, unsigned id)
{
    switch (id) {
    case Top:
        top.specify(decl->top);
        break;
    case Right:
        right.specify(decl->right);
        break;
    case Left:
        left.specify(decl->left);
        break;
    case Bottom:
        bottom.specify(decl->bottom);
        break;
    case Width:
        width.specify(decl->width);
        break;
    case Height:
        height.specify(decl->height);
        break;
    case BackgroundAttachment:
        backgroundAttachment.specify(decl->backgroundAttachment);
        break;
    case BackgroundColor:
        backgroundColor.specify(decl->backgroundColor);
        break;
    case BackgroundImage:
        backgroundImage.specify(decl->backgroundImage);
        break;
    case BackgroundPosition:
        backgroundPosition.specify(decl->backgroundPosition);
        break;
    case BackgroundRepeat:
        backgroundRepeat.specify(decl->backgroundRepeat);
        break;
    case Background:
        background.specify(this, decl);
        break;
    case BorderCollapse:
        borderCollapse.specify(decl->borderCollapse);
        break;
    case BorderSpacing:
        borderSpacing.specify(decl->borderSpacing);
        break;
    case BorderTopColor:
        borderTopColor.specify(decl->borderTopColor);
        break;
    case BorderRightColor:
        borderRightColor.specify(decl->borderRightColor);
        break;
    case BorderBottomColor:
        borderBottomColor.specify(decl->borderBottomColor);
        break;
    case BorderLeftColor:
        borderLeftColor.specify(decl->borderLeftColor);
        break;
    case BorderColor:
        borderColor.specify(this, decl);
        break;
    case BorderTopStyle:
        borderTopStyle.specify(decl->borderTopStyle);
        break;
    case BorderRightStyle:
        borderRightStyle.specify(decl->borderRightStyle);
        break;
    case BorderBottomStyle:
        borderBottomStyle.specify(decl->borderBottomStyle);
        break;
    case BorderLeftStyle:
        borderLeftStyle.specify(decl->borderLeftStyle);
        break;
    case BorderStyle:
        borderStyle.specify(this, decl);
        break;
    case BorderTopWidth:
        borderTopWidth.specify(decl->borderTopWidth);
        break;
    case BorderRightWidth:
        borderRightWidth.specify(decl->borderRightWidth);
        break;
    case BorderBottomWidth:
        borderBottomWidth.specify(decl->borderBottomWidth);
        break;
    case BorderLeftWidth:
        borderLeftWidth.specify(decl->borderLeftWidth);
        break;
    case BorderWidth:
        borderWidth.specify(this, decl);
        break;
    case BorderTop:
        borderTop.specify(this, decl);
        break;
    case BorderRight:
        borderRight.specify(this, decl);
        break;
    case BorderBottom:
        borderBottom.specify(this, decl);
        break;
    case BorderLeft:
        borderLeft.specify(this, decl);
        break;
    case Border:
        border.specify(this, decl);
        break;
    case CaptionSide:
        captionSide.specify(decl->captionSide);
        break;
    case Clear:
        clear.specify(decl->clear);
        break;
    case Color:
        color.specify(decl->color);
        break;
    case Content:
        content.specify(decl->content);
        break;
    case CounterIncrement:
        counterIncrement.specify(decl->counterIncrement);
        break;
    case CounterReset:
        counterReset.specify(decl->counterReset);
        break;
    case Direction:
        direction.specify(decl->direction);
        break;
    case Display:
        display.specify(decl->display);
        break;
    case Float:
        float_.specify(decl->float_);
        break;
    case FontFamily:
        fontFamily.specify(decl->fontFamily);
        break;
    case FontSize:
        fontSize.specify(decl->fontSize);
        break;
    case FontStyle:
        fontStyle.specify(decl->fontStyle);
        break;
    case FontVariant:
        fontVariant.specify(decl->fontVariant);
        break;
    case FontWeight:
        fontWeight.specify(decl->fontWeight);
        break;
    case Font:
        font.specify(this, decl);
        break;
    case LineHeight:
        lineHeight.specify(decl->lineHeight);
        break;
    case ListStyleType:
        listStyleType.specify(decl->listStyleType);
        break;
    case Margin:
        margin.specify(this, decl);
        break;
    case MarginTop:
        marginTop.specify(decl->marginTop);
        break;
    case MarginRight:
        marginRight.specify(decl->marginRight);
        break;
    case MarginBottom:
        marginBottom.specify(decl->marginBottom);
        break;
    case MarginLeft:
        marginLeft.specify(decl->marginLeft);
        break;
    case MaxHeight:
        maxHeight.specify(decl->maxHeight);
        break;
    case MaxWidth:
        maxWidth.specify(decl->maxWidth);
        break;
    case MinHeight:
        minHeight.specify(decl->minHeight);
        break;
    case MinWidth:
        minWidth.specify(decl->minWidth);
        break;
    case Overflow:
        overflow.specify(decl->overflow);
        break;
    case PaddingTop:
        paddingTop.specify(decl->paddingTop);
        break;
    case PaddingRight:
        paddingRight.specify(decl->paddingRight);
        break;
    case PaddingBottom:
        paddingBottom.specify(decl->paddingBottom);
        break;
    case PaddingLeft:
        paddingLeft.specify(decl->paddingLeft);
        break;
    case Padding:
        padding.specify(this, decl);
        break;
    case PageBreakAfter:
        pageBreakAfter.specify(decl->pageBreakAfter);
        break;
    case PageBreakBefore:
        pageBreakBefore.specify(decl->pageBreakBefore);
        break;
    case PageBreakInside:
        pageBreakInside.specify(decl->pageBreakInside);
        break;
    case Position:
        position.specify(decl->position);
        break;
    case TableLayout:
        tableLayout.specify(decl->tableLayout);
        break;
    case TextAlign:
        textAlign.specify(decl->textAlign);
        break;
    case TextDecoration:
        textDecoration.specify(decl->textDecoration);
        break;
    case TextIndent:
        textIndent.specify(decl->textIndent);
        break;
    case TextTransform:
        textTransform.specify(decl->textTransform);
        break;
    case UnicodeBidi:
        unicodeBidi.specify(decl->unicodeBidi);
        break;
    case VerticalAlign:
        verticalAlign.specify(decl->verticalAlign);
        break;
    case Visibility:
        visibility.specify(decl->visibility);
        break;
    case WhiteSpace:
        whiteSpace.specify(decl->whiteSpace);
        break;
    case ZIndex:
        zIndex.specify(decl->zIndex);
        break;
    case Binding:
        binding.specify(decl->binding);
        break;
    case HtmlAlign:
        htmlAlign.specify(decl->htmlAlign);
        break;
    default:
        break;
    }
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationImp* decl, const std::bitset<PropertyCount>& set)
{
    for (unsigned id = 1; id < MaxProperties; ++id) {
        if (!set.test(id))
            continue;
        if (decl->inheritSet.test(id))
            setInherit(id);
        else {
            resetInherit(id);
            specify(decl, id);
        }
        propertySet.set(id);  // Note computed values do not have any important bit set.
    }
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationImp* style)
{
    if (style)
        specify(style, style->propertySet);
}

void CSSStyleDeclarationImp::specifyImportant(const CSSStyleDeclarationImp* style)
{
    if (style)
        specify(style, style->importantSet);
}

void CSSStyleDeclarationImp::reset(unsigned id)
{
    switch (id) {
    case Top:
        top.setValue();
        break;
    case Right:
        right.setValue();
        break;
    case Left:
        left.setValue();
        break;
    case Bottom:
        bottom.setValue();
        break;
    case Width:
        width.setValue();
        break;
    case Height:
        height.setValue();
        break;
    case BackgroundAttachment:
        backgroundAttachment.setValue();
        break;
    case BackgroundColor:
        backgroundColor.setValue(static_cast<unsigned int>(0x00000000));  // TODO
        break;
    case BackgroundImage:
        backgroundImage.setValue();
        break;
    case BackgroundPosition:
        backgroundPosition.setValue();
        break;
    case BackgroundRepeat:
        backgroundRepeat.setValue();
        break;
    case Background:
        reset(BackgroundAttachment);
        reset(BackgroundColor);
        reset(BackgroundImage);
        reset(BackgroundPosition);
        reset(BackgroundRepeat);
        break;
    case BorderCollapse:
        borderCollapse.setValue();
        break;
    case BorderSpacing:
        borderSpacing.setValue();
        break;
    case BorderTopColor:
        borderTopColor.reset();
        break;
    case BorderRightColor:
        borderRightColor.reset();
        break;
    case BorderBottomColor:
        borderBottomColor.reset();
        break;
    case BorderLeftColor:
        borderLeftColor.reset();
        break;
    case BorderColor:
        reset(BorderTopColor);
        reset(BorderRightColor);
        reset(BorderBottomColor);
        reset(BorderLeftColor);
        break;
    case BorderTopStyle:
        borderTopStyle.setValue();
        break;
    case BorderRightStyle:
        borderRightStyle.setValue();
        break;
    case BorderBottomStyle:
        borderBottomStyle.setValue();
        break;
    case BorderLeftStyle:
        borderLeftStyle.setValue();
        break;
    case BorderStyle:
        reset(BorderTopStyle);
        reset(BorderRightStyle);
        reset(BorderBottomStyle);
        reset(BorderLeftStyle);
        break;
    case BorderTopWidth:
        borderTopWidth.setValue();
        break;
    case BorderRightWidth:
        borderRightWidth.setValue();
        break;
    case BorderBottomWidth:
        borderBottomWidth.setValue();
        break;
    case BorderLeftWidth:
        borderLeftWidth.setValue();
        break;
    case BorderWidth:
        reset(BorderTopWidth);
        reset(BorderRightWidth);
        reset(BorderBottomWidth);
        reset(BorderLeftWidth);
        break;
    case BorderTop:
        reset(BorderTopWidth);
        reset(BorderTopStyle);
        reset(BorderTopColor);
        break;
    case BorderRight:
        reset(BorderRightWidth);
        reset(BorderRightStyle);
        reset(BorderRightColor);
        break;
    case BorderBottom:
        reset(BorderBottomWidth);
        reset(BorderBottomStyle);
        reset(BorderBottomColor);
        break;
    case BorderLeft:
        reset(BorderLeftWidth);
        reset(BorderLeftStyle);
        reset(BorderLeftColor);
        break;
    case Border:
        reset(BorderColor);
        reset(BorderStyle);
        reset(BorderWidth);
        break;
    case CaptionSide:
        captionSide.setValue();
        break;
    case Clear:
        clear.setValue();
        break;
    case Color:
        color.setValue();
        break;
    case Content:
        content.reset();
        break;
    case CounterIncrement:
        counterIncrement.reset();
        break;
    case CounterReset:
        counterReset.reset();
        break;
    case Direction:
        direction.setValue();
        break;
    case Display:
        display.setValue();
        break;
    case Float:
        float_.setValue();
        break;
    case FontFamily:
        fontFamily.reset();
        break;
    case FontSize:
        fontSize.setValue();
        break;
    case FontStyle:
        fontStyle.setValue();
        break;
    case FontVariant:
        fontVariant.setValue();
        break;
    case FontWeight:
        fontWeight.setValue();
        break;
    case Font:
        font.reset(this);
        break;
    case LineHeight:
        lineHeight.setValue();
        break;
    case ListStyleType:
        listStyleType.setValue();
        break;
    case Margin:
        reset(MarginTop);
        reset(MarginRight);
        reset(MarginBottom);
        reset(MarginLeft);
        break;
    case MarginTop:
        marginTop.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case MarginRight:
        marginRight.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case MarginBottom:
        marginBottom.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case MarginLeft:
        marginLeft.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case MaxHeight:
        maxHeight.setValue();
        break;
    case MaxWidth:
        maxWidth.setValue();
        break;
    case MinHeight:
        minHeight.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case MinWidth:
        minWidth.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case Overflow:
        overflow.setValue();
        break;
    case PaddingTop:
        paddingTop.setValue();
        break;
    case PaddingRight:
        paddingRight.setValue();
        break;
    case PaddingBottom:
        paddingBottom.setValue();
        break;
    case PaddingLeft:
        paddingLeft.setValue();
        break;
    case Padding:
        reset(PaddingTop);
        reset(PaddingRight);
        reset(PaddingBottom);
        reset(PaddingLeft);
        break;
    case PageBreakAfter:
        pageBreakAfter.setValue();
        break;
    case PageBreakBefore:
        pageBreakBefore.setValue();
        break;
    case PageBreakInside:
        pageBreakInside.setValue();
        break;
    case Position:
        position.setValue();
        break;
    case TableLayout:
        tableLayout.setValue();
        break;
    case TextAlign:
        textAlign.setValue();
        break;
    case TextDecoration:
        textDecoration.setValue();
        break;
    case TextIndent:
        textIndent.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
        break;
    case TextTransform:
        textTransform.setValue();
        break;
    case UnicodeBidi:
        unicodeBidi.setValue();
        break;
    case VerticalAlign:
        verticalAlign.setValue();
        break;
    case Visibility:
        visibility.setValue();
        break;
    case WhiteSpace:
        whiteSpace.setValue();
        break;
    case ZIndex:
        zIndex.setValue();
        break;
    case Binding:
        binding.setValue();
        break;
    case HtmlAlign:
        htmlAlign.setValue();
    default:
        break;
    }
}

void CSSStyleDeclarationImp::resetInheritedProperties()
{
    for (unsigned id = 1; id < MaxProperties; ++id) {
        if (!inheritSet.test(id))
            continue;
        switch (id) {
        case Background:
        case BorderColor:
        case BorderStyle:
        case BorderWidth:
        case BorderTop:
        case BorderRight:
        case BorderBottom:
        case BorderLeft:
        case Border:
        case Font:  // TODO
        case Margin:
        case Padding:
            // ignore shorthand
            break;
        default:
            reset(id);
            break;
        }
    }
}

void CSSStyleDeclarationImp::copy(const CSSStyleDeclarationImp* parentStyle, unsigned id)
{
    assert(parentStyle);
    if (!inheritSet.test(id))
        return;
    switch (id) {
    case Background:
    case BorderColor:
    case BorderStyle:
    case BorderWidth:
    case BorderTop:
    case BorderRight:
    case BorderBottom:
    case BorderLeft:
    case Border:
    case Font:  // TODO
    case Margin:
    case Padding:
        // ignore shorthand
        break;
    default:
        specify(parentStyle, id);
        break;
    }
}

void CSSStyleDeclarationImp::copyInheritedProperties(const CSSStyleDeclarationImp* parentStyle)
{
    assert(parentStyle);
    for (unsigned id = 1; id < MaxProperties; ++id)
        copy(parentStyle, id);
}

void CSSStyleDeclarationImp::compute(ViewCSSImp* view, CSSStyleDeclarationImp* parentStyle, Element element)
{
    if (!parentStyle)  // is it the root element?
        resetInheritedProperties();
    else
        copyInheritedProperties(parentStyle);
    display.compute(this, element);
    fontSize.compute(view, parentStyle);
    fontWeight.compute(view, parentStyle);
    fontTexture = view->selectFont(this);
    lineHeight.compute(view, this);
    verticalAlign.compute(view, this);
    if (position.getValue() == CSSPositionValueImp::Static)
        left = right = top = bottom.setValue();  // set to 'auto'
    if (element.getLocalName() == u"body") {  // TODO: check HTML namespace
        // TODO: do not update computed values here, just *use* them.
        assert(parentStyle);
        if (parentStyle->overflow.getValue() == CSSOverflowValueImp::Visible) {
            parentStyle->overflow.specify(overflow);
            overflow.setValue(CSSOverflowValueImp::Visible);
        }
        if (parentStyle->backgroundColor.getARGB() == 0 &&  // transparent?
            parentStyle->backgroundImage.isNone()) {
            parentStyle->background.specify(parentStyle, this);
            background.reset(this);
        }
    }

    width.compute(view, this);
    height.compute(view, this);

    top.compute(view, this);
    right.compute(view, this);
    bottom.compute(view, this);
    left.compute(view, this);

    marginTop.compute(view, this);
    marginRight.compute(view, this);
    marginBottom.compute(view, this);
    marginLeft.compute(view, this);

    // CSS3 would allow percentages in border width, the resolved values of these should be the used values, too.
    borderTopWidth.compute(view, borderTopStyle, this);
    borderRightWidth.compute(view, borderRightStyle, this);
    borderBottomWidth.compute(view, borderBottomStyle, this);
    borderLeftWidth.compute(view, borderLeftStyle, this);

    paddingTop.compute(view, this);
    paddingRight.compute(view, this);
    paddingBottom.compute(view, this);
    paddingLeft.compute(view, this);

    borderTopColor.compute(this);
    borderRightColor.compute(this);
    borderBottomColor.compute(this);
    borderLeftColor.compute(this);

    if (isFloat() || isAbsolutelyPositioned() || !parentStyle)  // TODO or the contents of atomic inline-level descendants such as inline blocks and inline tables.
        textDecorationContext.update(this);
    else if (parentStyle->textDecorationContext.hasDecoration())
        textDecorationContext = parentStyle->textDecorationContext;
    else
        textDecorationContext.update(this);

    if (!parentStyle) {
        assert(view->getStackingContexts() == 0);
        view->setStackingContexts(new(std::nothrow) StackingContext(false, zIndex.getValue()));
        stackingContext = view->getStackingContexts();
    } else if (isPositioned()) {
        if (zIndex.isAuto())
            stackingContext = parentStyle->stackingContext->getAuto();
        else
            stackingContext = parentStyle->stackingContext->addContext(zIndex.getValue());
    } else
        stackingContext = parentStyle->stackingContext;

    // Note the parent style of a pseudo element style is not always the corresponding element's style.
    // It will be computed layter by layout().
}

// calculate resolved values that requite containing block information for calucuration
// cf. CSSOM 7. Resolved Values
// TODO: make sure this function is not called again and again
void CSSStyleDeclarationImp::resolve(ViewCSSImp* view, const ContainingBlock* containingBlock, Element element)
{
    if (resolved)
        return;

    if (Element parentElement = element.getParentElement()) {
        if (CSSStyleDeclarationImp* parentStyle = view->getStyle(parentElement)) {
            // TODO: Refine
            if (!propertySet.test(Margin) && !propertySet.test(MarginLeft) && !propertySet.test(MarginRight) &&
                !inheritSet.test(Margin) && !inheritSet.test(MarginLeft) && !inheritSet.test(MarginRight)) {
                switch (parentStyle->htmlAlign.getValue()) {
                case HTMLAlignValueImp::Left:
                    marginLeft.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
                    marginRight.setValue();
                    break;
                case HTMLAlignValueImp::Center:
                    marginLeft.setValue();
                    marginRight.setValue();
                    break;
                case HTMLAlignValueImp::Right:
                    marginLeft.setValue();
                    marginRight.setValue(0.0f, css::CSSPrimitiveValue::CSS_PX);
                    break;
                }
            }
            if (!propertySet.test(TextAlign) && !inheritSet.test(TextAlign)) {
                switch (htmlAlign.getValue()) {
                case HTMLAlignValueImp::Left:
                    textAlign.setValue(CSSTextAlignValueImp::Left);
                    break;
                case HTMLAlignValueImp::Center:
                    textAlign.setValue(CSSTextAlignValueImp::Center);
                    break;
                case HTMLAlignValueImp::Right:
                    textAlign.setValue(CSSTextAlignValueImp::Right);
                    break;
                }
            }
        }
    }

    // Recompute properties that depend on the containing block size
    width.resolve(view, this, containingBlock->width);
    if (containingBlock->height == 0 && height.isPercentage())  // TODO: check more conditions
        height.setValue();  // change height to 'auto'.
    else
        height.resolve(view, this, containingBlock->height);  // TODO: check more conditions

    marginTop.resolve(view, this, containingBlock->width);
    marginRight.resolve(view, this, containingBlock->width);
    marginBottom.resolve(view, this, containingBlock->width);
    marginLeft.resolve(view, this, containingBlock->width);

    paddingTop.resolve(view, this, containingBlock->width);
    paddingRight.resolve(view, this, containingBlock->width);
    paddingBottom.resolve(view, this, containingBlock->width);
    paddingLeft.resolve(view, this, containingBlock->width);

    left.resolve(view, this, containingBlock->width);
    right.resolve(view, this, containingBlock->width);
    top.resolve(view, this, containingBlock->height);
    bottom.resolve(view, this, containingBlock->height);

    minHeight.resolve(view, this, containingBlock->height);
    minWidth.resolve(view, this, containingBlock->width);
    maxHeight.resolve(view, this, containingBlock->height);
    maxWidth.resolve(view, this, containingBlock->width);

    resolved = true;
}

size_t CSSStyleDeclarationImp::processWhiteSpace(std::u16string& data, char16_t& prevChar)
{
    unsigned prop = whiteSpace.getValue();
    switch (prop) {
    case CSSWhiteSpaceValueImp::Normal:
    case CSSWhiteSpaceValueImp::Nowrap:
    case CSSWhiteSpaceValueImp::PreLine:
        for (int i = 0; i < data.length();) {
            char16_t c = data[i];
            if (c == '\n') {  // linefeed
                int j;
                for (j = i - 1; 0 <= j && isSpace(data[j]); --j)
                    ;
                ++j;
                if (j < i) {
                    data.erase(j, i - j);
                    i = j;
                }
                for (j = i + 1; j < data.length() && isSpace(data[j]); ++j)
                    ;
                --j;
                if (i < j)
                    data.erase(i + 1, j - i);
                switch (prop) {
                case CSSWhiteSpaceValueImp::Normal:
                case CSSWhiteSpaceValueImp::Nowrap:
                    c = data[i] = ' ';
                    break;
                }
            }
            // tab, and space following another space
            if (c == '\t')
                data[i] = ' ';
            if (c == ' ' && prevChar == ' ') {
                data.erase(i, 1);
                continue;  // do not increment i.
            }
            prevChar = c;
            ++i;
        }
        break;
    default:
        prevChar = 0;
        break;
    }
    return data.length();
}

size_t CSSStyleDeclarationImp::processLineHeadWhiteSpace(std::u16string& data)
{
    std::u16string::iterator i;
    switch (whiteSpace.getValue()) {
    case CSSWhiteSpaceValueImp::Normal:
    case CSSWhiteSpaceValueImp::Nowrap:
    case CSSWhiteSpaceValueImp::PreLine:
        i = data.begin();
        while (i != data.end() && isSpace(*i))
            ++i;
        if (i != data.begin())
            i = data.erase(data.begin(), i);
        break;
    default:
        break;
    }
    return data.length();
}

bool CSSStyleDeclarationImp::isFlowRoot() const
{
    return float_.getValue() != CSSFloatValueImp::None ||
           overflow.getValue() != CSSOverflowValueImp::Visible ||
           position.getValue() != CSSPositionValueImp::Static && position.getValue() != CSSPositionValueImp::Relative;
           /* TODO || and more conditions... */
}

CSSStyleDeclarationImp* CSSStyleDeclarationImp::getPseudoElementStyle(int id)
{
    assert(0 <= id && id < CSSPseudoElementSelector::MaxPseudoElements);
    return pseudoElements[id].get();
}

CSSStyleDeclarationImp* CSSStyleDeclarationImp::getPseudoElementStyle(const std::u16string& name)
{
    int id = CSSPseudoElementSelector::getPseudoElementID(name);
    if (0 <= id)
        return pseudoElements[id].get();
    return 0;
}

CSSStyleDeclarationImp* CSSStyleDeclarationImp::createPseudoElementStyle(int id) {
    assert(0 <= id && id < CSSPseudoElementSelector::MaxPseudoElements);
    CSSStyleDeclarationImp* style = pseudoElements[id].get();
    if (!style) {
        if (style = new(std::nothrow) CSSStyleDeclarationImp)
            pseudoElements[id] = style;
    }
    return style;
}

//
// CSSStyleDeclaration
//

std::u16string CSSStyleDeclarationImp::getCssText()
{
    std::u16string text;
    std::u16string separator;
    for (size_t i = 0; i < MaxCSSProperties; ++i) {
        if (propertySet.test(i) || importantSet.test(i)) {
            if (inheritSet.test(i))
                text += separator + getPropertyName(i) + u": inherit";
            else if (CSSPropertyValueImp* property = getProperty(i)) {
                text += separator + getPropertyName(i) + u": ";
                text += property->getCssText(this);
            } else {
                continue;
            }
            if (importantSet.test(i))
                text += u" !important";
            separator = u"; ";
        }

    }
    return text;
}

void CSSStyleDeclarationImp::setCssText(std::u16string cssText)
{
    CSSParser parser;
    parser.setStyleDeclaration(this);
    parser.parseDeclarations(cssText);
}

unsigned int CSSStyleDeclarationImp::getLength()
{
    return importantSet.count() + propertySet.count();
}

std::u16string CSSStyleDeclarationImp::item(unsigned int index)
{
    if (inheritSet.test(index))
        return u"inherit";
    if (propertySet.test(index))
        return getProperty(index)->getCssText(this);
    return u"";
}

std::u16string CSSStyleDeclarationImp::getPropertyValue(std::u16string property)
{
    toLower(property);
    return item(getPropertyID(property));
}

std::u16string CSSStyleDeclarationImp::getPropertyPriority(std::u16string property)
{
    toLower(property);
    if (importantSet.test(getPropertyID(property)))
        return u"important";
    else
        return u"";
}

void CSSStyleDeclarationImp::setProperty(int id, Nullable<std::u16string> value, const std::u16string& prio)
{
    if (!value.hasValue()) {
        removeProperty(id);
        return;
    }
    std::u16string v = value.value();
    stripLeadingAndTrailingWhitespace(v);
    if (v == u"") {
        removeProperty(id);
        return;
    }
    CSSParser parser;
    CSSParserExpr* expr = parser.parseExpression(v);
    if (expr) {
        setProperty(id, expr, prio);
        delete expr;
        if (owner) {
            events::EventTarget target(owner);
            events::MutationEvent event = new(std::nothrow) MutationEventImp;
            // TODO: prev, new
            event.initMutationEvent(u"DOMAttrModified",
                                    true, false, this, u"", u"", getPropertyName(id), events::MutationEvent::MODIFICATION);
            target.dispatchEvent(event);
        }
    }
}

void CSSStyleDeclarationImp::setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value)
{
    if (!property.hasValue())
        return;
    std::u16string propertyName = property.value();
    toLower(propertyName);
    int id = getPropertyID(propertyName);
    if (id == Unknown)
        return;
    setProperty(id, value, u"");
}

void CSSStyleDeclarationImp::setProperty(Nullable<std::u16string> property, Nullable<std::u16string> value, Nullable<std::u16string> priority)
{
    if (!property.hasValue())
        return;
    std::u16string propertyName = property.value();
    toLower(propertyName);
    int id = getPropertyID(propertyName);
    if (id == Unknown)
        return;
    setProperty(id, value, priority.hasValue() ? priority.value() : u"");
}

std::u16string CSSStyleDeclarationImp::removeProperty(int id)
{
    reset(id);
    return u"";  // ask Anne
}

std::u16string CSSStyleDeclarationImp::removeProperty(std::u16string property)
{
    toLower(property);
    return removeProperty(getPropertyID(property));
}

css::CSSStyleDeclarationValue CSSStyleDeclarationImp::getValues()
{
}

css::CSSRule CSSStyleDeclarationImp::getParentRule()
{
    return parentRule;
}

//
// CSS2Properties
//

Nullable<std::u16string> CSSStyleDeclarationImp::getAzimuth()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setAzimuth(Nullable<std::u16string> azimuth)
{
    setProperty(Azimuth, azimuth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackground()
{
    return background.getCssText(this);
}

void CSSStyleDeclarationImp::setBackground(Nullable<std::u16string> background)
{
    setProperty(Background, background);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundAttachment()
{
    return backgroundAttachment.getCssText(this);
}

void CSSStyleDeclarationImp::setBackgroundAttachment(Nullable<std::u16string> backgroundAttachment)
{
    setProperty(BackgroundAttachment, backgroundAttachment);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundColor()
{
    return backgroundColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBackgroundColor(Nullable<std::u16string> backgroundColor)
{
    setProperty(BackgroundColor, backgroundColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundImage()
{
    return backgroundImage.getCssText(this);
}

void CSSStyleDeclarationImp::setBackgroundImage(Nullable<std::u16string> backgroundImage)
{
    setProperty(BackgroundImage, backgroundImage);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundPosition()
{
    return backgroundPosition.getCssText(this);
}

void CSSStyleDeclarationImp::setBackgroundPosition(Nullable<std::u16string> backgroundPosition)
{
    setProperty(BackgroundPosition, backgroundPosition);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundRepeat()
{
    return backgroundRepeat.getCssText(this);
}

void CSSStyleDeclarationImp::setBackgroundRepeat(Nullable<std::u16string> backgroundRepeat)
{
    setProperty(BackgroundRepeat, backgroundRepeat);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorder()
{
    return border.getCssText(this);
}

void CSSStyleDeclarationImp::setBorder(Nullable<std::u16string> border)
{
    setProperty(Border, border);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderCollapse()
{
    return borderCollapse.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderCollapse(Nullable<std::u16string> borderCollapse)
{
    setProperty(BorderCollapse, borderCollapse);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderColor()
{
    return borderColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderColor(Nullable<std::u16string> borderColor)
{
    setProperty(BorderColor, borderColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderSpacing()
{
    return borderSpacing.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderSpacing(Nullable<std::u16string> borderSpacing)
{
    setProperty(BorderSpacing, borderSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderStyle()
{
    return borderStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderStyle(Nullable<std::u16string> borderStyle)
{
    setProperty(BorderStyle, borderStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTop()
{
    return borderTop.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderTop(Nullable<std::u16string> borderTop)
{
    setProperty(BorderTop, borderTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRight()
{
    return borderRight.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderRight(Nullable<std::u16string> borderRight)
{
    setProperty(BorderRight, borderRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottom()
{
    return borderBottom.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderBottom(Nullable<std::u16string> borderBottom)
{
    setProperty(BorderBottom, borderBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeft()
{
    return borderLeft.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderLeft(Nullable<std::u16string> borderLeft)
{
    setProperty(BorderLeft, borderLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopColor()
{
    return borderTopColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderTopColor(Nullable<std::u16string> borderTopColor)
{
    setProperty(BorderTopColor, borderTopColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightColor()
{
    return borderRightColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderRightColor(Nullable<std::u16string> borderRightColor)
{
    setProperty(BorderRightColor, borderRightColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomColor()
{
    return borderBottomColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderBottomColor(Nullable<std::u16string> borderBottomColor)
{
    setProperty(BorderBottomColor, borderBottomColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftColor()
{
    return borderLeftColor.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderLeftColor(Nullable<std::u16string> borderLeftColor)
{
    setProperty(BorderLeftColor, borderLeftColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopStyle()
{
    return borderTopStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderTopStyle(Nullable<std::u16string> borderTopStyle)
{
    setProperty(BorderTopStyle, borderTopStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightStyle()
{
    return borderRightStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderRightStyle(Nullable<std::u16string> borderRightStyle)
{
    setProperty(BorderRightStyle, borderRightStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomStyle()
{
    return borderBottomStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderBottomStyle(Nullable<std::u16string> borderBottomStyle)
{
    setProperty(BorderBottomStyle, borderBottomStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftStyle()
{
    return borderLeftStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderLeftStyle(Nullable<std::u16string> borderLeftStyle)
{
    setProperty(BorderLeftStyle, borderLeftStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopWidth()
{
    return borderTopWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderTopWidth(Nullable<std::u16string> borderTopWidth)
{
    setProperty(BorderTopWidth, borderTopWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightWidth()
{
    return borderRightWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderRightWidth(Nullable<std::u16string> borderRightWidth)
{
    setProperty(BorderRightWidth, borderRightWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomWidth()
{
    return borderBottomWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderBottomWidth(Nullable<std::u16string> borderBottomWidth)
{
    setProperty(BorderBottomWidth, borderBottomWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftWidth()
{
    return borderLeftWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderLeftWidth(Nullable<std::u16string> borderLeftWidth)
{
    setProperty(BorderLeftWidth, borderLeftWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderWidth()
{
    return borderWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setBorderWidth(Nullable<std::u16string> borderWidth)
{
    setProperty(BorderWidth, borderWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBottom()
{
    return bottom.getCssText(this);
}

void CSSStyleDeclarationImp::setBottom(Nullable<std::u16string> bottom)
{
    setProperty(Bottom, bottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCaptionSide()
{
    return captionSide.getCssText(this);
}

void CSSStyleDeclarationImp::setCaptionSide(Nullable<std::u16string> captionSide)
{
    setProperty(CaptionSide, captionSide);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getClear()
{
    return clear.getCssText(this);
}

void CSSStyleDeclarationImp::setClear(Nullable<std::u16string> clear)
{
    setProperty(Clear, clear);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getClip()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setClip(Nullable<std::u16string> clip)
{
    setProperty(Clip, clip);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getColor()
{
    return color.getCssText(this);
}

void CSSStyleDeclarationImp::setColor(Nullable<std::u16string> color)
{
    setProperty(Color, color);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getContent()
{
    return content.getCssText(this);
}

void CSSStyleDeclarationImp::setContent(Nullable<std::u16string> content)
{
    setProperty(Content, content);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCounterIncrement()
{
    return counterIncrement.getCssText(this);
}

void CSSStyleDeclarationImp::setCounterIncrement(Nullable<std::u16string> counterIncrement)
{
    setProperty(CounterIncrement, counterIncrement);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCounterReset()
{
    return counterReset.getCssText(this);
}

void CSSStyleDeclarationImp::setCounterReset(Nullable<std::u16string> counterReset)
{
    setProperty(CounterReset, counterReset);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCue()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCue(Nullable<std::u16string> cue)
{
    setProperty(Cue, cue);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCueAfter()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCueAfter(Nullable<std::u16string> cueAfter)
{
    setProperty(CueAfter, cueAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCueBefore()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCueBefore(Nullable<std::u16string> cueBefore)
{
    setProperty(CueBefore, cueBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCursor()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCursor(Nullable<std::u16string> cursor)
{
    setProperty(Cursor, cursor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getDirection()
{
    return direction.getCssText(this);
}

void CSSStyleDeclarationImp::setDirection(Nullable<std::u16string> direction)
{
    setProperty(Direction, direction);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getDisplay()
{
    return display.getCssText(this);
}

void CSSStyleDeclarationImp::setDisplay(Nullable<std::u16string> display)
{
    setProperty(Display, display);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getElevation()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setElevation(Nullable<std::u16string> elevation)
{
    setProperty(Elevation, elevation);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getEmptyCells()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setEmptyCells(Nullable<std::u16string> emptyCells)
{
    setProperty(EmptyCells, emptyCells);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCssFloat()
{
    return float_.getCssText(this);
}

void CSSStyleDeclarationImp::setCssFloat(Nullable<std::u16string> cssFloat)
{
    setProperty(Float, cssFloat);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFont()
{
    return font.getCssText(this);
}

void CSSStyleDeclarationImp::setFont(Nullable<std::u16string> font)
{
    setProperty(Font, font);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontFamily()
{
    return fontFamily.getCssText(this);
}

void CSSStyleDeclarationImp::setFontFamily(Nullable<std::u16string> fontFamily)
{
    setProperty(FontFamily, fontFamily);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontSize()
{
    return fontSize.getCssText(this);
}

void CSSStyleDeclarationImp::setFontSize(Nullable<std::u16string> fontSize)
{
    setProperty(FontSize, fontSize);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontSizeAdjust()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setFontSizeAdjust(Nullable<std::u16string> fontSizeAdjust)
{
    setProperty(FontSizeAdjust, fontSizeAdjust);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontStretch()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setFontStretch(Nullable<std::u16string> fontStretch)
{
    setProperty(FontStretch, fontStretch);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontStyle()
{
    return fontStyle.getCssText(this);
}

void CSSStyleDeclarationImp::setFontStyle(Nullable<std::u16string> fontStyle)
{
    setProperty(FontStyle, fontStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontVariant()
{
    return fontVariant.getCssText(this);
}

void CSSStyleDeclarationImp::setFontVariant(Nullable<std::u16string> fontVariant)
{
    setProperty(FontVariant, fontVariant);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontWeight()
{
    return fontWeight.getCssText(this);
}

void CSSStyleDeclarationImp::setFontWeight(Nullable<std::u16string> fontWeight)
{
    setProperty(FontWeight, fontWeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getHeight()
{
    return height.getCssText(this);
}

void CSSStyleDeclarationImp::setHeight(Nullable<std::u16string> height)
{
    setProperty(Height, height);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLeft()
{
    return left.getCssText(this);
}

void CSSStyleDeclarationImp::setLeft(Nullable<std::u16string> left)
{
    setProperty(Left, left);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLetterSpacing()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setLetterSpacing(Nullable<std::u16string> letterSpacing)
{
    setProperty(LetterSpacing, letterSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLineHeight()
{
    return lineHeight.getCssText(this);
}

void CSSStyleDeclarationImp::setLineHeight(Nullable<std::u16string> lineHeight)
{
    setProperty(LineHeight, lineHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyle()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setListStyle(Nullable<std::u16string> listStyle)
{
    setProperty(ListStyle, listStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyleImage()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setListStyleImage(Nullable<std::u16string> listStyleImage)
{
    setProperty(ListStyleImage, listStyleImage);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStylePosition()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setListStylePosition(Nullable<std::u16string> listStylePosition)
{
    setProperty(ListStylePosition, listStylePosition);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyleType()
{
    return listStyleType.getCssText(this);
}

void CSSStyleDeclarationImp::setListStyleType(Nullable<std::u16string> listStyleType)
{
    setProperty(ListStyleType, listStyleType);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMargin()
{
    return margin.getCssText(this);
}

void CSSStyleDeclarationImp::setMargin(Nullable<std::u16string> margin)
{
    setProperty(Margin, margin);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginTop()
{
    return marginTop.getCssText(this);
}

void CSSStyleDeclarationImp::setMarginTop(Nullable<std::u16string> marginTop)
{
    setProperty(MarginTop, marginTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginRight()
{
    return marginRight.getCssText(this);
}

void CSSStyleDeclarationImp::setMarginRight(Nullable<std::u16string> marginRight)
{
    setProperty(MarginRight, marginRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginBottom()
{
    return marginBottom.getCssText(this);
}

void CSSStyleDeclarationImp::setMarginBottom(Nullable<std::u16string> marginBottom)
{
    setProperty(MarginBottom, marginBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginLeft()
{
    return marginLeft.getCssText(this);
}

void CSSStyleDeclarationImp::setMarginLeft(Nullable<std::u16string> marginLeft)
{
    setProperty(MarginLeft, marginLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarkerOffset()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setMarkerOffset(Nullable<std::u16string> markerOffset)
{
    setProperty(MarkerOffset, markerOffset);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarks()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setMarks(Nullable<std::u16string> marks)
{
    setProperty(Marks, marks);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMaxHeight()
{
    return maxHeight.getCssText(this);
}

void CSSStyleDeclarationImp::setMaxHeight(Nullable<std::u16string> maxHeight)
{
    setProperty(MaxHeight, maxHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMaxWidth()
{
    return maxWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setMaxWidth(Nullable<std::u16string> maxWidth)
{
    setProperty(MaxWidth, maxWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMinHeight()
{
    return minHeight.getCssText(this);
}

void CSSStyleDeclarationImp::setMinHeight(Nullable<std::u16string> minHeight)
{
    setProperty(MinHeight, minHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMinWidth()
{
    return minWidth.getCssText(this);
}

void CSSStyleDeclarationImp::setMinWidth(Nullable<std::u16string> minWidth)
{
    setProperty(MinWidth, minWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOrphans()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOrphans(Nullable<std::u16string> orphans)
{
    setProperty(Orphans, orphans);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutline()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOutline(Nullable<std::u16string> outline)
{
    setProperty(Outline, outline);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineColor()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOutlineColor(Nullable<std::u16string> outlineColor)
{
    setProperty(OutlineColor, outlineColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineStyle()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOutlineStyle(Nullable<std::u16string> outlineStyle)
{
    setProperty(OutlineStyle, outlineStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineWidth()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOutlineWidth(Nullable<std::u16string> outlineWidth)
{
    setProperty(OutlineWidth, outlineWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOverflow()
{
    return overflow.getCssText(this);
}

void CSSStyleDeclarationImp::setOverflow(Nullable<std::u16string> overflow)
{
    setProperty(Overflow, overflow);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPadding()
{
    return padding.getCssText(this);
}

void CSSStyleDeclarationImp::setPadding(Nullable<std::u16string> padding)
{
    setProperty(Padding, padding);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingTop()
{
    return paddingTop.getCssText(this);
}

void CSSStyleDeclarationImp::setPaddingTop(Nullable<std::u16string> paddingTop)
{
    setProperty(PaddingTop, paddingTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingRight()
{
    return paddingRight.getCssText(this);
}

void CSSStyleDeclarationImp::setPaddingRight(Nullable<std::u16string> paddingRight)
{
    setProperty(PaddingRight, paddingRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingBottom()
{
    return paddingBottom.getCssText(this);
}

void CSSStyleDeclarationImp::setPaddingBottom(Nullable<std::u16string> paddingBottom)
{
    setProperty(PaddingBottom, paddingBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingLeft()
{
    return paddingLeft.getCssText(this);
}

void CSSStyleDeclarationImp::setPaddingLeft(Nullable<std::u16string> paddingLeft)
{
    setProperty(PaddingLeft, paddingLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPage()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPage(Nullable<std::u16string> page)
{
    setProperty(Page, page);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakAfter()
{
    return pageBreakAfter.getCssText(this);
}

void CSSStyleDeclarationImp::setPageBreakAfter(Nullable<std::u16string> pageBreakAfter)
{
    setProperty(PageBreakAfter, pageBreakAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakBefore()
{
    return pageBreakBefore.getCssText(this);
}

void CSSStyleDeclarationImp::setPageBreakBefore(Nullable<std::u16string> pageBreakBefore)
{
    setProperty(PageBreakBefore, pageBreakBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakInside()
{
    return pageBreakInside.getCssText(this);
}

void CSSStyleDeclarationImp::setPageBreakInside(Nullable<std::u16string> pageBreakInside)
{
    setProperty(PageBreakInside, pageBreakInside);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPause()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPause(Nullable<std::u16string> pause)
{
    setProperty(Pause, pause);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPauseAfter()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPauseAfter(Nullable<std::u16string> pauseAfter)
{
    setProperty(PauseAfter, pauseAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPauseBefore()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPauseBefore(Nullable<std::u16string> pauseBefore)
{
    setProperty(PauseBefore, pauseBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPitch()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPitch(Nullable<std::u16string> pitch)
{
    setProperty(Pitch, pitch);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPitchRange()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPitchRange(Nullable<std::u16string> pitchRange)
{
    setProperty(PitchRange, pitchRange);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPlayDuring()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPlayDuring(Nullable<std::u16string> playDuring)
{
    setProperty(PlayDuring, playDuring);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPosition()
{
    return position.getCssText(this);
}

void CSSStyleDeclarationImp::setPosition(Nullable<std::u16string> position)
{
    setProperty(Position, position);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getQuotes()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setQuotes(Nullable<std::u16string> quotes)
{
    setProperty(Quotes, quotes);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getRichness()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setRichness(Nullable<std::u16string> richness)
{
    setProperty(Richness, richness);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getRight()
{
    return right.getCssText(this);
}

void CSSStyleDeclarationImp::setRight(Nullable<std::u16string> right)
{
    setProperty(Right, right);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSize()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSize(Nullable<std::u16string> size)
{
    setProperty(Size, size);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeak()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeak(Nullable<std::u16string> speak)
{
    setProperty(Speak, speak);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakHeader()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakHeader(Nullable<std::u16string> speakHeader)
{
    setProperty(SpeakHeader, speakHeader);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakNumeral()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakNumeral(Nullable<std::u16string> speakNumeral)
{
    setProperty(SpeakNumeral, speakNumeral);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakPunctuation()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakPunctuation(Nullable<std::u16string> speakPunctuation)
{
    setProperty(SpeakPunctuation, speakPunctuation);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeechRate()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeechRate(Nullable<std::u16string> speechRate)
{
    setProperty(SpeechRate, speechRate);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getStress()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setStress(Nullable<std::u16string> stress)
{
    setProperty(Stress, stress);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTableLayout()
{
    return tableLayout.getCssText(this);
}

void CSSStyleDeclarationImp::setTableLayout(Nullable<std::u16string> tableLayout)
{
    setProperty(TableLayout, tableLayout);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextAlign()
{
    return textAlign.getCssText(this);
}

void CSSStyleDeclarationImp::setTextAlign(Nullable<std::u16string> textAlign)
{
    setProperty(TextAlign, textAlign);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextDecoration()
{
    return textDecoration.getCssText(this);
}

void CSSStyleDeclarationImp::setTextDecoration(Nullable<std::u16string> textDecoration)
{
    setProperty(TextDecoration, textDecoration);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextIndent()
{
    return textIndent.getCssText(this);
}

void CSSStyleDeclarationImp::setTextIndent(Nullable<std::u16string> textIndent)
{
    setProperty(TextIndent, textIndent);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextShadow()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setTextShadow(Nullable<std::u16string> textShadow)
{
    setProperty(TextShadow, textShadow);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextTransform()
{
    return textTransform.getCssText(this);
}

void CSSStyleDeclarationImp::setTextTransform(Nullable<std::u16string> textTransform)
{
    setProperty(TextTransform, textTransform);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTop()
{
    return top.getCssText(this);
}

void CSSStyleDeclarationImp::setTop(Nullable<std::u16string> top)
{
    setProperty(Top, top);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getUnicodeBidi()
{
    return unicodeBidi.getCssText(this);
}

void CSSStyleDeclarationImp::setUnicodeBidi(Nullable<std::u16string> unicodeBidi)
{
    setProperty(UnicodeBidi, unicodeBidi);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVerticalAlign()
{
    return verticalAlign.getCssText(this);
}

void CSSStyleDeclarationImp::setVerticalAlign(Nullable<std::u16string> verticalAlign)
{
    setProperty(VerticalAlign, verticalAlign);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVisibility()
{
    return visibility.getCssText(this);
}

void CSSStyleDeclarationImp::setVisibility(Nullable<std::u16string> visibility)
{
    setProperty(Visibility, visibility);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVoiceFamily()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setVoiceFamily(Nullable<std::u16string> voiceFamily)
{
    setProperty(VoiceFamily, voiceFamily);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVolume()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setVolume(Nullable<std::u16string> volume)
{
    setProperty(Volume, volume);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWhiteSpace()
{
    return whiteSpace.getCssText(this);
}

void CSSStyleDeclarationImp::setWhiteSpace(Nullable<std::u16string> whiteSpace)
{
    setProperty(WhiteSpace, whiteSpace);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWidows()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setWidows(Nullable<std::u16string> widows)
{
    setProperty(Widows, widows);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWidth()
{
    return width.getCssText(this);
}

void CSSStyleDeclarationImp::setWidth(Nullable<std::u16string> width)
{
    setProperty(Width, width);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWordSpacing()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setWordSpacing(Nullable<std::u16string> wordSpacing)
{
    setProperty(WordSpacing, wordSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getZIndex()
{
    return zIndex.getCssText(this);
}

void CSSStyleDeclarationImp::setZIndex(Nullable<std::u16string> zIndex)
{
    setProperty(ZIndex, zIndex);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getHTMLAlign()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setHTMLAlign(Nullable<std::u16string> align)
{
    unsigned a = HTMLAlignValueImp::None;
    if (align.hasValue()) {
        std::u16string value = align.value();
        toLower(value);
        if (value == u"left")
            a = HTMLAlignValueImp::Left;
        else if (value == u"center")
            a = HTMLAlignValueImp::Center;
        else if (value == u"right")
            a = HTMLAlignValueImp::Right;
        else
            return;
    }
    htmlAlign.setValue(a);
    resetInherit(HtmlAlign);
    setProperty(HtmlAlign);
}

CSSStyleDeclarationImp::CSSStyleDeclarationImp() :
    owner(0),
    parentRule(0),
    resolved(false),
    box(0),
    lastBox(0),
    stackingContext(0),
    fontTexture(0),
    backgroundColor(CSSColorValueImp::Transparent),
    counterIncrement(1),
    counterReset(0),
    borderTop(0),
    borderRight(1),
    borderBottom(2),
    borderLeft(3),
    marginTop(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginRight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginLeft(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginBottom(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minHeight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minWidth(0.0f, css::CSSPrimitiveValue::CSS_PX),
    textIndent(0.0f, css::CSSPrimitiveValue::CSS_PX)
{
    const static int defaultInherit[] = {
        Azimuth,
        BorderCollapse,
        BorderSpacing,
        CaptionSide,
        Color,
        Cursor,
        Direction,
        Elevation,
        EmptyCells,
        FontFamily,
        FontSize,
        FontStyle,
        FontVariant,
        FontWeight,
        Font,
        LetterSpacing,
        LineHeight,
        ListStyle,
        ListStyleImage,
        ListStylePosition,
        ListStyleType,
        Orphans,
        Pitch,
        PitchRange,
        Quotes,
        Richness,
        Speak,
        SpeakHeader,
        SpeakNumeral,
        SpeakPunctuation,
        SpeechRate,
        Stress,
        TextAlign,
        TextIndent,
        TextTransform,
        Visibility,
        VoiceFamily,
        Volume,
        WhiteSpace,
        Widows,
        WordSpacing,

        HtmlAlign
    };
    for (unsigned i = 0; i < sizeof defaultInherit / sizeof defaultInherit[0]; ++i)
        setInherit(defaultInherit[i]);
    pseudoElements[CSSPseudoElementSelector::NonPseudo] = this;
    for (int i = 1; i < CSSPseudoElementSelector::MaxPseudoElements; ++i)
        pseudoElements[i] = 0;
}

CSSStyleDeclarationImp::~CSSStyleDeclarationImp()
{
}

const char16_t* CSSStyleDeclarationImp::getPropertyName(int propertyID)
{
    if (propertyID < 0 || PropertyCount <= propertyID)
        return PropertyNames[0];
    return PropertyNames[propertyID];
}

}}}}  // org::w3c::dom::bootstrap
