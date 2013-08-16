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

int MediaListImp::getFeatureID(const std::u16string& feature)
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

    'width'	{ return *yyin ? Unknown : Width; }
    'min-width'	{ return *yyin ? Unknown : MinWidth; }
    'max-width'	{ return *yyin ? Unknown : MaxWidth; }
    'height'	{ return *yyin ? Unknown : Height; }
    'min-height'	{ return *yyin ? Unknown : MinHeight; }
    'max-height'	{ return *yyin ? Unknown : MaxHeight; }
    'device-width'	{ return *yyin ? Unknown : DeviceWidth; }
    'min-device-width'	{ return *yyin ? Unknown : MinDeviceWidth; }
    'max-device-width'	{ return *yyin ? Unknown : MaxDeviceWidth; }
    'device-height'	{ return *yyin ? Unknown : DeviceHeight; }
    'min-device-height'	{ return *yyin ? Unknown : MinDeviceHeight; }
    'max-device-height'	{ return *yyin ? Unknown : MaxDeviceHeight; }
    'orientation'	{ return *yyin ? Unknown : Orientation; }
    'aspect-ratio'	{ return *yyin ? Unknown : AspectRatio; }
    'min-aspect-ratio'	{ return *yyin ? Unknown : MinAspectRatio; }
    'max-aspect-ratio'	{ return *yyin ? Unknown : MaxAspectRatio; }
    'device-aspect-ratio'	{ return *yyin ? Unknown : DeviceAspectRatio; }
    'min-device-aspect-ratio'	{ return *yyin ? Unknown : MinDeviceAspectRatio; }
    'max-device-aspect-ratio'	{ return *yyin ? Unknown : MaxDeviceAspectRatio; }
    'color'	{ return *yyin ? Unknown : Color; }
    'min-color'	{ return *yyin ? Unknown : MinColor; }
    'max-color'	{ return *yyin ? Unknown : MaxColor; }
    'color-index'	{ return *yyin ? Unknown : ColorIndex; }
    'min-color-index'	{ return *yyin ? Unknown : MinColorIndex; }
    'max-color-index'	{ return *yyin ? Unknown : MaxColorIndex; }
    'monochrome'	{ return *yyin ? Unknown : Monochrome; }
    'min-monochrome'	{ return *yyin ? Unknown : MinMonochrome; }
    'max-monochrome'	{ return *yyin ? Unknown : MaxMonochrome; }
    'resolution'	{ return *yyin ? Unknown : Resolution; }
    'min-resolution'	{ return *yyin ? Unknown : MinResolution; }
    'max-resolution'	{ return *yyin ? Unknown : MaxResolution; }
    'scan'	{ return *yyin ? Unknown : Scan; }
    'grid'	{ return *yyin ? Unknown : Grid; }
    [^] { return Unknown;}

*/

}

}}}}  // org::w3c::dom::bootstrap
