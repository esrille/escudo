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

#include "Test.util.h"

#include <stdarg.h>
#include <time.h>

#include <sstream>
#include <chrono>
#include <ratio>

#include <GL/freeglut.h>

#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Text.h>

#include "DOMImplementationImp.h"
#include "NodeImp.h"
#include "html/HTMLInputStream.h"
#include "html/HTMLParser.h"
#include "css/Box.h"
#include "css/CSSInputStream.h"
#include "css/CSSParser.h"
#include "css/CSSStyleSheetImp.h"
#include "font/FontManager.h"
#include "utf.h"

using namespace org::w3c::dom::bootstrap;
using namespace org::w3c::dom;

namespace
{
    // 0: None
    // 1: Performance
    // 2: Harness
    // 3: Debug
    int logLevel = 3;

    typedef std::chrono::duration<unsigned long long, std::centi> Ticks;
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
    case Node::ELEMENT_NODE: {
        std::u16string name = node.getNodeName();
        stream << '<' << toLower(name) << '>';
        break;
    }
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

css::CSSStyleSheet loadStyleSheet(const char* path)
{
    char url[PATH_MAX + 7];
    strcpy(url, "file://");
    realpath(path, url + 7);
    std::ifstream stream(path);
    if (!stream) {
        std::cerr << "error: cannot open " << path << ".\n";
        exit(EXIT_FAILURE);
    }
    CSSParser parser;
    CSSInputStream cssStream(stream, "utf-8");
    css::CSSStyleSheet sheet = parser.parse(0, cssStream);
    if (auto imp = dynamic_cast<CSSStyleSheetImp*>(sheet.self()))
        imp->setHref(toString(url));
    return sheet;
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

unsigned recordTime(const char* msg, ...)
{
    typedef std::chrono::high_resolution_clock Clock;
    typedef Clock::time_point Point;
    static Point epoch(Point::min());

    if (logLevel == 0)
        return 0.0;
    auto point = Clock::now();
    if (epoch == Point::min())
        epoch = point;
    auto duration = point - epoch;
    if (msg) {
        va_list ap;
        va_start(ap, msg);
        printf("%02lu:%02lu:%02lu.%06lu: ",
               static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::hours>(duration).count() % 60),
               static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60),
               static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60),
               static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000));
        vprintf(msg, ap);
        printf("\n");
        va_end(ap);
    }
    return std::chrono::duration_cast<Ticks>(duration).count();
}

unsigned getTick()
{
    auto p = std::chrono::high_resolution_clock::now();
    auto duration = p.time_since_epoch();
    return std::chrono::duration_cast<Ticks>(duration).count();
}

void initLogLevel(int* argc, char* argv[], int defaultLevel)
{
    logLevel = defaultLevel;
    for (int i = 1; i < *argc; ++i) {
        if (strncmp(argv[i], "--v", 3) == 0) {
            if (argv[i][3] == '=')
                logLevel = atoi(argv[i] + 4);
            for (; i < *argc; ++i)
                argv[i] = argv[i + 1];
            --*argc;
            break;
        }
    }
}

int getLogLevel()
{
    return logLevel;
}

std::string getFileURL(const std::string& path)
{
    char url[PATH_MAX + 7];
    strcpy(url, "file://");
    realpath(path.c_str(), url + 7);
    return url;
}
