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

#include "RangeImp.h"

#include "CharacterDataImp.h"
#include "DocumentFragmentImp.h"
#include "DocumentImp.h"
#include "NodeImp.h"
#include "TextImp.h"

#include "utf.h"
#include "Test.util.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

namespace {

unsigned int getNodeLength(const NodePtr& node)
{
    switch (node->getNodeType()) {
    case Node::DOCUMENT_TYPE_NODE:
        return 0;
    case Node::TEXT_NODE:
    case Node::PROCESSING_INSTRUCTION_NODE:
    case Node::COMMENT_NODE:
        return std::static_pointer_cast<CharacterDataImp>(node)->getLength();
    default:
        return node->getChildCount();
    }
}

}

bool RangeImp::BoundaryPoint::operator<(const BoundaryPoint& other) const
{
    if (node == other.node)
        return offset < other.offset;
    if (other.node->compareDocumentPosition(node) & Node::DOCUMENT_POSITION_FOLLOWING) {
        if (other.node->isAncestorOf(node)) {
            NodePtr parent;
            for (NodePtr child = node; ; child = parent) {
                parent = child->getParent();
                if (parent == other.node)
                    return child->getPrecedingSiblingCount() < other.offset;
            }
        }
        return false;
    } else {
        if (node->isAncestorOf(other.node)) {
            NodePtr parent;
            for (NodePtr child = other.node; ; child = parent) {
                parent = child->getParent();
                if (parent == node)
                    return offset <= child->getPrecedingSiblingCount();
            }
        }
        return true;
    }
}

NodePtr RangeImp::BoundaryPoint::getTargetNode() const
{
    assert(node->getNodeType() != Node::TEXT_NODE);
    NodePtr target;
    if (offset < node->getChildCount()) {
        target = node->getFirstChildPtr();
        for (unsigned count = 0; count < offset; ++count)
            target = target->getNextSiblingPtr();
    } else {
        for (target = node; target; target = target->getParent()) {
            if (target->getNextSiblingPtr()) {
                target = target->getNextSiblingPtr();
                break;
            }
        }
    }
    return target;
}

void RangeImp::checkOwner()
{
    if (!start.node)
        return;

    DocumentPtr oldDocument = ownerDocument.lock();
    DocumentPtr newDocument = start.node->getOwnerDocumentImp();
    if (oldDocument != newDocument && newDocument) {
        RangePtr range(std::static_pointer_cast<RangeImp>(self()));
        if (oldDocument)
            oldDocument->eraseRange(range);
        newDocument->appendRange(range);
        ownerDocument = newDocument;
    }
}

NodePtr RangeImp::getRoot()
{
    return start.node ? start.node->getRoot() : nullptr;
}

bool RangeImp::contains(const NodePtr& node)
{
    BoundaryPoint nodeStart(node, 0);
    BoundaryPoint nodeEnd(node, getNodeLength(node));
    return start < nodeStart && nodeEnd < end;
}

bool RangeImp::partiallyContains(const NodePtr& node)
{
    return node->isInclusiveAncestorOf(start.node) ^ node->isInclusiveAncestorOf(end.node);
}

void RangeImp::onInsert(const NodePtr& parent, unsigned index, unsigned count)
{
     if (start.node == parent && index < start.offset)
         start.offset += count;
     if (end.node == parent && index < end.offset)
         end.offset += count;
}

void RangeImp::onRemove(const NodePtr& parent, const NodePtr& node, unsigned index)
{
    if (start.node->isDescendantOf(node)) {
        start.node = parent;
        start.offset = index;
    } else if (start.node == parent && index < start.offset)
        --start.offset;
    if (end.node->isDescendantOf(node)) {
        end.node = parent;
        end.offset = index;
    } else if (end.node == parent && index < end.offset)
        --end.offset;
}

void RangeImp::onReplaceData(const NodePtr& node, unsigned offset, unsigned count, unsigned length)
{
    if (start.node == node) {
        if (offset < start.offset && start.offset <= offset + count)
            start.offset = offset;
        if (offset + count < start.offset)
            start.offset += length - count;
    }
    if (end.node == node) {
        if (offset < end.offset && end.offset <= offset + count)
            end.offset = offset;
        if (offset + count < end.offset)
            end.offset += length - count;
    }
}

