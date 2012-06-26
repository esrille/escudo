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

#include "HTMLTokenizer.h"

#include <assert.h>
#include <string.h>

#include "utf.h"

#include "css/CSSSerialize.h"
#include "AttrImp.h"

#include <algorithm>

using namespace org::w3c::dom::bootstrap;

namespace
{

const unsigned UnicodeMax = 0x10FFFFu;
const char16_t SurrogateBegin = 0xd800;
const char16_t SurrogateEnd = 0xdfff;
const ssize_t MaxEntityName = 64;  // without including the terminating zero

struct ReplacementCharacter
{
    unsigned number;
    char16_t unicode;
};

bool compareReplacementCharacters(const ReplacementCharacter& s, const ReplacementCharacter& t)
{
    return s.number < t.number;
}

ReplacementCharacter replacementCharacters[] = {
    { 0x00, u'\xfffd' },
    { 0x0d, u'\x000d' },
    { 0x80, u'\x20ac' },
    { 0x81, u'\x0081' },
    { 0x81, u'\x0081' },
    { 0x82, u'\x201a' },
    { 0x83, u'\x0192' },
    { 0x84, u'\x201e' },
    { 0x85, u'\x2026' },
    { 0x86, u'\x2020' },
    { 0x87, u'\x2021' },
    { 0x88, u'\x02c6' },
    { 0x89, u'\x2030' },
    { 0x8a, u'\x0160' },
    { 0x8b, u'\x2039' },
    { 0x8c, u'\x0152' },
    { 0x8d, u'\x008d' },
    { 0x8e, u'\x017d' },
    { 0x8f, u'\x008f' },
    { 0x90, u'\x0090' },
    { 0x91, u'\x2018' },
    { 0x92, u'\x2019' },
    { 0x93, u'\x201c' },
    { 0x94, u'\x201d' },
    { 0x95, u'\x2022' },
    { 0x96, u'\x2013' },
    { 0x97, u'\x2014' },
    { 0x98, u'\x02dc' },
    { 0x99, u'\x2122' },
    { 0x9a, u'\x0161' },
    { 0x9b, u'\x203a' },
    { 0x9c, u'\x0153' },
    { 0x9d, u'\x009d' },
    { 0x9e, u'\x017e' },
    { 0x9f, u'\x0178' },
};

struct Entity
{
    const char* entity;
    char32_t unicode;
};

bool compareEntities(const Entity& s, const Entity& t)
{
    return strcmp(s.entity, t.entity) < 0;
}

Entity entities[] = {
    { "AElig", u'\xc6' },
    { "AElig;", u'\xc6' },
    { "AMP", u'&' },
    { "AMP;", u'&' },
    { "Aacute", u'\xc1' },
    { "Aacute;", u'\xc1' },
    { "Abreve;", u'\x0102' },
    { "Acirc", u'\xc2' },
    { "Acirc;", u'\xc2' },
    { "Acy;", u'\x0410' },
    { "Afr;", U'\x0001d504' },
    { "Agrave", u'\xc0' },
    { "Agrave;", u'\xc0' },
    { "Alpha;", u'\x0391' },
    { "Amacr;", u'\x0100' },
    { "And;", u'\x2a53' },
    { "Aogon;", u'\x0104' },
    { "Aopf;", U'\x0001d538' },
    { "ApplyFunction;", u'\x2061' },
    { "Aring", u'\xc5' },
    { "Aring;", u'\xc5' },
    { "Ascr;", U'\x0001d49c' },
    { "Assign;", u'\x2254' },
    { "Atilde", u'\xc3' },
    { "Atilde;", u'\xc3' },
    { "Auml", u'\xc4' },
    { "Auml;", u'\xc4' },
    { "Backslash;", u'\x2216' },
    { "Barv;", u'\x2ae7' },
    { "Barwed;", u'\x2306' },
    { "Bcy;", u'\x0411' },
    { "Because;", u'\x2235' },
    { "Bernoullis;", u'\x212c' },
    { "Beta;", u'\x0392' },
    { "Bfr;", U'\x0001d505' },
    { "Bopf;", U'\x0001d539' },
    { "Breve;", u'\x02d8' },
    { "Bscr;", u'\x212c' },
    { "Bumpeq;", u'\x224e' },
    { "CHcy;", u'\x0427' },
    { "COPY", u'\xa9' },
    { "COPY;", u'\xa9' },
    { "Cacute;", u'\x0106' },
    { "Cap;", u'\x22d2' },
    { "CapitalDifferentialD;", u'\x2145' },
    { "Cayleys;", u'\x212d' },
    { "Ccaron;", u'\x010c' },
    { "Ccedil", u'\xc7' },
    { "Ccedil;", u'\xc7' },
    { "Ccirc;", u'\x0108' },
    { "Cconint;", u'\x2230' },
    { "Cdot;", u'\x010a' },
    { "Cedilla;", u'\xb8' },
    { "CenterDot;", u'\xb7' },
    { "Cfr;", u'\x212d' },
    { "Chi;", u'\x03a7' },
    { "CircleDot;", u'\x2299' },
    { "CircleMinus;", u'\x2296' },
    { "CirclePlus;", u'\x2295' },
    { "CircleTimes;", u'\x2297' },
    { "ClockwiseContourIntegral;", u'\x2232' },
    { "CloseCurlyDoubleQuote;", u'\x201d' },
    { "CloseCurlyQuote;", u'\x2019' },
    { "Colon;", u'\x2237' },
    { "Colone;", u'\x2a74' },
    { "Congruent;", u'\x2261' },
    { "Conint;", u'\x222f' },
    { "ContourIntegral;", u'\x222e' },
    { "Copf;", u'\x2102' },
    { "Coproduct;", u'\x2210' },
    { "CounterClockwiseContourIntegral;", u'\x2233' },
    { "Cross;", u'\x2a2f' },
    { "Cscr;", U'\x0001d49e' },
    { "Cup;", u'\x22d3' },
    { "CupCap;", u'\x224d' },
    { "DD;", u'\x2145' },
    { "DDotrahd;", u'\x2911' },
    { "DJcy;", u'\x0402' },
    { "DScy;", u'\x0405' },
    { "DZcy;", u'\x040f' },
    { "Dagger;", u'\x2021' },
    { "Darr;", u'\x21a1' },
    { "Dashv;", u'\x2ae4' },
    { "Dcaron;", u'\x010e' },
    { "Dcy;", u'\x0414' },
    { "Del;", u'\x2207' },
    { "Delta;", u'\x0394' },
    { "Dfr;", U'\x0001d507' },
    { "DiacriticalAcute;", u'\xb4' },
    { "DiacriticalDot;", u'\x02d9' },
    { "DiacriticalDoubleAcute;", u'\x02dd' },
    { "DiacriticalGrave;", u'`' },
    { "DiacriticalTilde;", u'\x02dc' },
    { "Diamond;", u'\x22c4' },
    { "DifferentialD;", u'\x2146' },
    { "Dopf;", U'\x0001d53b' },
    { "Dot;", u'\xa8' },
    { "DotDot;", u'\x20dc' },
    { "DotEqual;", u'\x2250' },
    { "DoubleContourIntegral;", u'\x222f' },
    { "DoubleDot;", u'\xa8' },
    { "DoubleDownArrow;", u'\x21d3' },
    { "DoubleLeftArrow;", u'\x21d0' },
    { "DoubleLeftRightArrow;", u'\x21d4' },
    { "DoubleLeftTee;", u'\x2ae4' },
    { "DoubleLongLeftArrow;", u'\x27f8' },
    { "DoubleLongLeftRightArrow;", u'\x27fa' },
    { "DoubleLongRightArrow;", u'\x27f9' },
    { "DoubleRightArrow;", u'\x21d2' },
    { "DoubleRightTee;", u'\x22a8' },
    { "DoubleUpArrow;", u'\x21d1' },
    { "DoubleUpDownArrow;", u'\x21d5' },
    { "DoubleVerticalBar;", u'\x2225' },
    { "DownArrow;", u'\x2193' },
    { "DownArrowBar;", u'\x2913' },
    { "DownArrowUpArrow;", u'\x21f5' },
    { "DownBreve;", u'\x0311' },
    { "DownLeftRightVector;", u'\x2950' },
    { "DownLeftTeeVector;", u'\x295e' },
    { "DownLeftVector;", u'\x21bd' },
    { "DownLeftVectorBar;", u'\x2956' },
    { "DownRightTeeVector;", u'\x295f' },
    { "DownRightVector;", u'\x21c1' },
    { "DownRightVectorBar;", u'\x2957' },
    { "DownTee;", u'\x22a4' },
    { "DownTeeArrow;", u'\x21a7' },
    { "Downarrow;", u'\x21d3' },
    { "Dscr;", U'\x0001d49f' },
    { "Dstrok;", u'\x0110' },
    { "ENG;", u'\x014a' },
    { "ETH", u'\xd0' },
    { "ETH;", u'\xd0' },
    { "Eacute", u'\xc9' },
    { "Eacute;", u'\xc9' },
    { "Ecaron;", u'\x011a' },
    { "Ecirc", u'\xca' },
    { "Ecirc;", u'\xca' },
    { "Ecy;", u'\x042d' },
    { "Edot;", u'\x0116' },
    { "Efr;", U'\x0001d508' },
    { "Egrave", u'\xc8' },
    { "Egrave;", u'\xc8' },
    { "Element;", u'\x2208' },
    { "Emacr;", u'\x0112' },
    { "EmptySmallSquare;", u'\x25fb' },
    { "EmptyVerySmallSquare;", u'\x25ab' },
    { "Eogon;", u'\x0118' },
    { "Eopf;", U'\x0001d53c' },
    { "Epsilon;", u'\x0395' },
    { "Equal;", u'\x2a75' },
    { "EqualTilde;", u'\x2242' },
    { "Equilibrium;", u'\x21cc' },
    { "Escr;", u'\x2130' },
    { "Esim;", u'\x2a73' },
    { "Eta;", u'\x0397' },
    { "Euml", u'\xcb' },
    { "Euml;", u'\xcb' },
    { "Exists;", u'\x2203' },
    { "ExponentialE;", u'\x2147' },
    { "Fcy;", u'\x0424' },
    { "Ffr;", U'\x0001d509' },
    { "FilledSmallSquare;", u'\x25fc' },
    { "FilledVerySmallSquare;", u'\x25aa' },
    { "Fopf;", U'\x0001d53d' },
    { "ForAll;", u'\x2200' },
    { "Fouriertrf;", u'\x2131' },
    { "Fscr;", u'\x2131' },
    { "GJcy;", u'\x0403' },
    { "GT", u'>' },
    { "GT;", u'>' },
    { "Gamma;", u'\x0393' },
    { "Gammad;", u'\x03dc' },
    { "Gbreve;", u'\x011e' },
    { "Gcedil;", u'\x0122' },
    { "Gcirc;", u'\x011c' },
    { "Gcy;", u'\x0413' },
    { "Gdot;", u'\x0120' },
    { "Gfr;", U'\x0001d50a' },
    { "Gg;", u'\x22d9' },
    { "Gopf;", U'\x0001d53e' },
    { "GreaterEqual;", u'\x2265' },
    { "GreaterEqualLess;", u'\x22db' },
    { "GreaterFullEqual;", u'\x2267' },
    { "GreaterGreater;", u'\x2aa2' },
    { "GreaterLess;", u'\x2277' },
    { "GreaterSlantEqual;", u'\x2a7e' },
    { "GreaterTilde;", u'\x2273' },
    { "Gscr;", U'\x0001d4a2' },
    { "Gt;", u'\x226b' },
    { "HARDcy;", u'\x042a' },
    { "Hacek;", u'\x02c7' },
    { "Hat;", u'^' },
    { "Hcirc;", u'\x0124' },
    { "Hfr;", u'\x210c' },
    { "HilbertSpace;", u'\x210b' },
    { "Hopf;", u'\x210d' },
    { "HorizontalLine;", u'\x2500' },
    { "Hscr;", u'\x210b' },
    { "Hstrok;", u'\x0126' },
    { "HumpDownHump;", u'\x224e' },
    { "HumpEqual;", u'\x224f' },
    { "IEcy;", u'\x0415' },
    { "IJlig;", u'\x0132' },
    { "IOcy;", u'\x0401' },
    { "Iacute", u'\xcd' },
    { "Iacute;", u'\xcd' },
    { "Icirc", u'\xce' },
    { "Icirc;", u'\xce' },
    { "Icy;", u'\x0418' },
    { "Idot;", u'\x0130' },
    { "Ifr;", u'\x2111' },
    { "Igrave", u'\xcc' },
    { "Igrave;", u'\xcc' },
    { "Im;", u'\x2111' },
    { "Imacr;", u'\x012a' },
    { "ImaginaryI;", u'\x2148' },
    { "Implies;", u'\x21d2' },
    { "Int;", u'\x222c' },
    { "Integral;", u'\x222b' },
    { "Intersection;", u'\x22c2' },
    { "InvisibleComma;", u'\x2063' },
    { "InvisibleTimes;", u'\x2062' },
    { "Iogon;", u'\x012e' },
    { "Iopf;", U'\x0001d540' },
    { "Iota;", u'\x0399' },
    { "Iscr;", u'\x2110' },
    { "Itilde;", u'\x0128' },
    { "Iukcy;", u'\x0406' },
    { "Iuml", u'\xcf' },
    { "Iuml;", u'\xcf' },
    { "Jcirc;", u'\x0134' },
    { "Jcy;", u'\x0419' },
    { "Jfr;", U'\x0001d50d' },
    { "Jopf;", U'\x0001d541' },
    { "Jscr;", U'\x0001d4a5' },
    { "Jsercy;", u'\x0408' },
    { "Jukcy;", u'\x0404' },
    { "KHcy;", u'\x0425' },
    { "KJcy;", u'\x040c' },
    { "Kappa;", u'\x039a' },
    { "Kcedil;", u'\x0136' },
    { "Kcy;", u'\x041a' },
    { "Kfr;", U'\x0001d50e' },
    { "Kopf;", U'\x0001d542' },
    { "Kscr;", U'\x0001d4a6' },
    { "LJcy;", u'\x0409' },
    { "LT", u'<' },
    { "LT;", u'<' },
    { "Lacute;", u'\x0139' },
    { "Lambda;", u'\x039b' },
    { "Lang;", u'\x27ea' },
    { "Laplacetrf;", u'\x2112' },
    { "Larr;", u'\x219e' },
    { "Lcaron;", u'\x013d' },
    { "Lcedil;", u'\x013b' },
    { "Lcy;", u'\x041b' },
    { "LeftAngleBracket;", u'\x27e8' },
    { "LeftArrow;", u'\x2190' },
    { "LeftArrowBar;", u'\x21e4' },
    { "LeftArrowRightArrow;", u'\x21c6' },
    { "LeftCeiling;", u'\x2308' },
    { "LeftDoubleBracket;", u'\x27e6' },
    { "LeftDownTeeVector;", u'\x2961' },
    { "LeftDownVector;", u'\x21c3' },
    { "LeftDownVectorBar;", u'\x2959' },
    { "LeftFloor;", u'\x230a' },
    { "LeftRightArrow;", u'\x2194' },
    { "LeftRightVector;", u'\x294e' },
    { "LeftTee;", u'\x22a3' },
    { "LeftTeeArrow;", u'\x21a4' },
    { "LeftTeeVector;", u'\x295a' },
    { "LeftTriangle;", u'\x22b2' },
    { "LeftTriangleBar;", u'\x29cf' },
    { "LeftTriangleEqual;", u'\x22b4' },
    { "LeftUpDownVector;", u'\x2951' },
    { "LeftUpTeeVector;", u'\x2960' },
    { "LeftUpVector;", u'\x21bf' },
    { "LeftUpVectorBar;", u'\x2958' },
    { "LeftVector;", u'\x21bc' },
    { "LeftVectorBar;", u'\x2952' },
    { "Leftarrow;", u'\x21d0' },
    { "Leftrightarrow;", u'\x21d4' },
    { "LessEqualGreater;", u'\x22da' },
    { "LessFullEqual;", u'\x2266' },
    { "LessGreater;", u'\x2276' },
    { "LessLess;", u'\x2aa1' },
    { "LessSlantEqual;", u'\x2a7d' },
    { "LessTilde;", u'\x2272' },
    { "Lfr;", U'\x0001d50f' },
    { "Ll;", u'\x22d8' },
    { "Lleftarrow;", u'\x21da' },
    { "Lmidot;", u'\x013f' },
    { "LongLeftArrow;", u'\x27f5' },
    { "LongLeftRightArrow;", u'\x27f7' },
    { "LongRightArrow;", u'\x27f6' },
    { "Longleftarrow;", u'\x27f8' },
    { "Longleftrightarrow;", u'\x27fa' },
    { "Longrightarrow;", u'\x27f9' },
    { "Lopf;", U'\x0001d543' },
    { "LowerLeftArrow;", u'\x2199' },
    { "LowerRightArrow;", u'\x2198' },
    { "Lscr;", u'\x2112' },
    { "Lsh;", u'\x21b0' },
    { "Lstrok;", u'\x0141' },
    { "Lt;", u'\x226a' },
    { "Map;", u'\x2905' },
    { "Mcy;", u'\x041c' },
    { "MediumSpace;", u'\x205f' },
    { "Mellintrf;", u'\x2133' },
    { "Mfr;", U'\x0001d510' },
    { "MinusPlus;", u'\x2213' },
    { "Mopf;", U'\x0001d544' },
    { "Mscr;", u'\x2133' },
    { "Mu;", u'\x039c' },
    { "NJcy;", u'\x040a' },
    { "Nacute;", u'\x0143' },
    { "Ncaron;", u'\x0147' },
    { "Ncedil;", u'\x0145' },
    { "Ncy;", u'\x041d' },
    { "NegativeMediumSpace;", u'\x200b' },
    { "NegativeThickSpace;", u'\x200b' },
    { "NegativeThinSpace;", u'\x200b' },
    { "NegativeVeryThinSpace;", u'\x200b' },
    { "NestedGreaterGreater;", u'\x226b' },
    { "NestedLessLess;", u'\x226a' },
    { "NewLine;", u'\n' },
    { "Nfr;", U'\x0001d511' },
    { "NoBreak;", u'\x2060' },
    { "NonBreakingSpace;", u'\xa0' },
    { "Nopf;", u'\x2115' },
    { "Not;", u'\x2aec' },
    { "NotCongruent;", u'\x2262' },
    { "NotCupCap;", u'\x226d' },
    { "NotDoubleVerticalBar;", u'\x2226' },
    { "NotElement;", u'\x2209' },
    { "NotEqual;", u'\x2260' },
    { "NotExists;", u'\x2204' },
    { "NotGreater;", u'\x226f' },
    { "NotGreaterEqual;", u'\x2271' },
    { "NotGreaterLess;", u'\x2279' },
    { "NotGreaterTilde;", u'\x2275' },
    { "NotLeftTriangle;", u'\x22ea' },
    { "NotLeftTriangleEqual;", u'\x22ec' },
    { "NotLess;", u'\x226e' },
    { "NotLessEqual;", u'\x2270' },
    { "NotLessGreater;", u'\x2278' },
    { "NotLessTilde;", u'\x2274' },
    { "NotPrecedes;", u'\x2280' },
    { "NotPrecedesSlantEqual;", u'\x22e0' },
    { "NotReverseElement;", u'\x220c' },
    { "NotRightTriangle;", u'\x22eb' },
    { "NotRightTriangleEqual;", u'\x22ed' },
    { "NotSquareSubsetEqual;", u'\x22e2' },
    { "NotSquareSupersetEqual;", u'\x22e3' },
    { "NotSubsetEqual;", u'\x2288' },
    { "NotSucceeds;", u'\x2281' },
    { "NotSucceedsSlantEqual;", u'\x22e1' },
    { "NotSupersetEqual;", u'\x2289' },
    { "NotTilde;", u'\x2241' },
    { "NotTildeEqual;", u'\x2244' },
    { "NotTildeFullEqual;", u'\x2247' },
    { "NotTildeTilde;", u'\x2249' },
    { "NotVerticalBar;", u'\x2224' },
    { "Nscr;", U'\x0001d4a9' },
    { "Ntilde", u'\xd1' },
    { "Ntilde;", u'\xd1' },
    { "Nu;", u'\x039d' },
    { "OElig;", u'\x0152' },
    { "Oacute", u'\xd3' },
    { "Oacute;", u'\xd3' },
    { "Ocirc", u'\xd4' },
    { "Ocirc;", u'\xd4' },
    { "Ocy;", u'\x041e' },
    { "Odblac;", u'\x0150' },
    { "Ofr;", U'\x0001d512' },
    { "Ograve", u'\xd2' },
    { "Ograve;", u'\xd2' },
    { "Omacr;", u'\x014c' },
    { "Omega;", u'\x03a9' },
    { "Omicron;", u'\x039f' },
    { "Oopf;", U'\x0001d546' },
    { "OpenCurlyDoubleQuote;", u'\x201c' },
    { "OpenCurlyQuote;", u'\x2018' },
    { "Or;", u'\x2a54' },
    { "Oscr;", U'\x0001d4aa' },
    { "Oslash", u'\xd8' },
    { "Oslash;", u'\xd8' },
    { "Otilde", u'\xd5' },
    { "Otilde;", u'\xd5' },
    { "Otimes;", u'\x2a37' },
    { "Ouml", u'\xd6' },
    { "Ouml;", u'\xd6' },
    { "OverBar;", u'\x203e' },
    { "OverBrace;", u'\x23de' },
    { "OverBracket;", u'\x23b4' },
    { "OverParenthesis;", u'\x23dc' },
    { "PartialD;", u'\x2202' },
    { "Pcy;", u'\x041f' },
    { "Pfr;", U'\x0001d513' },
    { "Phi;", u'\x03a6' },
    { "Pi;", u'\x03a0' },
    { "PlusMinus;", u'\xb1' },
    { "Poincareplane;", u'\x210c' },
    { "Popf;", u'\x2119' },
    { "Pr;", u'\x2abb' },
    { "Precedes;", u'\x227a' },
    { "PrecedesEqual;", u'\x2aaf' },
    { "PrecedesSlantEqual;", u'\x227c' },
    { "PrecedesTilde;", u'\x227e' },
    { "Prime;", u'\x2033' },
    { "Product;", u'\x220f' },
    { "Proportion;", u'\x2237' },
    { "Proportional;", u'\x221d' },
    { "Pscr;", U'\x0001d4ab' },
    { "Psi;", u'\x03a8' },
    { "QUOT", u'"' },
    { "QUOT;", u'"' },
    { "Qfr;", U'\x0001d514' },
    { "Qopf;", u'\x211a' },
    { "Qscr;", U'\x0001d4ac' },
    { "RBarr;", u'\x2910' },
    { "REG", u'\xae' },
    { "REG;", u'\xae' },
    { "Racute;", u'\x0154' },
    { "Rang;", u'\x27eb' },
    { "Rarr;", u'\x21a0' },
    { "Rarrtl;", u'\x2916' },
    { "Rcaron;", u'\x0158' },
    { "Rcedil;", u'\x0156' },
    { "Rcy;", u'\x0420' },
    { "Re;", u'\x211c' },
    { "ReverseElement;", u'\x220b' },
    { "ReverseEquilibrium;", u'\x21cb' },
    { "ReverseUpEquilibrium;", u'\x296f' },
    { "Rfr;", u'\x211c' },
    { "Rho;", u'\x03a1' },
    { "RightAngleBracket;", u'\x27e9' },
    { "RightArrow;", u'\x2192' },
    { "RightArrowBar;", u'\x21e5' },
    { "RightArrowLeftArrow;", u'\x21c4' },
    { "RightCeiling;", u'\x2309' },
    { "RightDoubleBracket;", u'\x27e7' },
    { "RightDownTeeVector;", u'\x295d' },
    { "RightDownVector;", u'\x21c2' },
    { "RightDownVectorBar;", u'\x2955' },
    { "RightFloor;", u'\x230b' },
    { "RightTee;", u'\x22a2' },
    { "RightTeeArrow;", u'\x21a6' },
    { "RightTeeVector;", u'\x295b' },
    { "RightTriangle;", u'\x22b3' },
    { "RightTriangleBar;", u'\x29d0' },
    { "RightTriangleEqual;", u'\x22b5' },
    { "RightUpDownVector;", u'\x294f' },
    { "RightUpTeeVector;", u'\x295c' },
    { "RightUpVector;", u'\x21be' },
    { "RightUpVectorBar;", u'\x2954' },
    { "RightVector;", u'\x21c0' },
    { "RightVectorBar;", u'\x2953' },
    { "Rightarrow;", u'\x21d2' },
    { "Ropf;", u'\x211d' },
    { "RoundImplies;", u'\x2970' },
    { "Rrightarrow;", u'\x21db' },
    { "Rscr;", u'\x211b' },
    { "Rsh;", u'\x21b1' },
    { "RuleDelayed;", u'\x29f4' },
    { "SHCHcy;", u'\x0429' },
    { "SHcy;", u'\x0428' },
    { "SOFTcy;", u'\x042c' },
    { "Sacute;", u'\x015a' },
    { "Sc;", u'\x2abc' },
    { "Scaron;", u'\x0160' },
    { "Scedil;", u'\x015e' },
    { "Scirc;", u'\x015c' },
    { "Scy;", u'\x0421' },
    { "Sfr;", U'\x0001d516' },
    { "ShortDownArrow;", u'\x2193' },
    { "ShortLeftArrow;", u'\x2190' },
    { "ShortRightArrow;", u'\x2192' },
    { "ShortUpArrow;", u'\x2191' },
    { "Sigma;", u'\x03a3' },
    { "SmallCircle;", u'\x2218' },
    { "Sopf;", U'\x0001d54a' },
    { "Sqrt;", u'\x221a' },
    { "Square;", u'\x25a1' },
    { "SquareIntersection;", u'\x2293' },
    { "SquareSubset;", u'\x228f' },
    { "SquareSubsetEqual;", u'\x2291' },
    { "SquareSuperset;", u'\x2290' },
    { "SquareSupersetEqual;", u'\x2292' },
    { "SquareUnion;", u'\x2294' },
    { "Sscr;", U'\x0001d4ae' },
    { "Star;", u'\x22c6' },
    { "Sub;", u'\x22d0' },
    { "Subset;", u'\x22d0' },
    { "SubsetEqual;", u'\x2286' },
    { "Succeeds;", u'\x227b' },
    { "SucceedsEqual;", u'\x2ab0' },
    { "SucceedsSlantEqual;", u'\x227d' },
    { "SucceedsTilde;", u'\x227f' },
    { "SuchThat;", u'\x220b' },
    { "Sum;", u'\x2211' },
    { "Sup;", u'\x22d1' },
    { "Superset;", u'\x2283' },
    { "SupersetEqual;", u'\x2287' },
    { "Supset;", u'\x22d1' },
    { "THORN", u'\xde' },
    { "THORN;", u'\xde' },
    { "TRADE;", u'\x2122' },
    { "TSHcy;", u'\x040b' },
    { "TScy;", u'\x0426' },
    { "Tab;", u'\t' },
    { "Tau;", u'\x03a4' },
    { "Tcaron;", u'\x0164' },
    { "Tcedil;", u'\x0162' },
    { "Tcy;", u'\x0422' },
    { "Tfr;", U'\x0001d517' },
    { "Therefore;", u'\x2234' },
    { "Theta;", u'\x0398' },
    { "ThinSpace;", u'\x2009' },
    { "Tilde;", u'\x223c' },
    { "TildeEqual;", u'\x2243' },
    { "TildeFullEqual;", u'\x2245' },
    { "TildeTilde;", u'\x2248' },
    { "Topf;", U'\x0001d54b' },
    { "TripleDot;", u'\x20db' },
    { "Tscr;", U'\x0001d4af' },
    { "Tstrok;", u'\x0166' },
    { "Uacute", u'\xda' },
    { "Uacute;", u'\xda' },
    { "Uarr;", u'\x219f' },
    { "Uarrocir;", u'\x2949' },
    { "Ubrcy;", u'\x040e' },
    { "Ubreve;", u'\x016c' },
    { "Ucirc", u'\xdb' },
    { "Ucirc;", u'\xdb' },
    { "Ucy;", u'\x0423' },
    { "Udblac;", u'\x0170' },
    { "Ufr;", U'\x0001d518' },
    { "Ugrave", u'\xd9' },
    { "Ugrave;", u'\xd9' },
    { "Umacr;", u'\x016a' },
    { "UnderBar;", u'_' },
    { "UnderBrace;", u'\x23df' },
    { "UnderBracket;", u'\x23b5' },
    { "UnderParenthesis;", u'\x23dd' },
    { "Union;", u'\x22c3' },
    { "UnionPlus;", u'\x228e' },
    { "Uogon;", u'\x0172' },
    { "Uopf;", U'\x0001d54c' },
    { "UpArrow;", u'\x2191' },
    { "UpArrowBar;", u'\x2912' },
    { "UpArrowDownArrow;", u'\x21c5' },
    { "UpDownArrow;", u'\x2195' },
    { "UpEquilibrium;", u'\x296e' },
    { "UpTee;", u'\x22a5' },
    { "UpTeeArrow;", u'\x21a5' },
    { "Uparrow;", u'\x21d1' },
    { "Updownarrow;", u'\x21d5' },
    { "UpperLeftArrow;", u'\x2196' },
    { "UpperRightArrow;", u'\x2197' },
    { "Upsi;", u'\x03d2' },
    { "Upsilon;", u'\x03a5' },
    { "Uring;", u'\x016e' },
    { "Uscr;", U'\x0001d4b0' },
    { "Utilde;", u'\x0168' },
    { "Uuml", u'\xdc' },
    { "Uuml;", u'\xdc' },
    { "VDash;", u'\x22ab' },
    { "Vbar;", u'\x2aeb' },
    { "Vcy;", u'\x0412' },
    { "Vdash;", u'\x22a9' },
    { "Vdashl;", u'\x2ae6' },
    { "Vee;", u'\x22c1' },
    { "Verbar;", u'\x2016' },
    { "Vert;", u'\x2016' },
    { "VerticalBar;", u'\x2223' },
    { "VerticalLine;", u'|' },
    { "VerticalSeparator;", u'\x2758' },
    { "VerticalTilde;", u'\x2240' },
    { "VeryThinSpace;", u'\x200a' },
    { "Vfr;", U'\x0001d519' },
    { "Vopf;", U'\x0001d54d' },
    { "Vscr;", U'\x0001d4b1' },
    { "Vvdash;", u'\x22aa' },
    { "Wcirc;", u'\x0174' },
    { "Wedge;", u'\x22c0' },
    { "Wfr;", U'\x0001d51a' },
    { "Wopf;", U'\x0001d54e' },
    { "Wscr;", U'\x0001d4b2' },
    { "Xfr;", U'\x0001d51b' },
    { "Xi;", u'\x039e' },
    { "Xopf;", U'\x0001d54f' },
    { "Xscr;", U'\x0001d4b3' },
    { "YAcy;", u'\x042f' },
    { "YIcy;", u'\x0407' },
    { "YUcy;", u'\x042e' },
    { "Yacute", u'\xdd' },
    { "Yacute;", u'\xdd' },
    { "Ycirc;", u'\x0176' },
    { "Ycy;", u'\x042b' },
    { "Yfr;", U'\x0001d51c' },
    { "Yopf;", U'\x0001d550' },
    { "Yscr;", U'\x0001d4b4' },
    { "Yuml;", u'\x0178' },
    { "ZHcy;", u'\x0416' },
    { "Zacute;", u'\x0179' },
    { "Zcaron;", u'\x017d' },
    { "Zcy;", u'\x0417' },
    { "Zdot;", u'\x017b' },
    { "ZeroWidthSpace;", u'\x200b' },
    { "Zeta;", u'\x0396' },
    { "Zfr;", u'\x2128' },
    { "Zopf;", u'\x2124' },
    { "Zscr;", U'\x0001d4b5' },
    { "aacute", u'\xe1' },
    { "aacute;", u'\xe1' },
    { "abreve;", u'\x0103' },
    { "ac;", u'\x223e' },
    { "acd;", u'\x223f' },
    { "acirc", u'\xe2' },
    { "acirc;", u'\xe2' },
    { "acute", u'\xb4' },
    { "acute;", u'\xb4' },
    { "acy;", u'\x0430' },
    { "aelig", u'\xe6' },
    { "aelig;", u'\xe6' },
    { "af;", u'\x2061' },
    { "afr;", U'\x0001d51e' },
    { "agrave", u'\xe0' },
    { "agrave;", u'\xe0' },
    { "alefsym;", u'\x2135' },
    { "aleph;", u'\x2135' },
    { "alpha;", u'\x03b1' },
    { "amacr;", u'\x0101' },
    { "amalg;", u'\x2a3f' },
    { "amp", u'&' },
    { "amp;", u'&' },
    { "and;", u'\x2227' },
    { "andand;", u'\x2a55' },
    { "andd;", u'\x2a5c' },
    { "andslope;", u'\x2a58' },
    { "andv;", u'\x2a5a' },
    { "ang;", u'\x2220' },
    { "ange;", u'\x29a4' },
    { "angle;", u'\x2220' },
    { "angmsd;", u'\x2221' },
    { "angmsdaa;", u'\x29a8' },
    { "angmsdab;", u'\x29a9' },
    { "angmsdac;", u'\x29aa' },
    { "angmsdad;", u'\x29ab' },
    { "angmsdae;", u'\x29ac' },
    { "angmsdaf;", u'\x29ad' },
    { "angmsdag;", u'\x29ae' },
    { "angmsdah;", u'\x29af' },
    { "angrt;", u'\x221f' },
    { "angrtvb;", u'\x22be' },
    { "angrtvbd;", u'\x299d' },
    { "angsph;", u'\x2222' },
    { "angst;", u'\xc5' },
    { "angzarr;", u'\x237c' },
    { "aogon;", u'\x0105' },
    { "aopf;", U'\x0001d552' },
    { "ap;", u'\x2248' },
    { "apE;", u'\x2a70' },
    { "apacir;", u'\x2a6f' },
    { "ape;", u'\x224a' },
    { "apid;", u'\x224b' },
    { "apos;", u'\'' },
    { "approx;", u'\x2248' },
    { "approxeq;", u'\x224a' },
    { "aring", u'\xe5' },
    { "aring;", u'\xe5' },
    { "ascr;", U'\x0001d4b6' },
    { "ast;", u'*' },
    { "asymp;", u'\x2248' },
    { "asympeq;", u'\x224d' },
    { "atilde", u'\xe3' },
    { "atilde;", u'\xe3' },
    { "auml", u'\xe4' },
    { "auml;", u'\xe4' },
    { "awconint;", u'\x2233' },
    { "awint;", u'\x2a11' },
    { "bNot;", u'\x2aed' },
    { "backcong;", u'\x224c' },
    { "backepsilon;", u'\x03f6' },
    { "backprime;", u'\x2035' },
    { "backsim;", u'\x223d' },
    { "backsimeq;", u'\x22cd' },
    { "barvee;", u'\x22bd' },
    { "barwed;", u'\x2305' },
    { "barwedge;", u'\x2305' },
    { "bbrk;", u'\x23b5' },
    { "bbrktbrk;", u'\x23b6' },
    { "bcong;", u'\x224c' },
    { "bcy;", u'\x0431' },
    { "bdquo;", u'\x201e' },
    { "becaus;", u'\x2235' },
    { "because;", u'\x2235' },
    { "bemptyv;", u'\x29b0' },
    { "bepsi;", u'\x03f6' },
    { "bernou;", u'\x212c' },
    { "beta;", u'\x03b2' },
    { "beth;", u'\x2136' },
    { "between;", u'\x226c' },
    { "bfr;", U'\x0001d51f' },
    { "bigcap;", u'\x22c2' },
    { "bigcirc;", u'\x25ef' },
    { "bigcup;", u'\x22c3' },
    { "bigodot;", u'\x2a00' },
    { "bigoplus;", u'\x2a01' },
    { "bigotimes;", u'\x2a02' },
    { "bigsqcup;", u'\x2a06' },
    { "bigstar;", u'\x2605' },
    { "bigtriangledown;", u'\x25bd' },
    { "bigtriangleup;", u'\x25b3' },
    { "biguplus;", u'\x2a04' },
    { "bigvee;", u'\x22c1' },
    { "bigwedge;", u'\x22c0' },
    { "bkarow;", u'\x290d' },
    { "blacklozenge;", u'\x29eb' },
    { "blacksquare;", u'\x25aa' },
    { "blacktriangle;", u'\x25b4' },
    { "blacktriangledown;", u'\x25be' },
    { "blacktriangleleft;", u'\x25c2' },
    { "blacktriangleright;", u'\x25b8' },
    { "blank;", u'\x2423' },
    { "blk12;", u'\x2592' },
    { "blk14;", u'\x2591' },
    { "blk34;", u'\x2593' },
    { "block;", u'\x2588' },
    { "bnot;", u'\x2310' },
    { "bopf;", U'\x0001d553' },
    { "bot;", u'\x22a5' },
    { "bottom;", u'\x22a5' },
    { "bowtie;", u'\x22c8' },
    { "boxDL;", u'\x2557' },
    { "boxDR;", u'\x2554' },
    { "boxDl;", u'\x2556' },
    { "boxDr;", u'\x2553' },
    { "boxH;", u'\x2550' },
    { "boxHD;", u'\x2566' },
    { "boxHU;", u'\x2569' },
    { "boxHd;", u'\x2564' },
    { "boxHu;", u'\x2567' },
    { "boxUL;", u'\x255d' },
    { "boxUR;", u'\x255a' },
    { "boxUl;", u'\x255c' },
    { "boxUr;", u'\x2559' },
    { "boxV;", u'\x2551' },
    { "boxVH;", u'\x256c' },
    { "boxVL;", u'\x2563' },
    { "boxVR;", u'\x2560' },
    { "boxVh;", u'\x256b' },
    { "boxVl;", u'\x2562' },
    { "boxVr;", u'\x255f' },
    { "boxbox;", u'\x29c9' },
    { "boxdL;", u'\x2555' },
    { "boxdR;", u'\x2552' },
    { "boxdl;", u'\x2510' },
    { "boxdr;", u'\x250c' },
    { "boxh;", u'\x2500' },
    { "boxhD;", u'\x2565' },
    { "boxhU;", u'\x2568' },
    { "boxhd;", u'\x252c' },
    { "boxhu;", u'\x2534' },
    { "boxminus;", u'\x229f' },
    { "boxplus;", u'\x229e' },
    { "boxtimes;", u'\x22a0' },
    { "boxuL;", u'\x255b' },
    { "boxuR;", u'\x2558' },
    { "boxul;", u'\x2518' },
    { "boxur;", u'\x2514' },
    { "boxv;", u'\x2502' },
    { "boxvH;", u'\x256a' },
    { "boxvL;", u'\x2561' },
    { "boxvR;", u'\x255e' },
    { "boxvh;", u'\x253c' },
    { "boxvl;", u'\x2524' },
    { "boxvr;", u'\x251c' },
    { "bprime;", u'\x2035' },
    { "breve;", u'\x02d8' },
    { "brvbar", u'\xa6' },
    { "brvbar;", u'\xa6' },
    { "bscr;", U'\x0001d4b7' },
    { "bsemi;", u'\x204f' },
    { "bsim;", u'\x223d' },
    { "bsime;", u'\x22cd' },
    { "bsol;", u'\\' },
    { "bsolb;", u'\x29c5' },
    { "bsolhsub;", u'\x27c8' },
    { "bull;", u'\x2022' },
    { "bullet;", u'\x2022' },
    { "bump;", u'\x224e' },
    { "bumpE;", u'\x2aae' },
    { "bumpe;", u'\x224f' },
    { "bumpeq;", u'\x224f' },
    { "cacute;", u'\x0107' },
    { "cap;", u'\x2229' },
    { "capand;", u'\x2a44' },
    { "capbrcup;", u'\x2a49' },
    { "capcap;", u'\x2a4b' },
    { "capcup;", u'\x2a47' },
    { "capdot;", u'\x2a40' },
    { "caret;", u'\x2041' },
    { "caron;", u'\x02c7' },
    { "ccaps;", u'\x2a4d' },
    { "ccaron;", u'\x010d' },
    { "ccedil", u'\xe7' },
    { "ccedil;", u'\xe7' },
    { "ccirc;", u'\x0109' },
    { "ccups;", u'\x2a4c' },
    { "ccupssm;", u'\x2a50' },
    { "cdot;", u'\x010b' },
    { "cedil", u'\xb8' },
    { "cedil;", u'\xb8' },
    { "cemptyv;", u'\x29b2' },
    { "cent", u'\xa2' },
    { "cent;", u'\xa2' },
    { "centerdot;", u'\xb7' },
    { "cfr;", U'\x0001d520' },
    { "chcy;", u'\x0447' },
    { "check;", u'\x2713' },
    { "checkmark;", u'\x2713' },
    { "chi;", u'\x03c7' },
    { "cir;", u'\x25cb' },
    { "cirE;", u'\x29c3' },
    { "circ;", u'\x02c6' },
    { "circeq;", u'\x2257' },
    { "circlearrowleft;", u'\x21ba' },
    { "circlearrowright;", u'\x21bb' },
    { "circledR;", u'\xae' },
    { "circledS;", u'\x24c8' },
    { "circledast;", u'\x229b' },
    { "circledcirc;", u'\x229a' },
    { "circleddash;", u'\x229d' },
    { "cire;", u'\x2257' },
    { "cirfnint;", u'\x2a10' },
    { "cirmid;", u'\x2aef' },
    { "cirscir;", u'\x29c2' },
    { "clubs;", u'\x2663' },
    { "clubsuit;", u'\x2663' },
    { "colon;", u':' },
    { "colone;", u'\x2254' },
    { "coloneq;", u'\x2254' },
    { "comma;", u',' },
    { "commat;", u'@' },
    { "comp;", u'\x2201' },
    { "compfn;", u'\x2218' },
    { "complement;", u'\x2201' },
    { "complexes;", u'\x2102' },
    { "cong;", u'\x2245' },
    { "congdot;", u'\x2a6d' },
    { "conint;", u'\x222e' },
    { "copf;", U'\x0001d554' },
    { "coprod;", u'\x2210' },
    { "copy", u'\xa9' },
    { "copy;", u'\xa9' },
    { "copysr;", u'\x2117' },
    { "crarr;", u'\x21b5' },
    { "cross;", u'\x2717' },
    { "cscr;", U'\x0001d4b8' },
    { "csub;", u'\x2acf' },
    { "csube;", u'\x2ad1' },
    { "csup;", u'\x2ad0' },
    { "csupe;", u'\x2ad2' },
    { "ctdot;", u'\x22ef' },
    { "cudarrl;", u'\x2938' },
    { "cudarrr;", u'\x2935' },
    { "cuepr;", u'\x22de' },
    { "cuesc;", u'\x22df' },
    { "cularr;", u'\x21b6' },
    { "cularrp;", u'\x293d' },
    { "cup;", u'\x222a' },
    { "cupbrcap;", u'\x2a48' },
    { "cupcap;", u'\x2a46' },
    { "cupcup;", u'\x2a4a' },
    { "cupdot;", u'\x228d' },
    { "cupor;", u'\x2a45' },
    { "curarr;", u'\x21b7' },
    { "curarrm;", u'\x293c' },
    { "curlyeqprec;", u'\x22de' },
    { "curlyeqsucc;", u'\x22df' },
    { "curlyvee;", u'\x22ce' },
    { "curlywedge;", u'\x22cf' },
    { "curren", u'\xa4' },
    { "curren;", u'\xa4' },
    { "curvearrowleft;", u'\x21b6' },
    { "curvearrowright;", u'\x21b7' },
    { "cuvee;", u'\x22ce' },
    { "cuwed;", u'\x22cf' },
    { "cwconint;", u'\x2232' },
    { "cwint;", u'\x2231' },
    { "cylcty;", u'\x232d' },
    { "dArr;", u'\x21d3' },
    { "dHar;", u'\x2965' },
    { "dagger;", u'\x2020' },
    { "daleth;", u'\x2138' },
    { "darr;", u'\x2193' },
    { "dash;", u'\x2010' },
    { "dashv;", u'\x22a3' },
    { "dbkarow;", u'\x290f' },
    { "dblac;", u'\x02dd' },
    { "dcaron;", u'\x010f' },
    { "dcy;", u'\x0434' },
    { "dd;", u'\x2146' },
    { "ddagger;", u'\x2021' },
    { "ddarr;", u'\x21ca' },
    { "ddotseq;", u'\x2a77' },
    { "deg", u'\xb0' },
    { "deg;", u'\xb0' },
    { "delta;", u'\x03b4' },
    { "demptyv;", u'\x29b1' },
    { "dfisht;", u'\x297f' },
    { "dfr;", U'\x0001d521' },
    { "dharl;", u'\x21c3' },
    { "dharr;", u'\x21c2' },
    { "diam;", u'\x22c4' },
    { "diamond;", u'\x22c4' },
    { "diamondsuit;", u'\x2666' },
    { "diams;", u'\x2666' },
    { "die;", u'\xa8' },
    { "digamma;", u'\x03dd' },
    { "disin;", u'\x22f2' },
    { "div;", u'\xf7' },
    { "divide", u'\xf7' },
    { "divide;", u'\xf7' },
    { "divideontimes;", u'\x22c7' },
    { "divonx;", u'\x22c7' },
    { "djcy;", u'\x0452' },
    { "dlcorn;", u'\x231e' },
    { "dlcrop;", u'\x230d' },
    { "dollar;", u'$' },
    { "dopf;", U'\x0001d555' },
    { "dot;", u'\x02d9' },
    { "doteq;", u'\x2250' },
    { "doteqdot;", u'\x2251' },
    { "dotminus;", u'\x2238' },
    { "dotplus;", u'\x2214' },
    { "dotsquare;", u'\x22a1' },
    { "doublebarwedge;", u'\x2306' },
    { "downarrow;", u'\x2193' },
    { "downdownarrows;", u'\x21ca' },
    { "downharpoonleft;", u'\x21c3' },
    { "downharpoonright;", u'\x21c2' },
    { "drbkarow;", u'\x2910' },
    { "drcorn;", u'\x231f' },
    { "drcrop;", u'\x230c' },
    { "dscr;", U'\x0001d4b9' },
    { "dscy;", u'\x0455' },
    { "dsol;", u'\x29f6' },
    { "dstrok;", u'\x0111' },
    { "dtdot;", u'\x22f1' },
    { "dtri;", u'\x25bf' },
    { "dtrif;", u'\x25be' },
    { "duarr;", u'\x21f5' },
    { "duhar;", u'\x296f' },
    { "dwangle;", u'\x29a6' },
    { "dzcy;", u'\x045f' },
    { "dzigrarr;", u'\x27ff' },
    { "eDDot;", u'\x2a77' },
    { "eDot;", u'\x2251' },
    { "eacute", u'\xe9' },
    { "eacute;", u'\xe9' },
    { "easter;", u'\x2a6e' },
    { "ecaron;", u'\x011b' },
    { "ecir;", u'\x2256' },
    { "ecirc", u'\xea' },
    { "ecirc;", u'\xea' },
    { "ecolon;", u'\x2255' },
    { "ecy;", u'\x044d' },
    { "edot;", u'\x0117' },
    { "ee;", u'\x2147' },
    { "efDot;", u'\x2252' },
    { "efr;", U'\x0001d522' },
    { "eg;", u'\x2a9a' },
    { "egrave", u'\xe8' },
    { "egrave;", u'\xe8' },
    { "egs;", u'\x2a96' },
    { "egsdot;", u'\x2a98' },
    { "el;", u'\x2a99' },
    { "elinters;", u'\x23e7' },
    { "ell;", u'\x2113' },
    { "els;", u'\x2a95' },
    { "elsdot;", u'\x2a97' },
    { "emacr;", u'\x0113' },
    { "empty;", u'\x2205' },
    { "emptyset;", u'\x2205' },
    { "emptyv;", u'\x2205' },
    { "emsp13;", u'\x2004' },
    { "emsp14;", u'\x2005' },
    { "emsp;", u'\x2003' },
    { "eng;", u'\x014b' },
    { "ensp;", u'\x2002' },
    { "eogon;", u'\x0119' },
    { "eopf;", U'\x0001d556' },
    { "epar;", u'\x22d5' },
    { "eparsl;", u'\x29e3' },
    { "eplus;", u'\x2a71' },
    { "epsi;", u'\x03b5' },
    { "epsilon;", u'\x03b5' },
    { "epsiv;", u'\x03f5' },
    { "eqcirc;", u'\x2256' },
    { "eqcolon;", u'\x2255' },
    { "eqsim;", u'\x2242' },
    { "eqslantgtr;", u'\x2a96' },
    { "eqslantless;", u'\x2a95' },
    { "equals;", u'=' },
    { "equest;", u'\x225f' },
    { "equiv;", u'\x2261' },
    { "equivDD;", u'\x2a78' },
    { "eqvparsl;", u'\x29e5' },
    { "erDot;", u'\x2253' },
    { "erarr;", u'\x2971' },
    { "escr;", u'\x212f' },
    { "esdot;", u'\x2250' },
    { "esim;", u'\x2242' },
    { "eta;", u'\x03b7' },
    { "eth", u'\xf0' },
    { "eth;", u'\xf0' },
    { "euml", u'\xeb' },
    { "euml;", u'\xeb' },
    { "euro;", u'\x20ac' },
    { "excl;", u'!' },
    { "exist;", u'\x2203' },
    { "expectation;", u'\x2130' },
    { "exponentiale;", u'\x2147' },
    { "fallingdotseq;", u'\x2252' },
    { "fcy;", u'\x0444' },
    { "female;", u'\x2640' },
    { "ffilig;", u'\xfb03' },
    { "fflig;", u'\xfb00' },
    { "ffllig;", u'\xfb04' },
    { "ffr;", U'\x0001d523' },
    { "filig;", u'\xfb01' },
    { "flat;", u'\x266d' },
    { "fllig;", u'\xfb02' },
    { "fltns;", u'\x25b1' },
    { "fnof;", u'\x0192' },
    { "fopf;", U'\x0001d557' },
    { "forall;", u'\x2200' },
    { "fork;", u'\x22d4' },
    { "forkv;", u'\x2ad9' },
    { "fpartint;", u'\x2a0d' },
    { "frac12", u'\xbd' },
    { "frac12;", u'\xbd' },
    { "frac13;", u'\x2153' },
    { "frac14", u'\xbc' },
    { "frac14;", u'\xbc' },
    { "frac15;", u'\x2155' },
    { "frac16;", u'\x2159' },
    { "frac18;", u'\x215b' },
    { "frac23;", u'\x2154' },
    { "frac25;", u'\x2156' },
    { "frac34", u'\xbe' },
    { "frac34;", u'\xbe' },
    { "frac35;", u'\x2157' },
    { "frac38;", u'\x215c' },
    { "frac45;", u'\x2158' },
    { "frac56;", u'\x215a' },
    { "frac58;", u'\x215d' },
    { "frac78;", u'\x215e' },
    { "frasl;", u'\x2044' },
    { "frown;", u'\x2322' },
    { "fscr;", U'\x0001d4bb' },
    { "gE;", u'\x2267' },
    { "gEl;", u'\x2a8c' },
    { "gacute;", u'\x01f5' },
    { "gamma;", u'\x03b3' },
    { "gammad;", u'\x03dd' },
    { "gap;", u'\x2a86' },
    { "gbreve;", u'\x011f' },
    { "gcirc;", u'\x011d' },
    { "gcy;", u'\x0433' },
    { "gdot;", u'\x0121' },
    { "ge;", u'\x2265' },
    { "gel;", u'\x22db' },
    { "geq;", u'\x2265' },
    { "geqq;", u'\x2267' },
    { "geqslant;", u'\x2a7e' },
    { "ges;", u'\x2a7e' },
    { "gescc;", u'\x2aa9' },
    { "gesdot;", u'\x2a80' },
    { "gesdoto;", u'\x2a82' },
    { "gesdotol;", u'\x2a84' },
    { "gesles;", u'\x2a94' },
    { "gfr;", U'\x0001d524' },
    { "gg;", u'\x226b' },
    { "ggg;", u'\x22d9' },
    { "gimel;", u'\x2137' },
    { "gjcy;", u'\x0453' },
    { "gl;", u'\x2277' },
    { "glE;", u'\x2a92' },
    { "gla;", u'\x2aa5' },
    { "glj;", u'\x2aa4' },
    { "gnE;", u'\x2269' },
    { "gnap;", u'\x2a8a' },
    { "gnapprox;", u'\x2a8a' },
    { "gne;", u'\x2a88' },
    { "gneq;", u'\x2a88' },
    { "gneqq;", u'\x2269' },
    { "gnsim;", u'\x22e7' },
    { "gopf;", U'\x0001d558' },
    { "grave;", u'`' },
    { "gscr;", u'\x210a' },
    { "gsim;", u'\x2273' },
    { "gsime;", u'\x2a8e' },
    { "gsiml;", u'\x2a90' },
    { "gt", u'>' },
    { "gt;", u'>' },
    { "gtcc;", u'\x2aa7' },
    { "gtcir;", u'\x2a7a' },
    { "gtdot;", u'\x22d7' },
    { "gtlPar;", u'\x2995' },
    { "gtquest;", u'\x2a7c' },
    { "gtrapprox;", u'\x2a86' },
    { "gtrarr;", u'\x2978' },
    { "gtrdot;", u'\x22d7' },
    { "gtreqless;", u'\x22db' },
    { "gtreqqless;", u'\x2a8c' },
    { "gtrless;", u'\x2277' },
    { "gtrsim;", u'\x2273' },
    { "hArr;", u'\x21d4' },
    { "hairsp;", u'\x200a' },
    { "half;", u'\xbd' },
    { "hamilt;", u'\x210b' },
    { "hardcy;", u'\x044a' },
    { "harr;", u'\x2194' },
    { "harrcir;", u'\x2948' },
    { "harrw;", u'\x21ad' },
    { "hbar;", u'\x210f' },
    { "hcirc;", u'\x0125' },
    { "hearts;", u'\x2665' },
    { "heartsuit;", u'\x2665' },
    { "hellip;", u'\x2026' },
    { "hercon;", u'\x22b9' },
    { "hfr;", U'\x0001d525' },
    { "hksearow;", u'\x2925' },
    { "hkswarow;", u'\x2926' },
    { "hoarr;", u'\x21ff' },
    { "homtht;", u'\x223b' },
    { "hookleftarrow;", u'\x21a9' },
    { "hookrightarrow;", u'\x21aa' },
    { "hopf;", U'\x0001d559' },
    { "horbar;", u'\x2015' },
    { "hscr;", U'\x0001d4bd' },
    { "hslash;", u'\x210f' },
    { "hstrok;", u'\x0127' },
    { "hybull;", u'\x2043' },
    { "hyphen;", u'\x2010' },
    { "iacute", u'\xed' },
    { "iacute;", u'\xed' },
    { "ic;", u'\x2063' },
    { "icirc", u'\xee' },
    { "icirc;", u'\xee' },
    { "icy;", u'\x0438' },
    { "iecy;", u'\x0435' },
    { "iexcl", u'\xa1' },
    { "iexcl;", u'\xa1' },
    { "iff;", u'\x21d4' },
    { "ifr;", U'\x0001d526' },
    { "igrave", u'\xec' },
    { "igrave;", u'\xec' },
    { "ii;", u'\x2148' },
    { "iiiint;", u'\x2a0c' },
    { "iiint;", u'\x222d' },
    { "iinfin;", u'\x29dc' },
    { "iiota;", u'\x2129' },
    { "ijlig;", u'\x0133' },
    { "imacr;", u'\x012b' },
    { "image;", u'\x2111' },
    { "imagline;", u'\x2110' },
    { "imagpart;", u'\x2111' },
    { "imath;", u'\x0131' },
    { "imof;", u'\x22b7' },
    { "imped;", u'\x01b5' },
    { "in;", u'\x2208' },
    { "incare;", u'\x2105' },
    { "infin;", u'\x221e' },
    { "infintie;", u'\x29dd' },
    { "inodot;", u'\x0131' },
    { "int;", u'\x222b' },
    { "intcal;", u'\x22ba' },
    { "integers;", u'\x2124' },
    { "intercal;", u'\x22ba' },
    { "intlarhk;", u'\x2a17' },
    { "intprod;", u'\x2a3c' },
    { "iocy;", u'\x0451' },
    { "iogon;", u'\x012f' },
    { "iopf;", U'\x0001d55a' },
    { "iota;", u'\x03b9' },
    { "iprod;", u'\x2a3c' },
    { "iquest", u'\xbf' },
    { "iquest;", u'\xbf' },
    { "iscr;", U'\x0001d4be' },
    { "isin;", u'\x2208' },
    { "isinE;", u'\x22f9' },
    { "isindot;", u'\x22f5' },
    { "isins;", u'\x22f4' },
    { "isinsv;", u'\x22f3' },
    { "isinv;", u'\x2208' },
    { "it;", u'\x2062' },
    { "itilde;", u'\x0129' },
    { "iukcy;", u'\x0456' },
    { "iuml", u'\xef' },
    { "iuml;", u'\xef' },
    { "jcirc;", u'\x0135' },
    { "jcy;", u'\x0439' },
    { "jfr;", U'\x0001d527' },
    { "jmath;", u'\x0237' },
    { "jopf;", U'\x0001d55b' },
    { "jscr;", U'\x0001d4bf' },
    { "jsercy;", u'\x0458' },
    { "jukcy;", u'\x0454' },
    { "kappa;", u'\x03ba' },
    { "kappav;", u'\x03f0' },
    { "kcedil;", u'\x0137' },
    { "kcy;", u'\x043a' },
    { "kfr;", U'\x0001d528' },
    { "kgreen;", u'\x0138' },
    { "khcy;", u'\x0445' },
    { "kjcy;", u'\x045c' },
    { "kopf;", U'\x0001d55c' },
    { "kscr;", U'\x0001d4c0' },
    { "lAarr;", u'\x21da' },
    { "lArr;", u'\x21d0' },
    { "lAtail;", u'\x291b' },
    { "lBarr;", u'\x290e' },
    { "lE;", u'\x2266' },
    { "lEg;", u'\x2a8b' },
    { "lHar;", u'\x2962' },
    { "lacute;", u'\x013a' },
    { "laemptyv;", u'\x29b4' },
    { "lagran;", u'\x2112' },
    { "lambda;", u'\x03bb' },
    { "lang;", u'\x27e8' },
    { "langd;", u'\x2991' },
    { "langle;", u'\x27e8' },
    { "lap;", u'\x2a85' },
    { "laquo", u'\xab' },
    { "laquo;", u'\xab' },
    { "larr;", u'\x2190' },
    { "larrb;", u'\x21e4' },
    { "larrbfs;", u'\x291f' },
    { "larrfs;", u'\x291d' },
    { "larrhk;", u'\x21a9' },
    { "larrlp;", u'\x21ab' },
    { "larrpl;", u'\x2939' },
    { "larrsim;", u'\x2973' },
    { "larrtl;", u'\x21a2' },
    { "lat;", u'\x2aab' },
    { "latail;", u'\x2919' },
    { "late;", u'\x2aad' },
    { "lbarr;", u'\x290c' },
    { "lbbrk;", u'\x2772' },
    { "lbrace;", u'{' },
    { "lbrack;", u'[' },
    { "lbrke;", u'\x298b' },
    { "lbrksld;", u'\x298f' },
    { "lbrkslu;", u'\x298d' },
    { "lcaron;", u'\x013e' },
    { "lcedil;", u'\x013c' },
    { "lceil;", u'\x2308' },
    { "lcub;", u'{' },
    { "lcy;", u'\x043b' },
    { "ldca;", u'\x2936' },
    { "ldquo;", u'\x201c' },
    { "ldquor;", u'\x201e' },
    { "ldrdhar;", u'\x2967' },
    { "ldrushar;", u'\x294b' },
    { "ldsh;", u'\x21b2' },
    { "le;", u'\x2264' },
    { "leftarrow;", u'\x2190' },
    { "leftarrowtail;", u'\x21a2' },
    { "leftharpoondown;", u'\x21bd' },
    { "leftharpoonup;", u'\x21bc' },
    { "leftleftarrows;", u'\x21c7' },
    { "leftrightarrow;", u'\x2194' },
    { "leftrightarrows;", u'\x21c6' },
    { "leftrightharpoons;", u'\x21cb' },
    { "leftrightsquigarrow;", u'\x21ad' },
    { "leftthreetimes;", u'\x22cb' },
    { "leg;", u'\x22da' },
    { "leq;", u'\x2264' },
    { "leqq;", u'\x2266' },
    { "leqslant;", u'\x2a7d' },
    { "les;", u'\x2a7d' },
    { "lescc;", u'\x2aa8' },
    { "lesdot;", u'\x2a7f' },
    { "lesdoto;", u'\x2a81' },
    { "lesdotor;", u'\x2a83' },
    { "lesges;", u'\x2a93' },
    { "lessapprox;", u'\x2a85' },
    { "lessdot;", u'\x22d6' },
    { "lesseqgtr;", u'\x22da' },
    { "lesseqqgtr;", u'\x2a8b' },
    { "lessgtr;", u'\x2276' },
    { "lesssim;", u'\x2272' },
    { "lfisht;", u'\x297c' },
    { "lfloor;", u'\x230a' },
    { "lfr;", U'\x0001d529' },
    { "lg;", u'\x2276' },
    { "lgE;", u'\x2a91' },
    { "lhard;", u'\x21bd' },
    { "lharu;", u'\x21bc' },
    { "lharul;", u'\x296a' },
    { "lhblk;", u'\x2584' },
    { "ljcy;", u'\x0459' },
    { "ll;", u'\x226a' },
    { "llarr;", u'\x21c7' },
    { "llcorner;", u'\x231e' },
    { "llhard;", u'\x296b' },
    { "lltri;", u'\x25fa' },
    { "lmidot;", u'\x0140' },
    { "lmoust;", u'\x23b0' },
    { "lmoustache;", u'\x23b0' },
    { "lnE;", u'\x2268' },
    { "lnap;", u'\x2a89' },
    { "lnapprox;", u'\x2a89' },
    { "lne;", u'\x2a87' },
    { "lneq;", u'\x2a87' },
    { "lneqq;", u'\x2268' },
    { "lnsim;", u'\x22e6' },
    { "loang;", u'\x27ec' },
    { "loarr;", u'\x21fd' },
    { "lobrk;", u'\x27e6' },
    { "longleftarrow;", u'\x27f5' },
    { "longleftrightarrow;", u'\x27f7' },
    { "longmapsto;", u'\x27fc' },
    { "longrightarrow;", u'\x27f6' },
    { "looparrowleft;", u'\x21ab' },
    { "looparrowright;", u'\x21ac' },
    { "lopar;", u'\x2985' },
    { "lopf;", U'\x0001d55d' },
    { "loplus;", u'\x2a2d' },
    { "lotimes;", u'\x2a34' },
    { "lowast;", u'\x2217' },
    { "lowbar;", u'_' },
    { "loz;", u'\x25ca' },
    { "lozenge;", u'\x25ca' },
    { "lozf;", u'\x29eb' },
    { "lpar;", u'(' },
    { "lparlt;", u'\x2993' },
    { "lrarr;", u'\x21c6' },
    { "lrcorner;", u'\x231f' },
    { "lrhar;", u'\x21cb' },
    { "lrhard;", u'\x296d' },
    { "lrm;", u'\x200e' },
    { "lrtri;", u'\x22bf' },
    { "lsaquo;", u'\x2039' },
    { "lscr;", U'\x0001d4c1' },
    { "lsh;", u'\x21b0' },
    { "lsim;", u'\x2272' },
    { "lsime;", u'\x2a8d' },
    { "lsimg;", u'\x2a8f' },
    { "lsqb;", u'[' },
    { "lsquo;", u'\x2018' },
    { "lsquor;", u'\x201a' },
    { "lstrok;", u'\x0142' },
    { "lt", u'<' },
    { "lt;", u'<' },
    { "ltcc;", u'\x2aa6' },
    { "ltcir;", u'\x2a79' },
    { "ltdot;", u'\x22d6' },
    { "lthree;", u'\x22cb' },
    { "ltimes;", u'\x22c9' },
    { "ltlarr;", u'\x2976' },
    { "ltquest;", u'\x2a7b' },
    { "ltrPar;", u'\x2996' },
    { "ltri;", u'\x25c3' },
    { "ltrie;", u'\x22b4' },
    { "ltrif;", u'\x25c2' },
    { "lurdshar;", u'\x294a' },
    { "luruhar;", u'\x2966' },
    { "mDDot;", u'\x223a' },
    { "macr", u'\xaf' },
    { "macr;", u'\xaf' },
    { "male;", u'\x2642' },
    { "malt;", u'\x2720' },
    { "maltese;", u'\x2720' },
    { "map;", u'\x21a6' },
    { "mapsto;", u'\x21a6' },
    { "mapstodown;", u'\x21a7' },
    { "mapstoleft;", u'\x21a4' },
    { "mapstoup;", u'\x21a5' },
    { "marker;", u'\x25ae' },
    { "mcomma;", u'\x2a29' },
    { "mcy;", u'\x043c' },
    { "mdash;", u'\x2014' },
    { "measuredangle;", u'\x2221' },
    { "mfr;", U'\x0001d52a' },
    { "mho;", u'\x2127' },
    { "micro", u'\xb5' },
    { "micro;", u'\xb5' },
    { "mid;", u'\x2223' },
    { "midast;", u'*' },
    { "midcir;", u'\x2af0' },
    { "middot", u'\xb7' },
    { "middot;", u'\xb7' },
    { "minus;", u'\x2212' },
    { "minusb;", u'\x229f' },
    { "minusd;", u'\x2238' },
    { "minusdu;", u'\x2a2a' },
    { "mlcp;", u'\x2adb' },
    { "mldr;", u'\x2026' },
    { "mnplus;", u'\x2213' },
    { "models;", u'\x22a7' },
    { "mopf;", U'\x0001d55e' },
    { "mp;", u'\x2213' },
    { "mscr;", U'\x0001d4c2' },
    { "mstpos;", u'\x223e' },
    { "mu;", u'\x03bc' },
    { "multimap;", u'\x22b8' },
    { "mumap;", u'\x22b8' },
    { "nLeftarrow;", u'\x21cd' },
    { "nLeftrightarrow;", u'\x21ce' },
    { "nRightarrow;", u'\x21cf' },
    { "nVDash;", u'\x22af' },
    { "nVdash;", u'\x22ae' },
    { "nabla;", u'\x2207' },
    { "nacute;", u'\x0144' },
    { "nap;", u'\x2249' },
    { "napos;", u'\x0149' },
    { "napprox;", u'\x2249' },
    { "natur;", u'\x266e' },
    { "natural;", u'\x266e' },
    { "naturals;", u'\x2115' },
    { "nbsp", u'\xa0' },
    { "nbsp;", u'\xa0' },
    { "ncap;", u'\x2a43' },
    { "ncaron;", u'\x0148' },
    { "ncedil;", u'\x0146' },
    { "ncong;", u'\x2247' },
    { "ncup;", u'\x2a42' },
    { "ncy;", u'\x043d' },
    { "ndash;", u'\x2013' },
    { "ne;", u'\x2260' },
    { "neArr;", u'\x21d7' },
    { "nearhk;", u'\x2924' },
    { "nearr;", u'\x2197' },
    { "nearrow;", u'\x2197' },
    { "nequiv;", u'\x2262' },
    { "nesear;", u'\x2928' },
    { "nexist;", u'\x2204' },
    { "nexists;", u'\x2204' },
    { "nfr;", U'\x0001d52b' },
    { "nge;", u'\x2271' },
    { "ngeq;", u'\x2271' },
    { "ngsim;", u'\x2275' },
    { "ngt;", u'\x226f' },
    { "ngtr;", u'\x226f' },
    { "nhArr;", u'\x21ce' },
    { "nharr;", u'\x21ae' },
    { "nhpar;", u'\x2af2' },
    { "ni;", u'\x220b' },
    { "nis;", u'\x22fc' },
    { "nisd;", u'\x22fa' },
    { "niv;", u'\x220b' },
    { "njcy;", u'\x045a' },
    { "nlArr;", u'\x21cd' },
    { "nlarr;", u'\x219a' },
    { "nldr;", u'\x2025' },
    { "nle;", u'\x2270' },
    { "nleftarrow;", u'\x219a' },
    { "nleftrightarrow;", u'\x21ae' },
    { "nleq;", u'\x2270' },
    { "nless;", u'\x226e' },
    { "nlsim;", u'\x2274' },
    { "nlt;", u'\x226e' },
    { "nltri;", u'\x22ea' },
    { "nltrie;", u'\x22ec' },
    { "nmid;", u'\x2224' },
    { "nopf;", U'\x0001d55f' },
    { "not", u'\xac' },
    { "not;", u'\xac' },
    { "notin;", u'\x2209' },
    { "notinva;", u'\x2209' },
    { "notinvb;", u'\x22f7' },
    { "notinvc;", u'\x22f6' },
    { "notni;", u'\x220c' },
    { "notniva;", u'\x220c' },
    { "notnivb;", u'\x22fe' },
    { "notnivc;", u'\x22fd' },
    { "npar;", u'\x2226' },
    { "nparallel;", u'\x2226' },
    { "npolint;", u'\x2a14' },
    { "npr;", u'\x2280' },
    { "nprcue;", u'\x22e0' },
    { "nprec;", u'\x2280' },
    { "nrArr;", u'\x21cf' },
    { "nrarr;", u'\x219b' },
    { "nrightarrow;", u'\x219b' },
    { "nrtri;", u'\x22eb' },
    { "nrtrie;", u'\x22ed' },
    { "nsc;", u'\x2281' },
    { "nsccue;", u'\x22e1' },
    { "nscr;", U'\x0001d4c3' },
    { "nshortmid;", u'\x2224' },
    { "nshortparallel;", u'\x2226' },
    { "nsim;", u'\x2241' },
    { "nsime;", u'\x2244' },
    { "nsimeq;", u'\x2244' },
    { "nsmid;", u'\x2224' },
    { "nspar;", u'\x2226' },
    { "nsqsube;", u'\x22e2' },
    { "nsqsupe;", u'\x22e3' },
    { "nsub;", u'\x2284' },
    { "nsube;", u'\x2288' },
    { "nsubseteq;", u'\x2288' },
    { "nsucc;", u'\x2281' },
    { "nsup;", u'\x2285' },
    { "nsupe;", u'\x2289' },
    { "nsupseteq;", u'\x2289' },
    { "ntgl;", u'\x2279' },
    { "ntilde", u'\xf1' },
    { "ntilde;", u'\xf1' },
    { "ntlg;", u'\x2278' },
    { "ntriangleleft;", u'\x22ea' },
    { "ntrianglelefteq;", u'\x22ec' },
    { "ntriangleright;", u'\x22eb' },
    { "ntrianglerighteq;", u'\x22ed' },
    { "nu;", u'\x03bd' },
    { "num;", u'#' },
    { "numero;", u'\x2116' },
    { "numsp;", u'\x2007' },
    { "nvDash;", u'\x22ad' },
    { "nvHarr;", u'\x2904' },
    { "nvdash;", u'\x22ac' },
    { "nvinfin;", u'\x29de' },
    { "nvlArr;", u'\x2902' },
    { "nvrArr;", u'\x2903' },
    { "nwArr;", u'\x21d6' },
    { "nwarhk;", u'\x2923' },
    { "nwarr;", u'\x2196' },
    { "nwarrow;", u'\x2196' },
    { "nwnear;", u'\x2927' },
    { "oS;", u'\x24c8' },
    { "oacute", u'\xf3' },
    { "oacute;", u'\xf3' },
    { "oast;", u'\x229b' },
    { "ocir;", u'\x229a' },
    { "ocirc", u'\xf4' },
    { "ocirc;", u'\xf4' },
    { "ocy;", u'\x043e' },
    { "odash;", u'\x229d' },
    { "odblac;", u'\x0151' },
    { "odiv;", u'\x2a38' },
    { "odot;", u'\x2299' },
    { "odsold;", u'\x29bc' },
    { "oelig;", u'\x0153' },
    { "ofcir;", u'\x29bf' },
    { "ofr;", U'\x0001d52c' },
    { "ogon;", u'\x02db' },
    { "ograve", u'\xf2' },
    { "ograve;", u'\xf2' },
    { "ogt;", u'\x29c1' },
    { "ohbar;", u'\x29b5' },
    { "ohm;", u'\x03a9' },
    { "oint;", u'\x222e' },
    { "olarr;", u'\x21ba' },
    { "olcir;", u'\x29be' },
    { "olcross;", u'\x29bb' },
    { "oline;", u'\x203e' },
    { "olt;", u'\x29c0' },
    { "omacr;", u'\x014d' },
    { "omega;", u'\x03c9' },
    { "omicron;", u'\x03bf' },
    { "omid;", u'\x29b6' },
    { "ominus;", u'\x2296' },
    { "oopf;", U'\x0001d560' },
    { "opar;", u'\x29b7' },
    { "operp;", u'\x29b9' },
    { "oplus;", u'\x2295' },
    { "or;", u'\x2228' },
    { "orarr;", u'\x21bb' },
    { "ord;", u'\x2a5d' },
    { "order;", u'\x2134' },
    { "orderof;", u'\x2134' },
    { "ordf", u'\xaa' },
    { "ordf;", u'\xaa' },
    { "ordm", u'\xba' },
    { "ordm;", u'\xba' },
    { "origof;", u'\x22b6' },
    { "oror;", u'\x2a56' },
    { "orslope;", u'\x2a57' },
    { "orv;", u'\x2a5b' },
    { "oscr;", u'\x2134' },
    { "oslash", u'\xf8' },
    { "oslash;", u'\xf8' },
    { "osol;", u'\x2298' },
    { "otilde", u'\xf5' },
    { "otilde;", u'\xf5' },
    { "otimes;", u'\x2297' },
    { "otimesas;", u'\x2a36' },
    { "ouml", u'\xf6' },
    { "ouml;", u'\xf6' },
    { "ovbar;", u'\x233d' },
    { "par;", u'\x2225' },
    { "para", u'\xb6' },
    { "para;", u'\xb6' },
    { "parallel;", u'\x2225' },
    { "parsim;", u'\x2af3' },
    { "parsl;", u'\x2afd' },
    { "part;", u'\x2202' },
    { "pcy;", u'\x043f' },
    { "percnt;", u'%' },
    { "period;", u'.' },
    { "permil;", u'\x2030' },
    { "perp;", u'\x22a5' },
    { "pertenk;", u'\x2031' },
    { "pfr;", U'\x0001d52d' },
    { "phi;", u'\x03c6' },
    { "phiv;", u'\x03d5' },
    { "phmmat;", u'\x2133' },
    { "phone;", u'\x260e' },
    { "pi;", u'\x03c0' },
    { "pitchfork;", u'\x22d4' },
    { "piv;", u'\x03d6' },
    { "planck;", u'\x210f' },
    { "planckh;", u'\x210e' },
    { "plankv;", u'\x210f' },
    { "plus;", u'+' },
    { "plusacir;", u'\x2a23' },
    { "plusb;", u'\x229e' },
    { "pluscir;", u'\x2a22' },
    { "plusdo;", u'\x2214' },
    { "plusdu;", u'\x2a25' },
    { "pluse;", u'\x2a72' },
    { "plusmn", u'\xb1' },
    { "plusmn;", u'\xb1' },
    { "plussim;", u'\x2a26' },
    { "plustwo;", u'\x2a27' },
    { "pm;", u'\xb1' },
    { "pointint;", u'\x2a15' },
    { "popf;", U'\x0001d561' },
    { "pound", u'\xa3' },
    { "pound;", u'\xa3' },
    { "pr;", u'\x227a' },
    { "prE;", u'\x2ab3' },
    { "prap;", u'\x2ab7' },
    { "prcue;", u'\x227c' },
    { "pre;", u'\x2aaf' },
    { "prec;", u'\x227a' },
    { "precapprox;", u'\x2ab7' },
    { "preccurlyeq;", u'\x227c' },
    { "preceq;", u'\x2aaf' },
    { "precnapprox;", u'\x2ab9' },
    { "precneqq;", u'\x2ab5' },
    { "precnsim;", u'\x22e8' },
    { "precsim;", u'\x227e' },
    { "prime;", u'\x2032' },
    { "primes;", u'\x2119' },
    { "prnE;", u'\x2ab5' },
    { "prnap;", u'\x2ab9' },
    { "prnsim;", u'\x22e8' },
    { "prod;", u'\x220f' },
    { "profalar;", u'\x232e' },
    { "profline;", u'\x2312' },
    { "profsurf;", u'\x2313' },
    { "prop;", u'\x221d' },
    { "propto;", u'\x221d' },
    { "prsim;", u'\x227e' },
    { "prurel;", u'\x22b0' },
    { "pscr;", U'\x0001d4c5' },
    { "psi;", u'\x03c8' },
    { "puncsp;", u'\x2008' },
    { "qfr;", U'\x0001d52e' },
    { "qint;", u'\x2a0c' },
    { "qopf;", U'\x0001d562' },
    { "qprime;", u'\x2057' },
    { "qscr;", U'\x0001d4c6' },
    { "quaternions;", u'\x210d' },
    { "quatint;", u'\x2a16' },
    { "quest;", u'?' },
    { "questeq;", u'\x225f' },
    { "quot", u'"' },
    { "quot;", u'"' },
    { "rAarr;", u'\x21db' },
    { "rArr;", u'\x21d2' },
    { "rAtail;", u'\x291c' },
    { "rBarr;", u'\x290f' },
    { "rHar;", u'\x2964' },
    { "racute;", u'\x0155' },
    { "radic;", u'\x221a' },
    { "raemptyv;", u'\x29b3' },
    { "rang;", u'\x27e9' },
    { "rangd;", u'\x2992' },
    { "range;", u'\x29a5' },
    { "rangle;", u'\x27e9' },
    { "raquo", u'\xbb' },
    { "raquo;", u'\xbb' },
    { "rarr;", u'\x2192' },
    { "rarrap;", u'\x2975' },
    { "rarrb;", u'\x21e5' },
    { "rarrbfs;", u'\x2920' },
    { "rarrc;", u'\x2933' },
    { "rarrfs;", u'\x291e' },
    { "rarrhk;", u'\x21aa' },
    { "rarrlp;", u'\x21ac' },
    { "rarrpl;", u'\x2945' },
    { "rarrsim;", u'\x2974' },
    { "rarrtl;", u'\x21a3' },
    { "rarrw;", u'\x219d' },
    { "ratail;", u'\x291a' },
    { "ratio;", u'\x2236' },
    { "rationals;", u'\x211a' },
    { "rbarr;", u'\x290d' },
    { "rbbrk;", u'\x2773' },
    { "rbrace;", u'}' },
    { "rbrack;", u']' },
    { "rbrke;", u'\x298c' },
    { "rbrksld;", u'\x298e' },
    { "rbrkslu;", u'\x2990' },
    { "rcaron;", u'\x0159' },
    { "rcedil;", u'\x0157' },
    { "rceil;", u'\x2309' },
    { "rcub;", u'}' },
    { "rcy;", u'\x0440' },
    { "rdca;", u'\x2937' },
    { "rdldhar;", u'\x2969' },
    { "rdquo;", u'\x201d' },
    { "rdquor;", u'\x201d' },
    { "rdsh;", u'\x21b3' },
    { "real;", u'\x211c' },
    { "realine;", u'\x211b' },
    { "realpart;", u'\x211c' },
    { "reals;", u'\x211d' },
    { "rect;", u'\x25ad' },
    { "reg", u'\xae' },
    { "reg;", u'\xae' },
    { "rfisht;", u'\x297d' },
    { "rfloor;", u'\x230b' },
    { "rfr;", U'\x0001d52f' },
    { "rhard;", u'\x21c1' },
    { "rharu;", u'\x21c0' },
    { "rharul;", u'\x296c' },
    { "rho;", u'\x03c1' },
    { "rhov;", u'\x03f1' },
    { "rightarrow;", u'\x2192' },
    { "rightarrowtail;", u'\x21a3' },
    { "rightharpoondown;", u'\x21c1' },
    { "rightharpoonup;", u'\x21c0' },
    { "rightleftarrows;", u'\x21c4' },
    { "rightleftharpoons;", u'\x21cc' },
    { "rightrightarrows;", u'\x21c9' },
    { "rightsquigarrow;", u'\x219d' },
    { "rightthreetimes;", u'\x22cc' },
    { "ring;", u'\x02da' },
    { "risingdotseq;", u'\x2253' },
    { "rlarr;", u'\x21c4' },
    { "rlhar;", u'\x21cc' },
    { "rlm;", u'\x200f' },
    { "rmoust;", u'\x23b1' },
    { "rmoustache;", u'\x23b1' },
    { "rnmid;", u'\x2aee' },
    { "roang;", u'\x27ed' },
    { "roarr;", u'\x21fe' },
    { "robrk;", u'\x27e7' },
    { "ropar;", u'\x2986' },
    { "ropf;", U'\x0001d563' },
    { "roplus;", u'\x2a2e' },
    { "rotimes;", u'\x2a35' },
    { "rpar;", u')' },
    { "rpargt;", u'\x2994' },
    { "rppolint;", u'\x2a12' },
    { "rrarr;", u'\x21c9' },
    { "rsaquo;", u'\x203a' },
    { "rscr;", U'\x0001d4c7' },
    { "rsh;", u'\x21b1' },
    { "rsqb;", u']' },
    { "rsquo;", u'\x2019' },
    { "rsquor;", u'\x2019' },
    { "rthree;", u'\x22cc' },
    { "rtimes;", u'\x22ca' },
    { "rtri;", u'\x25b9' },
    { "rtrie;", u'\x22b5' },
    { "rtrif;", u'\x25b8' },
    { "rtriltri;", u'\x29ce' },
    { "ruluhar;", u'\x2968' },
    { "rx;", u'\x211e' },
    { "sacute;", u'\x015b' },
    { "sbquo;", u'\x201a' },
    { "sc;", u'\x227b' },
    { "scE;", u'\x2ab4' },
    { "scap;", u'\x2ab8' },
    { "scaron;", u'\x0161' },
    { "sccue;", u'\x227d' },
    { "sce;", u'\x2ab0' },
    { "scedil;", u'\x015f' },
    { "scirc;", u'\x015d' },
    { "scnE;", u'\x2ab6' },
    { "scnap;", u'\x2aba' },
    { "scnsim;", u'\x22e9' },
    { "scpolint;", u'\x2a13' },
    { "scsim;", u'\x227f' },
    { "scy;", u'\x0441' },
    { "sdot;", u'\x22c5' },
    { "sdotb;", u'\x22a1' },
    { "sdote;", u'\x2a66' },
    { "seArr;", u'\x21d8' },
    { "searhk;", u'\x2925' },
    { "searr;", u'\x2198' },
    { "searrow;", u'\x2198' },
    { "sect", u'\xa7' },
    { "sect;", u'\xa7' },
    { "semi;", u';' },
    { "seswar;", u'\x2929' },
    { "setminus;", u'\x2216' },
    { "setmn;", u'\x2216' },
    { "sext;", u'\x2736' },
    { "sfr;", U'\x0001d530' },
    { "sfrown;", u'\x2322' },
    { "sharp;", u'\x266f' },
    { "shchcy;", u'\x0449' },
    { "shcy;", u'\x0448' },
    { "shortmid;", u'\x2223' },
    { "shortparallel;", u'\x2225' },
    { "shy", u'\xad' },
    { "shy;", u'\xad' },
    { "sigma;", u'\x03c3' },
    { "sigmaf;", u'\x03c2' },
    { "sigmav;", u'\x03c2' },
    { "sim;", u'\x223c' },
    { "simdot;", u'\x2a6a' },
    { "sime;", u'\x2243' },
    { "simeq;", u'\x2243' },
    { "simg;", u'\x2a9e' },
    { "simgE;", u'\x2aa0' },
    { "siml;", u'\x2a9d' },
    { "simlE;", u'\x2a9f' },
    { "simne;", u'\x2246' },
    { "simplus;", u'\x2a24' },
    { "simrarr;", u'\x2972' },
    { "slarr;", u'\x2190' },
    { "smallsetminus;", u'\x2216' },
    { "smashp;", u'\x2a33' },
    { "smeparsl;", u'\x29e4' },
    { "smid;", u'\x2223' },
    { "smile;", u'\x2323' },
    { "smt;", u'\x2aaa' },
    { "smte;", u'\x2aac' },
    { "softcy;", u'\x044c' },
    { "sol;", u'/' },
    { "solb;", u'\x29c4' },
    { "solbar;", u'\x233f' },
    { "sopf;", U'\x0001d564' },
    { "spades;", u'\x2660' },
    { "spadesuit;", u'\x2660' },
    { "spar;", u'\x2225' },
    { "sqcap;", u'\x2293' },
    { "sqcup;", u'\x2294' },
    { "sqsub;", u'\x228f' },
    { "sqsube;", u'\x2291' },
    { "sqsubset;", u'\x228f' },
    { "sqsubseteq;", u'\x2291' },
    { "sqsup;", u'\x2290' },
    { "sqsupe;", u'\x2292' },
    { "sqsupset;", u'\x2290' },
    { "sqsupseteq;", u'\x2292' },
    { "squ;", u'\x25a1' },
    { "square;", u'\x25a1' },
    { "squarf;", u'\x25aa' },
    { "squf;", u'\x25aa' },
    { "srarr;", u'\x2192' },
    { "sscr;", U'\x0001d4c8' },
    { "ssetmn;", u'\x2216' },
    { "ssmile;", u'\x2323' },
    { "sstarf;", u'\x22c6' },
    { "star;", u'\x2606' },
    { "starf;", u'\x2605' },
    { "straightepsilon;", u'\x03f5' },
    { "straightphi;", u'\x03d5' },
    { "strns;", u'\xaf' },
    { "sub;", u'\x2282' },
    { "subE;", u'\x2ac5' },
    { "subdot;", u'\x2abd' },
    { "sube;", u'\x2286' },
    { "subedot;", u'\x2ac3' },
    { "submult;", u'\x2ac1' },
    { "subnE;", u'\x2acb' },
    { "subne;", u'\x228a' },
    { "subplus;", u'\x2abf' },
    { "subrarr;", u'\x2979' },
    { "subset;", u'\x2282' },
    { "subseteq;", u'\x2286' },
    { "subseteqq;", u'\x2ac5' },
    { "subsetneq;", u'\x228a' },
    { "subsetneqq;", u'\x2acb' },
    { "subsim;", u'\x2ac7' },
    { "subsub;", u'\x2ad5' },
    { "subsup;", u'\x2ad3' },
    { "succ;", u'\x227b' },
    { "succapprox;", u'\x2ab8' },
    { "succcurlyeq;", u'\x227d' },
    { "succeq;", u'\x2ab0' },
    { "succnapprox;", u'\x2aba' },
    { "succneqq;", u'\x2ab6' },
    { "succnsim;", u'\x22e9' },
    { "succsim;", u'\x227f' },
    { "sum;", u'\x2211' },
    { "sung;", u'\x266a' },
    { "sup1", u'\xb9' },
    { "sup1;", u'\xb9' },
    { "sup2", u'\xb2' },
    { "sup2;", u'\xb2' },
    { "sup3", u'\xb3' },
    { "sup3;", u'\xb3' },
    { "sup;", u'\x2283' },
    { "supE;", u'\x2ac6' },
    { "supdot;", u'\x2abe' },
    { "supdsub;", u'\x2ad8' },
    { "supe;", u'\x2287' },
    { "supedot;", u'\x2ac4' },
    { "suphsol;", u'\x27c9' },
    { "suphsub;", u'\x2ad7' },
    { "suplarr;", u'\x297b' },
    { "supmult;", u'\x2ac2' },
    { "supnE;", u'\x2acc' },
    { "supne;", u'\x228b' },
    { "supplus;", u'\x2ac0' },
    { "supset;", u'\x2283' },
    { "supseteq;", u'\x2287' },
    { "supseteqq;", u'\x2ac6' },
    { "supsetneq;", u'\x228b' },
    { "supsetneqq;", u'\x2acc' },
    { "supsim;", u'\x2ac8' },
    { "supsub;", u'\x2ad4' },
    { "supsup;", u'\x2ad6' },
    { "swArr;", u'\x21d9' },
    { "swarhk;", u'\x2926' },
    { "swarr;", u'\x2199' },
    { "swarrow;", u'\x2199' },
    { "swnwar;", u'\x292a' },
    { "szlig", u'\xdf' },
    { "szlig;", u'\xdf' },
    { "target;", u'\x2316' },
    { "tau;", u'\x03c4' },
    { "tbrk;", u'\x23b4' },
    { "tcaron;", u'\x0165' },
    { "tcedil;", u'\x0163' },
    { "tcy;", u'\x0442' },
    { "tdot;", u'\x20db' },
    { "telrec;", u'\x2315' },
    { "tfr;", U'\x0001d531' },
    { "there4;", u'\x2234' },
    { "therefore;", u'\x2234' },
    { "theta;", u'\x03b8' },
    { "thetasym;", u'\x03d1' },
    { "thetav;", u'\x03d1' },
    { "thickapprox;", u'\x2248' },
    { "thicksim;", u'\x223c' },
    { "thinsp;", u'\x2009' },
    { "thkap;", u'\x2248' },
    { "thksim;", u'\x223c' },
    { "thorn", u'\xfe' },
    { "thorn;", u'\xfe' },
    { "tilde;", u'\x02dc' },
    { "times", u'\xd7' },
    { "times;", u'\xd7' },
    { "timesb;", u'\x22a0' },
    { "timesbar;", u'\x2a31' },
    { "timesd;", u'\x2a30' },
    { "tint;", u'\x222d' },
    { "toea;", u'\x2928' },
    { "top;", u'\x22a4' },
    { "topbot;", u'\x2336' },
    { "topcir;", u'\x2af1' },
    { "topf;", U'\x0001d565' },
    { "topfork;", u'\x2ada' },
    { "tosa;", u'\x2929' },
    { "tprime;", u'\x2034' },
    { "trade;", u'\x2122' },
    { "triangle;", u'\x25b5' },
    { "triangledown;", u'\x25bf' },
    { "triangleleft;", u'\x25c3' },
    { "trianglelefteq;", u'\x22b4' },
    { "triangleq;", u'\x225c' },
    { "triangleright;", u'\x25b9' },
    { "trianglerighteq;", u'\x22b5' },
    { "tridot;", u'\x25ec' },
    { "trie;", u'\x225c' },
    { "triminus;", u'\x2a3a' },
    { "triplus;", u'\x2a39' },
    { "trisb;", u'\x29cd' },
    { "tritime;", u'\x2a3b' },
    { "trpezium;", u'\x23e2' },
    { "tscr;", U'\x0001d4c9' },
    { "tscy;", u'\x0446' },
    { "tshcy;", u'\x045b' },
    { "tstrok;", u'\x0167' },
    { "twixt;", u'\x226c' },
    { "twoheadleftarrow;", u'\x219e' },
    { "twoheadrightarrow;", u'\x21a0' },
    { "uArr;", u'\x21d1' },
    { "uHar;", u'\x2963' },
    { "uacute", u'\xfa' },
    { "uacute;", u'\xfa' },
    { "uarr;", u'\x2191' },
    { "ubrcy;", u'\x045e' },
    { "ubreve;", u'\x016d' },
    { "ucirc", u'\xfb' },
    { "ucirc;", u'\xfb' },
    { "ucy;", u'\x0443' },
    { "udarr;", u'\x21c5' },
    { "udblac;", u'\x0171' },
    { "udhar;", u'\x296e' },
    { "ufisht;", u'\x297e' },
    { "ufr;", U'\x0001d532' },
    { "ugrave", u'\xf9' },
    { "ugrave;", u'\xf9' },
    { "uharl;", u'\x21bf' },
    { "uharr;", u'\x21be' },
    { "uhblk;", u'\x2580' },
    { "ulcorn;", u'\x231c' },
    { "ulcorner;", u'\x231c' },
    { "ulcrop;", u'\x230f' },
    { "ultri;", u'\x25f8' },
    { "umacr;", u'\x016b' },
    { "uml", u'\xa8' },
    { "uml;", u'\xa8' },
    { "uogon;", u'\x0173' },
    { "uopf;", U'\x0001d566' },
    { "uparrow;", u'\x2191' },
    { "updownarrow;", u'\x2195' },
    { "upharpoonleft;", u'\x21bf' },
    { "upharpoonright;", u'\x21be' },
    { "uplus;", u'\x228e' },
    { "upsi;", u'\x03c5' },
    { "upsih;", u'\x03d2' },
    { "upsilon;", u'\x03c5' },
    { "upuparrows;", u'\x21c8' },
    { "urcorn;", u'\x231d' },
    { "urcorner;", u'\x231d' },
    { "urcrop;", u'\x230e' },
    { "uring;", u'\x016f' },
    { "urtri;", u'\x25f9' },
    { "uscr;", U'\x0001d4ca' },
    { "utdot;", u'\x22f0' },
    { "utilde;", u'\x0169' },
    { "utri;", u'\x25b5' },
    { "utrif;", u'\x25b4' },
    { "uuarr;", u'\x21c8' },
    { "uuml", u'\xfc' },
    { "uuml;", u'\xfc' },
    { "uwangle;", u'\x29a7' },
    { "vArr;", u'\x21d5' },
    { "vBar;", u'\x2ae8' },
    { "vBarv;", u'\x2ae9' },
    { "vDash;", u'\x22a8' },
    { "vangrt;", u'\x299c' },
    { "varepsilon;", u'\x03f5' },
    { "varkappa;", u'\x03f0' },
    { "varnothing;", u'\x2205' },
    { "varphi;", u'\x03d5' },
    { "varpi;", u'\x03d6' },
    { "varpropto;", u'\x221d' },
    { "varr;", u'\x2195' },
    { "varrho;", u'\x03f1' },
    { "varsigma;", u'\x03c2' },
    { "vartheta;", u'\x03d1' },
    { "vartriangleleft;", u'\x22b2' },
    { "vartriangleright;", u'\x22b3' },
    { "vcy;", u'\x0432' },
    { "vdash;", u'\x22a2' },
    { "vee;", u'\x2228' },
    { "veebar;", u'\x22bb' },
    { "veeeq;", u'\x225a' },
    { "vellip;", u'\x22ee' },
    { "verbar;", u'|' },
    { "vert;", u'|' },
    { "vfr;", U'\x0001d533' },
    { "vltri;", u'\x22b2' },
    { "vopf;", U'\x0001d567' },
    { "vprop;", u'\x221d' },
    { "vrtri;", u'\x22b3' },
    { "vscr;", U'\x0001d4cb' },
    { "vzigzag;", u'\x299a' },
    { "wcirc;", u'\x0175' },
    { "wedbar;", u'\x2a5f' },
    { "wedge;", u'\x2227' },
    { "wedgeq;", u'\x2259' },
    { "weierp;", u'\x2118' },
    { "wfr;", U'\x0001d534' },
    { "wopf;", U'\x0001d568' },
    { "wp;", u'\x2118' },
    { "wr;", u'\x2240' },
    { "wreath;", u'\x2240' },
    { "wscr;", U'\x0001d4cc' },
    { "xcap;", u'\x22c2' },
    { "xcirc;", u'\x25ef' },
    { "xcup;", u'\x22c3' },
    { "xdtri;", u'\x25bd' },
    { "xfr;", U'\x0001d535' },
    { "xhArr;", u'\x27fa' },
    { "xharr;", u'\x27f7' },
    { "xi;", u'\x03be' },
    { "xlArr;", u'\x27f8' },
    { "xlarr;", u'\x27f5' },
    { "xmap;", u'\x27fc' },
    { "xnis;", u'\x22fb' },
    { "xodot;", u'\x2a00' },
    { "xopf;", U'\x0001d569' },
    { "xoplus;", u'\x2a01' },
    { "xotime;", u'\x2a02' },
    { "xrArr;", u'\x27f9' },
    { "xrarr;", u'\x27f6' },
    { "xscr;", U'\x0001d4cd' },
    { "xsqcup;", u'\x2a06' },
    { "xuplus;", u'\x2a04' },
    { "xutri;", u'\x25b3' },
    { "xvee;", u'\x22c1' },
    { "xwedge;", u'\x22c0' },
    { "yacute", u'\xfd' },
    { "yacute;", u'\xfd' },
    { "yacy;", u'\x044f' },
    { "ycirc;", u'\x0177' },
    { "ycy;", u'\x044b' },
    { "yen", u'\xa5' },
    { "yen;", u'\xa5' },
    { "yfr;", U'\x0001d536' },
    { "yicy;", u'\x0457' },
    { "yopf;", U'\x0001d56a' },
    { "yscr;", U'\x0001d4ce' },
    { "yucy;", u'\x044e' },
    { "yuml", u'\xff' },
    { "yuml;", u'\xff' },
    { "zacute;", u'\x017a' },
    { "zcaron;", u'\x017e' },
    { "zcy;", u'\x0437' },
    { "zdot;", u'\x017c' },
    { "zeetrf;", u'\x2128' },
    { "zeta;", u'\x03b6' },
    { "zfr;", U'\x0001d537' },
    { "zhcy;", u'\x0436' },
    { "zigrarr;", u'\x21dd' },
    { "zopf;", U'\x0001d56b' },
    { "zscr;", U'\x0001d4cf' },
    { "zwj;", u'\x200d' },
    { "zwnj;", u'\x200c' },
    { "~", u'\xfffd' },  // sentinel. "~" is not a valid entity name.
};

Entity* lookupEntity(const char* entity)
{
    Entity target = { entity, 0 };
    return std::lower_bound(entities,
                            &entities[sizeof entities / sizeof entities[0] - 1],
                            target, compareEntities);
}

struct Key
{
    const char* key;
    bool caseSensitive;
};

class KeyLookup
{
    const Key* keys;
    size_t keyCount;
    const Key* current;
    int pos;
    std::string sequence;

