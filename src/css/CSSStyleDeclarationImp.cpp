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

#include "CSSStyleDeclarationImp.h"

#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/HTMLBodyElement.h>

#include "CSSPropertyValueImp.h"
#include "CSSValueParser.h"
#include "DocumentImp.h"
#include "MutationEventImp.h"
#include "StackingContext.h"
#include "Table.h"
#include "ViewCSSImp.h"
#include "WindowProxy.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

using namespace css;

const char16_t* CSSStyleDeclarationImp::PropertyNames[MaxProperties] = {
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

    u"opacity",
};

CSSStyleDeclarationBoard::CSSStyleDeclarationBoard(const CSSStyleDeclarationPtr& style) :
    counterIncrement(1),
    counterReset(0)
{
    borderCollapse.specify(style->borderCollapse);
    borderSpacing.specify(style->borderSpacing);
    borderTopWidth.specify(style->borderTopWidth);
    borderRightWidth.specify(style->borderRightWidth);
    borderBottomWidth.specify(style->borderBottomWidth);
    borderLeftWidth.specify(style->borderLeftWidth);
    bottom.specify(style->bottom);
    captionSide.specify(style->captionSide);
    clear.specify(style->clear);
    content.specify(style->content);
    counterIncrement.specify(style->counterIncrement);
    counterReset.specify(style->counterReset);
    direction.specify(style->direction);
    display.specify(style->display);
    float_.specify(style->float_);
    fontFamily.specify(style->fontFamily);
    fontSize.specify(style->fontSize);
    fontStyle.specify(style->fontStyle);
    fontVariant.specify(style->fontVariant);
    fontWeight.specify(style->fontWeight);
    height.specify(style->height);
    left.specify(style->left);
    letterSpacing.specify(style->letterSpacing);
    lineHeight.specify(style->lineHeight);
    listStyleImage.specify(style->listStyleImage);
    listStylePosition.specify(style->listStylePosition);
    listStyleType.specify(style->listStyleType);
    marginTop.specify(style->marginTop);
    marginRight.specify(style->marginRight);
    marginBottom.specify(style->marginBottom);
    marginLeft.specify(style->marginLeft);
    maxHeight.specify(style->maxHeight);
    maxWidth.specify(style->maxWidth);
    minHeight.specify(style->minHeight);
    minWidth.specify(style->minWidth);
    overflow.specify(style->overflow);
    paddingTop.specify(style->paddingTop);
    paddingRight.specify(style->paddingRight);
    paddingBottom.specify(style->paddingBottom);
    paddingLeft.specify(style->paddingLeft);
    position.specify(style->position);
    quotes.specify(style->quotes);
    right.specify(style->right);
    tableLayout.specify(style->tableLayout);
    textAlign.specify(style->textAlign);
    textDecoration.specify(style->textDecoration);
    textIndent.specify(style->textIndent);
    textTransform.specify(style->textTransform);
    top.specify(style->top);
    unicodeBidi.specify(style->unicodeBidi);
    verticalAlign.specify(style->verticalAlign);
    whiteSpace.specify(style->whiteSpace);
    wordSpacing.specify(style->wordSpacing);
    width.specify(style->width);
    zIndex.specify(style->zIndex);
    binding.specify(style);
    htmlAlign.specify(style->htmlAlign);
}

void CSSStyleDeclarationImp::restoreComputedValues(CSSStyleDeclarationBoard& board)
{
    borderCollapse.specify(board.borderCollapse);
    borderSpacing.specify(board.borderSpacing);
    borderTopWidth.specify(board.borderTopWidth);
    borderRightWidth.specify(board.borderRightWidth);
    borderBottomWidth.specify(board.borderBottomWidth);
    borderLeftWidth.specify(board.borderLeftWidth);
    bottom.specify(board.bottom);
    captionSide.specify(board.captionSide);
    clear.specify(board.clear);
    content.specify(board.content);
    counterIncrement.specify(board.counterIncrement);
    counterReset.specify(board.counterReset);
    direction.specify(board.direction);
    display.specify(board.display);
    float_.specify(board.float_);
    fontFamily.specify(board.fontFamily);
    fontSize.specify(board.fontSize);
    fontStyle.specify(board.fontStyle);
    fontVariant.specify(board.fontVariant);
    fontWeight.specify(board.fontWeight);
    height.specify(board.height);
    left.specify(board.left);
    letterSpacing.specify(board.letterSpacing);
    lineHeight.specify(board.lineHeight);
    listStyleImage.specify(board.listStyleImage);
    listStylePosition.specify(board.listStylePosition);
    listStyleType.specify(board.listStyleType);
    marginTop.specify(board.marginTop);
    marginRight.specify(board.marginRight);
    marginBottom.specify(board.marginBottom);
    marginLeft.specify(board.marginLeft);
    maxHeight.specify(board.maxHeight);
    maxWidth.specify(board.maxWidth);
    minHeight.specify(board.minHeight);
    minWidth.specify(board.minWidth);
    overflow.specify(board.overflow);
    paddingTop.specify(board.paddingTop);
    paddingRight.specify(board.paddingRight);
    paddingBottom.specify(board.paddingBottom);
    paddingLeft.specify(board.paddingLeft);
    position.specify(board.position);
    quotes.specify(board.quotes);
    right.specify(board.right);
    tableLayout.specify(board.tableLayout);
    textAlign.specify(board.textAlign);
    textDecoration.specify(board.textDecoration);
    textIndent.specify(board.textIndent);
    textTransform.specify(board.textTransform);
    top.specify(board.top);
    unicodeBidi.specify(board.unicodeBidi);
    verticalAlign.specify(board.verticalAlign);
    whiteSpace.specify(board.whiteSpace);
    wordSpacing.specify(board.wordSpacing);
    width.specify(board.width);
    zIndex.specify(board.zIndex);
    if (board.binding.getValue() ==CSSBindingValueImp::None)
        binding.setValue();
    else
        binding.setURL(board.binding.getURL());
    htmlAlign.specify(board.htmlAlign);
}

