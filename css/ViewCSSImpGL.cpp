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

#include "ViewCSSImp.h"

#include <org/w3c/dom/Text.h>
#include <org/w3c/dom/Comment.h>

#include <new>

#include "CSSStyleRuleImp.h"
#include "CSSStyleDeclarationImp.h"
#include "DocumentImp.h"

#include "Box.h"

#include "font/FontDatabase.h"
#include "font/FontManager.h"
#include "font/FontManagerBackEndGL.h"

namespace {

FontManagerBackEndGL backend;

const int Point = 24;

}

namespace org { namespace w3c { namespace dom { namespace bootstrap {

FontTexture* ViewCSSImp::selectFont(CSSStyleDeclarationImp* style)
{
    FontFileInfo* info = FontFileInfo::chooseFont(style);
    backend.getFontFace(info->filename);
    return backend.getFontTexture(Point);
}

}}}}  // org::w3c::dom::bootstrap