    bool compare(char s, char t, bool caseSensitive)
    {
        if (caseSensitive)
            return s == t;
        return toLower(s) == toLower(t);
    }

public:
    enum {
        NotFound = -1,
        Tentative = -2
    };

    KeyLookup(const Key* keys, size_t keyCount) :
        keys(keys),
        keyCount(keyCount),
        current(keys),
        pos(0)
    {
    }

    int lookup(char c)
    {
        bool repeat;
        if (0 <= c) {
            do {
                if (compare(current->key[pos], c, current->caseSensitive)) {
                    sequence += c;
                    ++pos;
                    if (current->key[pos] == '\0')
                        return current - keys;
                    return Tentative;
                }
                repeat = false;
                for (const Key* key = ++current; key < &keys[keyCount]; ++key) {
                    int result;
                    if (key->caseSensitive)
                        result = strncmp(key->key, sequence.c_str(), pos);
                    else
                        result = strncasecmp(key->key, sequence.c_str(), pos);
                    if (result == 0) {
                        current = key;
                        repeat = true;
                        break;
                    }
                }
            } while (repeat);
        }
        return NotFound;
    }

    std::string& getSequence()
    {
        return sequence;
    }
};

}  // namespace

Attribute::Attribute(int ch)
{
    assert(ch != EOF);
    assert(ch != 0);
    name += ch;
}

