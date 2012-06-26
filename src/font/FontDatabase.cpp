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

#include "FontDatabase.h"
#include "FontManager.h"

namespace {

const char* fontList[] =
{
#ifdef LIBERATON_TTF
    // LiberationSans
    LIBERATON_TTF "/LiberationSans-BoldItalic.ttf",
    LIBERATON_TTF "/LiberationSans-Italic.ttf",
    LIBERATON_TTF "/LiberationSans-Regular.ttf",
    LIBERATON_TTF "/LiberationSans-Bold.ttf",
    // LiberationSerif
    LIBERATON_TTF "/LiberationSerif-BoldItalic.ttf",
    LIBERATON_TTF "/LiberationSerif-Italic.ttf",
    LIBERATON_TTF "/LiberationSerif-Regular.ttf",
    LIBERATON_TTF "/LiberationSerif-Bold.ttf",
    // LiberationMono
    LIBERATON_TTF "/LiberationMono-BoldItalic.ttf",
    LIBERATON_TTF "/LiberationMono-Italic.ttf",
    LIBERATON_TTF "/LiberationMono-Regular.ttf",
    LIBERATON_TTF "/LiberationMono-Bold.ttf",
#endif
#ifdef HAVE_IPA_PGOTHIC
    // IPAPGothic
    HAVE_IPA_PGOTHIC,
#endif
#ifdef HAVE_IPA_PMINCHO
    // IPAPMincho
    HAVE_IPA_PMINCHO,
#endif
#ifdef HAVE_IPA_GOTHIC
    // IPAGothic
    HAVE_IPA_GOTHIC,
#endif
#ifdef HAVE_IPA_MINCHO
    // IPAMincho
    HAVE_IPA_MINCHO,
#endif
#ifdef DEJAVU_TTF
    DEJAVU_TTF "/DejaVuSans-Bold.ttf",
    DEJAVU_TTF "/DejaVuSans-BoldOblique.ttf",
    DEJAVU_TTF "/DejaVuSans-ExtraLight.ttf",
    DEJAVU_TTF "/DejaVuSans-Oblique.ttf",
    DEJAVU_TTF "/DejaVuSans.ttf",
    DEJAVU_TTF "/DejaVuSansMono-Bold.ttf",
    DEJAVU_TTF "/DejaVuSansMono-BoldOblique.ttf",
    DEJAVU_TTF "/DejaVuSansMono-Oblique.ttf",
    DEJAVU_TTF "/DejaVuSansMono.ttf",
    DEJAVU_TTF "/DejaVuSerif-Bold.ttf",
    DEJAVU_TTF "/DejaVuSerif-BoldItalic.ttf",
    DEJAVU_TTF "/DejaVuSerif-Italic.ttf",
    DEJAVU_TTF "/DejaVuSerif.ttf",
#endif
#ifdef HAVE_AEGEAN
    // Aegean
    HAVE_AEGEAN,
#endif
#ifdef HAVE_AHEM
    // Ahem
    HAVE_AHEM,
#endif
};

// Test fonts for CSS 2.1 test suite
const char* testFontList[] =
{
#ifdef TEST_FONTS
    // Ahem!
    TEST_FONTS "/AhemExtra/AHEM_Ahem!.TTF",
    // MissingNormal
    TEST_FONTS "/AhemExtra/AHEM_MissingNormal.TTF",
    // SmallCaps
    TEST_FONTS "/AhemExtra/AHEM_SmallCaps.TTF",
    // MissingItalicOblique
    TEST_FONTS "/AhemExtra/AHEM_MissingItalicOblique.TTF",
    // White Space
    TEST_FONTS "/AhemExtra/AHEM_WhiteSpace.TTF",
    // cursive
    TEST_FONTS "/AhemExtra/AHEM_cursive.TTF",
    // default
    TEST_FONTS "/AhemExtra/AHEM_default.TTF",
    // fantasy
    TEST_FONTS "/AhemExtra/AHEM_fantasy.TTF",
    // inherit
    TEST_FONTS "/AhemExtra/AHEM_inherit.TTF",
    // initial
    TEST_FONTS "/AhemExtra/AHEM_initial.TTF",
    // monospace
    TEST_FONTS "/AhemExtra/AHEM_monospace.TTF",
    // serif
    TEST_FONTS "/AhemExtra/AHEM_serif.TTF",
    // sans-serif
    TEST_FONTS "/AhemExtra/AHEM_sans-serif.TTF",
    // CSSTest ASCII
    TEST_FONTS "/CSSTest/csstest-ascii.ttf",
    // CSSTest Basic
    TEST_FONTS "/CSSTest/csstest-basic-bold.ttf",
    TEST_FONTS "/CSSTest/csstest-basic-bolditalic.ttf",
    TEST_FONTS "/CSSTest/csstest-basic-italic.ttf",
    TEST_FONTS "/CSSTest/csstest-basic-regular.ttf",
    // CSSTest Fallback
    TEST_FONTS "/CSSTest/csstest-fallback.ttf",
    // small-caps 1in CSSTest FamilyName Funky
    TEST_FONTS "/CSSTest/csstest-familyname-funkyA.ttf",
    // x-large CSSTest FamilyName Funky
    TEST_FONTS "/CSSTest/csstest-familyname-funkyB.ttf",
    // 12px CSSTest FamilyName Funky
    TEST_FONTS "/CSSTest/csstest-familyname-funkyC.ttf",
    // CSSTest FamilyName
    TEST_FONTS "/CSSTest/csstest-familyname.ttf",
    TEST_FONTS "/CSSTest/csstest-familyname-bold.ttf",
    // CSSTest Verify
    TEST_FONTS "/CSSTest/csstest-verify.ttf",
    // CSSTest Weights
    TEST_FONTS "/CSSTest/csstest-weights-100.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-1479-w1.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-1479-w4.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-1479-w7.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-1479-w9.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-15-w1.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-15-w5.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-200.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-24-w2.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-24-w4.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-2569-w2.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-2569-w5.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-2569-w6.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-2569-w9.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-258-w2.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-258-w5.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-258-w8.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-300.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-3589-w3.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-3589-w5.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-3589-w8.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-3589-w9.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-400.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-47-w4.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-47-w7.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-500.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-600.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-700.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-800.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-900.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w1.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w2.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w3.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w4.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w5.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w6.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w7.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w8.ttf",
    TEST_FONTS "/CSSTest/csstest-weights-full-w9.ttf",
    TEST_FONTS "/CSSTest/csstest-weights.ttf",
#endif  // TEST_FONTS
};

}

void FontDatabase::loadBaseFonts(FontManager* manager)
{
    for (auto i = fontList; i < &fontList[sizeof fontList / sizeof fontList[0]]; ++i) {
        try {
            manager->loadFont(*i);
        } catch (...) {
        }
    }
}

void FontDatabase::loadTestFonts(FontManager* manager)
{
    for (auto i = testFontList; i < &testFontList[sizeof testFontList / sizeof testFontList[0]]; ++i) {
        try {
            manager->loadFont(*i);
        } catch (...) {
        }
    }
}
