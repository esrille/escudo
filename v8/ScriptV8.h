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

#ifndef SCRIPT_H
#define SCRIPT_H

#include <Object.h>
#include <org/w3c/dom/html/Function.h>
#include <org/w3c/dom/events/Event.h>

// Script.cpp
struct JSRuntime;
struct JSObject;
JSRuntime* getRuntime();
JSObject* newGlobal();
void putGlobal(JSObject* global);
void callFunction(org::w3c::dom::html::Function function, org::w3c::dom::events::Event event);

#endif  // SCRIPT_H