void RangeImp::onSplitText(const NodePtr& parent, const NodePtr& node, const NodePtr& newNode, unsigned offset)
{
    if (parent) {
        if (start.node == node && offset < start.offset) {
            start.node = newNode;
            start.offset -= offset;
        }
        if (end.node == node && offset < end.offset) {
            end.node = newNode;
            end.offset -= offset;
        }
        unsigned index = node->getPrecedingSiblingCount();
        if (start.node == parent && index + 1 == start.offset)
            ++start.offset;
        if (end.node == parent && index + 1 == end.offset)
            ++end.offset;
    } else {
        if (start.node == node && offset < start.offset)
            start.offset = offset;
        if (end.node == node && offset < end.offset)
            end.offset -= offset;
    }
}

RangeImp::RangeImp(const DocumentPtr& ownerDocument) :
    ownerDocument(ownerDocument),
    start(ownerDocument),
    end(ownerDocument)
{
}

Node RangeImp::getStartContainer()
{
    return start.node;
}

unsigned int RangeImp::getStartOffset()
{
    return start.offset;
}

Node RangeImp::getEndContainer()
{
    return end.node;
}

unsigned int RangeImp::getEndOffset()
{
    return end.offset;
}

bool RangeImp::getCollapsed()
{
    return start == end;
}

Node RangeImp::getCommonAncestorContainer()
{
    for (NodePtr container = start.node; container; container = container->getParent()) {
        if (container->isInclusiveAncestorOf(end.node))
            return container;
    }
    return nullptr;
}

void RangeImp::setStart(Node refNode, unsigned int offset)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        if (getNodeLength(node) < offset)
            throw DOMException{DOMException::INDEX_SIZE_ERR};
        BoundaryPoint bp(node, offset);
        if (end < bp || getRoot() != node->getRoot())
            end = bp;
        start = bp;
        checkOwner();
    }
}

void RangeImp::setEnd(Node refNode, unsigned int offset)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        if (getNodeLength(node) < offset)
            throw DOMException{DOMException::INDEX_SIZE_ERR};
        BoundaryPoint bp(node, offset);
        if (bp < start || getRoot() != node->getRoot())
            start = bp;
        end = bp;
        checkOwner();
    }
}

void RangeImp::setStartBefore(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        auto parent = node->getParent();
        if (!parent)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        setStart(parent, node->getPrecedingSiblingCount());
    }
}

void RangeImp::setStartAfter(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        auto parent = node->getParent();
        if (!parent)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        setStart(parent, node->getPrecedingSiblingCount() + 1);
    }
}

void RangeImp::setEndBefore(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        auto parent = node->getParent();
        if (!parent)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        setEnd(parent, node->getPrecedingSiblingCount());
    }
}

void RangeImp::setEndAfter(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        auto parent = node->getParent();
        if (!parent)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        setEnd(parent, node->getPrecedingSiblingCount() + 1);
    }
}

void RangeImp::collapse(bool toStart)
{
    if (toStart)
        end = start;
    else
        start = end;
}

void RangeImp::selectNode(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        auto parent = node->getParent();
        if (!parent)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        unsigned int index = node->getPrecedingSiblingCount();
        setStart(parent, index);
        setEnd(parent, index + 1);
    } else
        throw DOMException{DOMException::TYPE_MISMATCH_ERR};
}

void RangeImp::selectNodeContents(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        setStart(node, 0);
        setEnd(node, getNodeLength(node));
    } else
        throw DOMException{DOMException::TYPE_MISMATCH_ERR};
}

short RangeImp::compareBoundaryPoints(unsigned short how, ranges::Range sourceRange)
{
    if (auto src = std::dynamic_pointer_cast<RangeImp>(sourceRange.self())) {
        BoundaryPoint point;
        BoundaryPoint other;
        switch (how) {
        case ranges::Range::START_TO_START:
            point = start;
            other = src->start;
            break;
        case ranges::Range::START_TO_END:
            point = end;
            other = src->start;
            break;
        case ranges::Range::END_TO_END:
            point = end;
            other = src->end;
            break;
        case ranges::Range::END_TO_START:
            point = start;
            other = src->end;
            break;
        default:
            throw DOMException{DOMException::NOT_SUPPORTED_ERR};
        }
        if (getRoot() != src->getRoot())
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (point == other)
            return 0;
        if (point < other)
            return -1;
        return 1;
    }
    throw DOMException{DOMException::TYPE_MISMATCH_ERR};
}

