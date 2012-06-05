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

#include "Profile.h"
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
        std::cout << "usage: " << argv[0] << " navigator_directory profile_directory\n";
        return EXIT_FAILURE;
    }

    // TODO: display HTML error pages instead of these text messages upon error.
    Profile profile(argv[2]);
    switch (profile.hasError()) {
    case 0:
        break;
    case EAGAIN:
        // TODO: use a named pipe for requesting to open another window.
        std::cerr << "error: another instance of the navigator might already be running.\n";
        return EXIT_FAILURE;
    case ENOENT:
        std::cerr << "error: the specified profile directory '" << profile.getProfilePath() << "' does not exist.\n";
        return EXIT_FAILURE;
    case ENOTDIR:
        std::cerr << "error: the specified profile '" << profile.getProfilePath() << "' is not a directory.\n";
        return EXIT_FAILURE;
    default:
        std::cerr << "error: the specified profile directory '" << profile.getProfilePath() << "' appears to be invalid.\n";
        return EXIT_FAILURE;
    }

    // Check cache directory in the profile.
    if (profile.createDirectory("cache") == -1) {
        std::cerr << "error: cannot access to the cache directory '" << profile.createPath("cache") << "'.\n";
        return EXIT_FAILURE;
    }
    HttpRequest::setCachePath(profile.createPath("cache"));

    init(&argc, argv);
    initLogLevel(&argc, argv, 0);
    initFonts(&argc, argv);

    // Load the default style sheet
    std::string defaultSheet = profile.getProfilePath() + "/default.css";
    if (!profile.hasFile(defaultSheet))
        defaultSheet = std::string(argv[1]) + "/default.css";
    getDOMImplementation()->setDefaultCSSStyleSheet(loadStyleSheet(defaultSheet.c_str()));

    // Load the user style sheet
    std::string userSheet = profile.getProfilePath() + "/user.css";
    if (profile.hasFile(userSheet))
        getDOMImplementation()->setUserCSSStyleSheet(loadStyleSheet(userSheet.c_str()));

    HttpRequest::setAboutPath(argv[1]);
    std::thread httpService(std::ref(HttpConnectionManager::getInstance()));

    std::string navigatorPath = profile.getProfilePath() + "/navigator.html";
    if (!profile.hasFile(navigatorPath))
        navigatorPath = std::string(argv[1]) + "/navigator.html";
    WindowImp* imp = new WindowImp();
    window = imp;
    imp->enableZoom(false);
    imp->open(utfconv(getFileURL(navigatorPath)), u"_self", u"", true);

    glutMainLoop();

    window = 0;

    ECMAScriptContext::shutDown();

    HttpConnectionManager::getInstance().stop();
    httpService.join();
}
