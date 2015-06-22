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

#include "DocumentImp.h"
#include "RangeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Node
unsigned short TextImp::getNodeType()
{
    return Node::TEXT_NODE;
}

// Text

namespace {

struct SplitTextFunctor
{
    const NodePtr& parent;
    const NodePtr& node;
    const NodePtr& newNode;
    unsigned offset;
    unsigned count;
    unsigned length;

    SplitTextFunctor(const NodePtr& parent, const NodePtr& node, const NodePtr& newNode, unsigned offset) :
        parent(parent),
        node(node),
        newNode(newNode),
        offset(offset)
    {
    }
    void operator()(const RangePtr& range) {
        range->onSplitText(parent, node, newNode, offset);
    }
};

}

Text TextImp::splitText(unsigned int offset)
{
    TextPtr node(std::static_pointer_cast<TextImp>(self()));

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
    DocumentPtr document = getOwnerDocumentImp();
    auto parent = getParent();
    if (parent) {
        parent->insert(newNode, getNextSiblingPtr());
        if (document)
            document->forEachRange(SplitTextFunctor(parent, node, newNode, offset));
    }
    replaceData(offset, count, u"");
    if (!parent && document)
        document->forEachRange(SplitTextFunctor(parent, node, newNode, offset));
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