void RangeImp::deleteContents()
{
    if (start == end)
        return;
    if (start.node == end.node) {
        switch (start.node->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE:
            assert(start.offset <= end.offset);
            std::static_pointer_cast<CharacterDataImp>(start.node)->deleteData(start.offset, end.offset - start.offset);
            return;
        default:
            break;
        }
    }
    BoundaryPoint newPoint = start;
    if (!start.node->isInclusiveAncestorOf(end.node)) {
        auto ref = start.node;
        for (auto parent = ref->getParent(); parent && !parent->isInclusiveAncestorOf(end.node); parent = ref->getParent())
            ref = parent;
        newPoint = BoundaryPoint(ref, 1 + ref->getPrecedingSiblingCount());
    }
    NodePtr firstNode;
    NodePtr lastNode;
    switch (start.node->getNodeType()) {
    case Node::TEXT_NODE:
    case Node::PROCESSING_INSTRUCTION_NODE:
    case Node::COMMENT_NODE:
        std::static_pointer_cast<CharacterDataImp>(start.node)->deleteData(start.offset, std::static_pointer_cast<CharacterDataImp>(start.node)->getLength() - start.offset);
        firstNode = start.node;
        break;
    default:
        firstNode = start.getTargetNode()->getPreviousNode();
        break;
    }
    switch (end.node->getNodeType()) {
    case Node::TEXT_NODE:
    case Node::PROCESSING_INSTRUCTION_NODE:
    case Node::COMMENT_NODE:
        std::static_pointer_cast<CharacterDataImp>(end.node)->deleteData(0, end.offset);
        lastNode = end.node;
        break;
    default:
        lastNode = end.getTargetNode();
        break;
    }
    for (NodePtr node = firstNode->getNextNode(); node && node != lastNode; node = firstNode->getNextNode()) {
        BoundaryPoint bp(node, getNodeLength(node));
        if (end < bp) {
            firstNode = node;
            continue;
        }
        node->getParent()->removeChild(Node(node));
    }
    start = end = newPoint;
}

DocumentFragment RangeImp::extractContents()
{
    auto frag = std::make_shared<DocumentFragmentImp>(ownerDocument.lock());
    if (start == end)
        return frag;
    if (start.node == end.node) {
        switch (start.node->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            assert(start.offset <= end.offset);
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(start.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(start.node);
            clone->setData(characterData->substringData(start.offset, end.offset - start.offset));
            frag->appendChild(CharacterData(clone));
            characterData->deleteData(start.offset, end.offset - start.offset);
            return frag;
        }
        default:
            break;
        }
    }

    NodePtr commonAncestor = start.node;
    while (!commonAncestor->isInclusiveAncestorOf(end.node)) {
        commonAncestor = commonAncestor->getParent();
        if (!commonAncestor)
            return frag;
    }
    NodePtr firstPartiallyContainedChild;
    if (!start.node->isInclusiveAncestorOf(end.node)) {
        for (auto node = commonAncestor->getFirstChildPtr(); node; node = node->getNextSiblingPtr()) {
            if (partiallyContains(node)) {
                firstPartiallyContainedChild = node;
                break;
            }
        }
    }
    NodePtr lastPartiallyContainedChild;
    if (!end.node->isInclusiveAncestorOf(start.node)) {
        for (auto node = commonAncestor->getLastChildPtr(); node; node = node->getPreviousSiblingPtr()) {
            if (partiallyContains(node)) {
                lastPartiallyContainedChild = node;
                break;
            }
        }
    }
    NodePtr firstContainedChild;
    NodePtr lastContainedChild;
    for (auto node = commonAncestor->getFirstChildPtr(); node; node = node->getNextSiblingPtr()) {
        if (!contains(node))
            continue;
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        if (!firstContainedChild)
            firstContainedChild = node;
        lastContainedChild = node;
    }

    BoundaryPoint newPoint = start;
    if (!start.node->isInclusiveAncestorOf(end.node)) {
        auto ref = start.node;
        for (auto parent = ref->getParent(); parent && !parent->isInclusiveAncestorOf(end.node); parent = ref->getParent())
            ref = parent;
        newPoint = BoundaryPoint(ref, 1 + ref->getPrecedingSiblingCount());
    }

    if (firstPartiallyContainedChild) {
        switch (firstPartiallyContainedChild->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(start.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(start.node);
            clone->setData(characterData->substringData(start.offset, characterData->getLength() - start.offset));
            frag->appendChild(CharacterData(clone));
            characterData->deleteData(start.offset, characterData->getLength() - start.offset);
            break;
        }
        default: {
            NodePtr clone = std::static_pointer_cast<NodeImp>(firstPartiallyContainedChild->cloneNode(false).self());
            frag->appendChild(Node(clone));
            auto subrange = std::make_shared<RangeImp>(ownerDocument.lock());
            subrange->start = start;
            subrange->end = BoundaryPoint(firstPartiallyContainedChild, getNodeLength(firstPartiallyContainedChild));
            auto subfrag = std::static_pointer_cast<DocumentFragmentImp>(subrange->extractContents().self());
            clone->appendChild(DocumentFragment(subfrag));
            break;
        }
        }
    }
    NodePtr next;
    for (auto node = firstContainedChild; node; node = next) {
        next = node->getNextSiblingPtr();
        frag->appendChild(Node(node));
        if (node == lastContainedChild)
            break;
    }
    if (lastPartiallyContainedChild) {
        switch (lastPartiallyContainedChild->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(end.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(end.node);
            clone->setData(characterData->substringData(0, end.offset));
            frag->appendChild(CharacterData(clone));
            characterData->deleteData(0, end.offset);
            break;
        }
        default: {
            NodePtr clone = std::static_pointer_cast<NodeImp>(lastPartiallyContainedChild->cloneNode(false).self());
            frag->appendChild(Node(clone));
            auto subrange = std::make_shared<RangeImp>(ownerDocument.lock());
            subrange->start = BoundaryPoint(lastPartiallyContainedChild, 0);
            subrange->end = end;
            auto subfrag = std::static_pointer_cast<DocumentFragmentImp>(subrange->extractContents().self());
            clone->appendChild(DocumentFragment(subfrag));
            break;
        }
        }
    }
    start = end = newPoint;
    return frag;
}