void Attribute::append(int ch)
{
    assert(ch != EOF);
    assert(ch != 0);
    name += ch;
}

void Attribute::appendValue(int ch)
{
    assert(ch != EOF);
    assert(ch != 0);
    value += ch;
}

Token::Token(int ucode) :
    type(Type::Character),
    flags(0),
    ucode(ucode)
{
    assert(ucode != EOF);
}

Token::Token(Token::Type type, int ch) :
    type(type),
    flags(0),
    ucode(0)
{
    assert(ch != EOF);
    name += ch;
}

Token::Token(Token::Type type, const std::u16string& name) :
    type(type),
    flags(0),
    ucode(0),
    name(name)
{
}

void Token::append(int ch)
{
    assert(ch != EOF);
    name += ch;
}

// returns true if attribute has been inserted to the attribute list
bool Token::append(Attribute& attribute)
{
    if (attribute.getName().length() == 0)
        return true;
    if (attrNames.find(attribute.getName()) == attrNames.end()) {
        Attr attr = new(std::nothrow) org::w3c::dom::bootstrap::AttrImp(Nullable<std::u16string>(), Nullable<std::u16string>(), attribute.getName(), attribute.getValue());
        if (attr) {
            attrNames.insert(attribute.getName());
            attrList.push_back(attr);
        }
        attribute.clear();
        return true;
    }
    attribute.clear();
    return false;
}

