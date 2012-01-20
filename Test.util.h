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

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <iostream>

#include <Object.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/css/CSSStyleSheet.h>

#include "css/ViewCSSImp.h"

std::ostream& operator<<(std::ostream& stream, org::w3c::dom::Node node);
void dumpTree(std::ostream& result, org::w3c::dom::Node node, std::string indent = "");

void dumpStyleSheet(std::ostream& result, org::w3c::dom::css::CSSStyleSheet sheet);

void printComputedValues(org::w3c::dom::Node node, org::w3c::dom::bootstrap::ViewCSSImp* view, std::string indent = "");

org::w3c::dom::css::CSSStyleSheet loadStyleSheet(std::istream& stream);
void eval(org::w3c::dom::Node node);

org::w3c::dom::Document loadDocument(std::istream& stream);
org::w3c::dom::Document loadDocument(const char* html);

void recordTime(const char* msg);

inline bool isReplacedElement(org::w3c::dom::Element& element)
{
    std::u16string tag = element.getLocalName();  // TODO: Check HTML namespace
    if (tag == u"img" || tag == u"iframe" || tag == u"video")  // TODO: more tags to come...
        return true;
    return false;
}

void initLogLevel(int* argc, char* argv[]);
int getLogLevel();

//
// Test.glut.cpp
//
void reshape(int w, int h);
void display() __attribute__((weak));
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void timer(int value);
void init(int* argc, char* argv[]);

#endif  // TEST_UTIL_H
