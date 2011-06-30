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

#include <GL/freeglut.h>

#include "DOMImplementationImp.h"
#include "DocumentImp.h"
#include "html/HTMLParser.h"
#include "css/ViewCSSImp.h"

#include "NodeImp.h"  // for eval()

#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

html::Window window(0);

ViewCSSImp* view;
Box* boxTree;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);

    if (boxTree)
        boxTree->render(view);

    // TODO: do GC here or maybe in the idle proc

    glutSwapBuffers();  // This would block until the sync happens
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "usage : " << argv[0] << " default.css html_file\n";
        return EXIT_FAILURE;
    }

    init(argc, argv);

    Document document(0);

    // Load the default CSS file
    std::ifstream styleStream(argv[1]);
    if (!styleStream) {
        std::cerr << "error: cannot open " << argv[1] << ".\n";
        return EXIT_FAILURE;
    }
    getDOMImplementation()->setDefaultCSSStyleSheet(loadDefaultSheet(styleStream));

    std::ifstream htmlStream(argv[2]);
    if (!htmlStream) {
        std::cerr << "error: cannot open " << argv[2] << ".\n";
        return EXIT_FAILURE;
    }
    document = loadDocument(htmlStream);
    assert(document);

    // Each HTML element will have a style attribute if there's the style content attribute.
    // Each HTML style element will have a style sheet.
    eval(document);

    dumpTree(std::cout, document);

    // create the default view
    view = new ViewCSSImp(document, getDOMImplementation()->getDefaultCSSStyleSheet());
    view->cascade();

    // printComputedValues(document, view);
    // std::cout << '\n';

    view->setSize(8.5f * 96, 11.0f * 96);  // US letter size, 96 DPI

    boxTree = view->layOut();
    if (!boxTree)
        return EXIT_FAILURE;
    view->dump();

    glutMainLoop();

    std::cout << "done.\n";
    return 0;
}
