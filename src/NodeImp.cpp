/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "NodeImp.h"
#include "DocumentImp.h"
#include "MutationEventImp.h"
#include "ElementImp.h"
#include "NodeListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

//
// Tree management
//

NodeImp* NodeImp::removeChild(NodeImp* item)
{
    NodeImp* next = item->nextSibling;
    NodeImp* prev = item->previousSibling;
    if (!next)
        lastChild = prev;
    else
        next->previousSibling = prev;
    if (!prev)
        firstChild = next;
    else
        prev->nextSibling = next;
    item->parentNode = item->previousSibling = item->nextSibling = 0;
    --childCount;
    return item;
}

NodeImp* NodeImp::insertBefore(NodeImp* item, NodeImp* after)
{
    item->previousSibling = after->previousSibling;
    item->nextSibling = after;
    after->previousSibling = item;
    if (!item->previousSibling)
        firstChild = item;
    else
        item->previousSibling->nextSibling = item;
    item->parentNode = this;
    ++childCount;
    return item;
}

NodeImp* NodeImp::appendChild(NodeImp* item)
{
    NodeImp* prev = lastChild;
    if (!prev)
        firstChild = item;
    else
        prev->nextSibling = item;
    item->previousSibling = prev;
    item->nextSibling = 0;
    lastChild = item;
    item->parentNode = this;
    ++childCount;
    return item;
}

void NodeImp::setOwnerDocument(DocumentImp* document)
{
    ownerDocument = document;
    for (NodeImp* child = firstChild; child; child = child->nextSibling)
        child->setOwnerDocument(document);
}

// Node
unsigned short NodeImp::getNodeType()
{
    // SHOULD NOT REACH HERE
    return 0;
}

std::u16string NodeImp::getNodeName()
{
    return nodeName;
}

Nullable<std::u16string> NodeImp::getBaseURI()
{
    if (!ownerDocument)
        return Nullable<std::u16string>();
    return ownerDocument->getDocumentURI();
}

Document NodeImp::getOwnerDocument()
{
    return ownerDocument;
}

Node NodeImp::getParentNode()
{
    return parentNode;
}

Element NodeImp::getParentElement()
{
    return dynamic_cast<ElementImp*>(parentNode);
}

bool NodeImp::hasChildNodes()
{
    return firstChild;
}

NodeList NodeImp::getChildNodes()
{
    NodeListImp* nodeList = new(std::nothrow) NodeListImp;
    if (nodeList) {
        for (NodeImp* node = firstChild; node; node = node->nextSibling)
            nodeList->addItem(node);
    }
    return nodeList;
}

Node NodeImp::getFirstChild()
{
    return firstChild;
}

Node NodeImp::getLastChild()
{
    return lastChild;
}

Node NodeImp::getPreviousSibling()
{
    return previousSibling;
}

Node NodeImp::getNextSibling()
{
    return nextSibling;
}

