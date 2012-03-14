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

#include "CSSSelector.h"
#include "CSSValueParser.h"
#include "CSSStyleDeclarationImp.h"
#include "CSSPropertyValueImp.h"

#include <org/w3c/dom/css/CSSPrimitiveValue.h>

#include <algorithm>
#include <bitset>

#include <iostream>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

CSSValueRule CSSValueParser::angle;
CSSValueRule CSSValueParser::percent_length;
CSSValueRule CSSValueParser::percent_non_negative_length;
CSSValueRule CSSValueParser::auto_length;
CSSValueRule CSSValueParser::auto_non_negative_length;
CSSValueRule CSSValueParser::none_length;
CSSValueRule CSSValueParser::none_non_negative_length;
CSSValueRule CSSValueParser::normal_length;
CSSValueRule CSSValueParser::auto_numbering;
CSSValueRule CSSValueParser::comma;
CSSValueRule CSSValueParser::ident;
CSSValueRule CSSValueParser::integer;
CSSValueRule CSSValueParser::length;
CSSValueRule CSSValueParser::non_negative_length;
CSSValueRule CSSValueParser::number;
CSSValueRule CSSValueParser::percentage;
CSSValueRule CSSValueParser::non_negative_percentage;
CSSValueRule CSSValueParser::slash;
CSSValueRule CSSValueParser::string;
CSSValueRule CSSValueParser::uri;
CSSValueRule CSSValueParser::lineHeight;
CSSValueRule CSSValueParser::list_style_type;
CSSValueRule CSSValueParser::listStyleType;
CSSValueRule CSSValueParser::absolute_size;
CSSValueRule CSSValueParser::border_width;
CSSValueRule CSSValueParser::border_style;
CSSValueRule CSSValueParser::counter;
CSSValueRule CSSValueParser::family_name;
CSSValueRule CSSValueParser::font_style;
CSSValueRule CSSValueParser::font_variant;
CSSValueRule CSSValueParser::font_weight;
CSSValueRule CSSValueParser::generic_family;
CSSValueRule CSSValueParser::relative_size;
CSSValueRule CSSValueParser::shape;
CSSValueRule CSSValueParser::azimuth;
CSSValueRule CSSValueParser::backgroundAttachment;
CSSValueRule CSSValueParser::backgroundImage;
CSSValueRule CSSValueParser::backgroundPosition;
CSSValueRule CSSValueParser::backgroundRepeat;
CSSValueRule CSSValueParser::background;
CSSValueRule CSSValueParser::borderCollapse;
CSSValueRule CSSValueParser::borderColor;
CSSValueRule CSSValueParser::borderSpacing;
CSSValueRule CSSValueParser::borderStyle;
CSSValueRule CSSValueParser::borderTopStyle; // = borderRightStyle, borderBottomStyle, borderLeftStyle
CSSValueRule CSSValueParser::borderTopWidth; // = borderRightWidth, borderBottomWidth, borderLeftWidth
CSSValueRule CSSValueParser::borderWidth;
CSSValueRule CSSValueParser::border;  // = borderTop, borderRight, borderBottom, borderLeft
CSSValueRule CSSValueParser::captionSide;
CSSValueRule CSSValueParser::clear;
CSSValueRule CSSValueParser::clip;
CSSValueRule CSSValueParser::color;
CSSValueRule CSSValueParser::content;
CSSValueRule CSSValueParser::cursor;
CSSValueRule CSSValueParser::direction;
CSSValueRule CSSValueParser::display;
CSSValueRule CSSValueParser::float_;
CSSValueRule CSSValueParser::fontFamily;
CSSValueRule CSSValueParser::fontSize;
CSSValueRule CSSValueParser::fontStyle;
CSSValueRule CSSValueParser::fontVariant;
CSSValueRule CSSValueParser::fontWeight;
CSSValueRule CSSValueParser::font;
CSSValueRule CSSValueParser::letterSpacing;
CSSValueRule CSSValueParser::listStyleImage;
CSSValueRule CSSValueParser::listStylePosition;
CSSValueRule CSSValueParser::listStyle;
CSSValueRule CSSValueParser::margin;
CSSValueRule CSSValueParser::outlineColor;
CSSValueRule CSSValueParser::outlineStyle;
CSSValueRule CSSValueParser::outlineWidth;
CSSValueRule CSSValueParser::outline;
CSSValueRule CSSValueParser::overflow;
CSSValueRule CSSValueParser::padding;
CSSValueRule CSSValueParser::pageBreak; // = pageBereakAfter, pageBreakBefore
CSSValueRule CSSValueParser::pageBreakInside;
CSSValueRule CSSValueParser::position;
CSSValueRule CSSValueParser::quotes;
CSSValueRule CSSValueParser::tableLayout;
CSSValueRule CSSValueParser::textAlign;
CSSValueRule CSSValueParser::textDecoration;
CSSValueRule CSSValueParser::textTransform;
CSSValueRule CSSValueParser::unicodeBidi;
CSSValueRule CSSValueParser::verticalAlign;
CSSValueRule CSSValueParser::visibility;
CSSValueRule CSSValueParser::whiteSpace;
CSSValueRule CSSValueParser::zIndex;

CSSValueRule CSSValueParser::binding;

