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

#include "css/CSSStyleDeclarationImp.h"

// TODO: support dynamic font configuration later...

namespace org { namespace w3c { namespace dom { namespace bootstrap {

struct FontInfo
{
    static const int MaxInfo = 4;

    const char16_t* family;
    unsigned generic;
    FontFileInfo fileInfo[MaxInfo];

    FontFileInfo* chooseFontFileInfo(unsigned style, unsigned weight) {
        FontFileInfo* chosen = fileInfo;
        unsigned score = 0;
        for (FontFileInfo* i = fileInfo; i < &fileInfo[MaxInfo]; ++i) {
            if (i->filename == 0)
                break;
            unsigned newScore = 0;
            if (i->style == style)
                newScore += 200;
            else if (style == CSSFontStyleValueImp::Italic && i->style == CSSFontStyleValueImp::Oblique)
                newScore += 100;
            if (i->weight == weight)
                newScore += 10;
            else if (weight <= 400) {
                if (weight == 400 && i->weight == 500)
                    newScore += 9;
                else if (i->weight < weight)
                    newScore += 9 - (weight - i->weight) / 100;
                else
                    newScore += (1000 - i->weight) / 100;
            } else if (500 <= weight) {
                if (weight == 500 && i->weight == 400)
                    newScore += 9;
                else if (weight < i->weight)
                    newScore += 9 - (i->weight - weight) / 100;
                else
                    newScore += i->weight / 100;
            }
            if (score < newScore) {
                chosen = i;
                score = newScore;
            }
        }
        return chosen;
    }
};

namespace {

FontInfo fontDatabase[] = {
#ifdef HAVE_IPA_PGOTHIC
    {
        u"IPAPGothic",
        CSSFontFamilyValueImp::SansSerif,
        {
            { HAVE_IPA_PGOTHIC, 400 }
        }
    },
#endif
#ifdef HAVE_IPA_PMINCHO
    {
        u"IPAPMincho",
        CSSFontFamilyValueImp::Serif,
        {
            { HAVE_IPA_PMINCHO, 400 }
        }
    },
#endif
#ifdef HAVE_IPA_GOTHIC
    {
        u"IPAGothic",
        CSSFontFamilyValueImp::Monospace,
        {
            { HAVE_IPA_GOTHIC, 400 }
        }
    },
#endif
#ifdef HAVE_IPA_MINCHO
    {
        u"IPAMincho",
        CSSFontFamilyValueImp::Monospace,
        {
            { HAVE_IPA_MINCHO, 400 }
        }
    },
#endif
    {
        u"LiberationSans",
        CSSFontFamilyValueImp::SansSerif,
        {
            { LIBERATON_TTF "/LiberationSans-BoldItalic.ttf", 700, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationSans-Italic.ttf", 400, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationSans-Regular.ttf", 400 },
            { LIBERATON_TTF "/LiberationSans-Bold.ttf", 700 },
        }
    },
    {
        u"LiberationSerif",
        CSSFontFamilyValueImp::Serif,
        {
            { LIBERATON_TTF "/LiberationSerif-BoldItalic.ttf", 700, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationSerif-Italic.ttf", 400, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationSerif-Regular.ttf", 400 },
            { LIBERATON_TTF "/LiberationSerif-Bold.ttf", 700 },
        }
    },
    {
        u"LiberationMono",
        CSSFontFamilyValueImp::Monospace,
        {
            { LIBERATON_TTF "/LiberationMono-BoldItalic.ttf", 700, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationMono-Italic.ttf", 400, CSSFontStyleValueImp::Italic },
            { LIBERATON_TTF "/LiberationMono-Regular.ttf", 400 },
            { LIBERATON_TTF "/LiberationMono-Bold.ttf", 700 },
        }
    },
#ifdef HAVE_AHEM
    {
        u"Ahem",
        CSSFontFamilyValueImp::Monospace,
        {
            { HAVE_AHEM, 400 }
        }
    },
#endif
};

const int fontDatabaseSize = sizeof fontDatabase / sizeof fontDatabase[0];

FontInfo* chooseFontInfo(const std::u16string& family)
{
    for (FontInfo* i = fontDatabase; i < &fontDatabase[fontDatabaseSize]; ++i) {
        if (!compareIgnoreCase(family, i->family))
            return i;
    }
    return 0;
}

FontInfo* chooseFontInfo(unsigned generic)
{
    // TODO: remove this switch statement once more fonts are supported
    switch (generic) {
    case CSSFontFamilyValueImp::Cursive:
        generic = CSSFontFamilyValueImp::Serif;
        break;
    case CSSFontFamilyValueImp::Fantasy:
        generic = CSSFontFamilyValueImp::SansSerif;
        break;
    default:
        break;
    }
    for (FontInfo* i = fontDatabase; i < &fontDatabase[fontDatabaseSize]; ++i) {
        if (generic == i->generic)
            return i;
    }
    return fontDatabase;
}

}  // namespace

FontFileInfo* FontFileInfo::chooseFont(const CSSStyleDeclarationImp* style)
{
    FontInfo* fontInfo = 0;
    for (auto i = style->fontFamily.getFamilyNames().begin();
         i != style->fontFamily.getFamilyNames().end();
         ++i) {
            if (fontInfo = chooseFontInfo(*i))
                break;
    }
    if (fontInfo == 0)
        fontInfo = chooseFontInfo(style->fontFamily.getGeneric());

    return fontInfo->chooseFontFileInfo(style->fontStyle.getStyle(), style->fontWeight.getWeight());
}

}}}}  // org::w3c::dom::bootstrap
