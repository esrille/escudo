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

#ifndef SCRIPT_H
#define SCRIPT_H

#include "ECMAScript.h"

#include "esjsapi.h"

class ECMAScriptContext::Impl
{
    JSObject* global;

    static JSRuntime* getRuntime() {
        static JSRuntime* runtime = 0;
        if (runtime)
            return runtime;
        runtime = JS_NewRuntime(8L * 1024L * 1024L);
        return runtime;
    }

    static JSContext* getContext();

public:
    Impl();
    ~Impl();

    void activate(ObjectImp* window);
    Any evaluate(const std::u16string& script);
    Object* compileFunction(const std::u16string& body);
    Any callFunction(Object thisObject, Object functionObject, int argc, Any* argv);
    Object* xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree);

    static void shutDown()
    {
        if (JSContext* context = getContext())
            JS_DestroyContext(context);
        if (JSRuntime* runtime = getRuntime())
            JS_DestroyRuntime(runtime);
        JS_ShutDown();
    }
};

#endif  // SCRIPT_H