void CSSValueParser::initializeRules()
{
    angle = CSSValueRule(CSSValueRule::Angle);
    comma = CSSValueRule(CSSValueRule::Comma);
    ident = CSSValueRule(CSSValueRule::AnyIdent);
    length = CSSValueRule(CSSValueRule::Length);
    non_negative_length = CSSValueRule(CSSValueRule::NonNegativeLength);
    integer = CSSValueRule(CSSValueRule::AnyInteger);
    number = CSSValueRule(CSSValueRule::AnyNumber);
    percentage = CSSValueRule(CSSValueRule::Percentage);
    non_negative_percentage = CSSValueRule(CSSValueRule::NonNegativePercentage);
    slash = CSSValueRule(CSSValueRule::Slash);
    string = CSSValueRule(CSSValueRule::String);
    uri = CSSValueRule(CSSValueRule::Uri);

    percent_length
        = percentage
        | length;
    percent_non_negative_length
        = non_negative_percentage
        | non_negative_length;
    auto_length
        = length
        | percentage
        | CSSValueRule(u"auto", 0);
    auto_non_negative_length
        = non_negative_length
        | non_negative_percentage
        | CSSValueRule(u"auto", 0);
    none_length
        = length
        | percentage
        | CSSValueRule(u"none", 0);
    none_non_negative_length
        = non_negative_length
        | non_negative_percentage
        | CSSValueRule(u"none", 0);
    normal_length
        = length
        | CSSValueRule(u"normal", 0);
    auto_numbering
        = CSSValueRule(1, CSSValueRule::Inf, ident + CSSValueRule(0, 1, integer))
        | CSSValueRule(u"none", 0);
    color
        = CSSValueRule(CSSValueRule::Color)
        | ident
            [&CSSValueParser::colorKeyword]
        | CSSValueRule(u"rgb", integer + comma + integer + comma + integer)
            [&CSSValueParser::rgb]
        | CSSValueRule(u"rgb", percentage + comma + percentage + comma + percentage)
            [&CSSValueParser::rgb]
#if 0
        | CSSValueRule(u"rgba", (integer | percentage) + comma +
                                (integer | percentage) + comma +
                                (integer | percentage) + comma +
                                number)
            // TODO: [&CSSValueParser::rgba]
        | CSSValueRule(u"hsl", number + comma +
                               percentage + comma +
                               percentage)
            // TODO: [&CSSValueParser::hsl]
        | CSSValueRule(u"hsla", number + comma +
                                percentage + comma +
                                percentage + comma +
                                number)
            // TODO: [&CSSValueParser::hsla]
#endif
        ;
    lineHeight
        = (CSSValueRule(u"normal", CSSLineHeightValueImp::Normal)
        |  number
        |  non_negative_length
        |  non_negative_percentage)
            [CSSStyleDeclarationImp::LineHeight];
    list_style_type
        = (CSSValueRule(u"disc", CSSListStyleTypeValueImp::Disc)
        |  CSSValueRule(u"circle", CSSListStyleTypeValueImp::Circle)
        |  CSSValueRule(u"square", CSSListStyleTypeValueImp::Square)
        |  CSSValueRule(u"decimal", CSSListStyleTypeValueImp::Decimal)
        |  CSSValueRule(u"decimal-leading-zero", CSSListStyleTypeValueImp::DecimalLeadingZero)
        |  CSSValueRule(u"lower-roman", CSSListStyleTypeValueImp::LowerRoman)
        |  CSSValueRule(u"upper-roman", CSSListStyleTypeValueImp::UpperRoman)
        |  CSSValueRule(u"lower-greek", CSSListStyleTypeValueImp::LowerGreek)
        |  CSSValueRule(u"lower-latin", CSSListStyleTypeValueImp::LowerLatin)
        |  CSSValueRule(u"upper-latin", CSSListStyleTypeValueImp::UpperLatin)
        |  CSSValueRule(u"armenian", CSSListStyleTypeValueImp::Armenian)
        |  CSSValueRule(u"georgian", CSSListStyleTypeValueImp::Georgian)
        |  CSSValueRule(u"lower-alpha", CSSListStyleTypeValueImp::LowerAlpha)
        |  CSSValueRule(u"upper-alpha", CSSListStyleTypeValueImp::UpperAlpha))
            [CSSStyleDeclarationImp::ListStyleType];
    listStyleType
        = (CSSValueRule(u"none", CSSListStyleTypeValueImp::None)
        |  list_style_type)
            [CSSStyleDeclarationImp::ListStyleType];

    absolute_size
        = CSSValueRule(u"xx-small", CSSFontSizeValueImp::XxSmall)
        | CSSValueRule(u"x-small", CSSFontSizeValueImp::XSmall)
        | CSSValueRule(u"small", CSSFontSizeValueImp::Small)
        | CSSValueRule(u"medium", CSSFontSizeValueImp::Medium)
        | CSSValueRule(u"large", CSSFontSizeValueImp::Large)
        | CSSValueRule(u"x-large", CSSFontSizeValueImp::XLarge)
        | CSSValueRule(u"xx-large", CSSFontSizeValueImp::XxLarge);
    border_width
        = (CSSValueRule(u"thin", CSSBorderWidthValueImp::Thin)
        |  CSSValueRule(u"medium", CSSBorderWidthValueImp::Medium)
        |  CSSValueRule(u"thick", CSSBorderWidthValueImp::Thick)
        |  non_negative_length)
            [CSSStyleDeclarationImp::BorderWidth];
    border_style
        = (CSSValueRule(u"none", CSSBorderStyleValueImp::None)
        |  CSSValueRule(u"hidden", CSSBorderStyleValueImp::Hidden)
        |  CSSValueRule(u"dotted", CSSBorderStyleValueImp::Dotted)
        |  CSSValueRule(u"dashed", CSSBorderStyleValueImp::Dashed)
        |  CSSValueRule(u"solid", CSSBorderStyleValueImp::Solid)
        |  CSSValueRule(u"double", CSSBorderStyleValueImp::Double)
        |  CSSValueRule(u"groove", CSSBorderStyleValueImp::Groove)
        |  CSSValueRule(u"ridge", CSSBorderStyleValueImp::Ridge)
        |  CSSValueRule(u"inset", CSSBorderStyleValueImp::Inset)
        |  CSSValueRule(u"outset", CSSBorderStyleValueImp::Outset))
            [CSSStyleDeclarationImp::BorderStyle];
    counter
        = CSSValueRule(u"counter", ident + CSSValueRule(0, 1, comma + listStyleType))
        | CSSValueRule(u"counters", ident + comma + string + CSSValueRule(0, 1, comma + listStyleType));
    family_name
        = string
        | CSSValueRule(1, CSSValueRule::Inf, ident);
    font_style
        = (CSSValueRule(u"italic", CSSFontStyleValueImp::Italic)
        |  CSSValueRule(u"oblique", CSSFontStyleValueImp::Oblique))
            [CSSStyleDeclarationImp::FontStyle];
    font_variant
        = (CSSValueRule(u"small-caps",CSSFontVariantValueImp::SmallCaps))
            [CSSStyleDeclarationImp::FontVariant];
    font_weight
        = (CSSValueRule(u"bold", CSSFontWeightValueImp::Bold)
        |  CSSValueRule(u"lighter", CSSFontWeightValueImp::Lighter)
        |  CSSValueRule(u"bolder", CSSFontWeightValueImp::Bolder)
        |  100.0f
        |  200.0f
        |  300.0f
        |  400.0f
        |  500.0f
        |  600.0f
        |  700.0f
        |  800.0f
        |  900.0f)
            [CSSStyleDeclarationImp::FontWeight];
    generic_family
        = CSSValueRule(u"serif", CSSFontFamilyValueImp::Serif)
        | CSSValueRule(u"sans-serif", CSSFontFamilyValueImp::SansSerif)
        | CSSValueRule(u"cursive", CSSFontFamilyValueImp::Cursive)
        | CSSValueRule(u"fantasy", CSSFontFamilyValueImp::Fantasy)
        | CSSValueRule(u"monospace", CSSFontFamilyValueImp::Monospace);
    relative_size
        = CSSValueRule(u"larger", CSSFontSizeValueImp::Larger)
        | CSSValueRule(u"smaller", CSSFontSizeValueImp::Smaller);
    shape
        = CSSValueRule(u"rect", (length | u"auto") + comma +
                                (length | u"auto") + comma +
                                (length | u"auto") + comma +
                                (length | u"auto"));

    azimuth
         = angle
         | ((CSSValueRule(u"left-side") |
             u"far-left" |
             u"left" |
             u"center-left" |
             u"center" |
             u"center-right" |
             u"right" |
             u"far-right" |
             u"right-side") ||
            u"behind")
         | u"leftwards"
         | u"rightwards";
    backgroundAttachment
        = (CSSValueRule(u"scroll", CSSBackgroundAttachmentValueImp::Scroll)
        |  CSSValueRule(u"fixed", CSSBackgroundAttachmentValueImp::Fixed))
            [CSSStyleDeclarationImp::BackgroundAttachment];
    backgroundImage
        = (uri
        |  CSSValueRule(u"none", CSSBackgroundImageValueImp::None))
            [CSSStyleDeclarationImp::BackgroundImage];
    backgroundPosition
        = (CSSValueRule(u"center", CSSBackgroundPositionValueImp::Center) +
           CSSValueRule(0, 1, (percentage | length |
                               CSSValueRule(u"top", CSSBackgroundPositionValueImp::Top) |
                               CSSValueRule(u"center", CSSBackgroundPositionValueImp::Center) |
                               CSSValueRule(u"bottom", CSSBackgroundPositionValueImp::Bottom) |
                               CSSValueRule(u"left", CSSBackgroundPositionValueImp::Left) |
                               CSSValueRule(u"right", CSSBackgroundPositionValueImp::Right)))
         | (percentage | length |
            CSSValueRule(u"left", CSSBackgroundPositionValueImp::Left) |
            CSSValueRule(u"right", CSSBackgroundPositionValueImp::Right)) +
           CSSValueRule(0, 1, (percentage | length |
                               CSSValueRule(u"top", CSSBackgroundPositionValueImp::Top) |
                               CSSValueRule(u"center", CSSBackgroundPositionValueImp::Center) |
                               CSSValueRule(u"bottom", CSSBackgroundPositionValueImp::Bottom)))
         | (CSSValueRule(u"top", CSSBackgroundPositionValueImp::Top) |
            CSSValueRule(u"bottom", CSSBackgroundPositionValueImp::Bottom)) +
           CSSValueRule(0, 1, CSSValueRule(u"left", CSSBackgroundPositionValueImp::Left) |
                              CSSValueRule(u"center", CSSBackgroundPositionValueImp::Center) |
                              CSSValueRule(u"right", CSSBackgroundPositionValueImp::Right)))
            [CSSStyleDeclarationImp::BackgroundPosition]
        ;
    backgroundRepeat
        = (CSSValueRule(u"repeat", CSSBackgroundRepeatValueImp::Repeat)
        |  CSSValueRule(u"repeat-x", CSSBackgroundRepeatValueImp::RepeatX)
        |  CSSValueRule(u"repeat-y", CSSBackgroundRepeatValueImp::RepeatY)
        |  CSSValueRule(u"no-repeat", CSSBackgroundRepeatValueImp::NoRepeat))
            [CSSStyleDeclarationImp::BackgroundRepeat];
    background
        = color ||
          backgroundImage ||
          backgroundRepeat ||
          backgroundAttachment ||
          backgroundPosition;
    borderCollapse
        = CSSValueRule(u"collapse", CSSBorderCollapseValueImp::Collapse)
        | CSSValueRule(u"separate", CSSBorderCollapseValueImp::Separate);
    borderColor
        = CSSValueRule(1, 4, color);
    borderSpacing
        = non_negative_length + CSSValueRule(0, 1, non_negative_length);
    borderStyle
        = CSSValueRule(1, 4, border_style);
    borderTopStyle // = borderRightStyle, borderBottomStyle, borderLeftStyle
        = border_style;
    borderTopWidth // = borderRightWidth, borderBottomWidth, borderLeftWidth
        = border_width;
    borderWidth
        = CSSValueRule(1, 4, border_width);
    border  // = borderTop, borderRight, borderBottom, borderLeft
        = border_width ||
          border_style ||
          color;
    captionSide
        = CSSValueRule(u"top", CSSCaptionSideValueImp::Top)
        | CSSValueRule(u"bottom", CSSCaptionSideValueImp::Bottom);
    clear
        = CSSValueRule(u"none", CSSClearValueImp::None)
        | CSSValueRule(u"left", CSSClearValueImp::Left)
        | CSSValueRule(u"right", CSSClearValueImp::Right)
        | CSSValueRule(u"both", CSSClearValueImp::Both);
    clip
        = shape
        | u"auto";
    content
        = CSSValueRule(u"normal", CSSContentValueImp::Normal)
        | CSSValueRule(u"none", CSSContentValueImp::None)
        | CSSValueRule(1, CSSValueRule::Inf,
                       string |
                       uri |
                       counter [CSSContentValueImp::Counter] |
                       CSSValueRule(u"attr", ident) [CSSContentValueImp::Attr] |
                       CSSValueRule(u"open-quote", CSSContentValueImp::OpenQuote) |
                       CSSValueRule(u"close-quote", CSSContentValueImp::CloseQuote) |
                       CSSValueRule(u"no-open-quote", CSSContentValueImp::NoOpenQuote) |
                       CSSValueRule(u"no-close-quote", CSSContentValueImp::NoCloseQuote));
    cursor
        = CSSValueRule(0, CSSValueRule::Inf, uri + comma) + (
          CSSValueRule(u"auto", CSSCursorValueImp::Auto) |
          CSSValueRule(u"crosshair", CSSCursorValueImp::Crosshair) |
          CSSValueRule(u"default", CSSCursorValueImp::Default) |
          CSSValueRule(u"pointer", CSSCursorValueImp::Pointer) |
          CSSValueRule(u"move", CSSCursorValueImp::Move) |
          CSSValueRule(u"e-resize", CSSCursorValueImp::EResize) |
          CSSValueRule(u"ne-resize", CSSCursorValueImp::NEResize) |
          CSSValueRule(u"nw-resize", CSSCursorValueImp::NWResize) |
          CSSValueRule(u"n-resize", CSSCursorValueImp::NResize) |
          CSSValueRule(u"se-resize", CSSCursorValueImp::SEResize) |
          CSSValueRule(u"sw-resize", CSSCursorValueImp::SWResize) |
          CSSValueRule(u"s-resize", CSSCursorValueImp::SResize) |
          CSSValueRule(u"w-resize", CSSCursorValueImp::WResize) |
          CSSValueRule(u"text", CSSCursorValueImp::Text) |
          CSSValueRule(u"wait", CSSCursorValueImp::Wait) |
          CSSValueRule(u"help", CSSCursorValueImp::Help) |
          CSSValueRule(u"progress", CSSCursorValueImp::Progress)
        );
    direction
        = CSSValueRule(u"ltr", CSSDirectionValueImp::Ltr)
        | CSSValueRule(u"rtl", CSSDirectionValueImp::Rtl);
    display
        = CSSValueRule(u"inline", CSSDisplayValueImp::Inline)
        | CSSValueRule(u"block", CSSDisplayValueImp::Block)
        | CSSValueRule(u"list-item", CSSDisplayValueImp::ListItem)
        | CSSValueRule(u"run-in", CSSDisplayValueImp::RunIn)
        | CSSValueRule(u"inline-block", CSSDisplayValueImp::InlineBlock)
        | CSSValueRule(u"table", CSSDisplayValueImp::Table)
        | CSSValueRule(u"inline-table", CSSDisplayValueImp::InlineTable)
        | CSSValueRule(u"table-row-group", CSSDisplayValueImp::TableRowGroup)
        | CSSValueRule(u"table-header-group", CSSDisplayValueImp::TableHeaderGroup)
        | CSSValueRule(u"table-footer-group", CSSDisplayValueImp::TableFooterGroup)
        | CSSValueRule(u"table-row", CSSDisplayValueImp::TableRow)
        | CSSValueRule(u"table-column-group", CSSDisplayValueImp::TableColumnGroup)
        | CSSValueRule(u"table-column", CSSDisplayValueImp::TableColumn)
        | CSSValueRule(u"table-cell", CSSDisplayValueImp::TableCell)
        | CSSValueRule(u"table-caption", CSSDisplayValueImp::TableCaption)
        | CSSValueRule(u"none", CSSDisplayValueImp::None);

    float_
        = CSSValueRule(u"none", CSSFloatValueImp::None)
        | CSSValueRule(u"left", CSSFloatValueImp::Left)
        | CSSValueRule(u"right", CSSFloatValueImp::Right);
    fontFamily
        = ((generic_family | family_name) +
           CSSValueRule(0, CSSValueRule::Inf, comma + (generic_family | family_name)))
            [CSSStyleDeclarationImp::FontFamily];
    fontSize
        = (absolute_size
        |  relative_size
        |  non_negative_length
        |  non_negative_percentage)
            [CSSStyleDeclarationImp::FontSize];
    fontStyle
        = CSSValueRule(u"normal", CSSFontStyleValueImp::Normal)
        | font_style;
    fontVariant
        = CSSValueRule(u"normal", CSSFontVariantValueImp::Normal)
        | font_variant;
    fontWeight
        = CSSValueRule(u"normal", CSSFontWeightValueImp::Normal)
        | font_weight;
    font
        = (CSSValueRule(0, 1, font_style || font_variant || font_weight ||
                              CSSValueRule(u"normal", CSSFontShorthandImp::Normal)) +
           fontSize +
           CSSValueRule(0, 1, slash + lineHeight) +
           fontFamily)
        | CSSValueRule(u"caption", CSSFontShorthandImp::Caption)
        | CSSValueRule(u"icon", CSSFontShorthandImp::Icon)
        | CSSValueRule(u"menu", CSSFontShorthandImp::Menu)
        | CSSValueRule(u"message-box", CSSFontShorthandImp::MessageBox)
        | CSSValueRule(u"small-caption", CSSFontShorthandImp::SmallCaption)
        | CSSValueRule(u"status-bar", CSSFontShorthandImp::StatusBar);
    letterSpacing
        = CSSValueRule(u"normal")
        | length;

    listStyleImage
        = (uri
        |  CSSValueRule(u"none", CSSListStyleImageValueImp::None))
            [CSSStyleDeclarationImp::ListStyleImage];
    listStylePosition
        = (CSSValueRule(u"inside", CSSListStylePositionValueImp::Inside)
         | CSSValueRule(u"outside", CSSListStylePositionValueImp::Outside))
            [CSSStyleDeclarationImp::ListStylePosition];
    listStyle
        = CSSValueRule(u"none", CSSListStyleShorthandImp::None) ||
          CSSValueRule(u"none", CSSListStyleShorthandImp::None) ||  // 'none' can appear twice.
          list_style_type ||
          listStylePosition ||
          uri;

    margin
        = CSSValueRule(1, 4, auto_length);

    outlineColor
        = color
        | CSSValueRule(u"invert", CSSOutlineColorValueImp::Invert);
    outlineStyle
        = (CSSValueRule(u"none", CSSBorderStyleValueImp::None)
        |  CSSValueRule(u"dotted", CSSBorderStyleValueImp::Dotted)
        |  CSSValueRule(u"dashed", CSSBorderStyleValueImp::Dashed)
        |  CSSValueRule(u"solid", CSSBorderStyleValueImp::Solid)
        |  CSSValueRule(u"double", CSSBorderStyleValueImp::Double)
        |  CSSValueRule(u"groove", CSSBorderStyleValueImp::Groove)
        |  CSSValueRule(u"ridge", CSSBorderStyleValueImp::Ridge)
        |  CSSValueRule(u"inset", CSSBorderStyleValueImp::Inset)
        |  CSSValueRule(u"outset", CSSBorderStyleValueImp::Outset))
            [CSSStyleDeclarationImp::BorderStyle];
    outlineWidth
        = border_width;
    outline
        = outlineColor ||
          outlineStyle ||
          outlineWidth;
    overflow
        = CSSValueRule(u"visible", CSSOverflowValueImp::Visible)
        | CSSValueRule(u"hidden", CSSOverflowValueImp::Hidden)
        | CSSValueRule(u"scroll", CSSOverflowValueImp::Scroll)
        | CSSValueRule(u"auto", CSSOverflowValueImp::Auto);
    padding
        = CSSValueRule(1, 4, percent_non_negative_length);
    pageBreak // = pageBreakAfter, pageBreakBefore
        = CSSValueRule(u"auto", CSSPageBreakValueImp::Auto)
        | CSSValueRule(u"always", CSSPageBreakValueImp::Always)
        | CSSValueRule(u"avoid", CSSPageBreakValueImp::Avoid)
        | CSSValueRule(u"left", CSSPageBreakValueImp::Left)
        | CSSValueRule(u"right", CSSPageBreakValueImp::Right);
    pageBreakInside
        = CSSValueRule(u"auto", CSSPageBreakValueImp::Auto)
        | CSSValueRule(u"avoid", CSSPageBreakValueImp::Avoid);

    position
        = CSSValueRule(u"static", CSSPositionValueImp::Static)
        | CSSValueRule(u"relative", CSSPositionValueImp::Relative)
        | CSSValueRule(u"absolute", CSSPositionValueImp::Absolute)
        | CSSValueRule(u"fixed", CSSPositionValueImp::Fixed);
    quotes
        = CSSValueRule(1, CSSValueRule::Inf, string + string)
        | CSSValueRule(u"none", CSSQuotesValueImp::None);

    tableLayout
        = CSSValueRule(u"auto", CSSTableLayoutValueImp::Auto)
        | CSSValueRule(u"fixed", CSSTableLayoutValueImp::Fixed);
    textAlign
        = CSSValueRule(u"left", CSSTextAlignValueImp::Left)
        | CSSValueRule(u"right", CSSTextAlignValueImp::Right)
        | CSSValueRule(u"center", CSSTextAlignValueImp::Center)
        | CSSValueRule(u"justify", CSSTextAlignValueImp::Justify);
    textDecoration
        = CSSValueRule(u"none", CSSTextDecorationValueImp::None)
        | (CSSValueRule(u"underline", CSSTextDecorationValueImp::Underline) ||
           CSSValueRule(u"overline", CSSTextDecorationValueImp::Overline) ||
           CSSValueRule(u"line-through", CSSTextDecorationValueImp::LineThrough) ||
           CSSValueRule(u"blink", CSSTextDecorationValueImp::Blink));
    textTransform
        = CSSValueRule(u"capitalize", CSSTextTransformValueImp::Capitalize)
        | CSSValueRule(u"uppercase", CSSTextTransformValueImp::Uppercase)
        | CSSValueRule(u"lowercase", CSSTextTransformValueImp::Lowercase)
        | CSSValueRule(u"none", CSSTextTransformValueImp::None);

    unicodeBidi
        = CSSValueRule(u"normal", CSSUnicodeBidiValueImp::Normal)
        | CSSValueRule(u"embed", CSSUnicodeBidiValueImp::Embed)
        | CSSValueRule(u"bidi-override", CSSUnicodeBidiValueImp::BidiOverride);

    verticalAlign
        = CSSValueRule(u"baseline", CSSVerticalAlignValueImp::Baseline)
        | CSSValueRule(u"sub", CSSVerticalAlignValueImp::Sub)
        | CSSValueRule(u"super", CSSVerticalAlignValueImp::Super)
        | CSSValueRule(u"top", CSSVerticalAlignValueImp::Top)
        | CSSValueRule(u"text-top", CSSVerticalAlignValueImp::TextTop)
        | CSSValueRule(u"middle", CSSVerticalAlignValueImp::Middle)
        | CSSValueRule(u"bottom", CSSVerticalAlignValueImp::Bottom)
        | CSSValueRule(u"text-bottom", CSSVerticalAlignValueImp::TextBottom)
        | percentage
        | length;
    visibility
        = CSSValueRule(u"visible", CSSVisibilityValueImp::Visible)
        | CSSValueRule(u"hidden", CSSVisibilityValueImp::Hidden)
        | CSSValueRule(u"collapse", CSSVisibilityValueImp::Collapse);

    whiteSpace
        = CSSValueRule(u"normal", CSSWhiteSpaceValueImp::Normal)
        | CSSValueRule(u"pre", CSSWhiteSpaceValueImp::Pre)
        | CSSValueRule(u"nowrap", CSSWhiteSpaceValueImp::Nowrap)
        | CSSValueRule(u"pre-wrap", CSSWhiteSpaceValueImp::PreWrap)
        | CSSValueRule(u"pre-line", CSSWhiteSpaceValueImp::PreLine);
    zIndex
        = CSSValueRule(u"auto", 0)
        | integer;

    binding
        = CSSValueRule(u"none", CSSBindingValueImp::None)
        | CSSValueRule(u"button", CSSBindingValueImp::Button)
        | CSSValueRule(u"details", CSSBindingValueImp::Details)
        | CSSValueRule(u"input-textfield", CSSBindingValueImp::InputTextfield)
        | CSSValueRule(u"input-password", CSSBindingValueImp::InputPassword)
        | CSSValueRule(u"input-datetime", CSSBindingValueImp::InputDatetime)
        | CSSValueRule(u"input-date", CSSBindingValueImp::InputDate)
        | CSSValueRule(u"input-month", CSSBindingValueImp::InputMonth)
        | CSSValueRule(u"input-week", CSSBindingValueImp::InputWeek)
        | CSSValueRule(u"input-time", CSSBindingValueImp::InputTime)
        | CSSValueRule(u"input-datetime-local", CSSBindingValueImp::InputDatetimeLocal)
        | CSSValueRule(u"input-number", CSSBindingValueImp::InputNumber)
        | CSSValueRule(u"input-range", CSSBindingValueImp::InputRange)
        | CSSValueRule(u"input-color", CSSBindingValueImp::InputColor)
        | CSSValueRule(u"input-checkbox", CSSBindingValueImp::InputCheckbox)
        | CSSValueRule(u"input-radio", CSSBindingValueImp::InputRadio)
        | CSSValueRule(u"input-file", CSSBindingValueImp::InputFile)
        | CSSValueRule(u"input-button", CSSBindingValueImp::InputButton)
        | CSSValueRule(u"marquee", CSSBindingValueImp::Marquee)
        | CSSValueRule(u"meter", CSSBindingValueImp::Meter)
        | CSSValueRule(u"progress", CSSBindingValueImp::Progress)
        | CSSValueRule(u"select", CSSBindingValueImp::Select)
        | CSSValueRule(u"textarea", CSSBindingValueImp::Textarea)
        | CSSValueRule(u"keygen", CSSBindingValueImp::Keygen)
        | CSSValueRule(u"time", CSSBindingValueImp::Time);
}

