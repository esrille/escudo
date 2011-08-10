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

#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Text.h>

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

#include "css/CSSSerialize.h"
#include "css/CSSStyleRuleImp.h"
#include "css/ViewCSSImp.h"

#include "css/Box.h"

#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

const char* htmlDocument =
    "<html>"
    "<head>"
    "<style>"
    "h3 { display: run-in }"
    "</style>"
    "</head>"
    "<body>"
    "<p style='display: inline'>"
    "This is anonymous text before the SPAN."
    "<span style='display: block'>This is the content of SPAN.</span>"
    "This is anonymous text after the SPAN."
    "</p>"
    "<p>Some <em>emphasized</em> text</p>"
    "<h3>A run-in heading.</h3>"
    "<p>And a paragraph of text that"
    "   follows it."
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

    dumpTree(std::cout, document);

    // create the default view
    DocumentWindowPtr window = new(std::nothrow) DocumentWindow;
    window->setDocument(document);
    ViewCSSImp* view = new ViewCSSImp(window, defaultStyleSheet);
    view->cascade();
    view->setSize(8.5f * 96, 11.0f * 96);  // US letter size, 96 DPI

    Box* boxTree = view->layOutBlockBoxes();
    if (!boxTree)
        return EXIT_FAILURE;

    boxTree->dump(view);

    std::cout << "done.\n";
    return 0;
}