DocumentFragment RangeImp::cloneContents()
{
    auto frag = std::make_shared<DocumentFragmentImp>(ownerDocument.lock());
    if (start == end)
        return frag;
    if (start.node == end.node) {
        switch (start.node->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            assert(start.offset <= end.offset);
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(start.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(start.node);
            clone->setData(characterData->substringData(start.offset, end.offset - start.offset));
            frag->appendChild(CharacterData(clone));
            return frag;
        }
        default:
            break;
        }
    }

    NodePtr commonAncestor = start.node;
    while (!commonAncestor->isInclusiveAncestorOf(end.node)) {
        commonAncestor = commonAncestor->getParent();
        if (!commonAncestor)
            return frag;
    }
    NodePtr firstPartiallyContainedChild;
    if (!start.node->isInclusiveAncestorOf(end.node)) {
        for (auto node = commonAncestor->getFirstChildPtr(); node; node = node->getNextSiblingPtr()) {
            if (partiallyContains(node)) {
                firstPartiallyContainedChild = node;
                break;
            }
        }
    }
    NodePtr lastPartiallyContainedChild;
    if (!end.node->isInclusiveAncestorOf(start.node)) {
        for (auto node = commonAncestor->getLastChildPtr(); node; node = node->getPreviousSiblingPtr()) {
            if (partiallyContains(node)) {
                lastPartiallyContainedChild = node;
                break;
            }
        }
    }
    NodePtr firstContainedChild;
    NodePtr lastContainedChild;
    for (auto node = commonAncestor->getFirstChildPtr(); node; node = node->getNextSiblingPtr()) {
        if (!contains(node))
            continue;
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        if (!firstContainedChild)
            firstContainedChild = node;
        lastContainedChild = node;
    }

    if (firstPartiallyContainedChild) {
        switch (firstPartiallyContainedChild->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(start.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(start.node);
            clone->setData(characterData->substringData(start.offset, characterData->getLength() - start.offset));
            frag->appendChild(CharacterData(clone));
            break;
        }
        default: {
            NodePtr clone = std::static_pointer_cast<NodeImp>(firstPartiallyContainedChild->cloneNode(false).self());
            frag->appendChild(Node(clone));
            auto subrange = std::make_shared<RangeImp>(ownerDocument.lock());
            subrange->start = start;
            subrange->end = BoundaryPoint(firstPartiallyContainedChild, getNodeLength(firstPartiallyContainedChild));
            auto subfrag = std::static_pointer_cast<DocumentFragmentImp>(subrange->cloneContents().self());
            clone->appendChild(DocumentFragment(subfrag));
            break;
        }
        }
    }
    for (auto node = firstContainedChild; node; node = node->getNextSiblingPtr()) {
        NodePtr clone = std::static_pointer_cast<NodeImp>(node->cloneNode(true).self());
        frag->appendChild(Node(clone));
        if (node == lastContainedChild)
            break;
    }
    if (lastPartiallyContainedChild) {
        switch (lastPartiallyContainedChild->getNodeType()) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE: {
            CharacterDataPtr clone = std::static_pointer_cast<CharacterDataImp>(end.node->cloneNode(false).self());
            auto characterData = std::static_pointer_cast<CharacterDataImp>(end.node);
            clone->setData(characterData->substringData(0, end.offset));
            frag->appendChild(CharacterData(clone));
            break;
        }
        default: {
            NodePtr clone = std::static_pointer_cast<NodeImp>(lastPartiallyContainedChild->cloneNode(false).self());
            frag->appendChild(Node(clone));
            auto subrange = std::make_shared<RangeImp>(ownerDocument.lock());
            subrange->start = BoundaryPoint(lastPartiallyContainedChild, 0);
            subrange->end = end;
            auto subfrag = std::static_pointer_cast<DocumentFragmentImp>(subrange->cloneContents().self());
            clone->appendChild(DocumentFragment(subfrag));
            break;
        }
        }
    }
    return frag;
}

