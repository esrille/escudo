/*
 * Copyright 2010-2012 Esrille Inc.
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

#include "DOMTokenListImp.h"

#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "utf.h"

#include "ElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

DOMTokenListImp::DOMTokenListImp(ElementImp* element, const std::u16string& localName) :
    element(element),
    localName(localName)
{
    if (!element)
        return;
    std::u16string value = element->getAttribute(localName);
    boost::algorithm::split(tokens, value, isSpace);
}

void DOMTokenListImp::update()
{
    if (localName.empty())
        return;
    element.setAttribute(localName, toString());
}

unsigned int DOMTokenListImp::getLength()
{
    return tokens.size();
}

Nullable<std::u16string> DOMTokenListImp::item(unsigned int index)
{
    if (getLength() <= index)
        return Nullable<std::u16string>();
    return tokens[index];
}

bool DOMTokenListImp::contains(const std::u16string& token)
{
    return std::find(tokens.begin(), tokens.end(), token) != tokens.end();
}

void DOMTokenListImp::add(Variadic<std::u16string> tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::u16string token = tokens[i];
        stripLeadingAndTrailingWhitespace(token);
        if (token.empty())
            continue;   // TODO: throw
        if (!contains(token))
            this->tokens.push_back(token);
    }
    update();
}

void DOMTokenListImp::remove(Variadic<std::u16string> tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::u16string token = tokens[i];
        stripLeadingAndTrailingWhitespace(token);
        if (token.empty())
            continue;   // TODO: throw
        auto found = std::find(this->tokens.begin(), this->tokens.end(), token);
        if (found != this->tokens.end())
            this->tokens.erase(found);
    }
    update();
}

bool DOMTokenListImp::toggle(const std::u16string& token)
{
    std::u16string t(token);
    stripLeadingAndTrailingWhitespace(t);
    if (t.empty())
        return false; // TODO: throw
    auto found = std::find(tokens.begin(), tokens.end(), t);
    if (found != tokens.end()) {
        tokens.erase(found);
        update();
        return false;
    } else {
        tokens.push_back(t);
        update();
        return true;
    }
}

bool DOMTokenListImp::toggle(const std::u16string& token, bool force)
{
    std::u16string t(token);
    stripLeadingAndTrailingWhitespace(t);
    if (t.empty())
        return false; // TODO: throw
    auto found = std::find(tokens.begin(), tokens.end(), t);
    if (force) {
        if (found == tokens.end()) {
            tokens.push_back(t);
            update();
        }
        return true;
    } else {
        if (found != tokens.end()) {
            tokens.erase(found);
            update();
        }
        return false;
    }
}

std::u16string DOMTokenListImp::toString()
{
    if (tokens.empty())
        return u"";
    std::u16string value;
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        if (it != tokens.begin())
            value += u' ';
        value += *it;
    }
    return value;
}

}
}
}
}