CSSValueRule::operator std::u16string() const
{
    switch (op) {
    case AB:
        return u"AB(" + CSSSerializeInteger(a) + u", " + CSSSerializeInteger(b) + u")";
    case AnyIdent:
        return u"AnyIdent";
    case AnyNumber:
        return u"AnyNumber";
    case AnyInteger:
        return u"AnyInteger";
    case Angle:
        return u"Angle";
    case Bar:
        return u"Bar";
    case Color:
        return u"Color";
    case Comma:
        return u"Comma";
    case DoubleBar:
        return u"DoubleBar";
    case Function:
        return u"Function(" + CSSSerializeIdentifier(text) + u")";
    case Ident:
        return u"Ident(" + CSSSerializeIdentifier(text) + u")";
    case Juxtapose:
        return u"Juxtapose";
    case Length:
        return u"Length";
    case NonNegativeLength:
        return u"NonNegativeLength";
    case Number:
        return u"Number";
    case Percentage:
        return u"Percentage";
    case NonNegativePercentage:
        return u"NonNegativePercentage";
    case Slash:
        return u"Slash";
    case String:
        return u"String";
    case Uri:
        return u"Uri";
    default:
        return u"##ERROR##";
    }
}

bool CSSValueRule::isValid(CSSValueParser* parser, unsigned propertyID) const
{
    CSSParserTerm& term = parser->getToken();
    unsigned count;
    CSSValueParser::Pos pos;

    if (this->propertyID)
        propertyID = this->propertyID;

    switch (op) {
    case AB: {
        pos = parser->getPos();
        const CSSValueRule& rule = list.front();
        for (count = 0; count < b; ++count) {
            if (a <= count)
                pos = parser->getPos();
            if (!rule.isValid(parser, propertyID)) {
                parser->setPos(pos);
                break;
            }
        }
        if (a <= count)
            return true;
        // TODO: clear stack up to count element
        break;
    }
    case AnyIdent:
        if (term.unit == CSSPrimitiveValue::CSS_IDENT) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case AnyNumber:
        if (term.unit == CSSPrimitiveValue::CSS_NUMBER) {
            parser->push(&term, propertyID);
            return parser->acceptToken();
        }
        break;
    case AnyInteger:
        if (term.unit == CSSPrimitiveValue::CSS_NUMBER && term.number.isInteger()) {
            parser->push(&term, propertyID);
            return parser->acceptToken();
        }
        break;
    case Angle:
        if (term.unit == CSSPrimitiveValue::CSS_DEG ||
            term.unit == CSSPrimitiveValue::CSS_RAD ||
            term.unit == CSSPrimitiveValue::CSS_GRAD) {
            parser->push(&term, propertyID);
            return parser->acceptToken();
        }
        break;
    case Bar:
        pos = parser->getPos();
        for (auto j = list.begin(); j != list.end(); ++j) {
            const CSSValueRule& r = *j;
            if (r.isValid(parser, propertyID)) {
                if (!f || (parser->*f)(*this))
                    return true;
            }
            parser->setPos(pos);
        }
        break;
    case Color:
        if (term.unit == CSSPrimitiveValue::CSS_RGBCOLOR && term.rgb != 0x00FFFFFF) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Comma:
        if (term.op == ',') {
            parser->push(&term, propertyID);
            return parser->acceptToken();
        }
        break;
    case DoubleBar: {
        std::bitset<32> set;  // TODO: XXX 32
        for (size_t i = 0; i < list.size(); ++i) {
            size_t count = set.count();
            size_t option = 0;
            pos = parser->getPos();
            for (auto j = list.begin(); j != list.end(); ++j, ++option) {
                const CSSValueRule& r = *j;
                if (set.test(option))
                    continue;
                if (r.isValid(parser, propertyID)) {
                    set.set(option);
                    break;
                }
                parser->setPos(pos);
            }
            if (count == set.count())
                break;
        }
        return 0 < set.count();
    }
    case Function:
        pos = parser->getPos();
        if (term.unit == CSSParserTerm::CSS_TERM_FUNCTION && term.text == text) {
            CSSParserExpr* prev = parser->switchExpression(term.expr);
            const CSSValueRule& expr = list.front();
            if (expr.isValid(parser, propertyID) && parser->getToken().unit == CSSParserTerm::CSS_TERM_END) {
                parser->switchExpression(prev);
                parser->setPos(pos);
                if (!f || (parser->*f)(*this)) {
                    parser->push(&term, propertyID);  // TODO: insert the function term before the expression term(s).
                    return parser->acceptToken();
                }
            }
            parser->switchExpression(prev);
            parser->setPos(pos);
        }
        break;
    case Ident:
        if (term.unit == CSSPrimitiveValue::CSS_IDENT && term.text == text) {
            if (hasKeyword()) {
                term.unit = CSSParserTerm::CSS_TERM_INDEX;
                term.rgb = b;
            } else if (hasHint() == Number) {
                term.unit = CSSPrimitiveValue::CSS_NUMBER;
                term.number.number = number;
                term.number.integer = false;
            }
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Juxtapose:
        pos = parser->getPos();
        for (auto j = list.begin(); j != list.end(); ++j) {
            const CSSValueRule& r = *j;
            if (!r.isValid(parser, propertyID)) {
                parser->setPos(pos);
                return false;
            }
        }
        if (!f || (parser->*f)(*this))
            return true;
        break;
    case Length:
        if (term.unit == CSSPrimitiveValue::CSS_NUMBER && term.number == 0.0)
            term.unit = CSSPrimitiveValue::CSS_PX;  // TODO: check this is okay for any other rules.
        if (CSSPrimitiveValue::CSS_EMS <= term.unit && term.unit <= CSSPrimitiveValue::CSS_PC) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case NonNegativeLength:
        if (term.unit == CSSPrimitiveValue::CSS_NUMBER && term.number == 0.0)
            term.unit = CSSPrimitiveValue::CSS_PX;  // TODO: check this is okay for any other rules.
        if (CSSPrimitiveValue::CSS_EMS <= term.unit && term.unit <= CSSPrimitiveValue::CSS_PC &&
            0.0f <= term.getNumber()) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Number:
        if (term.unit == CSSPrimitiveValue::CSS_NUMBER && term.number == number) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Percentage:
        if (term.unit == CSSPrimitiveValue::CSS_PERCENTAGE) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case NonNegativePercentage:
        if (term.unit == CSSPrimitiveValue::CSS_PERCENTAGE && 0.0f <= term.getNumber()) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Slash:
        if (term.op == '/') {
            parser->push(&term, propertyID);
            return parser->acceptToken();
        }
        break;
    case String:
        if (term.unit == CSSPrimitiveValue::CSS_STRING) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    case Uri:
        if (term.unit == CSSPrimitiveValue::CSS_URI) {
            if (!f || (parser->*f)(*this)) {
                parser->push(&term, propertyID);
                return parser->acceptToken();
            }
        }
        break;
    default:
        break;
    }
    return false;
}

bool CSSValueParser::isValid(CSSParserExpr* expr)
{
    if (!rule)
        return false;
    stack.clear();
    this->expr = expr;
    iter = expr->list.begin();
    op.op = 0;
    if (iter != expr->list.end()) {
        const CSSParserTerm& term = *iter;
        if (term.op)
            op.op = term.op;
    }
    return rule->isValid(this, propertyID) && iter == expr->list.end();
}

CSSParserTerm& CSSValueParser::getToken()
{
    static CSSParserTerm end = { 0, CSSParserTerm::CSS_TERM_END };
    if (op.op)
        return op;
    if (iter == expr->list.end())
        return end;
    return *iter;
}

bool CSSValueParser::acceptToken()
{
    if (op.op) {
        op.op = 0;
        return true;  // keep the current token since two consecutive operators won't appear in the expression.
    }
    if (++iter != expr->list.end()) {
        const CSSParserTerm& term = *iter;
        if (term.op)
            op.op = term.op;
    }
    return true;
}

CSSValueParser::Pos CSSValueParser::getPos() const
{
    return Pos(op.op, iter);
}

void CSSValueParser::setPos(const CSSValueParser::Pos& pos)
{
    iter = pos.iter;
    op.op = pos.op;
}

bool CSSValueParser::colorKeyword(const CSSValueRule& rule)
{
    unsigned rgb = CSSGetKeywordColor(getToken().text);
    if (rgb == 0x00FFFFFF)
        return false;
    getToken().unit = CSSPrimitiveValue::CSS_RGBCOLOR;
    getToken().rgb = rgb;
    return true;
}

bool CSSValueParser::rgb(const CSSValueRule& rule)
{
    unsigned rgb = 0;
    for (int i = stack.size() - 5; i < stack.size(); i += 2) {
        CSSParserTerm* term = stack.at(i);
        if (term->unit == CSSPrimitiveValue::CSS_NUMBER) {
            rgb <<= 8;
            rgb += static_cast<unsigned char>(std::max(0.0, std::min(255.0, term->getNumber())));
        } else {
            assert(term->unit == CSSPrimitiveValue::CSS_PERCENTAGE);
            rgb <<= 8;
            rgb += static_cast<unsigned char>(roundf(255.0f * (std::max(0.0, std::min(100.0, term->getNumber()) / 100.0f))));
        }
    }
    rgb |= 0xFF000000;
    stack.resize(stack.size() - 5);
    getToken().unit = CSSPrimitiveValue::CSS_RGBCOLOR;
    getToken().rgb = rgb;
    // TODO: delete getToken().expr ??
    return true;
}

CSSValueParser::CSSValueParser(int propertyID) :
    propertyID(propertyID),
    expr(0)
{
    static Initializer initializer;

    op.unit = CSSParserTerm::CSS_TERM_OPERATOR;
    switch (propertyID) {
    case CSSStyleDeclarationImp::Top:
    case CSSStyleDeclarationImp::Right:
    case CSSStyleDeclarationImp::Left:
    case CSSStyleDeclarationImp::Bottom:
        rule = &auto_length;
        break;
    case CSSStyleDeclarationImp::Width:
    case CSSStyleDeclarationImp::Height:
        rule = &auto_non_negative_length;
        break;
    case CSSStyleDeclarationImp::BackgroundAttachment:
        rule = &backgroundAttachment;
        break;
    case CSSStyleDeclarationImp::BackgroundColor:
        rule = &color;
        break;
    case CSSStyleDeclarationImp::BackgroundImage:
        rule = &backgroundImage;
        break;
    case CSSStyleDeclarationImp::BackgroundPosition:
        rule = &backgroundPosition;
        break;
    case CSSStyleDeclarationImp::BackgroundRepeat:
        rule = &backgroundRepeat;
        break;
    case CSSStyleDeclarationImp::Background:
        rule = &background;
        break;
    case CSSStyleDeclarationImp::BorderCollapse:
        rule = &borderCollapse;
        break;
    case CSSStyleDeclarationImp::BorderSpacing:
        rule = &borderSpacing;
        break;
    case CSSStyleDeclarationImp::BorderTopColor:
    case CSSStyleDeclarationImp::BorderRightColor:
    case CSSStyleDeclarationImp::BorderBottomColor:
    case CSSStyleDeclarationImp::BorderLeftColor:
        rule = &color;
        break;
    case CSSStyleDeclarationImp::BorderColor:
        rule = &borderColor;
        break;
    case CSSStyleDeclarationImp::BorderTopStyle:
    case CSSStyleDeclarationImp::BorderRightStyle:
    case CSSStyleDeclarationImp::BorderBottomStyle:
    case CSSStyleDeclarationImp::BorderLeftStyle:
        rule = &border_style;
        break;
    case CSSStyleDeclarationImp::BorderStyle:
        rule = &borderStyle;
        break;
    case CSSStyleDeclarationImp::BorderTopWidth:
    case CSSStyleDeclarationImp::BorderRightWidth:
    case CSSStyleDeclarationImp::BorderBottomWidth:
    case CSSStyleDeclarationImp::BorderLeftWidth:
        rule = &border_width;
        break;
    case CSSStyleDeclarationImp::BorderWidth:
        rule = &borderWidth;
        break;
    case CSSStyleDeclarationImp::BorderTop:
    case CSSStyleDeclarationImp::BorderRight:
    case CSSStyleDeclarationImp::BorderBottom:
    case CSSStyleDeclarationImp::BorderLeft:
    case CSSStyleDeclarationImp::Border:
        rule = &border;
        break;
    case CSSStyleDeclarationImp::CaptionSide:
        rule = &captionSide;
        break;
    case CSSStyleDeclarationImp::Clear:
        rule = &clear;
        break;
    case CSSStyleDeclarationImp::Color:
        rule = &color;
        break;
    case CSSStyleDeclarationImp::Content:
        rule = &content;
        break;
    case CSSStyleDeclarationImp::CounterIncrement:
        rule = &auto_numbering;
        break;
    case CSSStyleDeclarationImp::CounterReset:
        rule = &auto_numbering;
        break;
    case CSSStyleDeclarationImp::Cursor:
        rule = &cursor;
        break;
    case CSSStyleDeclarationImp::Direction:
        rule = &direction;
        break;
    case CSSStyleDeclarationImp::Display:
        rule = &display;
        break;
    case CSSStyleDeclarationImp::Float:
        rule = &float_;
        break;
    case CSSStyleDeclarationImp::FontFamily:
        rule = &fontFamily;
        break;
    case CSSStyleDeclarationImp::FontSize:
        rule = &fontSize;
        break;
    case CSSStyleDeclarationImp::FontStyle:
        rule = &fontStyle;
        break;
    case CSSStyleDeclarationImp::FontVariant:
        rule = &fontVariant;
        break;
    case CSSStyleDeclarationImp::FontWeight:
        rule = &fontWeight;
        break;
    case CSSStyleDeclarationImp::Font:
        rule = &font;
        break;
    case CSSStyleDeclarationImp::LetterSpacing:
        rule = &normal_length;
        break;
    case CSSStyleDeclarationImp::LineHeight:
        rule = &lineHeight;
        break;
    case CSSStyleDeclarationImp::ListStyleImage:
        rule = &listStyleImage;
        break;
    case CSSStyleDeclarationImp::ListStylePosition:
        rule = &listStylePosition;
        break;
    case CSSStyleDeclarationImp::ListStyleType:
        rule = &listStyleType;
        break;
    case CSSStyleDeclarationImp::ListStyle:
        rule = &listStyle;
        break;
    case CSSStyleDeclarationImp::Margin:
        rule = &margin;
        break;
    case CSSStyleDeclarationImp::MarginTop:
    case CSSStyleDeclarationImp::MarginRight:
    case CSSStyleDeclarationImp::MarginBottom:
    case CSSStyleDeclarationImp::MarginLeft:
        rule = &auto_length;
        break;
    case CSSStyleDeclarationImp::MaxHeight:
    case CSSStyleDeclarationImp::MaxWidth:
        rule = &none_non_negative_length;
        break;
    case CSSStyleDeclarationImp::MinHeight:
    case CSSStyleDeclarationImp::MinWidth:
        rule = &percent_non_negative_length;
        break;
    case CSSStyleDeclarationImp::Outline:
        rule = &outline;
        break;
    case CSSStyleDeclarationImp::OutlineColor:
        rule = &outlineColor;
        break;
    case CSSStyleDeclarationImp::OutlineStyle:
        rule = &outlineStyle;
        break;
    case CSSStyleDeclarationImp::OutlineWidth:
        rule = &outlineWidth;
        break;
    case CSSStyleDeclarationImp::Overflow:
        rule = &overflow;
        break;
    case CSSStyleDeclarationImp::PaddingTop:
    case CSSStyleDeclarationImp::PaddingRight:
    case CSSStyleDeclarationImp::PaddingBottom:
    case CSSStyleDeclarationImp::PaddingLeft:
        rule = &percent_non_negative_length;
        break;
    case CSSStyleDeclarationImp::Padding:
        rule = &padding;
        break;
    case CSSStyleDeclarationImp::PageBreakAfter:
    case CSSStyleDeclarationImp::PageBreakBefore:
        rule = &pageBreak;
        break;
    case CSSStyleDeclarationImp::PageBreakInside:
        rule = &pageBreakInside;
        break;
    case CSSStyleDeclarationImp::Position:
        rule = &position;
        break;
    case CSSStyleDeclarationImp::Quotes:
        rule = &quotes;
        break;
    case CSSStyleDeclarationImp::TableLayout:
        rule = &tableLayout;
        break;
    case CSSStyleDeclarationImp::TextAlign:
        rule = &textAlign;
        break;
    case CSSStyleDeclarationImp::TextDecoration:
        rule = &textDecoration;
        break;
    case CSSStyleDeclarationImp::TextIndent:
        rule = &percent_length;
        break;
    case CSSStyleDeclarationImp::TextTransform:
        rule = &textTransform;
        break;
    case CSSStyleDeclarationImp::UnicodeBidi:
        rule = &unicodeBidi;
        break;
    case CSSStyleDeclarationImp::VerticalAlign:
        rule = &verticalAlign;
        break;
    case CSSStyleDeclarationImp::Visibility:
        rule = &visibility;
        break;
    case CSSStyleDeclarationImp::WhiteSpace:
        rule = &whiteSpace;
        break;
    case CSSStyleDeclarationImp::WordSpacing:
        rule = &normal_length;
        break;
    case CSSStyleDeclarationImp::ZIndex:
        rule = &zIndex;
        break;
    case CSSStyleDeclarationImp::Binding:
        rule = &binding;
        break;
    default:
        rule = 0;
        break;
    }
}

}}}}  // org::w3c::dom::bootstrap