Nullable<std::u16string> Token::getAttribute(std::u16string name) const
{
    if (attrNames.find(name) != attrNames.end()) {
        for (auto i = attrList.begin(); i != attrList.end(); ++i) {
            Attr attr = *i;
            if (attr.getName() == name)
                return attr.getValue();
        }
    }
    return Nullable<std::u16string>();
}

HTMLTokenizer::DataState HTMLTokenizer::dataState;
HTMLTokenizer::RcdataState HTMLTokenizer::rcdataState;
HTMLTokenizer::RawtextState HTMLTokenizer::rawtextState;
HTMLTokenizer::ScriptDataState HTMLTokenizer::scriptDataState;
HTMLTokenizer::PlaintextState HTMLTokenizer::plaintextState;
HTMLTokenizer::TagOpenState HTMLTokenizer::tagOpenState;
HTMLTokenizer::EndTagOpenState HTMLTokenizer::endTagOpenState;
HTMLTokenizer::TagNameState HTMLTokenizer::tagNameState;
HTMLTokenizer::RcdataLessThanSignState HTMLTokenizer::rcdataLessThanSignState;
HTMLTokenizer::RcdataEndTagOpenState HTMLTokenizer::rcdataEndTagOpenState;
HTMLTokenizer::RcdataEndTagNameState HTMLTokenizer::rcdataEndTagNameState;
HTMLTokenizer::RawtextLessThanSignState HTMLTokenizer::rawtextLessThanSignState;
HTMLTokenizer::RawtextEndTagOpenState HTMLTokenizer::rawtextEndTagOpenState;
HTMLTokenizer::RawtextEndTagNameState HTMLTokenizer::rawtextEndTagNameState;
HTMLTokenizer::ScriptDataLessThanSignState HTMLTokenizer::scriptDataLessThanSignState;
HTMLTokenizer::ScriptDataEndTagOpenState HTMLTokenizer::scriptDataEndTagOpenState;
HTMLTokenizer::ScriptDataEndTagNameState HTMLTokenizer::scriptDataEndTagNameState;
HTMLTokenizer::ScriptDataEscapeStartState HTMLTokenizer::scriptDataEscapeStartState;
HTMLTokenizer::ScriptDataEscapeStartDashState HTMLTokenizer::scriptDataEscapeStartDashState;
HTMLTokenizer::ScriptDataEscapedState HTMLTokenizer::scriptDataEscapedState;
HTMLTokenizer::ScriptDataEscapedDashState HTMLTokenizer::scriptDataEscapedDashState;
HTMLTokenizer::ScriptDataEscapedDashDashState HTMLTokenizer::scriptDataEscapedDashDashState;
HTMLTokenizer::ScriptDataEscapedLessThanSignState HTMLTokenizer::scriptDataEscapedLessThanSignState;
HTMLTokenizer::ScriptDataEscapedEndTagOpenState HTMLTokenizer::scriptDataEscapedEndTagOpenState;
HTMLTokenizer::ScriptDataEscapedEndTagNameState HTMLTokenizer::scriptDataEscapedEndTagNameState;
HTMLTokenizer::ScriptDataDoubleEscapeStartState HTMLTokenizer::scriptDataDoubleEscapeStartState;
HTMLTokenizer::ScriptDataDoubleEscapedState HTMLTokenizer::scriptDataDoubleEscapedState;
HTMLTokenizer::ScriptDataDoubleEscapedDashState HTMLTokenizer::scriptDataDoubleEscapedDashState;
HTMLTokenizer::ScriptDataDoubleEscapedDashDashState HTMLTokenizer::scriptDataDoubleEscapedDashDashState;
HTMLTokenizer::ScriptDataDoubleEscapedLessThanSignState HTMLTokenizer::scriptDataDoubleEscapedLessThanSignState;
HTMLTokenizer::ScriptDataDoubleEscapeEndState HTMLTokenizer::scriptDataDoubleEscapeEndState;
HTMLTokenizer::BeforeAttributeNameState HTMLTokenizer::beforeAttributeNameState;
HTMLTokenizer::AttributeNameState HTMLTokenizer::attributeNameState;
HTMLTokenizer::AfterAttributeNameState HTMLTokenizer::afterAttributeNameState;
HTMLTokenizer::BeforeAttributeValueState HTMLTokenizer::beforeAttributeValueState;
HTMLTokenizer::AttributeValueDoubleQuotedState HTMLTokenizer::attributeValueDoubleQuotedState;
HTMLTokenizer::AttributeValueSingleQuotedState HTMLTokenizer::attributeValueSingleQuotedState;
HTMLTokenizer::AttributeValueUnquotedState HTMLTokenizer::attributeValueUnquotedState;
HTMLTokenizer::AfterAttributeValueQuotedState HTMLTokenizer::afterAttributeValueQuotedState;
HTMLTokenizer::SelfClosingStartTagState HTMLTokenizer::selfClosingStartTagState;
HTMLTokenizer::BogusCommentState HTMLTokenizer::bogusCommentState;
HTMLTokenizer::MarkupDeclarationOpenState HTMLTokenizer::markupDeclarationOpenState;
HTMLTokenizer::CommentStartState HTMLTokenizer::commentStartState;
HTMLTokenizer::CommentStartDashState HTMLTokenizer::commentStartDashState;
HTMLTokenizer::CommentState HTMLTokenizer::commentState;
HTMLTokenizer::CommentEndDashState HTMLTokenizer::commentEndDashState;
HTMLTokenizer::CommentEndState HTMLTokenizer::commentEndState;
HTMLTokenizer::CommentEndBangState HTMLTokenizer::commentEndBangState;
HTMLTokenizer::CommentEndSpaceState HTMLTokenizer::commentEndSpaceState;
HTMLTokenizer::DoctypeState HTMLTokenizer::doctypeState;
HTMLTokenizer::BeforeDoctypeNameState HTMLTokenizer::beforeDoctypeNameState;
HTMLTokenizer::DoctypeNameState HTMLTokenizer::doctypeNameState;
HTMLTokenizer::AfterDoctypeNameState HTMLTokenizer::afterDoctypeNameState;
HTMLTokenizer::AfterDoctypePublicKeywordState HTMLTokenizer::afterDoctypePublicKeywordState;
HTMLTokenizer::BeforeDoctypePublicIdentifierState HTMLTokenizer::beforeDoctypePublicIdentifierState;
HTMLTokenizer::DoctypePublicIdentifierDoubleQuotedState HTMLTokenizer::doctypePublicIdentifierDoubleQuotedState;
HTMLTokenizer::DoctypePublicIdentifierSingleQuotedState HTMLTokenizer::doctypePublicIdentifierSingleQuotedState;
HTMLTokenizer::AfterDoctypePublicIdentifierState HTMLTokenizer::afterDoctypePublicIdentifierState;
HTMLTokenizer::BetweenDoctypePublicAndSystemIdentifiersState HTMLTokenizer::betweenDoctypePublicAndSystemIdentifiersState;
HTMLTokenizer::AfterDoctypeSystemKeywordState HTMLTokenizer::afterDoctypeSystemKeywordState;
HTMLTokenizer::BeforeDoctypeSystemIdentifierState HTMLTokenizer::beforeDoctypeSystemIdentifierState;
HTMLTokenizer::DoctypeSystemIdentifierDoubleQuotedState HTMLTokenizer::doctypeSystemIdentifierDoubleQuotedState;
HTMLTokenizer::DoctypeSystemIdentifierSingleQuotedState HTMLTokenizer::doctypeSystemIdentifierSingleQuotedState;
HTMLTokenizer::AfterDoctypeSystemIdentifierState HTMLTokenizer::afterDoctypeSystemIdentifierState;
HTMLTokenizer::BogusDoctypeState HTMLTokenizer::bogusDoctypeState;
HTMLTokenizer::CdataSectionState HTMLTokenizer::cdataSectionState;

