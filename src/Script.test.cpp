/*
 * Copyright 2011-2013 Esrille Inc.
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

#include <thread>
#include <GL/freeglut.h>

#include "DOMImplementationImp.h"
#include "ECMAScript.h"
#include "WindowImp.h"
#include "font/FontDatabase.h"
#include "http/HTTPConnection.h"

#include "Test.util.h"

#ifdef USE_V8
#include "v8/ScriptV8.h"
#endif  // USE_V8

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

extern html::Window window;

int main(int argc, char* argv[])
{
#ifdef USE_V8
    v8::HandleScope handleScope;
#endif  // USE_V8

    if (argc < 3) {
        std::cout << "usage : " << argv[0] << " default.css [user.css] url\n";
        return EXIT_FAILURE;
    }

    init(&argc, argv, 816, 1056);
    initLogLevel(&argc, argv);
    initFonts(&argc, argv);

    const char* presHints = 0;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--preshints", 11) == 0) {
            if (argv[i][11] == '=')
                presHints = argv[i] + 12;
            for (int j = i; j < argc; ++j)
                argv[j] = argv[j + 1];
            --argc;
        }
    }
    if (presHints)
        getDOMImplementation()->setPresentationalHints(loadStyleSheet(presHints));

    // Load the default CSS file
    getDOMImplementation()->setDefaultStyleSheet(loadStyleSheet(argv[1]));

    // Load the user CSS file
    if (4 <= argc)
        getDOMImplementation()->setUserStyleSheet(loadStyleSheet(argv[2]));

    std::thread httpService(std::ref(HttpConnectionManager::getInstance()));

    window = new WindowImp(0, 0, WindowImp::TopLevel);
    window.open(utfconv(argv[argc - 1]), u"_self", u"", true);

    glutMainLoop();

    window = 0;

    ECMAScriptContext::shutDown();

    HttpConnectionManager::getInstance().stop();
    httpService.join();
}
