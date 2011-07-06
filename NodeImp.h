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

#ifndef NODE_IMP_H
#define NODE_IMP_H

#include <assert.h>

#include <Object.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventListener.h>
#include <org/w3c/dom/events/EventException.h>
#include <org/w3c/dom/DOMException.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/NodeList.h>

#include "EventTargetImp.h"

#include <list>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentImp;

class NodeImp : public ObjectMixin<NodeImp, EventTargetImp>
{
    friend class NodeListImp;
    friend class ElementImp;
    friend class EventTargetImp;
    friend class HTMLElementImp;  // for focus

    DocumentImp* ownerDocument;
    NodeImp* parentNode;
    NodeImp* firstChild;
    NodeImp* lastChild;
    NodeImp* previousSibling;
    NodeImp* nextSibling;
    unsigned int childCount;

    NodeImp* removeChild(NodeImp* item);
    NodeImp* appendChild(NodeImp* item);
    NodeImp* insertBefore(NodeImp* item, NodeImp* after);

protected:
    std::u16string nodeName;

public:
    NodeImp(DocumentImp* ownerDocument);
    NodeImp(NodeImp* org, bool deep);
    ~NodeImp();

    virtual void eval();

    // Returns true if this is an ancestor of the node
    bool isAncestorOf(NodeImp* node)
    {
        for (NodeImp* parent = node->parentNode; parent; parent = parent->parentNode) {
            if (this == parent)
                return true;
        }
        return false;
    }

    DocumentImp* getOwnerDocumentImp() const {
        return ownerDocument;
    }
    void setOwnerDocument(DocumentImp* document);

    unsigned int getChildCount()
    {
        return childCount;
    }

    // Node
    virtual unsigned short getNodeType() __attribute__((weak));
    virtual std::u16string getNodeName() __attribute__((weak));
    virtual Nullable<std::u16string> getBaseURI() __attribute__((weak));
    virtual Document getOwnerDocument() __attribute__((weak));
    virtual Node getParentNode() __attribute__((weak));
    virtual Element getParentElement() __attribute__((weak));
    virtual bool hasChildNodes() __attribute__((weak));
    virtual NodeList getChildNodes() __attribute__((weak));
    virtual Node getFirstChild() __attribute__((weak));
    virtual Node getLastChild() __attribute__((weak));
    virtual Node getPreviousSibling() __attribute__((weak));
    virtual Node getNextSibling() __attribute__((weak));
    virtual unsigned short compareDocumentPosition(Node other) __attribute__((weak));
    virtual Nullable<std::u16string> getNodeValue() __attribute__((weak));
    virtual void setNodeValue(Nullable<std::u16string> nodeValue) __attribute__((weak));
    virtual Nullable<std::u16string> getTextContent() __attribute__((weak));
    virtual void setTextContent(Nullable<std::u16string> textContent) __attribute__((weak));
    virtual Node insertBefore(Node newChild, Node refChild) __attribute__((weak));
    virtual Node replaceChild(Node newChild, Node oldChild) __attribute__((weak));
    virtual Node removeChild(Node oldChild) __attribute__((weak));
    virtual Node appendChild(Node newChild) __attribute__((weak));
    virtual Node cloneNode(bool deep) __attribute__((weak));
    virtual bool isSameNode(Node node) __attribute__((weak));
    virtual bool isEqualNode(Node node) __attribute__((weak));
    virtual std::u16string lookupPrefix(std::u16string _namespace) __attribute__((weak));
    virtual std::u16string lookupNamespaceURI(Nullable<std::u16string> prefix) __attribute__((weak));
    virtual bool isDefaultNamespace(std::u16string _namespace) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Node::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Node::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // NODE_IMP_H