unsigned CSSStyleDeclarationBoard::compare(const CSSStyleDeclarationPtr& style)
{
    unsigned flags = 0;
    //
    // Checks for Box::NEED_EXPANSION
    //
    // Note zIndex is checked inside compute().

    if (style->display.getValue() == CSSDisplayValueImp::ListItem) {
        if (style->listStyleImage != listStyleImage)
            flags |= Box::NEED_EXPANSION;
        if (style->listStyleType != listStyleType)
            flags |= Box::NEED_EXPANSION;
        if (style->listStylePosition != listStylePosition)
            flags |= Box::NEED_EXPANSION;
        if (flags & Box::NEED_EXPANSION)
            style->marker = nullptr;
    }

    if (style->display != display) {
        flags |= Box::NEED_EXPANSION;
        if (CSSDisplayValueImp::isProperTableChild(style->display.getValue()) || CSSDisplayValueImp::isProperTableChild(display.getValue()))
            flags |= Box::NEED_TABLE_REFLOW;
    }
    if (style->float_ != float_)
        flags |= Box::NEED_EXPANSION;
    if (style->position != position)
        flags |= Box::NEED_EXPANSION;
#if 0  // TODO: Check following properties
    binding;
#endif

    //
    // Checks for Box::NEED_REFLOW
    //
    // Note: in the following comparisons, the order of left and right sides do matter, which is not good design, though.
    //
    // Firstly, check properties that require style resolutions.
    if (style->borderTopWidth != borderTopWidth)
        flags |= Box::NEED_REFLOW;
    if (style->borderRightWidth != borderRightWidth)
        flags |= Box::NEED_REFLOW;
    if (style->borderBottomWidth != borderBottomWidth)
        flags |= Box::NEED_REFLOW;
    if (style->borderLeftWidth != borderLeftWidth)
        flags |= Box::NEED_REFLOW;
    if ((flags & Box::NEED_REFLOW) && style->display.isTableParts())  // cf. html4/border-collapse-dynamic-cell-003.htm
        flags |= Box::NEED_TABLE_REFLOW;

    if (style->isAbsolutelyPositioned()) {
        if (style->top != top)
            flags |= Box::NEED_REPOSITION;
        if (style->right != right)
            flags |= Box::NEED_REPOSITION;
        if (style->bottom != bottom)
            flags |= Box::NEED_REPOSITION;
        if (style->left != left)
            flags |= Box::NEED_REPOSITION;
    }

    if (style->width != width)
        flags |= Box::NEED_REFLOW;
    if (style->height != height)
        flags |= Box::NEED_REFLOW;

    if (style->marginTop != marginTop)
        flags |= Box::NEED_REFLOW;
    if (style->marginRight != marginRight)
        flags |= Box::NEED_REFLOW;
    if (style->marginBottom != marginBottom)
        flags |= Box::NEED_REFLOW;
    if (style->marginLeft != marginLeft)
        flags |= Box::NEED_REFLOW;

    if (style->maxHeight != maxHeight)
        flags |= Box::NEED_REFLOW;
    if (style->maxWidth != maxWidth)
        flags |= Box::NEED_REFLOW;

    if (style->minHeight != minHeight)
        flags |= Box::NEED_REFLOW;
    if (style->minWidth != minWidth)
        flags |= Box::NEED_REFLOW;

    if (style->paddingTop != paddingTop)
        flags |= Box::NEED_REFLOW;
    if (style->paddingRight != paddingRight)
        flags |= Box::NEED_REFLOW;
    if (style->paddingBottom != paddingBottom)
        flags |= Box::NEED_REFLOW;
    if (style->paddingLeft != paddingLeft)
        flags |= Box::NEED_REFLOW;

    if (style->lineHeight != lineHeight)
        flags |= Box::NEED_REFLOW;
    if (style->textIndent != textIndent)
        flags |= Box::NEED_REFLOW;
    if (style->verticalAlign != verticalAlign)
        flags |= Box::NEED_REFLOW;
    if (style->htmlAlign != htmlAlign)
        flags |= Box::NEED_REFLOW;

    // Check if style needs to be resolved later.
    if (flags & (Box::NEED_REFLOW | Box::NEED_REPOSITION))
        style->unresolve();  // This style needs to be resolved later.

    // Secondly, check properties that do not require style resolutions.
    if (style->clear != clear)
        flags |= Box::NEED_REFLOW;
    if (style->counterIncrement != counterIncrement)
        flags |= Box::NEED_REFLOW;
    if (style->counterReset != counterReset)
        flags |= Box::NEED_REFLOW;
    if (style->direction != direction)
        flags |= Box::NEED_REFLOW;
    if (style->fontFamily != fontFamily)
        flags |= Box::NEED_REFLOW;
    if (style->fontSize != fontSize)
        flags |= Box::NEED_REFLOW;
    if (style->fontStyle != fontStyle)
        flags |= Box::NEED_REFLOW;
    if (style->fontVariant != fontVariant)
        flags |= Box::NEED_REFLOW;
    if (style->fontWeight != fontWeight)
        flags |= Box::NEED_REFLOW;
    if (style->letterSpacing != letterSpacing)
        flags |= Box::NEED_REFLOW;
    if (style->quotes != quotes)
        flags |= Box::NEED_REFLOW;
    if (style->textAlign != textAlign)
        flags |= Box::NEED_REFLOW;
    if (style->textDecoration != textDecoration)
        flags |= Box::NEED_REFLOW;
    if (style->textTransform != textTransform)
        flags |= Box::NEED_REFLOW;
    if (style->unicodeBidi != unicodeBidi)
        flags |= Box::NEED_REFLOW;
    if (style->whiteSpace != whiteSpace)
        flags |= Box::NEED_REFLOW;
    if (style->wordSpacing != wordSpacing)
        flags |= Box::NEED_REFLOW;

    // Table related properties
    if (style->display.getValue() == CSSDisplayValueImp::Table || style->display.getValue() == CSSDisplayValueImp::InlineTable) {
        if (style->borderCollapse != borderCollapse)
            flags |= Box::NEED_TABLE_REFLOW;
        if (style->borderSpacing != borderSpacing)
            flags |= Box::NEED_TABLE_REFLOW;
        if (style->tableLayout != tableLayout)
            flags |= Box::NEED_TABLE_REFLOW;
    }
    if (style->display.getValue() == CSSDisplayValueImp::TableCaption) {
        if (style->captionSide != captionSide)
            flags |= Box::NEED_TABLE_REFLOW;
    }

    return flags;
}

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
    case Cursor:
        return &cursor;
    case Direction:
        return &direction;
    case Display:
        return &display;
    case EmptyCells:
        return &emptyCells;
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
    case LetterSpacing:
        return &letterSpacing;
    case LineHeight:
        return &lineHeight;
    case ListStyleImage:
        return &listStyleImage;
    case ListStylePosition:
        return &listStylePosition;
    case ListStyleType:
        return &listStyleType;
    case ListStyle:
        return &listStyle;
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
    case OutlineColor:
        return &outlineColor;
    case OutlineStyle:
        return &outlineStyle;
    case OutlineWidth:
        return &outlineWidth;
    case Outline:
        return &outline;
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
    case Quotes:
        return &quotes;
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
    case WordSpacing:
        return &wordSpacing;
    case ZIndex:
        return &zIndex;
    case Binding:
        return &binding;
    case Opacity:
        return &opacity;
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
    case ListStyle:
        setInherit(ListStyleType);
        setInherit(ListStylePosition);
        setInherit(ListStyleImage);
        break;
    case Margin:
        setInherit(MarginTop);
        setInherit(MarginRight);
        setInherit(MarginBottom);
        setInherit(MarginLeft);
        break;
    case Outline:
        setInherit(OutlineColor);
        setInherit(OutlineStyle);
        setInherit(OutlineWidth);
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
    case ListStyle:
        resetInherit(ListStyleType);
        resetInherit(ListStylePosition);
        resetInherit(ListStyleImage);
        break;
    case Margin:
        resetInherit(MarginTop);
        resetInherit(MarginRight);
        resetInherit(MarginBottom);
        resetInherit(MarginLeft);
        break;
    case Outline:
        resetInherit(OutlineColor);
        resetInherit(OutlineStyle);
        resetInherit(OutlineWidth);
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

int CSSStyleDeclarationImp::setProperty(int id, CSSParserExpr* expr, const std::u16string& prio)
{
    assert(expr);
    if (id == Unknown) {
        // TODO: delete expr; ?
        return Unknown;
    }
    if (expr->isInherit())
        setInherit(id);
    else {
        CSSValueParser parser(id);
        if (!parser.isValid(expr)) {
            // TODO: delete expr; ?
            return Unknown;
        }
        CSSPropertyValueImp* property = getProperty(id);
        if (!property) {
            // TODO: delete expr; ?
            return Unknown;
        }
        if (!property->setValue(this, &parser))
            return Unknown;
        resetInherit(id);
    }
    if (prio == u"important")
        setImportant(id);
    else
        setProperty(id);
    return id;
}

int CSSStyleDeclarationImp::setProperty(const std::u16string& property, CSSParserExpr* expr, const std::u16string& prio)
{
    if (!expr)
        return Unknown;
    return setProperty(getPropertyID(property), expr, prio);
}

int CSSStyleDeclarationImp::appendProperty(const std::u16string& property, CSSParserExpr* expr, const std::u16string& prio)
{
    propertyID = Unknown;
    if (expr) {
        propertyID = getPropertyID(property);
        expression = expr;
        priority = prio;
    }
    return propertyID;
}

int CSSStyleDeclarationImp::commitAppend()
{
    if (propertyID)
        propertyID = setProperty(propertyID, expression, priority);
    return propertyID;
}

int CSSStyleDeclarationImp::cancelAppend()
{
    propertyID = Unknown;
    return propertyID;
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationPtr& decl, unsigned id)
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
    case Cursor:
        cursor.specify(decl->cursor);
        break;
    case Direction:
        direction.specify(decl->direction);
        break;
    case Display:
        display.specify(decl->display);
        break;
    case EmptyCells:
        emptyCells.specify(decl->emptyCells);
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
    case LetterSpacing:
        letterSpacing.specify(decl->letterSpacing);
        break;
    case LineHeight:
        lineHeight.specify(decl->lineHeight);
        break;
    case ListStyleImage:
        listStyleImage.specify(decl->listStyleImage);
        break;
    case ListStylePosition:
        listStylePosition.specify(decl->listStylePosition);
        break;
    case ListStyleType:
        listStyleType.specify(decl->listStyleType);
        break;
    case ListStyle:
        listStyle.specify(this, decl);
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
    case OutlineColor:
        outlineColor.specify(decl->outlineColor);
        break;
    case OutlineStyle:
        outlineStyle.specify(decl->outlineStyle);
        break;
    case OutlineWidth:
        outlineWidth.specify(decl->outlineWidth);
        break;
    case Outline:
        outline.specify(this, decl);
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
    case Quotes:
        quotes.specify(decl->quotes);
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
    case WordSpacing:
        wordSpacing.specify(decl->wordSpacing);
        break;
    case ZIndex:
        zIndex.specify(decl->zIndex);
        break;
    case Binding:
        binding.specify(decl);
        break;
    case Opacity:
        opacity.specify(decl->opacity);
        break;
    case HtmlAlign:
        htmlAlign.specify(decl->htmlAlign);
        break;
    default:
        break;
    }
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationPtr& decl, const std::bitset<MaxProperties>& set)
{
    unsigned id;
    for (id = 1; id < MaxProperties; ++id) {
        if (!set.test(id))
            continue;
        if (decl->inheritSet.test(id)) {
            setInherit(id);
        } else {
            resetInherit(id);
            specify(decl, id);
        }
        propertySet.set(id);  // Note computed values do not have any important bit set.
    }
}

void CSSStyleDeclarationImp::specifyWithoutInherited(const CSSStyleDeclarationPtr& style)
{
    if (!style)
        return;

    for (unsigned id = 1; id < MaxProperties; ++id) {
        if (!style->propertySet.test(id) || style->inheritSet.test(id))
            continue;
        resetInherit(id);
        specify(style, id);
        propertySet.set(id);  // Note computed values do not have any important bit set.
    }
}

void CSSStyleDeclarationImp::specify(const CSSStyleDeclarationPtr& style)
{
    if (style)
        specify(style, style->propertySet);
}

void CSSStyleDeclarationImp::specifyImportant(const CSSStyleDeclarationPtr& style)
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
    case Cursor:
        cursor.reset();
        break;
    case Direction:
        direction.setValue();
        break;
    case Display:
        display.setValue();
        break;
    case EmptyCells:
        emptyCells.setValue();
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
    case LetterSpacing:
        letterSpacing.setValue();
        break;
    case LineHeight:
        lineHeight.setValue();
        break;
    case ListStyleImage:
        listStyleImage.setValue();
        break;
    case ListStylePosition:
        listStylePosition.setValue();
        break;
    case ListStyleType:
        listStyleType.setValue();
        break;
    case ListStyle:
        reset(ListStyleType);
        reset(ListStylePosition);
        reset(ListStyleImage);
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
    case OutlineColor:
        outlineColor.setValue();
        break;
    case OutlineStyle:
        outlineStyle.setValue();
        break;
    case OutlineWidth:
        outlineWidth.setValue();
        break;
    case Outline:
        outlineColor.setValue();
        outlineStyle.setValue();
        outlineWidth.setValue();
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
    case Quotes:
        quotes.reset();
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
    case WordSpacing:
        wordSpacing.setValue();
        break;
    case ZIndex:
        zIndex.setValue();
        break;
    case Binding:
        binding.setValue();
        break;
    case Opacity:
        opacity.setValue(1.0f);
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
        case ListStyle:
        case Margin:
        case Outline:
        case Padding:
            // ignore shorthand
            break;
        default:
            reset(id);
            break;
        }
    }
}

