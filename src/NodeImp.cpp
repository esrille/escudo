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

#include "NodeImp.h"
#include "DocumentImp.h"
#include "MutationEventImp.h"
#include "ElementImp.h"
#include "NodeListImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

//
// Tree management
//

NodePtr NodeImp::removeChild(NodePtr item)
{
    NodePtr next = item->nextSibling;
    NodePtr prev = item->previousSibling;
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

NodePtr NodeImp::insertBefore(NodePtr item, NodePtr after)
{
    item->previousSibling = after->previousSibling;
    item->nextSibling = after;
    after->previousSibling = item;
    if (!item->previousSibling)
        firstChild = item;
    else
        item->previousSibling->nextSibling = item;
    item->setParent(std::static_pointer_cast<NodeImp>(self()));
    ++childCount;
    return item;
}

NodePtr NodeImp::appendChild(NodePtr item)
{
    NodePtr prev = lastChild;
    if (!prev)
        firstChild = item;
    else
        prev->nextSibling = item;
    item->previousSibling = prev;
    item->nextSibling = 0;
    lastChild = item;
    item->setParent(std::static_pointer_cast<NodeImp>(self()));
    ++childCount;
    return item;
}

void NodeImp::setOwnerDocument(const DocumentPtr& document)
{
    if (getOwnerDocumentImp() != document) {
        ownerDocument = document;
        for (NodePtr child = firstChild; child; child = child->nextSibling)
            child->setOwnerDocument(document);
    }
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
    if (ownerDocument.expired())
        return Nullable<std::u16string>();
    return ownerDocument.lock()->getDocumentURI();
}

Document NodeImp::getOwnerDocument()
{
    return ownerDocument.lock();
}

Node NodeImp::getParentNode()
{
    return getParent();
}

Element NodeImp::getParentElement()
{
    if (auto parent = getParent())
        return std::dynamic_pointer_cast<ElementImp>(parent);
    return nullptr;
}

bool NodeImp::hasChildNodes()
{
    return static_cast<bool>(firstChild);
}

NodeList NodeImp::getChildNodes()
{
    NodeListPtr nodeList = std::make_shared<NodeListImp>();
    if (nodeList) {
        for (NodePtr node = firstChild; node; node = node->nextSibling)
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
    NodePtr node(std::static_pointer_cast<NodeImp>(self()));
    auto otherNode = std::dynamic_pointer_cast<NodeImp>(other.self());
    if (node == otherNode)
        return 0;
    if (!node || !otherNode) {   // not in the same tree?
        return Node::DOCUMENT_POSITION_DISCONNECTED | Node::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC |
               ((otherNode < node) ? Node::DOCUMENT_POSITION_PRECEDING : Node::DOCUMENT_POSITION_FOLLOWING);
    }

    NodePtr root;
    NodePtr otherRoot;
    size_t depth = 1;
    size_t otherDepth = 1;
    for (root = node; root->getParent(); root = root->getParent())
        ++depth;
    for (otherRoot = otherNode; otherRoot->getParent(); otherRoot = otherRoot->getParent())
        ++otherDepth;
    if (root != otherRoot) {  // not in the same tree?
        return Node::DOCUMENT_POSITION_DISCONNECTED | Node::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC |
               ((otherNode < node) ? Node::DOCUMENT_POSITION_PRECEDING : Node::DOCUMENT_POSITION_FOLLOWING);
    }

    NodePtr x = node;
    NodePtr y = otherNode;
    for (int i = depth - otherDepth; 0 < i; --i)
        x = x->getParent();
    for (int i = otherDepth - depth; 0 < i; --i)
        y = y->getParent();
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
    while (x->getParent() != y->getParent()) {
        x = x->getParent();
        y = y->getParent();
    }
    assert(x != y);
    if (!y->previousSibling || !x->nextSibling)
        return Node::DOCUMENT_POSITION_PRECEDING;
    if (!x->previousSibling || !y->nextSibling)
        return Node::DOCUMENT_POSITION_FOLLOWING;
    for (NodePtr i = x->previousSibling; i; i = i->previousSibling) {
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
    auto ref = std::dynamic_pointer_cast<NodeImp>(refChild.self());
    if (!ref)
        return appendChild(newChild);
    auto child = std::dynamic_pointer_cast<NodeImp>(newChild.self());
    if (!child)
        return child;
    if (child != ref) {
        DocumentPtr ownerOfChild = child->getOwnerDocumentImp();
        if (ownerOfChild != getOwnerDocumentImp() && ownerOfChild.get() != this)
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (ref->getParent().get() != this)
            throw DOMException{DOMException::NOT_FOUND_ERR};
        if (child.get() == this || child->isAncestorOf(std::static_pointer_cast<NodeImp>(self())))
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        if (child->getParent())
            child->getParent()->removeChild(child);
        insertBefore(child, ref);
        events::MutationEvent event = std::make_shared<MutationEventImp>();
        event.initMutationEvent(u"DOMNodeInserted", true, false, std::static_pointer_cast<NodeImp>(self()), u"", u"", u"", 0);
        child->dispatchEvent(event);
    }
    return child;
}

Node NodeImp::replaceChild(Node newChild, Node oldChild)
{
    auto child = std::dynamic_pointer_cast<NodeImp>(newChild.self());
    if (!child)
        return oldChild;
    auto ref = std::dynamic_pointer_cast<NodeImp>(oldChild.self());
    if (!ref || ref->getParent().get() != this)
        throw DOMException{DOMException::NOT_FOUND_ERR};
    if (child != ref) {
        DocumentPtr ownerOfChild = child->getOwnerDocumentImp();
        if (ownerOfChild != getOwnerDocumentImp() && ownerOfChild.get() != this)
            throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
        if (child.get() == this || child->isAncestorOf(std::static_pointer_cast<NodeImp>(self())))
            throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
        if (child->getParent())
            child->getParent()->removeChild(child);
        insertBefore(child, ref);
        removeChild(ref);
    }
    return ref;
}

Node NodeImp::removeChild(Node oldChild)
{
    auto child = std::dynamic_pointer_cast<NodeImp>(oldChild.self());
    if (!child)
        throw DOMException{DOMException::NOT_FOUND_ERR};
    if (child->getParent().get() != this)
        throw DOMException{DOMException::NOT_FOUND_ERR};
    auto event = std::make_shared<MutationEventImp>();
    event->initMutationEvent(u"DOMNodeRemoved", true, false, std::static_pointer_cast<NodeImp>(self()), u"", u"", u"", 0);
    child->dispatchEvent(event);
    removeChild(child);
    return child;
}

Node NodeImp::appendChild(Node newChild, bool clone)
{
    auto child = std::dynamic_pointer_cast<NodeImp>(newChild.self());
    if (!child)
        return child;
    DocumentPtr ownerOfChild = child->getOwnerDocumentImp();
    if (ownerOfChild != getOwnerDocumentImp() && ownerOfChild.get() != this)
        throw DOMException{DOMException::WRONG_DOCUMENT_ERR};
    if (child.get() == this || isDescendantOf(child))
        throw DOMException{DOMException::HIERARCHY_REQUEST_ERR};
    // TODO: case child is a DocumentFragment
    if (auto oldParent = child->getParent())
        oldParent->removeChild(child);
    appendChild(child);
    if (!clone) {
        auto event = std::make_shared<MutationEventImp>();
        event->initMutationEvent(u"DOMNodeInserted", true, false, std::static_pointer_cast<NodeImp>(self()), u"", u"", u"", 0);
        child->dispatchEvent(event);
    }
    return child;
}

void NodeImp::normalize()
{
    // TODO: implement me!
}

Node NodeImp::cloneNode(bool deep)
{
    auto node = std::make_shared<NodeImp>(*this);
    if (deep)
        node->cloneChildren(this);
    return node;
}

void NodeImp::cloneChildren(NodeImp* org)
{
    for (NodePtr node = org->firstChild; node; node = node->nextSibling) {
        NodePtr clone = std::static_pointer_cast<NodeImp>(node->cloneNode(true).self());
        appendChild(clone, true);
    }
}

bool NodeImp::isSameNode(Node other)
{
    return other.self().get() == this;
}

bool NodeImp::isEqualNode(Node arg)
{
    auto node = std::dynamic_pointer_cast<NodeImp>(arg.self());
    if (node.get() == this)
        return true;
    if (!node)
        return false;
    if (getNodeType() != node->getNodeType())
        return false;
    if (getChildCount() != node->getChildCount())
        return false;
    NodePtr x;
    NodePtr y;
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

NodeImp::NodeImp(DocumentImp* owner)
{
    if (owner)
        ownerDocument = std::dynamic_pointer_cast<DocumentImp>(owner->self());
}

NodeImp::NodeImp(const NodeImp& other) :
    ObjectMixin(other),
    ownerDocument(other.ownerDocument),
    nodeName(other.nodeName)
{
}

NodeImp::~NodeImp()
{
    while (0 < childCount)
        removeChild(firstChild);  // calling the internal removeChild()
}

}}}}  // org::w3c::dom::bootstrap
