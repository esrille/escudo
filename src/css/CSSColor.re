/*
 * Copyright 2010 Esrille Inc.
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

// cf. http://www.w3.org/TR/css3-color/

#include <string>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

unsigned CSSGetKeywordColor(const std::u16string& keyword)
{
    static const unsigned Inv = 0x00FFFFFF;
    const char16_t* yyin = keyword.c_str();
    const char16_t* yymarker = yyin;
    const char16_t* yylimit = yyin + keyword.length();

/*!re2c

    re2c:define:YYCTYPE  = "char16_t";
    re2c:define:YYCURSOR = yyin;
    re2c:define:YYMARKER = yymarker;
    re2c:define:YYLIMIT = yylimit;
    re2c:yyfill:enable   = 0;
    re2c:indent:top      = 1;
    re2c:indent:string = "    " ;

    "transparent" { return *yyin ? Inv : 0x00000000; }   /* rgba(0,0,0,0) */

    "aliceblue" { return *yyin ? Inv : 0xFFF0F8FF; }   /* 240,248,255 */
    "antiquewhite"  { return *yyin ? Inv : 0xFFFAEBD7; }   /* 250,235,215 */
    "aqua"  { return *yyin ? Inv : 0xFF00FFFF; }   /* 0,255,255 */
    "aquamarine"    { return *yyin ? Inv : 0xFF7FFFD4; }   /* 127,255,212 */
    "azure" { return *yyin ? Inv : 0xFFF0FFFF; }   /* 240,255,255 */
    "beige" { return *yyin ? Inv : 0xFFF5F5DC; }   /* 245,245,220 */
    "bisque"    { return *yyin ? Inv : 0xFFFFE4C4; }   /* 255,228,196 */
    "black" { return *yyin ? Inv : 0xFF000000; }   /* 0,0,0 */
    "blanchedalmond"    { return *yyin ? Inv : 0xFFFFEBCD; }   /* 255,235,205 */
    "blue"  { return *yyin ? Inv : 0xFF0000FF; }   /* 0,0,255 */
    "blueviolet"    { return *yyin ? Inv : 0xFF8A2BE2; }   /* 138,43,226 */
    "brown" { return *yyin ? Inv : 0xFFA52A2A; }   /* 165,42,42 */
    "burlywood" { return *yyin ? Inv : 0xFFDEB887; }   /* 222,184,135 */
    "cadetblue" { return *yyin ? Inv : 0xFF5F9EA0; }   /* 95,158,160 */
    "chartreuse"    { return *yyin ? Inv : 0xFF7FFF00; }   /* 127,255,0 */
    "chocolate" { return *yyin ? Inv : 0xFFD2691E; }   /* 210,105,30 */
    "coral" { return *yyin ? Inv : 0xFFFF7F50; }   /* 255,127,80 */
    "cornflowerblue"    { return *yyin ? Inv : 0xFF6495ED; }   /* 100,149,237 */
    "cornsilk"  { return *yyin ? Inv : 0xFFFFF8DC; }   /* 255,248,220 */
    "crimson"   { return *yyin ? Inv : 0xFFDC143C; }   /* 220,20,60 */
    "cyan"  { return *yyin ? Inv : 0xFF00FFFF; }   /* 0,255,255 */
    "darkblue"  { return *yyin ? Inv : 0xFF00008B; }   /* 0,0,139 */
    "darkcyan"  { return *yyin ? Inv : 0xFF008B8B; }   /* 0,139,139 */
    "darkgoldenrod" { return *yyin ? Inv : 0xFFB8860B; }   /* 184,134,11 */
    "darkgray"  { return *yyin ? Inv : 0xFFA9A9A9; }   /* 169,169,169 */
    "darkgreen" { return *yyin ? Inv : 0xFF006400; }   /* 0,100,0 */
    "darkgrey"  { return *yyin ? Inv : 0xFFA9A9A9; }   /* 169,169,169 */
    "darkkhaki" { return *yyin ? Inv : 0xFFBDB76B; }   /* 189,183,107 */
    "darkmagenta"   { return *yyin ? Inv : 0xFF8B008B; }   /* 139,0,139 */
    "darkolivegreen"    { return *yyin ? Inv : 0xFF556B2F; }   /* 85,107,47 */
    "darkorange"    { return *yyin ? Inv : 0xFFFF8C00; }   /* 255,140,0 */
    "darkorchid"    { return *yyin ? Inv : 0xFF9932CC; }   /* 153,50,204 */
    "darkred"   { return *yyin ? Inv : 0xFF8B0000; }   /* 139,0,0 */
    "darksalmon"    { return *yyin ? Inv : 0xFFE9967A; }   /* 233,150,122 */
    "darkseagreen"  { return *yyin ? Inv : 0xFF8FBC8F; }   /* 143,188,143 */
    "darkslateblue" { return *yyin ? Inv : 0xFF483D8B; }   /* 72,61,139 */
    "darkslategray" { return *yyin ? Inv : 0xFF2F4F4F; }   /* 47,79,79 */
    "darkslategrey" { return *yyin ? Inv : 0xFF2F4F4F; }   /* 47,79,79 */
    "darkturquoise" { return *yyin ? Inv : 0xFF00CED1; }   /* 0,206,209 */
    "darkviolet"    { return *yyin ? Inv : 0xFF9400D3; }   /* 148,0,211 */
    "deeppink"  { return *yyin ? Inv : 0xFFFF1493; }   /* 255,20,147 */
    "deepskyblue"   { return *yyin ? Inv : 0xFF00BFFF; }   /* 0,191,255 */
    "dimgray"   { return *yyin ? Inv : 0xFF696969; }   /* 105,105,105 */
    "dimgrey"   { return *yyin ? Inv : 0xFF696969; }   /* 105,105,105 */
    "dodgerblue"    { return *yyin ? Inv : 0xFF1E90FF; }   /* 30,144,255 */
    "firebrick" { return *yyin ? Inv : 0xFFB22222; }   /* 178,34,34 */
    "floralwhite"   { return *yyin ? Inv : 0xFFFFFAF0; }   /* 255,250,240 */
    "forestgreen"   { return *yyin ? Inv : 0xFF228B22; }   /* 34,139,34 */
    "fuchsia"   { return *yyin ? Inv : 0xFFFF00FF; }   /* 255,0,255 */
    "gainsboro" { return *yyin ? Inv : 0xFFDCDCDC; }   /* 220,220,220 */
    "ghostwhite"    { return *yyin ? Inv : 0xFFF8F8FF; }   /* 248,248,255 */
    "gold"  { return *yyin ? Inv : 0xFFFFD700; }   /* 255,215,0 */
    "goldenrod" { return *yyin ? Inv : 0xFFDAA520; }   /* 218,165,32 */
    "gray"  { return *yyin ? Inv : 0xFF808080; }   /* 128,128,128 */
    "green" { return *yyin ? Inv : 0xFF008000; }   /* 0,128,0 */
    "greenyellow"   { return *yyin ? Inv : 0xFFADFF2F; }   /* 173,255,47 */
    "grey"  { return *yyin ? Inv : 0xFF808080; }   /* 128,128,128 */
    "honeydew"  { return *yyin ? Inv : 0xFFF0FFF0; }   /* 240,255,240 */
    "hotpink"   { return *yyin ? Inv : 0xFFFF69B4; }   /* 255,105,180 */
    "indianred" { return *yyin ? Inv : 0xFFCD5C5C; }   /* 205,92,92 */
    "indigo"    { return *yyin ? Inv : 0xFF4B0082; }   /* 75,0,130 */
    "ivory" { return *yyin ? Inv : 0xFFFFFFF0; }   /* 255,255,240 */
    "khaki" { return *yyin ? Inv : 0xFFF0E68C; }   /* 240,230,140 */
    "lavender"  { return *yyin ? Inv : 0xFFE6E6FA; }   /* 230,230,250 */
    "lavenderblush" { return *yyin ? Inv : 0xFFFFF0F5; }   /* 255,240,245 */
    "lawngreen" { return *yyin ? Inv : 0xFF7CFC00; }   /* 124,252,0 */
    "lemonchiffon"  { return *yyin ? Inv : 0xFFFFFACD; }   /* 255,250,205 */
    "lightblue" { return *yyin ? Inv : 0xFFADD8E6; }   /* 173,216,230 */
    "lightcoral"    { return *yyin ? Inv : 0xFFF08080; }   /* 240,128,128 */
    "lightcyan" { return *yyin ? Inv : 0xFFE0FFFF; }   /* 224,255,255 */
    "lightgoldenrodyellow"  { return *yyin ? Inv : 0xFFFAFAD2; }   /* 250,250,210 */
    "lightgray" { return *yyin ? Inv : 0xFFD3D3D3; }   /* 211,211,211 */
    "lightgreen"    { return *yyin ? Inv : 0xFF90EE90; }   /* 144,238,144 */
    "lightgrey" { return *yyin ? Inv : 0xFFD3D3D3; }   /* 211,211,211 */
    "lightpink" { return *yyin ? Inv : 0xFFFFB6C1; }   /* 255,182,193 */
    "lightsalmon"   { return *yyin ? Inv : 0xFFFFA07A; }   /* 255,160,122 */
    "lightseagreen" { return *yyin ? Inv : 0xFF20B2AA; }   /* 32,178,170 */
    "lightskyblue"  { return *yyin ? Inv : 0xFF87CEFA; }   /* 135,206,250 */
    "lightslategray"    { return *yyin ? Inv : 0xFF778899; }   /* 119,136,153 */
    "lightslategrey"    { return *yyin ? Inv : 0xFF778899; }   /* 119,136,153 */
    "lightsteelblue"    { return *yyin ? Inv : 0xFFB0C4DE; }   /* 176,196,222 */
    "lightyellow"   { return *yyin ? Inv : 0xFFFFFFE0; }   /* 255,255,224 */
    "lime"  { return *yyin ? Inv : 0xFF00FF00; }   /* 0,255,0 */
    "limegreen" { return *yyin ? Inv : 0xFF32CD32; }   /* 50,205,50 */
    "linen" { return *yyin ? Inv : 0xFFFAF0E6; }   /* 250,240,230 */
    "magenta"   { return *yyin ? Inv : 0xFFFF00FF; }   /* 255,0,255 */
    "maroon"    { return *yyin ? Inv : 0xFF800000; }   /* 128,0,0 */
    "mediumaquamarine"  { return *yyin ? Inv : 0xFF66CDAA; }   /* 102,205,170 */
    "mediumblue"    { return *yyin ? Inv : 0xFF0000CD; }   /* 0,0,205 */
    "mediumorchid"  { return *yyin ? Inv : 0xFFBA55D3; }   /* 186,85,211 */
    "mediumpurple"  { return *yyin ? Inv : 0xFF9370DB; }   /* 147,112,219 */
    "mediumseagreen"    { return *yyin ? Inv : 0xFF3CB371; }   /* 60,179,113 */
    "mediumslateblue"   { return *yyin ? Inv : 0xFF7B68EE; }   /* 123,104,238 */
    "mediumspringgreen" { return *yyin ? Inv : 0xFF00FA9A; }   /* 0,250,154 */
    "mediumturquoise"   { return *yyin ? Inv : 0xFF48D1CC; }   /* 72,209,204 */
    "mediumvioletred"   { return *yyin ? Inv : 0xFFC71585; }   /* 199,21,133 */
    "midnightblue"  { return *yyin ? Inv : 0xFF191970; }   /* 25,25,112 */
    "mintcream" { return *yyin ? Inv : 0xFFF5FFFA; }   /* 245,255,250 */
    "mistyrose" { return *yyin ? Inv : 0xFFFFE4E1; }   /* 255,228,225 */
    "moccasin"  { return *yyin ? Inv : 0xFFFFE4B5; }   /* 255,228,181 */
    "navajowhite"   { return *yyin ? Inv : 0xFFFFDEAD; }   /* 255,222,173 */
    "navy"  { return *yyin ? Inv : 0xFF000080; }   /* 0,0,128 */
    "oldlace"   { return *yyin ? Inv : 0xFFFDF5E6; }   /* 253,245,230 */
    "olive" { return *yyin ? Inv : 0xFF808000; }   /* 128,128,0 */
    "olivedrab" { return *yyin ? Inv : 0xFF6B8E23; }   /* 107,142,35 */
    "orange"    { return *yyin ? Inv : 0xFFFFA500; }   /* 255,165,0 */
    "orangered" { return *yyin ? Inv : 0xFFFF4500; }   /* 255,69,0 */
    "orchid"    { return *yyin ? Inv : 0xFFDA70D6; }   /* 218,112,214 */
    "palegoldenrod" { return *yyin ? Inv : 0xFFEEE8AA; }   /* 238,232,170 */
    "palegreen" { return *yyin ? Inv : 0xFF98FB98; }   /* 152,251,152 */
    "paleturquoise" { return *yyin ? Inv : 0xFFAFEEEE; }   /* 175,238,238 */
    "palevioletred" { return *yyin ? Inv : 0xFFDB7093; }   /* 219,112,147 */
    "papayawhip"    { return *yyin ? Inv : 0xFFFFEFD5; }   /* 255,239,213 */
    "peachpuff" { return *yyin ? Inv : 0xFFFFDAB9; }   /* 255,218,185 */
    "peru"  { return *yyin ? Inv : 0xFFCD853F; }   /* 205,133,63 */
    "pink"  { return *yyin ? Inv : 0xFFFFC0CB; }   /* 255,192,203 */
    "plum"  { return *yyin ? Inv : 0xFFDDA0DD; }   /* 221,160,221 */
    "powderblue"    { return *yyin ? Inv : 0xFFB0E0E6; }   /* 176,224,230 */
    "purple"    { return *yyin ? Inv : 0xFF800080; }   /* 128,0,128 */
    "red"   { return *yyin ? Inv : 0xFFFF0000; }   /* 255,0,0 */
    "rosybrown" { return *yyin ? Inv : 0xFFBC8F8F; }   /* 188,143,143 */
    "royalblue" { return *yyin ? Inv : 0xFF4169E1; }   /* 65,105,225 */
    "saddlebrown"   { return *yyin ? Inv : 0xFF8B4513; }   /* 139,69,19 */
    "salmon"    { return *yyin ? Inv : 0xFFFA8072; }   /* 250,128,114 */
    "sandybrown"    { return *yyin ? Inv : 0xFFF4A460; }   /* 244,164,96 */
    "seagreen"  { return *yyin ? Inv : 0xFF2E8B57; }   /* 46,139,87 */
    "seashell"  { return *yyin ? Inv : 0xFFFFF5EE; }   /* 255,245,238 */
    "sienna"    { return *yyin ? Inv : 0xFFA0522D; }   /* 160,82,45 */
    "silver"    { return *yyin ? Inv : 0xFFC0C0C0; }   /* 192,192,192 */
    "skyblue"   { return *yyin ? Inv : 0xFF87CEEB; }   /* 135,206,235 */
    "slateblue" { return *yyin ? Inv : 0xFF6A5ACD; }   /* 106,90,205 */
    "slategray" { return *yyin ? Inv : 0xFF708090; }   /* 112,128,144 */
    "slategrey" { return *yyin ? Inv : 0xFF708090; }   /* 112,128,144 */
    "snow"  { return *yyin ? Inv : 0xFFFFFAFA; }   /* 255,250,250 */
    "springgreen"   { return *yyin ? Inv : 0xFF00FF7F; }   /* 0,255,127 */
    "steelblue" { return *yyin ? Inv : 0xFF4682B4; }   /* 70,130,180 */
    "tan"   { return *yyin ? Inv : 0xFFD2B48C; }   /* 210,180,140 */
    "teal"  { return *yyin ? Inv : 0xFF008080; }   /* 0,128,128 */
    "thistle"   { return *yyin ? Inv : 0xFFD8BFD8; }   /* 216,191,216 */
    "tomato"    { return *yyin ? Inv : 0xFFFF6347; }   /* 255,99,71 */
    "turquoise" { return *yyin ? Inv : 0xFF40E0D0; }   /* 64,224,208 */
    "violet"    { return *yyin ? Inv : 0xFFEE82EE; }   /* 238,130,238 */
    "wheat" { return *yyin ? Inv : 0xFFF5DEB3; }   /* 245,222,179 */
    "white" { return *yyin ? Inv : 0xFFFFFFFF; }   /* 255,255,255 */
    "whitesmoke"    { return *yyin ? Inv : 0xFFF5F5F5; }   /* 245,245,245 */
    "yellow"    { return *yyin ? Inv : 0xFFFFFF00; }   /* 255,255,0 */
    "yellowgreen"   { return *yyin ? Inv : 0xFF9ACD32; }   /* 154,205,50 */
    [^] { return Inv; }

*/

}

}}}}  // org::w3c::dom::bootstrap