void CSSStyleDeclarationImp::inherit(const CSSStyleDeclarationPtr& parentStyle, unsigned id)
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
    case ListStyle:
    case Margin:
    case Outline:
    case Padding:
        // ignore shorthand
        break;
    case Top:
        top.inherit(parentStyle->top);
        break;
    case Right:
        right.inherit(parentStyle->right);
        break;
    case Left:
        left.inherit(parentStyle->left);
        break;
    case Bottom:
        bottom.inherit(parentStyle->bottom);
        break;
    case Width:
        width.inherit(parentStyle->width);
        break;
    case Height:
        height.inherit(parentStyle->height);
        break;
    case BackgroundPosition:
        backgroundPosition.inherit(parentStyle->backgroundPosition);
        break;
    case BorderTopWidth:
        borderTopWidth.inherit(parentStyle->borderTopWidth);
        break;
    case BorderRightWidth:
        borderRightWidth.inherit(parentStyle->borderRightWidth);
        break;
    case BorderBottomWidth:
        borderBottomWidth.inherit(parentStyle->borderBottomWidth);
        break;
    case BorderLeftWidth:
        borderLeftWidth.inherit(parentStyle->borderLeftWidth);
        break;
    case BorderSpacing:
        borderSpacing.inherit(parentStyle->borderSpacing);
        break;
    case FontSize:
        fontSize.inherit(parentStyle->fontSize);
        break;
    case LetterSpacing:
        letterSpacing.inherit(parentStyle->letterSpacing);
        break;
    case LineHeight:
        lineHeight.inherit(parentStyle->lineHeight);
        break;
    case MarginTop:
        marginTop.inherit(parentStyle->marginTop);
        break;
    case MarginRight:
        marginRight.inherit(parentStyle->marginRight);
        break;
    case MarginBottom:
        marginBottom.inherit(parentStyle->marginBottom);
        break;
    case MarginLeft:
        marginLeft.inherit(parentStyle->marginLeft);
        break;
    case MaxHeight:
        maxHeight.inherit(parentStyle->maxHeight);
        break;
    case MaxWidth:
        maxWidth.inherit(parentStyle->maxWidth);
        break;
    case MinHeight:
        minHeight.inherit(parentStyle->minHeight);
        break;
    case MinWidth:
        minWidth.inherit(parentStyle->minWidth);
        break;
    case OutlineWidth:
        outlineWidth.inherit(parentStyle->outlineWidth);
        break;
    case PaddingTop:
        paddingTop.inherit(parentStyle->paddingTop);
        break;
    case PaddingRight:
        paddingRight.inherit(parentStyle->paddingRight);
        break;
    case PaddingBottom:
        paddingBottom.inherit(parentStyle->paddingBottom);
        break;
    case PaddingLeft:
        paddingLeft.inherit(parentStyle->paddingLeft);
        break;
    case TextIndent:
        textIndent.inherit(parentStyle->textIndent);
        break;
    case VerticalAlign:
        verticalAlign.inherit(parentStyle->verticalAlign);
        break;
    case WordSpacing:
        wordSpacing.inherit(parentStyle->wordSpacing);
        break;
    default:
        specify(parentStyle, id);
        break;
    }
}

void CSSStyleDeclarationImp::inheritProperties(const CSSStyleDeclarationPtr& parentStyle)
{
    assert(parentStyle);
    for (unsigned id = 1; id < MaxProperties; ++id)
        inherit(parentStyle, id);
}

void CSSStyleDeclarationImp::compute(ViewCSSImp* view, const CSSStyleDeclarationPtr& parentStyle, Element element)
{
    unresolve();

    if (this == parentStyle.get())
        return;

    bool wasPositioned = isPositioned();

    html::HTMLElement htmlElement;
    if (html::HTMLElement::hasInstance(element))
        htmlElement = interface_cast<html::HTMLElement>(element);

    if (getPseudoElementSelectorType() == CSSPseudoElementSelector::NonPseudo) {
        initialize();
        // TODO: Do the same for pseudo elements:
        for (unsigned i = 1; i < MaxProperties; ++i)
            reset(i);
        CSSStyleDeclarationPtr elementDecl;
        if (htmlElement)
            elementDecl = std::dynamic_pointer_cast<CSSStyleDeclarationImp>(htmlElement.getStyle().self());
        // Normal declarations
        for (auto i = ruleSet.begin(); i != ruleSet.end(); ++i) {
            if (CSSStyleDeclarationPtr pseudo = createPseudoElementStyle(i->getPseudoElementID())) {
                if (i->mql)
                    setFlags(MediaDependent);
                if (i->getMatches() && i->isActive(element, view) && i->getDeclaration())
                    pseudo->specify(i->getDeclaration()->getCSSStyleDeclarationPtr());
            }
        }
        if (elementDecl)
            specify(elementDecl);
        // Author important declarations
        for (auto i = ruleSet.begin(); i != ruleSet.end(); ++i) {
            if (CSSStyleDeclarationPtr pseudo = createPseudoElementStyle(i->getPseudoElementID())) {
                if (i->mql)
                    setFlags(MediaDependent);
                if (i->getMatches() && i->isActive(element, view) && !i->isUserStyle() && i->getDeclaration())
                    pseudo->specifyImportant(i->getDeclaration()->getCSSStyleDeclarationPtr());
            }
        }
        if (elementDecl)
            specifyImportant(elementDecl);
        // User important declarations
        for (auto i = ruleSet.begin(); i != ruleSet.end(); ++i) {
            if (CSSStyleDeclarationPtr pseudo = createPseudoElementStyle(i->getPseudoElementID())) {
                if (i->mql)
                    setFlags(MediaDependent);
                if (i->getMatches() && i->isActive(element, view) && i->isUserStyle() && i->getDeclaration())
                    pseudo->specifyImportant(i->getDeclaration()->getCSSStyleDeclarationPtr());
            }
        }
    }

    this->parentStyle = parentStyle;
    if (!parentStyle)  // is it the root element?
        resetInheritedProperties();
    else
        inheritProperties(parentStyle);

    backgroundColor.compute();
    borderTopStyle.compute();
    borderRightStyle.compute();
    borderBottomStyle.compute();
    borderLeftStyle.compute();
    color.compute();
    outlineColor.compute();
    outlineStyle.compute();

    visibility.compute();
    opacity.compute(view, this);
    opacity.clip(0.0f, 1.0f);

    display.compute(this, element);
    fontSize.compute(view, parentStyle);
    fontWeight.compute(view, parentStyle);
    fontTexture = view->selectFont(getCSSStyleDeclarationPtr());
    lineHeight.compute(view, this);
    verticalAlign.compute(view, this);

    width.compute(view, this);
    height.compute(view, this);

    minWidth.compute(view, this);
    minHeight.compute(view, this);

    if (isPositioned()) {   // TODO: Check what to do if only 'opacity' is less than 1.0.
        top.compute(view, this);
        right.compute(view, this);
        bottom.compute(view, this);
        left.compute(view, this);
    }

    marginTop.compute(view, this);
    marginRight.compute(view, this);
    marginBottom.compute(view, this);
    marginLeft.compute(view, this);

    // CSS3 would allow percentages in border width, the resolved values of these should be the used values, too.
    borderTopWidth.compute(view, borderTopStyle, this);
    borderRightWidth.compute(view, borderRightStyle, this);
    borderBottomWidth.compute(view, borderBottomStyle, this);
    borderLeftWidth.compute(view, borderLeftStyle, this);
    outlineWidth.compute(view, outlineStyle, this);

    paddingTop.compute(view, this);
    paddingRight.compute(view, this);
    paddingBottom.compute(view, this);
    paddingLeft.compute(view, this);

    borderTopColor.compute(this);
    borderRightColor.compute(this);
    borderBottomColor.compute(this);
    borderLeftColor.compute(this);

    backgroundImage.compute(view);
    backgroundPosition.compute(view, this);

    borderSpacing.compute(view, this);

    listStyleImage.compute(view, this);
    listStylePosition.compute(view, this);
    content.compute(view, this);

    textIndent.compute(view, this);
    letterSpacing.compute(view, this);
    wordSpacing.compute(view, this);

    if (isFloat() || isAbsolutelyPositioned() || !parentStyle || isInlineBlock())
        textDecorationContext.update(this);
    else if (parentStyle->textDecorationContext.hasDecoration())
        textDecorationContext = parentStyle->textDecorationContext;
    else
        textDecorationContext.update(this);

    computeStackingContext(view, parentStyle, wasPositioned);

    // Note the parent style of a pseudo element style is not always the corresponding element's style.
    // It will be computed layter by layout().

    if (auto body = bodyStyle.lock()) {
        body->clearFlags(CSSStyleDeclarationImp::Computed);
        body->compute(view, getCSSStyleDeclarationPtr(), view->getDocument()->getBody());
        if (overflow.getValue() == CSSOverflowValueImp::Visible)
            overflow.useBodyValue(body->overflow);
        if (backgroundColor.getARGB() == 0 && backgroundImage.isNone()) {
            background.specify(this, body);
            body->fontSize.compute(view, getCSSStyleDeclarationPtr());
            backgroundImage.compute(view);
            // Note if the lengths are given by 'em' or 'ex', the referred font size is
            // the one of the 'body' style.
            backgroundPosition.compute(view, body.get());
            body->backgroundColor.setValue(CSSColorValueImp::Transparent);
            body->backgroundImage.setValue();
        }
    }

    setFlags(Computed);
}

