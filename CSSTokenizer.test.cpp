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

#include "css/CSSSelector.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "utf.h"

std::ostream& operator<<(std::ostream& output, const std::u16string string)
{
    char utf8[5];
    char* end;
    for (std::u16string::const_iterator i = string.begin(); i < string.end(); ++i) {
        switch (*i) {
        case '\\':
            output << "\\\\";
            break;
        case '"':
            output << "\\\"";
            break;
        case '/':
            output << "\\/";
            break;
        case '\t':
            output << "\\t";
            break;
        case '\n':
            output << "\\n";
            break;
        case '\f':
            output << "\\f";
            break;
        default:
            end = utf32to8(*i, utf8);
            *end = '\0';
            output << utf8;
            break;
        }
    }
    return output;
}

int main(int argc, char** argv)
{
    org::w3c::dom::bootstrap::CSSTokenizer tokenizer;
    tokenizer.reset(u"20em 30px @media #id /* ははは */ 漢字 'テスト' 20deg @PAGE nth-child(");

    int token;
    do {
        token = tokenizer.getToken();
        std::cout << "yylex = " << token << "\n";
    }
    while (token);
    return 0;
}
