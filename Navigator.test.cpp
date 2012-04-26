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

html::Window window(0);

int main(int argc, char* argv[])
{
#ifdef USE_V8
    v8::HandleScope handleScope;
#endif  // USE_V8

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

    std::thread httpService(std::ref(HttpConnectionManager::getInstance()));

    // Set privileges to the navigator window.
    WindowImp* imp = new WindowImp();
    window = imp;
    imp->enableZoom(false);
    imp->open(utfconv(argv[argc - 1]), u"_self", u"", true);

    glutMainLoop();

    window = 0;

    ECMAScriptContext::shutDown();

    HttpConnectionManager::getInstance().stop();
    httpService.join();
}