void CSSStyleDeclarationImp::computeStackingContext(ViewCSSImp* view, const CSSStyleDeclarationPtr& parentStyle, bool wasPositioned)
{
    // Reorganize the stacking context if the 'position' value has been changed
    if (wasPositioned ^ isPositioned())
        stackingContext = 0;

    int zValue = (position.getValue() == CSSPositionValueImp::Static) ? 0 : zIndex.getValue();
    if (!parentStyle) {
        if (view->getStackingContexts() == 0) {
            view->setStackingContexts(new(std::nothrow) StackingContext(false, zValue, getCSSStyleDeclarationPtr()));
            stackingContext = view->getStackingContexts();
        }
    } else if (isPositioned()) {
        bool isAuto = (opacity.getValue() < 1.0f) ? false : zIndex.isAuto();
        if (!stackingContext) {
            if (isAuto)
                stackingContext = parentStyle->stackingContext->getAuto(getCSSStyleDeclarationPtr());
            else
                stackingContext = parentStyle->stackingContext->addContext(zValue, getCSSStyleDeclarationPtr());
        } else {
            // Update z-index
            stackingContext->setZIndex(isAuto, zValue);
        }
    } else
        stackingContext = parentStyle->stackingContext;
}

bool CSSStyleDeclarationImp::updateCounters(ViewCSSImp* view, CSSAutoNumberingValueImp::CounterContext* context)
{
    if (display.isNone())
        return false;

    bool updated(false);
    if (getPseudoElementSelectorType() == CSSPseudoElementSelector::Marker) {
        // Execute implicit 'counter-increment: list-item;'
        if (CounterImpPtr counter = view->getCounter(u"list-item")) {
            counter->increment(1);
            updated = true;
        }
    }
    if (counterReset.hasCounter()) {
        counterReset.resetCounter(view, context);
        updated = true;
    }
    if (counterIncrement.hasCounter()) {
        counterIncrement.incrementCounter(view, context);
        updated = true;
    }
    return updated;
}