unsigned short NodeImp::compareDocumentPosition(Node other)
{
    NodeImp* node = this;
    NodeImp* otherNode = dynamic_cast<NodeImp*>(other.self());
    if (node == otherNode)
        return 0;
    if (!node || !otherNode) {   // not in the same tree?
        return Node::DOCUMENT_POSITION_DISCONNECTED | Node::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC |
               ((otherNode < node) ? Node::DOCUMENT_POSITION_PRECEDING : Node::DOCUMENT_POSITION_FOLLOWING);
    }

    NodeImp* root;
    NodeImp* otherRoot;
    size_t depth = 1;
    size_t otherDepth = 1;
    for (root = node; root->parentNode; root = root->parentNode)
        ++depth;
    for (otherRoot = otherNode; otherRoot->parentNode; otherRoot = otherRoot->parentNode)
        ++otherDepth;
    if (root != otherRoot) {  // not in the same tree?
        return Node::DOCUMENT_POSITION_DISCONNECTED | Node::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC |
               ((otherNode < node) ? Node::DOCUMENT_POSITION_PRECEDING : Node::DOCUMENT_POSITION_FOLLOWING);
    }

    NodeImp* x = node;
    NodeImp* y = otherNode;
    for (int i = depth - otherDepth; 0 < i; --i)
        x = x->parentNode;
    for (int i = otherDepth - depth; 0 < i; --i)
        y = y->parentNode;
    if (x == y) {
        if (x == node) {
            assert(depth < otherDepth);
            return Node::DOCUMENT_POSITION_CONTAINED_BY | Node::DOCUMENT_POSITION_FOLLOWING;
        }
        if (y == otherNode) {
            assert(otherDepth < depth);
            return Node::DOCUMENT_POSITION_CONTAINS | Node::DOCUMENT_POSITION_PRECEDING;
        }
    }
    while (x->parentNode != y->parentNode) {
        x = x->parentNode;
        y = y->parentNode;
    }
    assert(x != y);
    if (!y->previousSibling || !x->nextSibling)
        return Node::DOCUMENT_POSITION_PRECEDING;
    if (!x->previousSibling || !y->nextSibling)
        return Node::DOCUMENT_POSITION_FOLLOWING;
    for (NodeImp* i = x->previousSibling; i; i = i->previousSibling) {
        if (i == y)
            return Node::DOCUMENT_POSITION_PRECEDING;
    }
    return Node::DOCUMENT_POSITION_FOLLOWING;
}

bool NodeImp::contains(Node other)
{
    // TODO: implement me!
    return false;
}

Nullable<std::u16string> NodeImp::getNodeValue()
{
    return Nullable<std::u16string>();
}

void NodeImp::setNodeValue(const Nullable<std::u16string>& nodeValue)
{
}

Nullable<std::u16string> NodeImp::getTextContent()
{
    return Nullable<std::u16string>();
}

void NodeImp::setTextContent(const Nullable<std::u16string>& textContent)
{
}

Node NodeImp::insertBefore(Node newChild, Node refChild)
{
    if (!newChild)
        return newChild;
    if (!refChild)
        return appendChild(newChild);
    if (newChild != refChild) {
        Document ownerOfChild = newChild.getOwnerDocument();
        if (ownerOfChild != getOwnerDocument() && ownerOfChild != *this)
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (refChild.getParentNode() != *this)
            throw DOMException{DOMException::NOT_FOUND_ERR};
        if (NodeImp* child = dynamic_cast<NodeImp*>(newChild.self())) {
            if (child == this || child->isAncestorOf(this))
                throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
            if (NodeImp* ref = dynamic_cast<NodeImp*>(refChild.self())) {
                child->retain_();
                if (child->parentNode) {
                    child->parentNode->removeChild(child);
                    child->release_();
                }
                insertBefore(child, ref);

                events::MutationEvent event = new(std::nothrow) MutationEventImp;
                event.initMutationEvent(u"DOMNodeInserted",
                                        true, false, this, u"", u"", u"", 0);
                child->dispatchEvent(event);
            }
        }
    }
    return newChild;
}

Node NodeImp::replaceChild(Node newChild, Node oldChild)
{
    if (!newChild)
        return oldChild;
    if (!oldChild || oldChild.getParentNode() != *this)
        throw DOMException{DOMException::NOT_FOUND_ERR};
    if (newChild != oldChild) {
        Document ownerOfChild = newChild.getOwnerDocument();
        if (ownerOfChild != getOwnerDocument() && ownerOfChild != *this)
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (NodeImp* child = dynamic_cast<NodeImp*>(newChild.self())) {
            if (child == this || child->isAncestorOf(this))
                throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
            if (NodeImp* ref = dynamic_cast<NodeImp*>(oldChild.self())) {
                child->retain_();
                if (child->parentNode) {
                    child->parentNode->removeChild(child);
                    child->release_();
                }
                insertBefore(child, ref);
                removeChild(ref);
                ref->release_();
            }
        }
    }
    return oldChild;
}

