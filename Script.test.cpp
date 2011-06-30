/*
 * Copyright 2011 Esrille Inc.
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

// #define DEBUG 1 // for JS_DumpHeap

#include <GL/freeglut.h>

#include <esjsapi.h>

#include "DOMImplementationImp.h"
#include "WindowImp.h"

#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

html::Window window(0);

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "usage : " << argv[0] << " default.css url\n";
        return EXIT_FAILURE;
    }

    init(argc, argv);

    // Load the default CSS file
    std::ifstream styleStream(argv[1]);
    if (!styleStream) {
        std::cerr << "error: cannot open " << argv[1] << ".\n";
        return EXIT_FAILURE;
    }
    getDOMImplementation()->setDefaultCSSStyleSheet(loadDefaultSheet(styleStream));

    window = new WindowImp();
    window.open(utfconv(argv[2]), u"_self", u"", true);

    glutMainLoop();

    window = 0;

#ifdef DEBUG
    JS_DumpHeap(jscontext, stdout, 0, 0, 0, 32, 0);
#endif

    // JSAPI bridge cleanup
    JS_DestroyContext(jscontext);
    JS_DestroyRuntime(getRuntime());
    JS_ShutDown();

    return 0;
}
