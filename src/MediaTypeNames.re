/*
 * Copyright 2013 Esrille Inc.
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

#include "MediaListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

unsigned MediaListImp::getMediaTypeID(const std::u16string& feature)
{
    const char16_t* yyin = feature.c_str();
    const char16_t* yymarker = yyin;
    const char16_t* yylimit = yyin + feature.length();

/*!re2c

    re2c:define:YYCTYPE  = "char16_t";
    re2c:define:YYCURSOR = yyin;
    re2c:define:YYMARKER = yymarker;
    re2c:define:YYLIMIT = yylimit;
    re2c:yyfill:enable   = 0;
    re2c:indent:top      = 1;
    re2c:indent:string = "    " ;

    'all'	{ return *yyin ? 0 : All; }
    'braille'	{ return *yyin ? 0 : Braille; }
    'embossed'	{ return *yyin ? 0 : Embossed; }
    'handheld'	{ return *yyin ? 0 : Handheld; }
    'print'	{ return *yyin ? 0 : Print; }
    'projection'	{ return *yyin ? 0 : Projection; }
    'screen'	{ return *yyin ? 0 : Screen; }
    'speech'	{ return *yyin ? 0 : Speech; }
    'tty'	{ return *yyin ? 0 : Tty; }
    'tv'	{ return *yyin ? 0 : Tv; }
    [^] { return 0;}

*/

}

}}}}  // org::w3c::dom::bootstrap