void RangeImp::insertNode(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        NodePtr reference;
        unsigned short type = start.node->getNodeType();
        switch (type) {
        case Node::TEXT_NODE:
        case Node::PROCESSING_INSTRUCTION_NODE:
        case Node::COMMENT_NODE:
            if (!start.node->getParent())
                throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
            break;
        default:
            break;
        }
        if (type == Node::TEXT_NODE)
            reference = std::static_pointer_cast<NodeImp>(std::static_pointer_cast<TextImp>(start.node)->splitText(start.offset).self());
        else {
            unsigned index = 0;
            for (auto child = start.node->getFirstChildPtr(); child; child = child->getNextSiblingPtr(), ++index) {
                if (index == start.offset) {
                    reference = child;
                    break;
                }
            }
        }
        NodePtr parent;
        unsigned offset;
        if (!reference) {
            parent = start.node;
            offset = getNodeLength(parent);
        } else {
            parent = reference->getParent();
            offset = reference->getPrecedingSiblingCount();
        }
        if (node == reference)
            reference = reference->getNextSiblingPtr();
        if (auto nodeParent = node->getParent())
            nodeParent->remove(node);
        if (type == Node::DOCUMENT_FRAGMENT_NODE)
            offset += getNodeLength(node);
        else
            ++offset;
        parent->preInsert(node, reference);
        if (start == end)
            end = BoundaryPoint(parent, offset);
    }
}

void RangeImp::surroundContents(Node newParent)
{
    if (auto parent = std::dynamic_pointer_cast<NodeImp>(newParent.self())) {
        NodePtr commonAncestor = start.node;
        while (!commonAncestor->isInclusiveAncestorOf(end.node)) {
            commonAncestor = commonAncestor->getParent();
            if (!commonAncestor)
                return;
        }
        NodePtr firstPartiallyContainedChild;
        if (!start.node->isInclusiveAncestorOf(end.node)) {
            for (auto node = commonAncestor->getFirstChildPtr(); node; node = node->getNextSiblingPtr()) {
                if (partiallyContains(node)) {
                    firstPartiallyContainedChild = node;
                    break;
                }
            }
        }
        NodePtr lastPartiallyContainedChild;
        if (!end.node->isInclusiveAncestorOf(start.node)) {
            for (auto node = commonAncestor->getLastChildPtr(); node; node = node->getPreviousSiblingPtr()) {
                if (partiallyContains(node)) {
                    lastPartiallyContainedChild = node;
                    break;
                }
            }
        }
        if (firstPartiallyContainedChild->getNodeType() != Node::TEXT_NODE || lastPartiallyContainedChild->getNodeType() != Node::TEXT_NODE)
            throw DOMException{DOMException::INVALID_STATE_ERR};
        unsigned short type = parent->getNodeType();
        switch (type) {
        case Node::DOCUMENT_NODE:
        case Node::DOCUMENT_TYPE_NODE:
        case Node::DOCUMENT_FRAGMENT_NODE:
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        default:
            break;
        }
        auto fragment = std::static_pointer_cast<DocumentFragmentImp>(extractContents().self());
        while (0 < parent->getChildCount())
            parent->removeChild(parent->getFirstChild());
        insertNode(newParent);
        parent->appendChild(DocumentFragment(fragment));
        selectNode(newParent);
    }
}

