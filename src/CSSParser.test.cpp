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

#include "css/CSSSelector.h"
#include "css/CSSInputStream.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <org/w3c/dom/css/CSSMediaRule.h>
#include <org/w3c/dom/css/CSSStyleRule.h>
#include <org/w3c/dom/stylesheets/MediaList.h>

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

void test(std::istream& stream)
{
    CSSParser parser;
    CSSInputStream cssStream(stream, "utf-8");
    std::u16string cssText = cssStream;

    css::CSSStyleSheet styleSheet = parser.parse(0, cssText);
    if (!styleSheet)
        return;
    css::CSSRuleList list = styleSheet.getCssRules();
    if (!list)
        return;
    for (unsigned int i = 0; i < list.getLength(); ++i) {
        css::CSSRule rule = list.getElement(i);
        switch (rule.getType()) {
        case css::CSSRule::STYLE_RULE: {
            css::CSSStyleRule styleRule = interface_cast<css::CSSStyleRule>(rule);
            std::cout << styleRule.getSelectorText() << " {";
            if (css::CSSStyleDeclaration style = styleRule.getStyle())
                std::cout << ' ' << style.getCssText();
            std::cout << " }\n";
            }
            break;
        case css::CSSRule::MEDIA_RULE: {
            css::CSSMediaRule mediaRule = interface_cast<css::CSSMediaRule>(rule);
            std::cout << "@media ";
            stylesheets::MediaList media = mediaRule.getMedia();
            std::cout << media.getMediaText();
            std::cout << " {\n";
            css::CSSRuleList list = mediaRule.getCssRules();
            for (unsigned int i = 0; i < list.getLength(); ++i) {
                css::CSSRule rule = list.getElement(i);
                if (rule.getType() == css::CSSRule::STYLE_RULE) {
                    css::CSSStyleRule styleRule = interface_cast<css::CSSStyleRule>(rule);
                    std::cout << styleRule.getSelectorText() << " {";
                    if (css::CSSStyleDeclaration style = styleRule.getStyle())
                        std::cout << ' ' << style.getCssText();
                    std::cout << " }\n";
                }
            }
            std::cout << "}\n";
            }
            break;
        }
    }
}

void test(const char* data)
{
    std::istringstream stream(data);
    return test(stream);
}

