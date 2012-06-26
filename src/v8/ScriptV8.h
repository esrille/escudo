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

#ifndef SCRIPTV8_H
#define SCRIPTV8_H

#include "ECMAScript.h"
#include "esv8api.h"

class ECMAScriptContext::Impl
{
    v8::Persistent<v8::Context> context;

    static v8::Persistent<v8::ObjectTemplate> getGlobalTemplate();

public:
    Impl();
    ~Impl();
    void activate(ObjectImp* window);

    Object* compileFunction(const std::u16string& body);
    Object* xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree);

    static void shutDown();
};

#endif  // SCRIPTV8_H