Node NodeImp::removeChild(Node oldChild)
{
    if (!oldChild)
        throw DOMException{DOMException::NOT_FOUND_ERR};
    if (NodeImp* child = dynamic_cast<NodeImp*>(oldChild.self())) {
        if (child->parentNode != this)
            throw DOMException{DOMException::NOT_FOUND_ERR};
        if (0 < count_()) {  // Prevent dispatching an event from the destructor.
            events::MutationEvent event = new(std::nothrow) MutationEventImp;
            event.initMutationEvent(u"DOMNodeRemoved",
                                    true, false, this, u"", u"", u"", 0);
            child->dispatchEvent(event);
        }
        removeChild(child);
        child->release_();
    }
    return oldChild;
}

Node NodeImp::appendChild(Node newChild, bool clone)
{
    if (!newChild)
        return newChild;
    Document ownerOfChild = newChild.getOwnerDocument();
    if (ownerOfChild != getOwnerDocument() && ownerOfChild != *this)
        throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
    if (NodeImp* child = dynamic_cast<NodeImp*>(newChild.self())) {
        if (child == this || child->isAncestorOf(this))
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        // TODO: case newChild is a DocumentFragment
        child->retain_();
        if (child->parentNode) {
            child->parentNode->removeChild(child);
            child->release_();
        }
        appendChild(child);
        if (!clone) {
            events::MutationEvent event = new(std::nothrow) MutationEventImp;
            event.initMutationEvent(u"DOMNodeInserted",
                                    true, false, this, u"", u"", u"", 0);
            child->dispatchEvent(event);
        }
    }  // TODO: else ...
    return newChild;
}

void NodeImp::normalize()
{
    // TODO: implement me!
}

Node NodeImp::cloneNode(bool deep)
{
    return new(std::nothrow) NodeImp(this, deep);
}

bool NodeImp::isSameNode(Node other)
{
    NodeImp* node = dynamic_cast<NodeImp*>(other.self());
    return this == node;
}

bool NodeImp::isEqualNode(Node arg)
{
    NodeImp* node = dynamic_cast<NodeImp*>(arg.self());
    if (this == node)
        return true;
    if (!node)
        return false;
    if (getNodeType() != node->getNodeType())
        return false;
    if (getChildCount() != node->getChildCount())
        return false;
    NodeImp* x;
    NodeImp* y;
    for (x = firstChild, y = node->firstChild; x; x = x->nextSibling, y = y->nextSibling) {
        if (!y || !x->isEqualNode(y))
            return false;
    }
    return y ? false : true;
}

Nullable<std::u16string> NodeImp::lookupPrefix(const Nullable<std::u16string>& _namespace)
{
    // TODO: implement me!
    return u"";
}

Nullable<std::u16string> NodeImp::lookupNamespaceURI(const Nullable<std::u16string>& prefix)
{
    // TODO: implement me!
    return u"";
}

bool NodeImp::isDefaultNamespace(const Nullable<std::u16string>& _namespace)
{
    // TODO: implement me!
    return 0;
}

NodeImp::NodeImp(DocumentImp* ownerDocument) :
    ownerDocument(ownerDocument),
    parentNode(0),
    firstChild(0),
    lastChild(0),
    previousSibling(0),
    nextSibling(0),
    childCount(0)
{
}

NodeImp::NodeImp(NodeImp* org, bool deep) :
    ObjectMixin(org),
    ownerDocument(0),
    parentNode(0),
    firstChild(0),
    lastChild(0),
    previousSibling(0),
    nextSibling(0),
    childCount(0),
    nodeName(org->nodeName)
{
    setOwnerDocument(org->ownerDocument);
    if (!deep)
        return;
    for (NodeImp* node = org->firstChild; node; node = node->nextSibling) {
        Node clone = node->cloneNode(true);
        appendChild(clone, true);
    }
}

NodeImp::~NodeImp()
{
    assert(0 == count_());
    while (0 < childCount)
        removeChild(getFirstChild());
}

}}}}  // org::w3c::dom::bootstrap
