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

#ifndef ES_FONT_DATABASE_H
#define ES_FONT_DATABASE_H

class FontTexture;

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CSSStyleDeclarationImp;

struct FontFileInfo
{
    const char* filename;
    unsigned weight;
    unsigned style;

    static FontFileInfo* chooseFont(const CSSStyleDeclarationImp* style);
    static FontFileInfo* chooseAltFont(const CSSStyleDeclarationImp* style, FontTexture* current, char32_t u);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ES_FONT_DATABASE_H
