/*
 * Copyright 2010-2015 Esrille Inc.
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

#include "TextImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Node
unsigned short TextImp::getNodeType()
{
    return Node::TEXT_NODE;
}

// Text
Text TextImp::splitText(unsigned int offset)
{
    unsigned int length = getLength();
    if (length < offset)
        throw DOMException{DOMException::INDEX_SIZE_ERR};
    unsigned int count = length - offset;
    std::u16string newData = substringData(offset, count);
    TextPtr newNode;
    try {
        newNode = std::make_shared<TextImp>(getOwnerDocumentImp().get(), newData);
    } catch (...) {
        return nullptr;
    }
    auto parent = getParent();
    if (parent) {
        parent->insert(newNode, getNextSiblingPtr());
        // TODO: 2.5
    }
    replaceData(offset, count, u"");
    // TODO: 9.
    return newNode;
}

std::u16string TextImp::getWholeText()
{
    // TODO: implement me!
    return u"";
}

bool TextImp::getSerializeAsCDATA()
{
    // TODO: implement me!
    return 0;
}

void TextImp::setSerializeAsCDATA(bool serializeAsCDATA)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