int main(int argc, char** argv)
{
    CSSSerializeControl.serializeSpecificity = false;

    // test selectors
    test("h1 { color: red }");
    test("* { color: red }");
    test("E.warning { color: red }");
    test("E#myid { color: red }");
    test("h1, h2, h3 { font-family: sans-serif }");
    test(".warning { color: red }");
    test("#myid { color: red }");
    test("h1[title] { color: red }");
    test("span[class=\"example\"] { color: red }");
    test("span[class~=\"example\"] { color: red }");
    test("span[class|=\"example\"] { color: red }");
    test("a[hreflang=fr] { color: red }");
    test("span[hello=\"Cleveland\"][goodbye=\"Columbus\"] { color: red }");
    test("object[type^=\"image/\"] { color: red }");
    test("a[href$=\".html\"] { color: red }");
    test("p[title*=\"hello\"] { color: red }");
    test("a:link { color: red }");
    test("a:link:hover { color: red }");
    test("tr:nth-child(2n+1) { color: red }");
    test("tr:nth-child(odd) { color: red }");
    test("tr:nth-child(2n+0) { color: red }");
    test("tr:nth-child(even) { color: red }");
    test("tr:nth-child(15) { color: red }");
    test("tr:nth-child( 3n + 1 ) { color: red }");
    test("tr:nth-child( +3n - 2 ) { color: red }");
    test("tr:nth-child( -n+ 6) { color: red }");
    test("tr:nth-child( +6 ) { color: red }");
    test("tr:nth-last-child(-n+2) { color: red }");
    test("tr:nth-of-type(2n+1) { color: red }");
    test("tr:nth-last-of-type(n+2) { color: red }");
    test("tr:nth-last-of-type(0n+20) { color: red }");
    test("p:first-child { color: red }");
    test("div p:first-child { color: red }");
    test("div > p:first-child { color: red }");
    test("div + p:first-child { color: red }");
    test("div ~ p:first-child { color: red }");
    test("button:not([DISABLED]) { color: red }");
    test("*:not(FOO) { color: red }");
    test("html|*:not(:link):not(:visited) { color: red }");
    test("*|*:not(*) { color: red }");
    test("*|*:not(:hover) { color: red }");
    test("p:first-line { color: red }");
    test("p:first-letter { color: red }");

    // test declarations
    test("h1 { volume : 300 }");
    test("h1 { width : 10% }");
    test("h1 { width : 10px }");
    test("h1 { width : 10cm }");
    test("h1 { width : 10mm }");
    test("h1 { width : 10in }");
    test("h1 { width : 10pt }");
    test("h1 { width : 10pc }");
    test("h1 { width : 10em }");
    test("h1 { width : 10ex }");
    test("h1 { azimuth : 10deg }");
    test("h1 { azimuth : 0.4rad }");
    test("h1 { azimuth : 0.3grad }");
    test("h1 { pause-after : 3ms }");
    test("h1 { pause-after : 4s }");
    test("h1 { pitch : 100Hz }");
    test("h1 { pitch : 3kHz }");
    test("h1 { width : -10px }");
    test("h1 { color: #fff }");
    test("body { font-family: \"New Century Schoolbook\" }");
    test("body { font-family: serif }");
    test("body { background-image: url(\"marble.png\") }");
    test("body { background: url(\"http://www.example.com/pinkish.png\") }");
    test("body { background-image: url(  \"marble.png\"   ) }");
    test("body { unicode-range: U+A5; }");
    test("body { unicode-range: U+0-7F; }");
    test("body { unicode-range: U+590-5ff; }");
    test("body { unicode-range: U+A5, U+4E00-9FFF, U+30??, U+FF00-FF9F; }");
    test("div > p:first-child { color: #fff }");
    test("div > p:first-child { color: #1f3f4f }");
    test("body { bogus: bogus(1, 2, id); }");
    test("h1 {"
         "  font-weight: bold;"
         "  font-size: 12px;"
         "  line-height: 14px;"
         "  font-family: Helvetica;"
         "  font-variant: normal;"
         "  font-style: normal"
         "}");
    test("body {"
         "    background-color: #95928d"
         "}"
         "a img {"
         "    border: 0"
         "}");
    test("body {"
         "    background-color: #95928d"
         "}"
         "a img {"
         "    border: 0"
         "}");
    test("h1:before { content: 'Chapter ' }");
    test("h1:before { content: url(test.img) }");
    test("h1:before { content: none }");
    test("h1:before { content: normal }");
    test("h1:before { content: open-quote }");
    test("h1:before { content: attr(x) }");
    test("h1:before { content: counter(chapter) }");
    test("h1:before { content: counters(item, '.') }");
    test("h1:before { content: counters(item, '.', upper-roman) ' ' }");
    test("h1:before { counter-reset: item }");
    test("h1:before { counter-reset: item 10 }");
    test("h1:before { counter-reset: item 10 section }");
    test("h1:before { counter-increment: none }");

    test("html { font: 10px/1 Verdana, sans-serif }");
    test("p { font: 12px/14px sans-serif }");
    test("p { font: 80% sans-serif }");
    test("p { font: x-large/110% \"New Century Schoolbook\", serif }");
    test("p { font: bold italic large Palatino, serif }");
    test("p { font: normal small-caps 120%/120% fantasy }");

    test("p { list-style-position: inside; }");
    test("p { list-style-position: outside; }");
    test("p { list-style: square inside; }");
    test("p { list-style: none; }");
    test("p { list-style: none disc; }");
    test("p { list-style: none url(\"http://www.example.com/pinkish.png\"); }");

    // test @media
    test("@media print {"
         "  h1            { page-break-before: always }"
         "  h1, h2, h3,"
         "  h4, h5, h6    { page-break-after: avoid }"
         "  ul, ol, dl    { page-break-before: avoid }"
         "}");
    test("@media print, tty {"
         "  h1            { page-break-before: always }"
         "  h1, h2, h3,"
         "  h4, h5, h6    { page-break-after: avoid }"
         "  ul, ol, dl    { page-break-before: avoid }"
         "}");

    // check specificity
    CSSSerializeControl.serializeSpecificity = true;
    test("*               /* a=0 b=0 c=0 -> specificity =     0 */ {}");
    test("LI              /* a=0 b=0 c=1 -> specificity =     1 */ {}");
    test("UL LI           /* a=0 b=0 c=2 -> specificity =     2 */ {}");
    test("UL OL+LI        /* a=0 b=0 c=3 -> specificity =     3 */ {}");
    test("H1 + *[REL=up]  /* a=0 b=1 c=1 -> specificity =   101 */ {}");
    test("UL OL LI.red    /* a=0 b=1 c=3 -> specificity =   103 */ {}");
    test("LI.red.level    /* a=0 b=2 c=1 -> specificity =   201 */ {}");
    test("#x34y           /* a=1 b=0 c=0 -> specificity = 10000 */ {}");
    test("#s12:not(FOO)   /* a=1 b=0 c=1 -> specificity = 10001 */ {}");
    CSSSerializeControl.serializeSpecificity = false;

    // test syntax errors
    test("#g0{ color: red; filter: alpha(opacity=100) }");
    test(".g1{ color: red; filter:progid:DXImageTransform.Microsoft.Blur(pixelradius=5) }");
    test(".g2{ color: red; *opacity: 1 }");
    test(".g3{ color: red; -ms-filter: \"progid:DXImageTransform.Microsoft.Blur(pixelradius=5)\" }");
    test(".g4{ color: red; opacity: 1\\0/; }");
    test("#-1ident, .four { color: red; }");

    return 0;
}
