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

#include "Test.util.h"

#include <time.h>

#include <sstream>

#include <GL/freeglut.h>

#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Text.h>

#include "css/CSSInputStream.h"
#include "css/CSSParser.h"
#include "DOMImplementationImp.h"
#include "html/HTMLParser.h"
#include "NodeImp.h"

#include "css/Box.h"
#include "font/FontManager.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

namespace
{
    // 0: None
    // 1: Performance
    // 2: Harness
    // 3: Debug
    int logLevel = 3;
}

std::ostream& operator<<(std::ostream& stream, Node node)
{
    switch (node.getNodeType()) {
    case Node::DOCUMENT_TYPE_NODE:
        stream << "<!DOCTYPE " << node.getNodeName() << '>';
        break;
    case Node::COMMENT_NODE:
        stream << "<!-- " << interface_cast<Comment>(node).getData() << " -->";
        break;
    case Node::ELEMENT_NODE:
        stream << '<' << node.getNodeName() << '>';
        break;
    case Node::TEXT_NODE:
        stream << '"' << interface_cast<Text>(node).getData() << '"';
        break;
    default:
        stream << node.getNodeName();
        break;
    }
    return stream;
}

void dumpTree(std::ostream& result, Node node, std::string indent)
{
    while (node) {
        result << indent << node << '\n';
        Element element = interface_cast<Element>(node);
        if (element.getNodeType() == Node::ELEMENT_NODE) {
            ObjectArray<Attr> attrArray = element.getAttributes();
            assert(attrArray);
            for (unsigned int i = 0; i < attrArray.getLength(); ++i) {
                Attr attr = attrArray.getElement(i);
                assert(attr);
                result << indent << "  " << attr.getName() << "=\"" << attr.getValue() << "\"\n";
            }
        }
        if (node.hasChildNodes())
            dumpTree(result, node.getFirstChild(), indent + ((node.getNodeType() == Node::DOCUMENT_NODE) ? "| " : "  "));
        node = node.getNextSibling();
    }
}

void dumpStyleSheet(std::ostream& result, css::CSSStyleSheet sheet)
{
    css::CSSRuleList list = sheet.getCssRules();
    unsigned length = list.getLength();
    for (unsigned i = 0; i < length; ++i) {
        css::CSSRule rule = list.getElement(i);
        result << rule.getCssText() << '\n';
    }
}

void printComputedValues(Node node, ViewCSSImp* view, std::string indent)
{
    while (node) {
        if (node.getNodeType() == Node::ELEMENT_NODE) {
            Element element = interface_cast<Element>(node);
            std::cout << indent << '<' << element.getLocalName() << '>';
            if (css::CSSStyleDeclaration decl = view->getComputedStyle(element, Nullable<std::u16string>()))
                std::cout << ' ' << decl.getCssText();
            std::cout << '\n';
        }
        if (node.hasChildNodes())
            printComputedValues(node.getFirstChild(), view, indent + "  ");
        node = node.getNextSibling();
    }
}

css::CSSStyleSheet loadStyleSheet(std::istream& stream)
{
    CSSParser parser;
    CSSInputStream cssStream(stream, "utf-8");
    return parser.parse(0, cssStream);
}

void eval(Node node)
{
    while (node) {
        if (node.hasChildNodes())
            eval(node.getFirstChild());
        dynamic_cast<NodeImp*>(node.self())->eval();;
        node = node.getNextSibling();
    }
}

Document loadDocument(std::istream& stream)
{
    HTMLInputStream htmlInputStream(stream, "utf-8");
    HTMLTokenizer tokenizer(&htmlInputStream);
    Document document = bootstrap::getDOMImplementation()->createDocument(u"", u"", 0);
    HTMLParser parser(document, &tokenizer);
    parser.mainLoop();
    return document;
}

Document loadDocument(const char* html)
{
    std::istringstream stream(html);
    return loadDocument(stream);
}

void recordTime(const char* msg)
{
    if (logLevel == 0)
        return;
    static double last = 0.0;
    timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    double now = spec.tv_sec + (spec.tv_nsec / 1000000000.0);
    std::cerr << msg << ": " << now - last << '\n';
    last = now;
}

void initLogLevel(int* argc, char* argv[])
{
    for (int i = 1; i < *argc; ++i) {
        if (strncmp(argv[i], "--v", 3) == 0) {
            if (argv[i][3] == '=')
                logLevel = atoi(argv[i] + 4);
            for (; i < *argc; ++i)
                argv[i] = argv[i + 1];
            --*argc;
            std::cerr << "logLevel=" << logLevel << '\n';
            break;
        }
    }
}

int getLogLevel()
{
    return logLevel;
}
