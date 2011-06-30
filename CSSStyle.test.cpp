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

#include "css/CSSSelector.h"
#include "css/CSSInputStream.h"

#include <assert.h>

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#include <org/w3c/dom/css/CSSMediaRule.h>
#include <org/w3c/dom/css/CSSStyleRule.h>
#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/stylesheets/MediaList.h>
#include <org/w3c/dom/html/HTMLElement.h>

#include "html/HTMLParser.h"
#include "DOMImplementationImp.h"
#include "DocumentImp.h"

#include "html/HTMLStyleElementImp.h"

#include "css/CSSStyleRuleImp.h"
#include "css/ViewCSSImp.h"

#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

const char* htmlDocument =
    "<html>"
    "<head>"
    "<style>"
    "p { color: red }"
    "p b { color: blue }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>h1</h1>"
    "<p>Hello <b>world</b>.</p>"
    "<p>Hello <i>world</i>.</p>"
    "<b style='color: rgb(1, 2, 3); foo : hoge'>foo</b>"
    "<p style='font-family: Helvetica, New Century Schoolbook, sans-serif'>foo</p>"
    "<p style='text-decoration: blink underline'>foo</p>"
    "<p style='border-top-width: thin'>foo</p>"
    "<p style='border-width: thick'>foo</p>"
    "<p style='border-top-style: dashed'>foo</p>"
    "<p style='border-style: dashed; color: transparent'>foo</p>"
    "<p style='border-top-color: blue'>foo</p>"
    "<p style='border-color: red rgb(1,2,3) green'>foo</p>"
    "<p style='direction: ltr'>foo</p>"
    "<p style='background-position: bottom center; background-image: url(\"redball.png\")'>bp</p>"
    "<p style='background: 10% 24% url(\"redball.png\") fixed no-repeat'>background</p>"
    "<address>address</address>"
    "<blockquote>blockquote</blockquote>"
    "<center>center</center>"
    "<hr>hr</hr>"
    "<kbd>kbd</kbd>"
    "<ol><li>1</li><li>2</li><li>3</li></ol>"
    "<pre>pre</pre>"
    "<strike>strike</strike>"
    "<strong>strong</strong>"
    "<sub>sub</sub>"
    "<table>table</table>"
    "<div style='clear: both'>clear</div>"
    "<div style='float: left'>float</div>"
    "<div style='visibility: hidden'>clear</div>"
    "<div style='overflow: scroll'>float</div>"
    "<div style='min-width: 30px; min-height: 40px; text-indent: 16%'>percentage or length</div>"
    "<div style='max-width: 30px; max-height: none'>none, percentage or length</div>"
    "<div style='padding-top: 1px; padding-right: 2px; padding-bottom: 3px; padding-left: 4px'>padding</div>"
    "<div style='padding: 1px 5px'>padding</div>"
    "<div style='border-collapse: separate'>border-collapse</div>"
    "<div style='z-index: auto'>z-index</div>"
    "<div style='z-index: -1'>z-index</div>"
    "<div style='line-height: normal'>line-height</div>"
    "<div style='line-height: 10px'>line-height</div>"
    "<div style='line-height: 140%'>line-height</div>"
    "</body>"
    "</html>";

int main(int argc, char** argv)
{
    css::CSSStyleSheet defaultStyleSheet(0);
    Document document(0);

    // Load the default CSS file
    if (1 < argc) {
        std::ifstream stream(argv[1]);
        if (!stream) {
            std::cerr << "error: cannot open " << argv[1] << ".\n";
            return EXIT_FAILURE;
        }
        defaultStyleSheet = loadDefaultSheet(stream);
        getDOMImplementation()->setDefaultCSSStyleSheet(defaultStyleSheet);
    }

    document = loadDocument(htmlDocument);
    assert(document);

    // Each HTML element will have a style attribute if there's the style content attribute.
    // Each HTML style element will have a style sheet.
    eval(document);

    // create the default view
    ViewCSSImp* view = new ViewCSSImp(document, defaultStyleSheet);
    view->cascade();
    printComputedValues(document, view);

    std::cout << "done.\n";
    return 0;
}
