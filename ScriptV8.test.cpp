/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include "v8/esv8api.h"
#include "v8/ScriptV8.h"

#include "DOMImplementationImp.h"
#include "WindowImp.h"
#include "font/FontDatabase.h"

#include "Test.util.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

html::Window window(0);

int main(int argc, char* argv[])
{
    v8::HandleScope handleScope;

    if (argc < 3) {
        std::cout << "usage : " << argv[0] << " default.css [user.css] url\n";
        return EXIT_FAILURE;
    }

    init(&argc, argv);
    initLogLevel(&argc, argv);
    initFonts(&argc, argv);

    // Load the default CSS file
    getDOMImplementation()->setDefaultCSSStyleSheet(loadStyleSheet(argv[1]));

    // Load the user CSS file
    if (4 <= argc)
        getDOMImplementation()->setUserCSSStyleSheet(loadStyleSheet(argv[2]));

    window = new WindowImp();
    window.open(utfconv(argv[argc - 1]), u"_self", u"", true);

    glutMainLoop();

    window = 0;

    ECMAScriptContext::shutDown();
}
