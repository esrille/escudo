/*
 * Copyright 2010, 2011 Esrille Inc.
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

#include "html/HTMLParser.h"

#include <org/w3c/dom/Comment.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/DocumentType.h>
#include <org/w3c/dom/Text.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <assert.h>

#include "utf.h"

#include "css/CSSSerialize.h"
#include "DOMImplementationImp.h"

#include "Test.util.h"

using namespace org::w3c::dom;

char data[128*1024];

void test(std::ostream& result, const char* data)
{
    std::istringstream stream(data);
    HTMLInputStream htmlInputStream(stream, "utf-8");
    HTMLTokenizer tokenizer(&htmlInputStream);
    Document document = bootstrap::getDOMImplementation()->createDocument(u"", u"", 0);
    HTMLParser parser(document, &tokenizer);
    parser.mainLoop();
    dumpTree(result, document);
}

const char* load(std::ifstream& stream, char* data)
{
    static char type[256];
    char c = 0;
    char last = '\n';

    char*p = data;
    *p = 0;
    while (stream.get(c) && (c != '#' || last != '\n'))
        last = c;
    if (c != '#' || !stream.getline(type, sizeof type))
        return 0;
    while (stream.get(c)) {
        if (c == '#' && last == '\n') {
            stream.unget();
            break;
        }
        *p++ = c;
        last = c;
    }
    while (data < p && p[-1] == '\n')
        --p;
    *p= 0;
    return type;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " [test.dat]...\n";
        exit(EXIT_FAILURE);
    }
    int rc = EXIT_SUCCESS;
    for (int i = 1; i < argc; ++i) {
        std::ifstream stream(argv[i]);
        if (!stream) {
            std::cerr << "error: cannot open " << argv[i] << ".\n";
            continue;
        }
        std::ostringstream result;
        while (const char* type = load(stream, data)) {
            std::cout << '#' << type << '\n' << data << '\n';
            if (strcmp(type, "data") == 0)
                test(result, data);
            else if (strcmp(type, "document") == 0) {
                if (result.str().substr(10, result.str().length() - 11) == data)
                    std::cout << "PASS\n";
                else {
                    std::cout << "#result\n" << result.str().substr(10) << '\n';
                    std::cout << "FAIL\n";
                }
                result.str("");
                result.clear();
                result << std::dec;
            }
        }
    }
    return rc;
}
