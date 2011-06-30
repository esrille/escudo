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

#include "FontDatabase.h"

#include <iostream>

#include "css/CSSStyleDeclarationImp.h"

using namespace org::w3c::dom::bootstrap;

int main()
{
    CSSStyleDeclarationImp style;
    style.fontFamily.setGeneric(CSSFontFamilyValueImp::Serif);
    style.fontStyle.setValue(CSSFontStyleValueImp::Normal);
    style.fontWeight.setValue(400);

    FontFileInfo* info;

    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';

    style.fontWeight.setValue(900);
    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';

    style.fontStyle.setValue(CSSFontStyleValueImp::Italic);
    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';

    style.fontWeight.setValue(100);
    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';

    style.fontFamily.addFamily(u"LiberationSans");
    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';

    style.fontFamily.reset();
    style.fontFamily.addFamily(u"Lucida Console");
    style.fontFamily.setGeneric(CSSFontFamilyValueImp::Monospace);
    info = FontFileInfo::chooseFont(&style);
    std::cout << info->filename << '\n';
}