ranges::Range RangeImp::cloneRange()
{
    try {
        auto clone = std::make_shared<RangeImp>(ownerDocument.lock());
        clone->start = start;
        clone->end = end;
        return clone;
    } catch (...) {
        return nullptr;
    }
}

void RangeImp::detach()
{
}

bool RangeImp::isPointInRange(Node refNode, unsigned int offset)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getRoot() != getRoot())
            return false;
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        if (getNodeLength(node) < offset)
            throw DOMException{DOMException::INDEX_SIZE_ERR};
        BoundaryPoint bp(node, offset);
        if (bp < start || end < bp)
            return false;
        return true;
    }
    throw DOMException{DOMException::TYPE_MISMATCH_ERR};
}

short RangeImp::comparePoint(Node refNode, unsigned int offset)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getRoot() != getRoot())
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (node->getNodeType() == Node::DOCUMENT_TYPE_NODE)
            throw DOMException{DOMException::INVALID_NODE_TYPE_ERR};
        if (getNodeLength(node) < offset)
            throw DOMException{DOMException::INDEX_SIZE_ERR};
        BoundaryPoint bp(node, offset);
        if (bp < start)
            return -1;
        if (end < bp)
            return 1;
        return 0;
    }
    throw DOMException{DOMException::TYPE_MISMATCH_ERR};
}

bool RangeImp::intersectsNode(Node refNode)
{
    if (auto node = std::dynamic_pointer_cast<NodeImp>(refNode.self())) {
        if (node->getRoot() != getRoot())
            return false;
        auto parent = node->getParent();
        if (!parent)
            return true;
        unsigned int offset = node->getPrecedingSiblingCount();
        BoundaryPoint nodeStart(parent, offset);
        BoundaryPoint nodeEnd(parent, offset + 1);
        return nodeStart < end && start < nodeEnd;
    }
    return false;
}

std::u16string RangeImp::toString()
{
    std::u16string s;
    std::u16string t;
    if (start.node == end.node) {
        switch (start.node->getNodeType()) {
        case Node::TEXT_NODE:
            assert(start.offset <= end.offset);
            return std::static_pointer_cast<CharacterDataImp>(start.node)->substringData(start.offset, end.offset - start.offset);
        default:
            break;
        }
    }
    NodePtr firstNode;
    NodePtr lastNode;
    switch (start.node->getNodeType()) {
    case Node::TEXT_NODE:
        s = std::static_pointer_cast<CharacterDataImp>(start.node)->substringData(start.offset, std::static_pointer_cast<CharacterDataImp>(start.node)->getLength() - start.offset);
        firstNode = start.node->getNextNode();
        break;
    default:
        firstNode = start.getTargetNode();
        break;
    }
    switch (end.node->getNodeType()) {
    case Node::TEXT_NODE:
        t = std::static_pointer_cast<CharacterDataImp>(end.node)->substringData(0, end.offset);
        lastNode = end.node;
        break;
    default:
        lastNode = end.getTargetNode();
        break;
    }
    for (NodePtr node = firstNode; node && node != lastNode; node = node->getNextNode()) {
        if (node->getNodeType() == Node::TEXT_NODE)
            s += std::static_pointer_cast<CharacterDataImp>(node)->getData();
    }
    return s + t;
}

DocumentFragment RangeImp::createContextualFragment(const std::u16string& fragment)
{
    // TODO: implement me!
    return nullptr;
}

DOMRectList RangeImp::getClientRects()
{
    // TODO: implement me!
    return nullptr;
}

DOMRect RangeImp::getBoundingClientRect()
{
    // TODO: implement me!
    return nullptr;
}

}
}
}
}
