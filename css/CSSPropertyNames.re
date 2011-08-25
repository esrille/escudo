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

#include "css/CSSStyleDeclarationImp.h"
#include "css/CSSParser.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

int CSSStyleDeclarationImp::getPropertyID(const std::u16string& ident)
{
    const char16_t* yyin = ident.c_str();
    const char16_t* yymarker = yyin;
    const char16_t* yylimit = yyin + ident.length();

/*!re2c

    re2c:define:YYCTYPE  = "char16_t";
    re2c:define:YYCURSOR = yyin;
    re2c:define:YYMARKER = yymarker;
    re2c:define:YYLIMIT = yylimit;
    re2c:yyfill:enable   = 0;
    re2c:indent:top      = 1;
    re2c:indent:string = "    " ;

    "azimuth"	{ return *yyin ? Unknown : Azimuth; }
    "background"	{ return *yyin ? Unknown : Background; }
    "background-attachment"	{ return *yyin ? Unknown : BackgroundAttachment; }
    "background-color"	{ return *yyin ? Unknown : BackgroundColor; }
    "background-image"	{ return *yyin ? Unknown : BackgroundImage; }
    "background-position"	{ return *yyin ? Unknown : BackgroundPosition; }
    "background-repeat"	{ return *yyin ? Unknown : BackgroundRepeat; }
    "border"	{ return *yyin ? Unknown : Border; }
    "border-collapse"	{ return *yyin ? Unknown : BorderCollapse; }
    "border-color"	{ return *yyin ? Unknown : BorderColor; }
    "border-spacing"	{ return *yyin ? Unknown : BorderSpacing; }
    "border-style"	{ return *yyin ? Unknown : BorderStyle; }
    "border-top"	{ return *yyin ? Unknown : BorderTop; }
    "border-right"	{ return *yyin ? Unknown : BorderRight; }
    "border-bottom"	{ return *yyin ? Unknown : BorderBottom; }
    "border-left"	{ return *yyin ? Unknown : BorderLeft; }
    "border-top-color"	{ return *yyin ? Unknown : BorderTopColor; }
    "border-right-color"	{ return *yyin ? Unknown : BorderRightColor; }
    "border-bottom-color"	{ return *yyin ? Unknown : BorderBottomColor; }
    "border-left-color"	{ return *yyin ? Unknown : BorderLeftColor; }
    "border-top-style"	{ return *yyin ? Unknown : BorderTopStyle; }
    "border-right-style"	{ return *yyin ? Unknown : BorderRightStyle; }
    "border-bottom-style"	{ return *yyin ? Unknown : BorderBottomStyle; }
    "border-left-style"	{ return *yyin ? Unknown : BorderLeftStyle; }
    "border-top-width"	{ return *yyin ? Unknown : BorderTopWidth; }
    "border-right-width"	{ return *yyin ? Unknown : BorderRightWidth; }
    "border-bottom-width"	{ return *yyin ? Unknown : BorderBottomWidth; }
    "border-left-width"	{ return *yyin ? Unknown : BorderLeftWidth; }
    "border-width"	{ return *yyin ? Unknown : BorderWidth; }
    "bottom"	{ return *yyin ? Unknown : Bottom; }
    "caption-side"	{ return *yyin ? Unknown : CaptionSide; }
    "clear"	{ return *yyin ? Unknown : Clear; }
    "clip"	{ return *yyin ? Unknown : Clip; }
    "color"	{ return *yyin ? Unknown : Color; }
    "content"	{ return *yyin ? Unknown : Content; }
    "counter-increment"	{ return *yyin ? Unknown : CounterIncrement; }
    "counter-reset"	{ return *yyin ? Unknown : CounterReset; }
    "cue"	{ return *yyin ? Unknown : Cue; }
    "cue-after"	{ return *yyin ? Unknown : CueAfter; }
    "cue-before"	{ return *yyin ? Unknown : CueBefore; }
    "cursor"	{ return *yyin ? Unknown : Cursor; }
    "direction"	{ return *yyin ? Unknown : Direction; }
    "display"	{ return *yyin ? Unknown : Display; }
    "elevation"	{ return *yyin ? Unknown : Elevation; }
    "empty-cells"	{ return *yyin ? Unknown : EmptyCells; }
    "float"	{ return *yyin ? Unknown : Float; }
    "font"	{ return *yyin ? Unknown : Font; }
    "font-family"	{ return *yyin ? Unknown : FontFamily; }
    "font-size"	{ return *yyin ? Unknown : FontSize; }
    "font-size-adjust"	{ return *yyin ? Unknown : FontSizeAdjust; }
    "font-stretch"	{ return *yyin ? Unknown : FontStretch; }
    "font-style"	{ return *yyin ? Unknown : FontStyle; }
    "font-variant"	{ return *yyin ? Unknown : FontVariant; }
    "font-weight"	{ return *yyin ? Unknown : FontWeight; }
    "height"	{ return *yyin ? Unknown : Height; }
    "left"	{ return *yyin ? Unknown : Left; }
    "letter-spacing"	{ return *yyin ? Unknown : LetterSpacing; }
    "line-height"	{ return *yyin ? Unknown : LineHeight; }
    "list-style"	{ return *yyin ? Unknown : ListStyle; }
    "list-style-image"	{ return *yyin ? Unknown : ListStyleImage; }
    "list-style-position"	{ return *yyin ? Unknown : ListStylePosition; }
    "list-style-type"	{ return *yyin ? Unknown : ListStyleType; }
    "margin"	{ return *yyin ? Unknown : Margin; }
    "margin-top"	{ return *yyin ? Unknown : MarginTop; }
    "margin-right"	{ return *yyin ? Unknown : MarginRight; }
    "margin-bottom"	{ return *yyin ? Unknown : MarginBottom; }
    "margin-left"	{ return *yyin ? Unknown : MarginLeft; }
    "marker-offset"	{ return *yyin ? Unknown : MarkerOffset; }
    "marks"	{ return *yyin ? Unknown : Marks; }
    "max-height"	{ return *yyin ? Unknown : MaxHeight; }
    "max-width"	{ return *yyin ? Unknown : MaxWidth; }
    "min-height"	{ return *yyin ? Unknown : MinHeight; }
    "min-width"	{ return *yyin ? Unknown : MinWidth; }
    "orphans"	{ return *yyin ? Unknown : Orphans; }
    "outline"	{ return *yyin ? Unknown : Outline; }
    "outline-color"	{ return *yyin ? Unknown : OutlineColor; }
    "outline-style"	{ return *yyin ? Unknown : OutlineStyle; }
    "outline-width"	{ return *yyin ? Unknown : OutlineWidth; }
    "overflow"	{ return *yyin ? Unknown : Overflow; }
    "padding"	{ return *yyin ? Unknown : Padding; }
    "padding-top"	{ return *yyin ? Unknown : PaddingTop; }
    "padding-right"	{ return *yyin ? Unknown : PaddingRight; }
    "padding-bottom"	{ return *yyin ? Unknown : PaddingBottom; }
    "padding-left"	{ return *yyin ? Unknown : PaddingLeft; }
    "page"	{ return *yyin ? Unknown : Page; }
    "page-break-after"	{ return *yyin ? Unknown : PageBreakAfter; }
    "page-break-before"	{ return *yyin ? Unknown : PageBreakBefore; }
    "page-break-inside"	{ return *yyin ? Unknown : PageBreakInside; }
    "pause"	{ return *yyin ? Unknown : Pause; }
    "pause-after"	{ return *yyin ? Unknown : PauseAfter; }
    "pause-before"	{ return *yyin ? Unknown : PauseBefore; }
    "pitch"	{ return *yyin ? Unknown : Pitch; }
    "pitch-range"	{ return *yyin ? Unknown : PitchRange; }
    "play-during"	{ return *yyin ? Unknown : PlayDuring; }
    "position"	{ return *yyin ? Unknown : Position; }
    "quotes"	{ return *yyin ? Unknown : Quotes; }
    "richness"	{ return *yyin ? Unknown : Richness; }
    "right"	{ return *yyin ? Unknown : Right; }
    "size"	{ return *yyin ? Unknown : Size; }
    "speak"	{ return *yyin ? Unknown : Speak; }
    "speak-header"	{ return *yyin ? Unknown : SpeakHeader; }
    "speak-numeral"	{ return *yyin ? Unknown : SpeakNumeral; }
    "speak-punctuation"	{ return *yyin ? Unknown : SpeakPunctuation; }
    "speech-rate"	{ return *yyin ? Unknown : SpeechRate; }
    "stress"	{ return *yyin ? Unknown : Stress; }
    "table-layout"	{ return *yyin ? Unknown : TableLayout; }
    "text-align"	{ return *yyin ? Unknown : TextAlign; }
    "text-decoration"	{ return *yyin ? Unknown : TextDecoration; }
    "text-indent"	{ return *yyin ? Unknown : TextIndent; }
    "text-shadow"	{ return *yyin ? Unknown : TextShadow; }
    "text-transform"	{ return *yyin ? Unknown : TextTransform; }
    "top"	{ return *yyin ? Unknown : Top; }
    "unicode-bidi"	{ return *yyin ? Unknown : UnicodeBidi; }
    "vertical-align"	{ return *yyin ? Unknown : VerticalAlign; }
    "visibility"	{ return *yyin ? Unknown : Visibility; }
    "voice-family"	{ return *yyin ? Unknown : VoiceFamily; }
    "volume"	{ return *yyin ? Unknown : Volume; }
    "white-space"	{ return *yyin ? Unknown : WhiteSpace; }
    "widows"	{ return *yyin ? Unknown : Widows; }
    "width"	{ return *yyin ? Unknown : Width; }
    "word-spacing"	{ return *yyin ? Unknown : WordSpacing; }
    "z-index"	{ return *yyin ? Unknown : ZIndex; }
    "binding"	{ return *yyin ? Unknown : Binding; }
    [^] { return Unknown;}

*/

}

}}}}  // org::w3c::dom::bootstrap