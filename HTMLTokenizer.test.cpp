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

#include "html/HTMLTokenizer.h"

#include <fstream>
#include <sstream>

#include "utf.h"

#include "picojson.h"

static const char* separator;

bool emit(const Token& token, std::ostream& output)
{
    static bool characterMode = false;
    static std::u16string characters;
    bool eof = false;

    if (characterMode && token.getType() != Token::Type::Character) {
        output << separator << "[\"Character\",\"" << espaceString(characters) << "\"]";
        separator = ",";
        characterMode = false;
        characters.clear();
    }

    switch (token.getType()) {
    case Token::Type::StartTag:
        output << separator << "[\"StartTag\",\"" << espaceString(token.getName()) << "\",{";
        separator = ",";
        for (auto i = token.getAttributes().begin(); i != token.getAttributes().end(); ++i) {
            if (i != token.getAttributes().begin())
                output << ',';
            auto attr = *i;
            output << '"' << espaceString(attr.getName()) << "\":\"" << attr.getValue() << '"';
        }
        output << '}';
        if (token.getFlags() & Token::SelfClosing)
            output << ",true";
        output << ']';
        break;
    case Token::Type::EndTag:
        output << separator << "[\"EndTag\",\"" << espaceString(token.getName()) << "\"]";
        separator = ",";
        break;
    case Token::Type::Comment:
        output << separator << "[\"Comment\",\"" << espaceString(token.getName()) << "\"]";
        separator = ",";
        break;
    case Token::Type::Doctype:
        output << separator << "[\"DOCTYPE\",\"" << espaceString(token.getName()) << "\",";
        separator = ",";
        if (!token.hasPublicId())
            output << "null,";
        else
            output << '"' << espaceString(token.getPublicId()) << "\",";
        if (!token.hasSystemId())
            output << "null,";
        else
            output << '"' << espaceString(token.getSystemId()) << "\",";
        output << ((token.getFlags() & Token::ForceQuirks) ? "false" : "true");
        output << ']';
        break;
    case Token::Type::Character:
        characterMode = true;
        characters += token.getChar();
        break;
    case Token::Type::EndOfFile:
        eof = true;
        break;
    case Token::Type::ParseError:
    default:
        output << separator << "\"ParseError\"";
        separator = ",";
        break;
    }
    return !eof;
}

int test(const std::string& description, const std::string& input, const std::string& output)
{
    std::ostringstream result;
    std::istringstream stream(input);
    HTMLInputStream htmlInputStream(stream, "utf-8");
    HTMLTokenizer tokenizer(&htmlInputStream);
    separator = "";
    result << '[';
    for (;;) {
        Token token = tokenizer.getToken();
        if (!emit(token, result))
            break;
    }
    result << ']';
    std::cout << "description: " << description << '\n';
    std::cout << "input: " << input << '\n';
    std::cout << "expected: " << output << '\n';
    std::cout << "output: " << result.str() << '\n';
    if (result.str() == output) {
        std::cout << "PASS\n";
        return EXIT_SUCCESS;
    }
    std::cout << "FAIL: " << result.str() << '\n';
    return EXIT_FAILURE;
}

int load(const char* filename)
{
    int rc = EXIT_SUCCESS;
    std::ifstream stream(filename);
    if (!stream) {
        std::cerr << "error: cannot open " << filename << ".\n";
        return EXIT_FAILURE;
    }

    picojson::value value;
    stream >> value;
    if (!stream)
        return EXIT_FAILURE;

    picojson::object obj = value.get<picojson::object>();
    picojson::array array = obj["tests"].get<picojson::array>();
    picojson::array::iterator it;
    for (it = array.begin(); it != array.end(); it++) {
        picojson::object data = it->get<picojson::object>();
        rc += test(data["description"].to_str(), data["input"].to_str(), data["output"].serialize());
    }

    return rc;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " [tokenizer.test]...\n";
        exit(EXIT_FAILURE);
    }
    int rc = EXIT_SUCCESS;
    for (int i = 1; i < argc; ++i)
        rc |= load(argv[i]);
    return rc;
}