bool HTMLTokenizer::DataState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '&':
        ch = tokenizer->consumeCharacterReference();
        if (!ch)
            emitted |= tokenizer->emit('&');
        else
            emitted |= tokenizer->emit(ch);
        break;
    case '<':
        tokenizer->setState(&tokenizer->tagOpenState);
        break;
    case EOF:
        emitted |= tokenizer->emit(EOF);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::RcdataState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '&':
        ch = tokenizer->consumeCharacterReference();
        if (!ch)
            emitted |= tokenizer->emit('&');
        else
            emitted |= tokenizer->emit(ch);
        break;
    case '<':
        tokenizer->setState(&tokenizer->rcdataLessThanSignState);
        break;
    case EOF:
        emitted |= tokenizer->emit(EOF);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::RawtextState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '<':
        tokenizer->setState(&tokenizer->rawtextLessThanSignState);
        break;
    case EOF:
        emitted |= tokenizer->emit(EOF);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '<':
        tokenizer->setState(&tokenizer->scriptDataLessThanSignState);
        break;
    case EOF:
        emitted |= tokenizer->emit(EOF);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::PlaintextState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case EOF:
        emitted |= tokenizer->emit(EOF);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::TagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '!':
        tokenizer->setState(&tokenizer->markupDeclarationOpenState);
        break;
    case '/':
        tokenizer->setState(&tokenizer->endTagOpenState);
        break;
    case '?':
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->bogusCommentState, ch);
        break;
    default:
        if (isAlpha(ch)) {
            tokenizer->currentToken = Token(Token::Type::StartTag, toLower(ch));
            tokenizer->setState(&tokenizer->tagNameState);
        } else {
            tokenizer->parseError();
            emitted |= tokenizer->emit('<');
            tokenizer->setState(&tokenizer->dataState, ch);
        }
        break;
    }
    return emitted;
}