// calculate resolved values that requite containing block information for calucuration
// cf. CSSOM 7. Resolved Values
unsigned CSSStyleDeclarationImp::resolve(ViewCSSImp* view, const ContainingBlock* containingBlock)
{
    unsigned result = 0;

    if (!setContainingBlockSize(containingBlock->width, containingBlock->height))
        result = Box::NEED_REFLOW;
    else if (isResolved())
        return result;

    auto parentStyle = getParentStyle();
    if (parentStyle) {
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

    lineHeight.resolve(view, this);
    verticalAlign.resolve(view, this);

    bool nonExplicitWidth = false;
    bool nonExplicitHeight = false;
    if (const Box* containingBox = dynamic_cast<const Box*>(containingBlock)) {
        CSSStyleDeclarationPtr containingStyle = containingBox->getStyle();
        if (containingStyle && !isAbsolutelyPositioned()) {
            // While it is not defined in CSS 2.1, we treat an unknown percentage width as
            // 'auto'. A percentage width is unknown if the style associated with the
            // containing block has the 'auto' width, and its width is to (potentially) shrink-to-fit.
            // Particularly we'd like to pass the following CSS 2.1 tests:
            //   http://test.csswg.org/suites/css2.1/20110323/html4/margin-collapse-143.htm
            //   http://test.csswg.org/suites/css2.1/20110323/html4/margin-collapse-clear-012.htm
            //   http://test.csswg.org/suites/css2.1/20110323/html4/abspos-width-002.htm
            if (containingStyle->width.isAuto() &&
                (containingStyle->isFloat() || containingStyle->isInlineBlock() || containingStyle->isAbsolutelyPositioned()))
                nonExplicitWidth = true;
            if (containingStyle->height.isAuto())
                nonExplicitHeight = true;
        }
    }

    // Resolve properties that depend on the containing block size

    if (width.isPercentage() && nonExplicitWidth && parentStyle)
        width.setValue();  // change height to 'auto'.
    else
        width.resolve(view, this, containingBlock->width);

    if (height.isPercentage() && nonExplicitHeight && parentStyle)
        height.setValue();  // change height to 'auto'.
    else
        height.resolve(view, this, containingBlock->height);

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

    if (minWidth.isPercentage() && nonExplicitWidth)
        minWidth.setValue(0.0f);
    else
        minWidth.resolve(view, this, containingBlock->width);
    if (maxWidth.isPercentage() && nonExplicitWidth)
        maxWidth.setValue();
    else
        maxWidth.resolve(view, this, containingBlock->width);

    if (minHeight.isPercentage() && nonExplicitHeight)
        minHeight.setValue(0.0f);
    else
        minHeight.resolve(view, this, containingBlock->height);
    if (maxHeight.isPercentage() && nonExplicitHeight)
        maxHeight.setValue();
    else
        maxHeight.resolve(view, this, containingBlock->height);

    textIndent.resolve(view, this, containingBlock->width);

    setFlags(Resolved);
    return result;
}

// Calculate left, right, top, bottom for a 'relative' element.
// TODO: rtl
bool CSSStyleDeclarationImp::resolveRelativeOffset(float& x, float &y)
{
    if (getUsedPosition() != CSSPositionValueImp::Relative)
        return false;

    float h = 0.0f;
    if (!left.isAuto())
        h = left.getPx();
    else if (!right.isAuto())
        h = -right.getPx();
    x += h;

    float v = 0.0f;
    if (!top.isAuto())
        v = top.getPx();
    else if (!bottom.isAuto())
        v = -bottom.getPx();
    y += v;

    return true;
}

size_t CSSStyleDeclarationImp::processWhiteSpace(std::u16string& data, char16_t& prevChar)
{
    unsigned prop = whiteSpace.getValue();
    switch (prop) {
    case CSSWhiteSpaceValueImp::Normal:
    case CSSWhiteSpaceValueImp::Nowrap:
    case CSSWhiteSpaceValueImp::PreLine: {
        size_t spacePos;
        size_t spaceLen = 0;
        for (size_t i = 0; i < data.length(); ++i) {
            char16_t c = data[i];
            switch (c) {
            case '\t':
                c = data[i] = ' ';
                break;
            case '\r':
                c = data[i] = '\n';
                // FALL THROUGH
            case '\n':
                if (prop == CSSWhiteSpaceValueImp::Normal || prop == CSSWhiteSpaceValueImp::Nowrap)
                    c = data[i] = ' ';
                break;
            default:
                break;
            }
            if (c == ' ') {
                if (spaceLen == 0)
                    spacePos = i;
                ++spaceLen;
            } else {
                if (0 < spaceLen) {
                    if (prevChar == ' ' || c == '\n') {
                        data.erase(spacePos, spaceLen);
                        i = spacePos;
                    } else if (1 < spaceLen) {
                        ++spacePos;
                        --spaceLen;
                        data.erase(spacePos, spaceLen);
                        i = spacePos;
                    }
                    spaceLen = 0;
                }
                if (c == '\n') {
                    size_t j;
                    for (j = i + 1; j < data.length() && isSpace(data[j]); ++j)
                        ;
                    --j;
                    if (i < j)
                        data.erase(i + 1, j - i);
                }
                prevChar = c;
            }
        }
        if (0 < spaceLen) {
            if (prevChar == ' ')
                data.erase(spacePos, spaceLen);
            else if (1 < spaceLen) {
                ++spacePos;
                --spaceLen;
                data.erase(spacePos, spaceLen);
            }
            prevChar = ' ';
        }
        break;
    }
    default:
        if (0 < data.length())
            prevChar = 0;
        break;
    }
    return data.length();
}

size_t CSSStyleDeclarationImp::processLineHeadWhiteSpace(const std::u16string& data, size_t position)
{
    switch (whiteSpace.getValue()) {
    case CSSWhiteSpaceValueImp::Normal:
    case CSSWhiteSpaceValueImp::Nowrap:
    case CSSWhiteSpaceValueImp::PreLine:
        if (position < data.length() && data[position] == u' ')
            ++position;
        break;
    default:
        break;
    }
    return position;
}

FontTexture* CSSStyleDeclarationImp::getAltFontTexture(ViewCSSImp* view, FontTexture* current, char32_t u)
{
    return view->selectAltFont(getCSSStyleDeclarationPtr(), current, u);
}

bool CSSStyleDeclarationImp::isFlowRoot() const
{
    return float_.getValue() != CSSFloatValueImp::None ||
           overflow.getValue() != CSSOverflowValueImp::Visible ||
           display.isFlowRoot() ||
           binding.isInlineBlock() ||
           isAbsolutelyPositioned();
           /* TODO || and more conditions... */
}

void CSSStyleDeclarationImp::clearBox()
{
    box = lastBox = 0;
}

void CSSStyleDeclarationImp::addBox(Box* b)
{
    if (dynamic_cast<Block*>(b))
        box = lastBox = b;
    else if (InlineBox* inlineBox = dynamic_cast<InlineBox*>(b)) {
        if (isBlockLevel())
            return;
        if (isInlineBlock() && inlineBox->getFont())
            return;
        if (!box)
            box = lastBox = b;
        else
            lastBox = b;
        if (auto parentStyle = getParentStyle())
            parentStyle->addBox(b);
    }
}

void CSSStyleDeclarationImp::removeBox(Box* b)
{
    CSSStyleDeclarationPtr style = getCSSStyleDeclarationPtr();
    do {
        if (style->box == b && style->lastBox == b)
            style->box = style->lastBox = 0;
        else if (style->box == b)
            style->box = style->lastBox;
        else if (style->lastBox == b)
            style->lastBox = style->box;
        else
            break;
    } while (style = style->getParentStyle());
}

Block* CSSStyleDeclarationImp::updateInlines(Element element)
{
    CSSStyleDeclarationPtr style = getCSSStyleDeclarationPtr();
    do {
        switch (style->display.getValue()) {
        case CSSDisplayValueImp::None:
        case CSSDisplayValueImp::TableColumnGroup:
        case CSSDisplayValueImp::TableColumn:
            if (!style->getBox())
                return 0;
            break;
        case CSSDisplayValueImp::Table:
        case CSSDisplayValueImp::InlineTable:
            if (TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(style->getBox())) {
                table->revertTablePart(element);
                return table;
            }
            break;
        case CSSDisplayValueImp::Block:
        case CSSDisplayValueImp::ListItem:
        case CSSDisplayValueImp::InlineBlock:
        case CSSDisplayValueImp::TableCell:
        case CSSDisplayValueImp::TableCaption:
            if (Block* block = dynamic_cast<Block*>(style->getBox())) {
                block->clearInlines();
                return block;
            }
            break;
        case CSSDisplayValueImp::TableRowGroup:
        case CSSDisplayValueImp::TableHeaderGroup:
        case CSSDisplayValueImp::TableFooterGroup:
        case CSSDisplayValueImp::TableRow:
            // These styles do not have corresponding boxes; so look up for the containing
            // table wrapper box and clear its grid.
            break;
        default:
            break;
        }
    } while (style = style->getParentStyle());
    return 0;
}

Block* CSSStyleDeclarationImp::revert(Element element)
{
    auto parentStyle = getParentStyle();
    if (parentStyle && stackingContext && parentStyle->stackingContext != stackingContext) {
        stackingContext->detach();
        stackingContext = 0;
    }

    Block* block = dynamic_cast<Block*>(getBox());
    if (!block)
        return updateInlines(element);

    if (stackingContext)
        stackingContext->removeBox(block);

    Block* holder = 0;
    for (Box* parent = block->getParentBox(); parent; parent = parent->getParentBox()) {
        if (holder = dynamic_cast<Block*>(parent))
            break;
    }
    if (!holder)
        return 0;

    TableWrapperBox* table = dynamic_cast<TableWrapperBox*>(holder);
    if (!table)
        table = dynamic_cast<TableWrapperBox*>(holder->getParentBox());
    if (table) {
        table->revertTablePart(block->getNode());
        return table;
    }
    if (holder->removeBlock(block->getNode()))
        holder->clearInlines();
    else {
        assert(block->getParentBox() == holder);
        holder->removeChild(block);
        block->removeDescendants();
        block->release_();
        holder->setFlags(Box::NEED_REFLOW);
    }
    clearBox();

    if (holder->isAnonymous()) {
        block = holder;
        holder = dynamic_cast<Block*>(block->getParentBox());
        if (holder)
            holder->clearInlines();
        else {
            // block can be an anonymous table part:
            // cf. html4/table-anonymous-objects-108.htm
            // TODO: Review the following code again.
            if (auto parent = block->getParentBox()) {
                parent->removeChild(block);
                block->removeDescendants();
                block->release_();
            }
            holder = 0;
        }
    }

    return holder;
}

void CSSStyleDeclarationImp::requestReconstruct(unsigned short flags)
{
    for (CSSStyleDeclarationPtr style = getCSSStyleDeclarationPtr(); style; style = style->getParentStyle()) {
        if (Block* block = dynamic_cast<Block*>(style->getBox())) {
            block->setFlags(flags);
            return;
        }
    }
}

void CSSStyleDeclarationImp::clearFlags(unsigned f)
{
    flags &= ~f;
    if (f & Computed) {
        for (int id = 1; id < CSSPseudoElementSelector::MaxPseudoElements; ++id) {
            if (CSSStyleDeclarationImp* pseudo = pseudoElements[id].get()) {
                if (pseudo != this)
                    pseudo->clearFlags(Computed);
            }
        }
        auto parentStyle = getParentStyle();
        if (parentStyle && parentStyle->getBodyStyle().get() == this)
            parentStyle->clearFlags(Computed);
    }
}

CSSStyleDeclarationPtr CSSStyleDeclarationImp::getPseudoElementStyle(int id)
{
    assert(0 <= id && id < CSSPseudoElementSelector::MaxPseudoElements);
    if (id == CSSPseudoElementSelector::Marker) {
        // Check marker
        if (!display.isListItem())
            return nullptr;
        CSSStyleDeclarationPtr markerStyle = pseudoElements[id];
        if (!markerStyle) {
            if (markerStyle = createPseudoElementStyle(CSSPseudoElementSelector::Marker)) {
                // Set the default marker style
                markerStyle->setDisplay(u"inline-block");
                markerStyle->setLetterSpacing(u"normal");
                markerStyle->setWordSpacing(u"normal");
                markerStyle->setFontFamily(u"sans-serif");
            }
        }
    }
    return (id == 0) ? getCSSStyleDeclarationPtr() : pseudoElements[id];
}

CSSStyleDeclarationPtr CSSStyleDeclarationImp::getPseudoElementStyle(const std::u16string& name)
{
    return getPseudoElementStyle(CSSPseudoElementSelector::getPseudoElementID(name));
}

CSSStyleDeclarationPtr CSSStyleDeclarationImp::createPseudoElementStyle(int id)
{
    assert(0 <= id && id < CSSPseudoElementSelector::MaxPseudoElements);
    if (id == 0)
        return getCSSStyleDeclarationPtr();
    CSSStyleDeclarationPtr style = pseudoElements[id];
    if (!style) {
        if (style = std::make_shared<CSSStyleDeclarationImp>(id))
            pseudoElements[id] = style;
    }
    return style;
}

CSSStyleDeclarationPtr CSSStyleDeclarationImp::getAffectedByHover()
{
    CSSStyleDeclarationPtr affected;
    for (CSSStyleDeclarationPtr style = getCSSStyleDeclarationPtr(); style; style = style->getParentStyle()) {
        if (style->affectedBits & (1u << CSSPseudoClassSelector::Hover))
            affected = style;
    }
    return affected;
}

std::u16string CSSStyleDeclarationImp::resolveRelativeURL(const std::u16string& url)
{
    std::u16string href = getParentRule().getParentStyleSheet().getHref();
    if (href.empty())
        return url;
    URL base(href);
    URL target(href, url);
    return target;
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

void CSSStyleDeclarationImp::setCssText(const std::u16string& cssText)
{
    CSSParser parser;
    parser.setStyleDeclaration(getCSSStyleDeclarationPtr());
    parser.parseDeclarations(cssText);
}

unsigned int CSSStyleDeclarationImp::getLength()
{
    return importantSet.count() + propertySet.count();
}

std::u16string CSSStyleDeclarationImp::item(unsigned int index)
{
    if (getFlags() & ComputedStyle) {
        if (auto document = std::dynamic_pointer_cast<DocumentImp>(getOwner().self())) {
            if (WindowProxyPtr window = document->getDefaultWindow())
                window->updateView();
        }
        return getProperty(index)->getCssText(this);
    }

    if (inheritSet.test(index))
        return u"inherit";
    if (propertySet.test(index))
        return getProperty(index)->getCssText(this);
    return u"";
}

std::u16string CSSStyleDeclarationImp::getPropertyValue(const std::u16string& property)
{
    return item(getPropertyID(property));
}

std::u16string CSSStyleDeclarationImp::getPropertyPriority(const std::u16string& property)
{
    if (importantSet.test(getPropertyID(property)))
        return u"important";
    else
        return u"";
}

void CSSStyleDeclarationImp::setProperty(int id, Nullable<std::u16string> value, const std::u16string& prio)
{
    if (!value.hasValue())
        removeProperty(id);
    else {
        std::u16string v = value.value();
        stripLeadingAndTrailingWhitespace(v);
        if (v.empty())
            removeProperty(id);
        else {
            CSSParser parser;
            CSSParserExpr* expr = parser.parseExpression(v);
            if (!expr)
                return;
            setProperty(id, expr, prio);
            delete expr;
        }
    }

    auto owner = getOwner();
    if (owner && html::HTMLElement::hasInstance(owner)) {
        assert(getPseudoElementSelectorType() == CSSPseudoElementSelector::NonPseudo);
        html::HTMLElement element(owner.self());
        // Note the mutation event triggered by the following operation must be ignored in the element.
        setFlags(Mutated);
        element.setAttribute(u"style", getCssText());
        clearFlags(Mutated);
    }
}

void CSSStyleDeclarationImp::setProperty(const std::u16string& property, const std::u16string& value)
{
    int id = getPropertyID(property);
    if (id == Unknown)
        return;
    setProperty(id, value, u"");
}

void CSSStyleDeclarationImp::setProperty(const std::u16string& property, const std::u16string& value, const std::u16string& priority)
{
    if (priority == u"non-css") {  // ES extension
        if (CSSStyleDeclarationPtr nonCSS = createPseudoElementStyle(CSSPseudoElementSelector::NonCSS)) {
            nonCSS->setProperty(property, value);
            ElementPtr elm = std::dynamic_pointer_cast<ElementImp>(getOwner().self());
            if (!elm)
                return;
            DocumentPtr doc = elm->getOwnerDocumentImp();
            if (!doc)
                return;
            WindowProxyPtr window = doc->getDefaultWindow();
            if (!window || !window->getView())
                return;
            CSSStyleDeclarationPtr style = window->getView()->getStyle(elm);
            if (!style)
                return;
            style->requestReconstruct(Box::NEED_STYLE_RECALCULATION);
            style->clearFlags(CSSStyleDeclarationImp::Computed);
        }
        return;
    }

    int id = getPropertyID(property);
    if (id == Unknown)
        return;
    setProperty(id, value, priority);
}

std::u16string CSSStyleDeclarationImp::removeProperty(int id)
{
    reset(id);
    return u"";  // ask Anne
}

std::u16string CSSStyleDeclarationImp::removeProperty(const std::u16string& property)
{
    std::u16string result = removeProperty(getPropertyID(property));
    auto owner = getOwner();
    if (owner && html::HTMLElement::hasInstance(owner)) {
        html::HTMLElement element(owner.self());
        // Note the mutation event triggered by the following operation must be ignored in the element.
        setFlags(Mutated);
        element.setAttribute(u"style", getCssText());
        clearFlags(Mutated);
    }
    return result;
}

css::CSSRule CSSStyleDeclarationImp::getParentRule()
{
    return parentRule.lock();
}

//
// CSS2Properties
//

Nullable<std::u16string> CSSStyleDeclarationImp::getAzimuth()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setAzimuth(const Nullable<std::u16string>& azimuth)
{
    setProperty(Azimuth, azimuth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackground()
{
    return item(Background);
}

void CSSStyleDeclarationImp::setBackground(const Nullable<std::u16string>& background)
{
    setProperty(Background, background);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundAttachment()
{
    return item(BackgroundAttachment);
}

void CSSStyleDeclarationImp::setBackgroundAttachment(const Nullable<std::u16string>& backgroundAttachment)
{
    setProperty(BackgroundAttachment, backgroundAttachment);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundColor()
{
    return item(BackgroundColor);
}

void CSSStyleDeclarationImp::setBackgroundColor(const Nullable<std::u16string>& backgroundColor)
{
    setProperty(BackgroundColor, backgroundColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundImage()
{
    return item(BackgroundImage);
}

void CSSStyleDeclarationImp::setBackgroundImage(const Nullable<std::u16string>& backgroundImage)
{
    setProperty(BackgroundImage, backgroundImage);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundPosition()
{
    return item(BackgroundPosition);
}

void CSSStyleDeclarationImp::setBackgroundPosition(const Nullable<std::u16string>& backgroundPosition)
{
    setProperty(BackgroundPosition, backgroundPosition);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBackgroundRepeat()
{
    return item(BackgroundRepeat);
}

void CSSStyleDeclarationImp::setBackgroundRepeat(const Nullable<std::u16string>& backgroundRepeat)
{
    setProperty(BackgroundRepeat, backgroundRepeat);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorder()
{
    return item(Border);
}

void CSSStyleDeclarationImp::setBorder(const Nullable<std::u16string>& border)
{
    setProperty(Border, border);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderCollapse()
{
    return item(BorderCollapse);
}

void CSSStyleDeclarationImp::setBorderCollapse(const Nullable<std::u16string>& borderCollapse)
{
    setProperty(BorderCollapse, borderCollapse);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderColor()
{
    return item(BorderColor);
}

void CSSStyleDeclarationImp::setBorderColor(const Nullable<std::u16string>& borderColor)
{
    setProperty(BorderColor, borderColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderSpacing()
{
    return item(BorderSpacing);
}

void CSSStyleDeclarationImp::setBorderSpacing(const Nullable<std::u16string>& borderSpacing)
{
    setProperty(BorderSpacing, borderSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderStyle()
{
    return item(BorderStyle);
}

void CSSStyleDeclarationImp::setBorderStyle(const Nullable<std::u16string>& borderStyle)
{
    setProperty(BorderStyle, borderStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTop()
{
    return item(BorderTop);
}

void CSSStyleDeclarationImp::setBorderTop(const Nullable<std::u16string>& borderTop)
{
    setProperty(BorderTop, borderTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRight()
{
    return item(BorderRight);
}

void CSSStyleDeclarationImp::setBorderRight(const Nullable<std::u16string>& borderRight)
{
    setProperty(BorderRight, borderRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottom()
{
    return item(BorderBottom);
}

void CSSStyleDeclarationImp::setBorderBottom(const Nullable<std::u16string>& borderBottom)
{
    setProperty(BorderBottom, borderBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeft()
{
    return item(BorderLeft);
}

void CSSStyleDeclarationImp::setBorderLeft(const Nullable<std::u16string>& borderLeft)
{
    setProperty(BorderLeft, borderLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopColor()
{
    return item(BorderTopColor);
}

void CSSStyleDeclarationImp::setBorderTopColor(const Nullable<std::u16string>& borderTopColor)
{
    setProperty(BorderTopColor, borderTopColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightColor()
{
    return item(BorderRightColor);
}

void CSSStyleDeclarationImp::setBorderRightColor(const Nullable<std::u16string>& borderRightColor)
{
    setProperty(BorderRightColor, borderRightColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomColor()
{
    return item(BorderBottomColor);
}

void CSSStyleDeclarationImp::setBorderBottomColor(const Nullable<std::u16string>& borderBottomColor)
{
    setProperty(BorderBottomColor, borderBottomColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftColor()
{
    return item(BorderLeftColor);
}

void CSSStyleDeclarationImp::setBorderLeftColor(const Nullable<std::u16string>& borderLeftColor)
{
    setProperty(BorderLeftColor, borderLeftColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopStyle()
{
    return item(BorderTopStyle);
}

void CSSStyleDeclarationImp::setBorderTopStyle(const Nullable<std::u16string>& borderTopStyle)
{
    setProperty(BorderTopStyle, borderTopStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightStyle()
{
    return item(BorderRightStyle);
}

void CSSStyleDeclarationImp::setBorderRightStyle(const Nullable<std::u16string>& borderRightStyle)
{
    setProperty(BorderRightStyle, borderRightStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomStyle()
{
    return item(BorderBottomStyle);
}

void CSSStyleDeclarationImp::setBorderBottomStyle(const Nullable<std::u16string>& borderBottomStyle)
{
    setProperty(BorderBottomStyle, borderBottomStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftStyle()
{
    return item(BorderLeftStyle);
}

void CSSStyleDeclarationImp::setBorderLeftStyle(const Nullable<std::u16string>& borderLeftStyle)
{
    setProperty(BorderLeftStyle, borderLeftStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderTopWidth()
{
    return item(BorderTopWidth);
}

void CSSStyleDeclarationImp::setBorderTopWidth(const Nullable<std::u16string>& borderTopWidth)
{
    setProperty(BorderTopWidth, borderTopWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderRightWidth()
{
    return item(BorderRightWidth);
}

void CSSStyleDeclarationImp::setBorderRightWidth(const Nullable<std::u16string>& borderRightWidth)
{
    setProperty(BorderRightWidth, borderRightWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderBottomWidth()
{
    return item(BorderBottomWidth);
}

void CSSStyleDeclarationImp::setBorderBottomWidth(const Nullable<std::u16string>& borderBottomWidth)
{
    setProperty(BorderBottomWidth, borderBottomWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderLeftWidth()
{
    return item(BorderLeftWidth);
}

void CSSStyleDeclarationImp::setBorderLeftWidth(const Nullable<std::u16string>& borderLeftWidth)
{
    setProperty(BorderLeftWidth, borderLeftWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBorderWidth()
{
    return item(BorderWidth);
}

void CSSStyleDeclarationImp::setBorderWidth(const Nullable<std::u16string>& borderWidth)
{
    setProperty(BorderWidth, borderWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getBottom()
{
    return item(Bottom);
}

void CSSStyleDeclarationImp::setBottom(const Nullable<std::u16string>& bottom)
{
    setProperty(Bottom, bottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCaptionSide()
{
    return item(CaptionSide);
}

void CSSStyleDeclarationImp::setCaptionSide(const Nullable<std::u16string>& captionSide)
{
    setProperty(CaptionSide, captionSide);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getClear()
{
    return item(Clear);
}

void CSSStyleDeclarationImp::setClear(const Nullable<std::u16string>& clear)
{
    setProperty(Clear, clear);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getClip()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setClip(const Nullable<std::u16string>& clip)
{
    setProperty(Clip, clip);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getColor()
{
    return item(Color);
}

void CSSStyleDeclarationImp::setColor(const Nullable<std::u16string>& color)
{
    setProperty(Color, color);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getContent()
{
    return item(Content);
}

void CSSStyleDeclarationImp::setContent(const Nullable<std::u16string>& content)
{
    setProperty(Content, content);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCounterIncrement()
{
    return item(CounterIncrement);
}

void CSSStyleDeclarationImp::setCounterIncrement(const Nullable<std::u16string>& counterIncrement)
{
    setProperty(CounterIncrement, counterIncrement);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCounterReset()
{
    return item(CounterReset);
}

void CSSStyleDeclarationImp::setCounterReset(const Nullable<std::u16string>& counterReset)
{
    setProperty(CounterReset, counterReset);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCue()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCue(const Nullable<std::u16string>& cue)
{
    setProperty(Cue, cue);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCueAfter()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCueAfter(const Nullable<std::u16string>& cueAfter)
{
    setProperty(CueAfter, cueAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCueBefore()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setCueBefore(const Nullable<std::u16string>& cueBefore)
{
    setProperty(CueBefore, cueBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCursor()
{
    return item(Cursor);
}

void CSSStyleDeclarationImp::setCursor(const Nullable<std::u16string>& cursor)
{
    setProperty(Cursor, cursor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getDirection()
{
    return item(Direction);
}

void CSSStyleDeclarationImp::setDirection(const Nullable<std::u16string>& direction)
{
    setProperty(Direction, direction);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getDisplay()
{
    return item(Display);
}

void CSSStyleDeclarationImp::setDisplay(const Nullable<std::u16string>& display)
{
    setProperty(Display, display);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getElevation()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setElevation(const Nullable<std::u16string>& elevation)
{
    setProperty(Elevation, elevation);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getEmptyCells()
{
    return item(EmptyCells);
}

void CSSStyleDeclarationImp::setEmptyCells(const Nullable<std::u16string>& emptyCells)
{
    setProperty(EmptyCells, emptyCells);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getCssFloat()
{
    return item(Float);
}

void CSSStyleDeclarationImp::setCssFloat(const Nullable<std::u16string>& cssFloat)
{
    setProperty(Float, cssFloat);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFont()
{
    return item(Font);
}

void CSSStyleDeclarationImp::setFont(const Nullable<std::u16string>& font)
{
    setProperty(Font, font);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontFamily()
{
    return item(FontFamily);
}

void CSSStyleDeclarationImp::setFontFamily(const Nullable<std::u16string>& fontFamily)
{
    setProperty(FontFamily, fontFamily);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontSize()
{
    return item(FontSize);
}

void CSSStyleDeclarationImp::setFontSize(const Nullable<std::u16string>& fontSize)
{
    setProperty(FontSize, fontSize);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontSizeAdjust()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setFontSizeAdjust(const Nullable<std::u16string>& fontSizeAdjust)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontStretch()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setFontStretch(const Nullable<std::u16string>& fontStretch)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontStyle()
{
    return item(FontStyle);
}

void CSSStyleDeclarationImp::setFontStyle(const Nullable<std::u16string>& fontStyle)
{
    setProperty(FontStyle, fontStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontVariant()
{
    return item(FontVariant);
}

void CSSStyleDeclarationImp::setFontVariant(const Nullable<std::u16string>& fontVariant)
{
    setProperty(FontVariant, fontVariant);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getFontWeight()
{
    return item(FontWeight);
}

void CSSStyleDeclarationImp::setFontWeight(const Nullable<std::u16string>& fontWeight)
{
    setProperty(FontWeight, fontWeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getHeight()
{
    return item(Height);
}

void CSSStyleDeclarationImp::setHeight(const Nullable<std::u16string>& height)
{
    setProperty(Height, height);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLeft()
{
    return item(Left);
}

void CSSStyleDeclarationImp::setLeft(const Nullable<std::u16string>& left)
{
    setProperty(Left, left);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLetterSpacing()
{
    return item(LetterSpacing);
}

void CSSStyleDeclarationImp::setLetterSpacing(const Nullable<std::u16string>& letterSpacing)
{
    setProperty(LetterSpacing, letterSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getLineHeight()
{
    return item(LineHeight);
}

void CSSStyleDeclarationImp::setLineHeight(const Nullable<std::u16string>& lineHeight)
{
    setProperty(LineHeight, lineHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyle()
{
    return item(ListStyle);
}

void CSSStyleDeclarationImp::setListStyle(const Nullable<std::u16string>& listStyle)
{
    setProperty(ListStyle, listStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyleImage()
{
    return item(ListStyleImage);
}

void CSSStyleDeclarationImp::setListStyleImage(const Nullable<std::u16string>& listStyleImage)
{
    setProperty(ListStyleImage, listStyleImage);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStylePosition()
{
    return item(ListStylePosition);
}

void CSSStyleDeclarationImp::setListStylePosition(const Nullable<std::u16string>& listStylePosition)
{
    setProperty(ListStylePosition, listStylePosition);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getListStyleType()
{
    return item(ListStyleType);
}

void CSSStyleDeclarationImp::setListStyleType(const Nullable<std::u16string>& listStyleType)
{
    setProperty(ListStyleType, listStyleType);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMargin()
{
    return item(Margin);
}

void CSSStyleDeclarationImp::setMargin(const Nullable<std::u16string>& margin)
{
    setProperty(Margin, margin);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginTop()
{
    return item(MarginTop);
}

void CSSStyleDeclarationImp::setMarginTop(const Nullable<std::u16string>& marginTop)
{
    setProperty(MarginTop, marginTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginRight()
{
    return item(MarginRight);
}

void CSSStyleDeclarationImp::setMarginRight(const Nullable<std::u16string>& marginRight)
{
    setProperty(MarginRight, marginRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginBottom()
{
    return item(MarginBottom);
}

void CSSStyleDeclarationImp::setMarginBottom(const Nullable<std::u16string>& marginBottom)
{
    setProperty(MarginBottom, marginBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarginLeft()
{
    return item(MarginLeft);
}

void CSSStyleDeclarationImp::setMarginLeft(const Nullable<std::u16string>& marginLeft)
{
    setProperty(MarginLeft, marginLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarkerOffset()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setMarkerOffset(const Nullable<std::u16string>& markerOffset)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMarks()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setMarks(const Nullable<std::u16string>& marks)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMaxHeight()
{
    return item(MaxHeight);
}

void CSSStyleDeclarationImp::setMaxHeight(const Nullable<std::u16string>& maxHeight)
{
    setProperty(MaxHeight, maxHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMaxWidth()
{
    return item(MaxWidth);
}

void CSSStyleDeclarationImp::setMaxWidth(const Nullable<std::u16string>& maxWidth)
{
    setProperty(MaxWidth, maxWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMinHeight()
{
    return item(MinHeight);
}

void CSSStyleDeclarationImp::setMinHeight(const Nullable<std::u16string>& minHeight)
{
    setProperty(MinHeight, minHeight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getMinWidth()
{
    return item(MinWidth);
}

void CSSStyleDeclarationImp::setMinWidth(const Nullable<std::u16string>& minWidth)
{
    setProperty(MinWidth, minWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOrphans()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setOrphans(const Nullable<std::u16string>& orphans)
{
    setProperty(Orphans, orphans);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutline()
{
    return item(Outline);
}

void CSSStyleDeclarationImp::setOutline(const Nullable<std::u16string>& outline)
{
    setProperty(Outline, outline);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineColor()
{
    return item(OutlineColor);
}

void CSSStyleDeclarationImp::setOutlineColor(const Nullable<std::u16string>& outlineColor)
{
    setProperty(OutlineColor, outlineColor);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineStyle()
{
    return item(OutlineStyle);
}

void CSSStyleDeclarationImp::setOutlineStyle(const Nullable<std::u16string>& outlineStyle)
{
    setProperty(OutlineStyle, outlineStyle);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOutlineWidth()
{
    return item(OutlineWidth);
}

void CSSStyleDeclarationImp::setOutlineWidth(const Nullable<std::u16string>& outlineWidth)
{
    setProperty(OutlineWidth, outlineWidth);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getOverflow()
{
    return item(Overflow);
}

void CSSStyleDeclarationImp::setOverflow(const Nullable<std::u16string>& overflow)
{
    setProperty(Overflow, overflow);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPadding()
{
    return item(Padding);
}

void CSSStyleDeclarationImp::setPadding(const Nullable<std::u16string>& padding)
{
    setProperty(Padding, padding);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingTop()
{
    return item(PaddingTop);
}

void CSSStyleDeclarationImp::setPaddingTop(const Nullable<std::u16string>& paddingTop)
{
    setProperty(PaddingTop, paddingTop);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingRight()
{
    return item(PaddingRight);
}

void CSSStyleDeclarationImp::setPaddingRight(const Nullable<std::u16string>& paddingRight)
{
    setProperty(PaddingRight, paddingRight);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingBottom()
{
    return item(PaddingBottom);
}

void CSSStyleDeclarationImp::setPaddingBottom(const Nullable<std::u16string>& paddingBottom)
{
    setProperty(PaddingBottom, paddingBottom);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPaddingLeft()
{
    return item(PaddingLeft);
}

void CSSStyleDeclarationImp::setPaddingLeft(const Nullable<std::u16string>& paddingLeft)
{
    setProperty(PaddingLeft, paddingLeft);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPage()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPage(const Nullable<std::u16string>& page)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakAfter()
{
    return item(PageBreakAfter);
}

void CSSStyleDeclarationImp::setPageBreakAfter(const Nullable<std::u16string>& pageBreakAfter)
{
    setProperty(PageBreakAfter, pageBreakAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakBefore()
{
    return item(PageBreakBefore);
}

void CSSStyleDeclarationImp::setPageBreakBefore(const Nullable<std::u16string>& pageBreakBefore)
{
    setProperty(PageBreakBefore, pageBreakBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPageBreakInside()
{
    return item(PageBreakInside);
}

void CSSStyleDeclarationImp::setPageBreakInside(const Nullable<std::u16string>& pageBreakInside)
{
    setProperty(PageBreakInside, pageBreakInside);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPause()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPause(const Nullable<std::u16string>& pause)
{
    setProperty(Pause, pause);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPauseAfter()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPauseAfter(const Nullable<std::u16string>& pauseAfter)
{
    setProperty(PauseAfter, pauseAfter);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPauseBefore()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPauseBefore(const Nullable<std::u16string>& pauseBefore)
{
    setProperty(PauseBefore, pauseBefore);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPitch()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPitch(const Nullable<std::u16string>& pitch)
{
    setProperty(Pitch, pitch);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPitchRange()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPitchRange(const Nullable<std::u16string>& pitchRange)
{
    setProperty(PitchRange, pitchRange);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPlayDuring()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setPlayDuring(const Nullable<std::u16string>& playDuring)
{
    setProperty(PlayDuring, playDuring);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getPosition()
{
    return item(Position);
}

void CSSStyleDeclarationImp::setPosition(const Nullable<std::u16string>& position)
{
    setProperty(Position, position);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getQuotes()
{
    return item(Quotes);
}

void CSSStyleDeclarationImp::setQuotes(const Nullable<std::u16string>& quotes)
{
    setProperty(Quotes, quotes);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getRichness()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setRichness(const Nullable<std::u16string>& richness)
{
    setProperty(Richness, richness);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getRight()
{
    return item(Right);
}

void CSSStyleDeclarationImp::setRight(const Nullable<std::u16string>& right)
{
    setProperty(Right, right);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSize()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSize(const Nullable<std::u16string>& size)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeak()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeak(const Nullable<std::u16string>& speak)
{
    setProperty(Speak, speak);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakHeader()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakHeader(const Nullable<std::u16string>& speakHeader)
{
    setProperty(SpeakHeader, speakHeader);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakNumeral()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakNumeral(const Nullable<std::u16string>& speakNumeral)
{
    setProperty(SpeakNumeral, speakNumeral);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeakPunctuation()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeakPunctuation(const Nullable<std::u16string>& speakPunctuation)
{
    setProperty(SpeakPunctuation, speakPunctuation);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getSpeechRate()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setSpeechRate(const Nullable<std::u16string>& speechRate)
{
    setProperty(SpeechRate, speechRate);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getStress()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setStress(const Nullable<std::u16string>& stress)
{
    setProperty(Stress, stress);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTableLayout()
{
    return item(TableLayout);
}

void CSSStyleDeclarationImp::setTableLayout(const Nullable<std::u16string>& tableLayout)
{
    setProperty(TableLayout, tableLayout);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextAlign()
{
    return item(TextAlign);
}

void CSSStyleDeclarationImp::setTextAlign(const Nullable<std::u16string>& textAlign)
{
    setProperty(TextAlign, textAlign);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextDecoration()
{
    return item(TextDecoration);
}

void CSSStyleDeclarationImp::setTextDecoration(const Nullable<std::u16string>& textDecoration)
{
    setProperty(TextDecoration, textDecoration);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextIndent()
{
    return item(TextIndent);
}

void CSSStyleDeclarationImp::setTextIndent(const Nullable<std::u16string>& textIndent)
{
    setProperty(TextIndent, textIndent);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextShadow()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setTextShadow(const Nullable<std::u16string>& textShadow)
{
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTextTransform()
{
    return item(TextTransform);
}

void CSSStyleDeclarationImp::setTextTransform(const Nullable<std::u16string>& textTransform)
{
    setProperty(TextTransform, textTransform);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getTop()
{
    return item(Top);
}

void CSSStyleDeclarationImp::setTop(const Nullable<std::u16string>& top)
{
    setProperty(Top, top);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getUnicodeBidi()
{
    return item(UnicodeBidi);
}

void CSSStyleDeclarationImp::setUnicodeBidi(const Nullable<std::u16string>& unicodeBidi)
{
    setProperty(UnicodeBidi, unicodeBidi);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVerticalAlign()
{
    return item(VerticalAlign);
}

void CSSStyleDeclarationImp::setVerticalAlign(const Nullable<std::u16string>& verticalAlign)
{
    setProperty(VerticalAlign, verticalAlign);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVisibility()
{
    return item(Visibility);
}

void CSSStyleDeclarationImp::setVisibility(const Nullable<std::u16string>& visibility)
{
    setProperty(Visibility, visibility);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVoiceFamily()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setVoiceFamily(const Nullable<std::u16string>& voiceFamily)
{
    setProperty(VoiceFamily, voiceFamily);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getVolume()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setVolume(const Nullable<std::u16string>& volume)
{
    setProperty(Volume, volume);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWhiteSpace()
{
    return item(WhiteSpace);
}

void CSSStyleDeclarationImp::setWhiteSpace(const Nullable<std::u16string>& whiteSpace)
{
    setProperty(WhiteSpace, whiteSpace);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWidows()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setWidows(const Nullable<std::u16string>& widows)
{
    setProperty(Widows, widows);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWidth()
{
    return item(Width);
}

void CSSStyleDeclarationImp::setWidth(const Nullable<std::u16string>& width)
{
    setProperty(Width, width);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getWordSpacing()
{
    return item(WordSpacing);
}

void CSSStyleDeclarationImp::setWordSpacing(const Nullable<std::u16string>& wordSpacing)
{
    setProperty(WordSpacing, wordSpacing);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getZIndex()
{
    return item(ZIndex);
}

void CSSStyleDeclarationImp::setZIndex(const Nullable<std::u16string>& zIndex)
{
    setProperty(ZIndex, zIndex);
}

Nullable<std::u16string> CSSStyleDeclarationImp::getHTMLAlign()
{
    // TODO: implement me!
    return Nullable<std::u16string>();
}

void CSSStyleDeclarationImp::setHTMLAlign(const Nullable<std::u16string>& align)
{
    unsigned a = HTMLAlignValueImp::None;
    if (align.hasValue()) {
        std::u16string value = align.value();
        toLower(value);
        if (value == u"left")
            a = HTMLAlignValueImp::Left;
        else if (value == u"right")
            a = HTMLAlignValueImp::Right;
        else if (value == u"center" || value == u"middle")
            a = HTMLAlignValueImp::Center;
        else if (value == u"justify")
            a = HTMLAlignValueImp::Justify;
        else
            a = HTMLAlignValueImp::None;
    }
    htmlAlign.setValue(a);
    resetInherit(HtmlAlign);
    setProperty(HtmlAlign);
}

//
// CSSStyleDeclaration-48
//

Nullable<std::u16string> CSSStyleDeclarationImp::getOpacity()
{
    return item(Opacity);
}

void CSSStyleDeclarationImp::setOpacity(const Nullable<std::u16string>& opacity)
{
    setProperty(Opacity, opacity);
}

void CSSStyleDeclarationImp::initialize(bool ctor)
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
    inheritSet.reset();
    for (unsigned i = 0; i < sizeof defaultInherit / sizeof defaultInherit[0]; ++i)
        setInherit(defaultInherit[i]);
    if (!ctor && !getParentStyle()) {
        // for the interaction with the "body" element.
        overflow.setValue();
        background.reset(this);
    }
    flags &= ~MediaDependent;
}

void CSSStyleDeclarationImp::clearProperties()
{
    propertySet.reset();
    importantSet.reset();
    initialize();
}

void CSSStyleDeclarationImp::resetComputedStyle()
{
    ruleSet.clear();
    affectedBits = 0;
    for (int i = CSSPseudoElementSelector::NonCSS; i < CSSPseudoElementSelector::MaxPseudoElements; ++i)
        pseudoElements[i] = nullptr;
    marker = before = after = nullptr;
}

void CSSStyleDeclarationImp::reset()
{
    parentRule.reset();
    flags = 0;
    parentStyle.reset();
    bodyStyle.reset();
    stackingContext = 0;
    fontTexture = 0;
    clearProperties();
    resetComputedStyle();
    clearBox();
}

CSSStyleDeclarationImp::CSSStyleDeclarationImp(int pseudoElementSelectorType) :
    propertyID(Unknown),
    expression(0),
    flags(0),
    affectedBits(0),
    emptyInline(0),
    stackingContext(0),
    fontTexture(0),
    pseudoElementSelectorType(pseudoElementSelectorType),
    containingBlockWidth(0.0f),
    containingBlockHeight(0.0f),
    box(0),
    lastBox(0),
    backgroundColor(CSSColorValueImp::Transparent),
    borderTop(0),
    borderRight(1),
    borderBottom(2),
    borderLeft(3),
    counterIncrement(1),
    counterReset(0),
    marginTop(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginRight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginBottom(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginLeft(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minHeight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minWidth(0.0f, css::CSSPrimitiveValue::CSS_PX),
    textIndent(0.0f, css::CSSPrimitiveValue::CSS_PX),
    opacity(1.0f)
{
    initialize(true);
    for (int i = 0; i < CSSPseudoElementSelector::MaxPseudoElements; ++i)
        pseudoElements[i] = 0;
}

// for cloneNode()
CSSStyleDeclarationImp::CSSStyleDeclarationImp(const CSSStyleDeclarationPtr& org) :
    propertyID(Unknown),
    expression(0),
    flags(0),
    affectedBits(0),
    emptyInline(0),
    stackingContext(0),
    fontTexture(0),
    pseudoElementSelectorType(org->pseudoElementSelectorType),
    containingBlockWidth(0.0f),
    containingBlockHeight(0.0f),
    box(0),
    lastBox(0),
    backgroundColor(CSSColorValueImp::Transparent),
    borderTop(0),
    borderRight(1),
    borderBottom(2),
    borderLeft(3),
    counterIncrement(1),
    counterReset(0),
    marginTop(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginRight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginBottom(0.0f, css::CSSPrimitiveValue::CSS_PX),
    marginLeft(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minHeight(0.0f, css::CSSPrimitiveValue::CSS_PX),
    minWidth(0.0f, css::CSSPrimitiveValue::CSS_PX),
    textIndent(0.0f, css::CSSPrimitiveValue::CSS_PX),
    opacity(1.0f)
{
    for (int i = 0; i < CSSPseudoElementSelector::MaxPseudoElements; ++i)
        pseudoElements[i] = 0;
    specify(org);
    specifyImportant(org);
}

CSSStyleDeclarationImp::~CSSStyleDeclarationImp()
{
}

const char16_t* CSSStyleDeclarationImp::getPropertyName(int propertyID)
{
    if (propertyID < 0 || MaxProperties <= propertyID)
        return PropertyNames[0];
    return PropertyNames[propertyID];
}

}}}}  // org::w3c::dom::bootstrap