bool HTMLTokenizer::EndTagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '>':
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        tokenizer->setState(&tokenizer->dataState);
        tokenizer->state->consume(tokenizer, ch);
        break;
    default:
        if (isAlpha(ch)) {
            tokenizer->currentToken = Token(Token::Type::EndTag, toLower(ch));
            tokenizer->setState(&tokenizer->tagNameState);
        } else {
            tokenizer->parseError();
            tokenizer->setState(&tokenizer->bogusCommentState, ch);
        }
        break;
    }
    return emitted;
}

bool HTMLTokenizer::TagNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->beforeAttributeNameState);
        break;
    case '/':
        tokenizer->setState(&tokenizer->selfClosingStartTagState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentToken.append(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::RcdataLessThanSignState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '/':
        tokenizer->temporaryBuffer = u"";
        tokenizer->setState(&tokenizer->rcdataEndTagOpenState);
        break;
    default:
        emitted |= tokenizer->emit('<');
        tokenizer->setState(&tokenizer->rcdataState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::RcdataEndTagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (isAlpha(ch)) {
        tokenizer->currentToken = Token(Token::Type::EndTag, toLower(ch));
        tokenizer->temporaryBuffer += ch;
        tokenizer->setState(&tokenizer->rcdataEndTagNameState);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        tokenizer->setState(&tokenizer->rcdataState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::RcdataEndTagNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (tokenizer->isAppropriate(tokenizer->currentToken.getName())) {
        switch (ch) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            tokenizer->setState(&tokenizer->beforeAttributeNameState);
            return emitted;
        case '/':
            tokenizer->setState(&tokenizer->selfClosingStartTagState);
            return emitted;
        case '>':
            emitted |= tokenizer->emit(tokenizer->currentToken);
            tokenizer->setState(&tokenizer->dataState);
            return emitted;
        default:
            break;
        }
    }
    if (isAlpha(ch)) {
        tokenizer->temporaryBuffer += ch;
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentToken.append(ch);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        emitted |= tokenizer->emit(tokenizer->temporaryBuffer);
        tokenizer->setState(&tokenizer->rcdataState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::RawtextLessThanSignState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '/':
        tokenizer->temporaryBuffer = u"";
        tokenizer->setState(&tokenizer->rawtextEndTagOpenState);
        break;
    default:
        emitted |= tokenizer->emit('<');
        tokenizer->setState(&tokenizer->rawtextState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::RawtextEndTagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (isAlpha(ch)) {
        tokenizer->currentToken = Token(Token::Type::EndTag, toLower(ch));
        tokenizer->temporaryBuffer += ch;
        tokenizer->setState(&tokenizer->rawtextEndTagNameState);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        tokenizer->setState(&tokenizer->rawtextState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::RawtextEndTagNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (tokenizer->isAppropriate(tokenizer->currentToken.getName())) {
        switch (ch) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            tokenizer->setState(&tokenizer->beforeAttributeNameState);
            return emitted;
        case '/':
            tokenizer->setState(&tokenizer->selfClosingStartTagState);
            return emitted;
        case '>':
            emitted |= tokenizer->emit(tokenizer->currentToken);
            tokenizer->setState(&tokenizer->dataState);
            return emitted;
        default:
            break;
        }
    }
    if (isAlpha(ch)) {
        tokenizer->temporaryBuffer += ch;
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentToken.append(ch);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        emitted |= tokenizer->emit(tokenizer->temporaryBuffer);
        tokenizer->setState(&tokenizer->rawtextState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataLessThanSignState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '/':
        tokenizer->temporaryBuffer = u"";
        tokenizer->setState(&tokenizer->scriptDataEndTagOpenState);
        break;
    case '!':
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('!');
        tokenizer->setState(&tokenizer->scriptDataEscapeStartState);
        break;
    default:
        emitted |= tokenizer->emit('<');
        tokenizer->setState(&tokenizer->scriptDataState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEndTagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (isAlpha(ch)) {
        tokenizer->currentToken = Token(Token::Type::EndTag, toLower(ch));
        tokenizer->temporaryBuffer += ch;
        tokenizer->setState(&tokenizer->scriptDataEndTagNameState);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        tokenizer->setState(&tokenizer->scriptDataState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEndTagNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (tokenizer->isAppropriate(tokenizer->currentToken.getName())) {
        switch (ch) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            tokenizer->setState(&tokenizer->beforeAttributeNameState);
            return emitted;
        case '/':
            tokenizer->setState(&tokenizer->selfClosingStartTagState);
            return emitted;
        case '>':
            emitted |= tokenizer->emit(tokenizer->currentToken);
            tokenizer->setState(&tokenizer->dataState);
            return emitted;
        default:
            break;
        }
    }
    if (isAlpha(ch)) {
        tokenizer->temporaryBuffer += ch;
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentToken.append(ch);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        emitted |= tokenizer->emit(tokenizer->temporaryBuffer);
        tokenizer->setState(&tokenizer->scriptDataState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapeStartState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataEscapeStartDashState);
        break;
    default:
        tokenizer->setState(&tokenizer->scriptDataState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapeStartDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataEscapedDashDashState);
        break;
    default:
        tokenizer->setState(&tokenizer->scriptDataState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataEscapeStartDashState);
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataEscapedLessThanSignState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataEscapedDashDashState);
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataEscapedLessThanSignState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedDashDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataEscapedLessThanSignState);
        break;
    case '>':
        emitted |= tokenizer->emit('>');
        tokenizer->setState(&tokenizer->scriptDataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedLessThanSignState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '/':
        tokenizer->temporaryBuffer = u"";
        tokenizer->setState(&tokenizer->scriptDataEscapedEndTagOpenState);
        break;
    default:
        if (isAlpha(ch)) {
            emitted |= tokenizer->emit('<');
            emitted |= tokenizer->emit(ch);
            tokenizer->temporaryBuffer = toLower(ch);
            tokenizer->setState(&tokenizer->scriptDataDoubleEscapeStartState);
        } else {
            emitted |= tokenizer->emit('<');
            tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
        }
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedEndTagOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (isAlpha(ch)) {
        tokenizer->currentToken = Token(Token::Type::EndTag, toLower(ch));
        tokenizer->temporaryBuffer += ch;
        tokenizer->setState(&tokenizer->scriptDataEscapedEndTagNameState);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataEscapedEndTagNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    if (tokenizer->isAppropriate(tokenizer->currentToken.getName())) {
        switch (ch) {
        case '\t':
        case '\n':
        case '\f':
        case ' ':
            tokenizer->setState(&tokenizer->beforeAttributeNameState);
            return emitted;
        case '/':
            tokenizer->setState(&tokenizer->selfClosingStartTagState);
            return emitted;
        case '>':
            emitted |= tokenizer->emit(tokenizer->currentToken);
            tokenizer->setState(&tokenizer->dataState);
            return emitted;
        default:
            break;
        }
    }
    if (isAlpha(ch)) {
        tokenizer->temporaryBuffer += ch;
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentToken.append(ch);
    } else {
        emitted |= tokenizer->emit('<');
        emitted |= tokenizer->emit('/');
        emitted |= tokenizer->emit(tokenizer->temporaryBuffer);
        tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapeStartState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '/':
    case '>':
        emitted |= tokenizer->emit(ch);
        if (tokenizer->temporaryBuffer == u"script")
            tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState);
        else
            tokenizer->setState(&tokenizer->scriptDataEscapedState);
        break;
    default:
        if (isAlpha(ch)) {
            emitted |= tokenizer->emit(ch);
            if (isUpper(ch))
                ch += 0x20;
            tokenizer->temporaryBuffer += ch;
        } else
            tokenizer->setState(&tokenizer->scriptDataEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedDashState);
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedLessThanSignState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapedDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedDashDashState);
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedLessThanSignState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapedDashDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        emitted |= tokenizer->emit('-');
        break;
    case '<':
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedLessThanSignState);
        break;
    case '>':
        emitted |= tokenizer->emit('>');
        tokenizer->setState(&tokenizer->scriptDataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        emitted |= tokenizer->emit(ch);
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapedLessThanSignState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '/':
        emitted |= tokenizer->emit('/');
        tokenizer->temporaryBuffer = u"";
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapeEndState);
        break;
    default:
        tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::ScriptDataDoubleEscapeEndState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '/':
    case '>':
        emitted |= tokenizer->emit(ch);
        if (tokenizer->temporaryBuffer == u"script")
            tokenizer->setState(&tokenizer->scriptDataEscapedState);
        else
            tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState);
        break;
    default:
        if (isAlpha(ch)) {
            emitted |= tokenizer->emit(ch);
            if (isUpper(ch))
                ch += 0x20;
            tokenizer->temporaryBuffer += ch;
        } else
            tokenizer->setState(&tokenizer->scriptDataDoubleEscapedState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BeforeAttributeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '/':
        tokenizer->setState(&tokenizer->selfClosingStartTagState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    case '"':
    case '\'':
    case '<':
    case '=':
        tokenizer->parseError();
        // FALL THROUGH
    default:
        tokenizer->currentAttribute = Attribute(toLower(ch));
        tokenizer->setState(&tokenizer->attributeNameState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AttributeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->afterAttributeNameState);
        break;
    case '/':
        tokenizer->setState(&tokenizer->selfClosingStartTagState);
        break;
    case '=':
        tokenizer->setState(&tokenizer->beforeAttributeValueState);
        break;
    case '>':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    case '"':
    case '\'':
    case '<':
        tokenizer->parseError();
        // FALL THROUGH
    default:
        if (isUpper(ch))
            ch += 0x20;
        tokenizer->currentAttribute.append(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterAttributeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '/':
        tokenizer->setState(&tokenizer->selfClosingStartTagState);
        break;
    case '=':
        tokenizer->setState(&tokenizer->beforeAttributeValueState);
        break;
    case '>':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    case '"':
    case '\'':
    case '<':
        tokenizer->parseError();
        // FALL THROUGH
    default:
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        tokenizer->currentAttribute = Attribute(toLower(ch));
        tokenizer->setState(&tokenizer->attributeNameState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BeforeAttributeValueState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '"':
        tokenizer->setState(&tokenizer->attributeValueDoubleQuotedState);
        break;
    case '&':
        tokenizer->setState(&tokenizer->attributeValueUnquotedState, ch);
        break;
    case '\'':
        tokenizer->setState(&tokenizer->attributeValueSingleQuotedState);
        break;
    case '>':
        tokenizer->parseError();
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    case '<':
    case '=':
    case '`':
        tokenizer->parseError();
        // FALL THROUGH
    default:
        tokenizer->currentAttribute.appendValue(ch);
        tokenizer->setState(&tokenizer->attributeValueUnquotedState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AttributeValueDoubleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '"':
        tokenizer->setState(&tokenizer->afterAttributeValueQuotedState);
        break;
    case '&':
        ch = tokenizer->consumeCharacterReference('"');
        if (!ch)
            tokenizer->currentAttribute.appendValue('&');
        else
            tokenizer->currentAttribute.appendValue(ch);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        tokenizer->currentAttribute.appendValue(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AttributeValueSingleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\'':
        tokenizer->setState(&tokenizer->afterAttributeValueQuotedState);
        break;
    case '&':
        ch = tokenizer->consumeCharacterReference('\'');
        if (!ch)
            tokenizer->currentAttribute.appendValue('&');
        else
            tokenizer->currentAttribute.appendValue(ch);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        tokenizer->currentAttribute.appendValue(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AttributeValueUnquotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        tokenizer->setState(&tokenizer->beforeAttributeNameState);
        break;
    case '&':
        ch = tokenizer->consumeCharacterReference('>');
        if (!ch)
            tokenizer->currentAttribute.appendValue('&');
        else
            tokenizer->currentAttribute.appendValue(ch);
        break;
    case '>':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    case '"':
    case '\'':
    case '<':
    case '=':
    case '`':
        tokenizer->parseError();
        // FALL THROUGH
    default:
        tokenizer->currentAttribute.appendValue(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterAttributeValueQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        tokenizer->setState(&tokenizer->beforeAttributeNameState);
        break;
    case '/':
        tokenizer->setState(&tokenizer->selfClosingStartTagState);
        break;
    case '>':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.append(tokenizer->currentAttribute);
        tokenizer->setState(&tokenizer->beforeAttributeNameState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::SelfClosingStartTagState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '>':
        if (!tokenizer->currentToken.append(tokenizer->currentAttribute))
            tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::SelfClosing);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->dataState, ch);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.append(tokenizer->currentAttribute);
        tokenizer->setState(&tokenizer->beforeAttributeNameState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BogusCommentState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    tokenizer->currentToken = Token(Token::Type::Comment);
    while (ch != EOF && ch != '>') {
        if (ch == 0)
            ch = 0xfffd;
        tokenizer->currentToken.append(ch);
        ch = tokenizer->getChar();
    }
    emitted |= tokenizer->emit(tokenizer->currentToken);
    if (ch == EOF)
        tokenizer->setState(&tokenizer->dataState, EOF);
    else
        tokenizer->setState(&tokenizer->dataState);
    return emitted;
}

bool HTMLTokenizer::MarkupDeclarationOpenState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    static const Key keys[] = {
        { "--", true },
        { "DOCTYPE", false },
        { "[CDATA[", true },
    };
    KeyLookup context(keys, sizeof keys / sizeof keys[0]);

    for (;;) {
        switch (context.lookup(ch)) {
        case 0:  // "--"
            tokenizer->currentToken = Token(Token::Type::Comment);
            tokenizer->setState(&tokenizer->commentStartState);
            return emitted;
        case 1:  // "DOCTYPE"
            tokenizer->setState(&tokenizer->doctypeState);
            return emitted;
        case 2:  // "[CDATA["
            // TODO: Check conditions
            tokenizer->setState(&tokenizer->cdataSectionState);
            return emitted;
        case KeyLookup::NotFound:
            tokenizer->parseError();
            tokenizer->ungetChar(ch);
            tokenizer->ungetString(context.getSequence());
            tokenizer->setState(&tokenizer->bogusCommentState);
            return emitted;
        default:
            ch = tokenizer->getChar();
            break;
        }
    }
    return emitted;
}

bool HTMLTokenizer::CommentStartState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        tokenizer->setState(&tokenizer->commentStartDashState);
        break;
    case '>':
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentStartDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        tokenizer->setState(&tokenizer->commentEndState);
        break;
    case '>':
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        tokenizer->setState(&tokenizer->commentEndDashState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append(ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentEndDashState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        tokenizer->setState(&tokenizer->commentEndState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentEndState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->parseError();
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentEndSpaceState);
        break;
    case '!':
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->commentEndBangState);
        break;
    case '-':
        tokenizer->parseError();
        tokenizer->currentToken.append('-');
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentEndBangState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '-':
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('!');
        tokenizer->setState(&tokenizer->commentEndDashState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('-');
        tokenizer->currentToken.append('!');
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CommentEndSpaceState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->currentToken.append(ch);
        break;
    case '-':
        tokenizer->setState(&tokenizer->commentEndDashState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append(ch);
        tokenizer->setState(&tokenizer->commentState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypeState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->beforeDoctypeNameState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken = Token(Token::Type::Doctype);
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->beforeDoctypeNameState, ch);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BeforeDoctypeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken = Token(Token::Type::Doctype);
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken = Token(Token::Type::Doctype);
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken = Token(Token::Type::Doctype, toLower(ch));
        tokenizer->setState(&tokenizer->doctypeNameState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->afterDoctypeNameState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.append(toLower(ch));
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterDoctypeNameState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    static const Key keys[] = {
        { "PUBLIC", false },
        { "SYSTEM", false },
    };
    KeyLookup context(keys, sizeof keys / sizeof keys[0]);

    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        for (;;) {
            switch (context.lookup(ch)) {
            case 0:  // "PUBLIC"
                tokenizer->setState(&tokenizer->afterDoctypePublicKeywordState);
                return emitted;
            case 1:  // "SYSTEM"
                tokenizer->setState(&tokenizer->afterDoctypeSystemKeywordState);
                return emitted;
            case KeyLookup::NotFound:
                tokenizer->parseError();
                tokenizer->ungetChar(ch);
                tokenizer->ungetString(context.getSequence());
                tokenizer->currentToken.setFlag(Token::ForceQuirks);
                tokenizer->setState(&tokenizer->bogusDoctypeState);
                return emitted;
            default:
                ch = tokenizer->getChar();
                break;
            }
        }
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterDoctypePublicKeywordState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->beforeDoctypePublicIdentifierState);
        break;
    case '"':
        tokenizer->parseError();
        tokenizer->currentToken.setPublicId(u"");
        tokenizer->setState(&tokenizer->doctypePublicIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->parseError();
        tokenizer->currentToken.setPublicId(u"");
        tokenizer->setState(&tokenizer->doctypePublicIdentifierSingleQuotedState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BeforeDoctypePublicIdentifierState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '"':
        tokenizer->currentToken.setPublicId(u"");
        tokenizer->setState(&tokenizer->doctypePublicIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->currentToken.setPublicId(u"");
        tokenizer->setState(&tokenizer->doctypePublicIdentifierSingleQuotedState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypePublicIdentifierDoubleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '"':
        tokenizer->setState(&tokenizer->afterDoctypePublicIdentifierState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.getPublicId() += ch;
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypePublicIdentifierSingleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\'':
        tokenizer->setState(&tokenizer->afterDoctypePublicIdentifierState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.getPublicId() += ch;
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterDoctypePublicIdentifierState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->betweenDoctypePublicAndSystemIdentifiersState);
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case '"':
        tokenizer->parseError();
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->parseError();
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierSingleQuotedState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BetweenDoctypePublicAndSystemIdentifiersState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case '"':
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierSingleQuotedState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterDoctypeSystemKeywordState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        tokenizer->setState(&tokenizer->beforeDoctypeSystemIdentifierState);
        break;
    case '"':
        tokenizer->parseError();
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->parseError();
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierSingleQuotedState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BeforeDoctypeSystemIdentifierState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '"':
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierDoubleQuotedState);
        break;
    case '\'':
        tokenizer->currentToken.setSystemId(u"");
        tokenizer->setState(&tokenizer->doctypeSystemIdentifierSingleQuotedState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypeSystemIdentifierDoubleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '"':
        tokenizer->setState(&tokenizer->afterDoctypeSystemIdentifierState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.getSystemId() += ch;
        break;
    }
    return emitted;
}

bool HTMLTokenizer::DoctypeSystemIdentifierSingleQuotedState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\'':
        tokenizer->setState(&tokenizer->afterDoctypeSystemIdentifierState);
        break;
    case '>':
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->currentToken.getSystemId() += ch;
        break;
    }
    return emitted;
}

bool HTMLTokenizer::AfterDoctypeSystemIdentifierState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
        break;
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        tokenizer->parseError();
        tokenizer->currentToken.setFlag(Token::ForceQuirks);
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        tokenizer->parseError();
        tokenizer->setState(&tokenizer->bogusDoctypeState);
        break;
    }
    return emitted;
}

bool HTMLTokenizer::BogusDoctypeState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    switch (ch) {
    case '>':
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState);
        break;
    case EOF:
        emitted |= tokenizer->emit(tokenizer->currentToken);
        tokenizer->setState(&tokenizer->dataState, EOF);
        break;
    default:
        break;
    }
    return emitted;
}

bool HTMLTokenizer::CdataSectionState::consume(HTMLTokenizer* tokenizer, int ch)
{
    bool emitted = false;
    const char16_t seq[] = { ']', ']', '>' };
    const char16_t* seqLimit = seq;
    for (;;) {
        ch = tokenizer->getChar();
        if (ch == *seqLimit) {
            ++seqLimit;
            if (sizeof seq / sizeof seq[0] == seqLimit - seq)
                break;
            continue;
        }
        for (const char16_t* p = seq; p < seqLimit; ++p)
            emitted |= tokenizer->emit(*p);
        seqLimit = seq;
        if (ch == EOF)
            break;
        emitted |= tokenizer->emit(ch);
    }
    if (ch == EOF)
        tokenizer->setState(&tokenizer->dataState, EOF);
    else
        tokenizer->setState(&tokenizer->dataState);
    return emitted;
}

char32_t HTMLTokenizer::replaceCharacter(char32_t number)
{
    if (UnicodeMax < number || (SurrogateBegin <= number && number <= SurrogateEnd)) {
        parseError();
        return u'\xfffd';
    }
    ReplacementCharacter target = { number, 0 };
    ReplacementCharacter* result;
    result = std::lower_bound(replacementCharacters,
                              &replacementCharacters[sizeof replacementCharacters / sizeof replacementCharacters[0]],
                              target, compareReplacementCharacters);
    if (result->number == number) {
        parseError();
        return result->unicode;
    }
    // TODO:
    return number;
}

// cf. 10.2.4.70 Tokenizing character references
int HTMLTokenizer::consumeCharacterReference(int additionalAllowedCharacter)
{
    char32_t number = 0;
    unsigned digits = 0;
    int radix;
    char name[MaxEntityName + 1];
    char* nameLimit;
    char* entityLimit;
    Entity* entity;
    Entity* found;

    int ch = peekChar();
    if (ch == additionalAllowedCharacter)
        return 0;
    switch (ch) {
    case '\t':
    case '\n':
    case '\f':
    case ' ':
    case '<':
    case '&':
    case EOF:
        ch = 0;
        break;
    case '#':
        getChar();
        radix = peekChar();
        if (radix == 'x' || radix == 'X') {
            getChar();
            while (int offset = isHexDigit(ch = peekChar())) {
                getChar();
                if (number <= UnicodeMax) {
                    number *= 16;
                    number += ch - offset;
                    ++digits;
                }
            }
            if (!digits)
                ungetChar(radix);
        } else {
            while (isDigit(ch = peekChar())) {
                getChar();
                if (number <= UnicodeMax) {
                    number *= 10;
                    number += ch - '0';
                    ++digits;
                }
            }
        }
        if (!digits) {
            ungetChar('#');
            ch = 0;
            parseError();
            break;
        }
        if (ch == ';')
            getChar();
        else
            parseError();
        ch = replaceCharacter(number);
        break;
    default:
        found = 0;
        entityLimit = nameLimit = name;
        while (isAlnum(ch = peekChar()) || ch == ';') {
            getChar();
            *nameLimit++ = static_cast<char>(ch);
            *nameLimit = '\0';
            entity = lookupEntity(name);
            if (strcmp(entity->entity, name) == 0) {
                found = entity;
                entityLimit = nameLimit;
            } else if (strncmp(entity->entity, name, nameLimit - name) != 0)
                break;
            if (ch == ';' || MaxEntityName <= nameLimit - name)
                break;
        }
        while (entityLimit <= --nameLimit)
            ungetChar(*nameLimit);
        if (!found) {
            // TODO: parseError();
            ch = 0;
        } else if (fromAttribute && ch != ';' && (isAlnum(ch) || ch == '=')) {
            while (name <= --entityLimit)
                ungetChar(*entityLimit);
            ch = 0;
        } else {
            if (ch != ';')
                parseError();
            ch = found->unicode;
        }
        break;
    }
    return ch;
}

void HTMLTokenizer::parseError()
{
    tokenQueue.push(Token(Token::Type::ParseError));
}

bool HTMLTokenizer::emit(int c)
{
    if (c != EOF)
        tokenQueue.push(Token(c));
    else
        tokenQueue.push(Token(Token::Type::EndOfFile));
    return true;
}

bool HTMLTokenizer::emit(const std::u16string& s)
{
    std::u16string::const_iterator i;
    for (i = s.begin(); i < s.end(); ++i)
        tokenQueue.push(Token(*i));
    return true;
}

bool HTMLTokenizer::emit(const Token& tag)
{
    if (tag.getType() == Token::Type::StartTag)
        appropriateTagName = tag.getName();
    if (tag.getType() == Token::Type::EndTag) {
        if (!tag.getAttributes().empty())
            parseError();
        if (tag.getFlags() & Token::SelfClosing)
            parseError();
    }
    tokenQueue.push(currentToken);
    return true;
}

Token HTMLTokenizer::peekToken()
{
    for (;;) {
        if (!tokenQueue.empty())
            return tokenQueue.front();
        int c;
        do {
            c = getChar();
        } while (!state->consume(this, c));
    }
}

Token HTMLTokenizer::getToken()
{
    Token token = peekToken();
    tokenQueue.pop();
    return token;
}
