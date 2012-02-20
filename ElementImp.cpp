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

#include "ElementImp.h"

#include <new>

#include "AttrImp.h"
#include "DocumentImp.h"
#include "MutationEventImp.h"
#include "css/CSSSerialize.h"
#include "html/HTMLTokenizer.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class AttrArray : public Object
{
    ElementImp* element;
public:
    virtual unsigned int getLength() {
        return element->attributes.size();
    }
    virtual void setLength(unsigned int length) {
    }
    virtual Attr getElement(unsigned int index) {
        if (element->attributes.size() <= index)
            return 0;
        return element->attributes[index];
    }
    virtual void setElement(unsigned int index, Attr value) {
    }
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return ObjectArray<Attr>::dispatch(this, selector, id, argc, argv);
    }
    AttrArray(ElementImp* element) :
        Object(this),
        element(element) {
    }
};

void ElementImp::setAttributes(const std::deque<Attr>& attributes)
{
    if (this->attributes.empty()) {
        this->attributes = attributes;
        return;
    }
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        setAttributeNS(attr.getNamespaceURI(), attr.getName(), attr.getValue());
    }
}

ElementImp* ElementImp::getNextElement(ElementImp* root)
{
    NodeImp* n = this;
    for (auto i = n->firstChild; i; i = i->nextSibling) {
        if (i->getNodeType() == Node::ELEMENT_NODE)
            return dynamic_cast<ElementImp*>(i);
    }
    while (n != root) {
        while (n->nextSibling) {
            n = n->nextSibling;
            if (n->getNodeType() == Node::ELEMENT_NODE)
                return dynamic_cast<ElementImp*>(n);
        }
        n = n->parentNode;
    }
    return 0;
}

// Node

unsigned short ElementImp::getNodeType()
{
    return Node::ELEMENT_NODE;
}

Node ElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) ElementImp(this, deep);
}

Nullable<std::u16string> ElementImp::getTextContent()
{
    std::u16string content;
    for (Node child = getFirstChild(); child; child = child.getNextSibling()) {
        if (child.getNodeType() == Node::COMMENT_NODE)
            continue;
        Nullable<std::u16string> text = child.getTextContent();
        if (text.hasValue())
            content += text.value();
    }
    return content;
}

void ElementImp::setTextContent(std::u16string textContent)
{
    while (hasChildNodes())
        removeChild(getFirstChild());
    if (0 < textContent.length()) {
        org::w3c::dom::Text text = ownerDocument->createTextNode(textContent);
        appendChild(text);
    }
}

// Element
Nullable<std::u16string> ElementImp::getNamespaceURI()
{
    return namespaceURI;
}

Nullable<std::u16string> ElementImp::getPrefix()
{
    return prefix;
}

std::u16string ElementImp::getLocalName()
{
    return localName;
}

std::u16string ElementImp::getTagName()
{
    if (0 < prefix.length())
        return prefix + u':' + localName;
    return localName;
}

dom::ObjectArray<Attr> ElementImp:: getAttributes()
{
    return new(std::nothrow) AttrArray(this);
}

Nullable<std::u16string> ElementImp::getAttribute(std::u16string name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
        toLower(name);
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == name)
            return attr.getValue();
    }
    return Nullable<std::u16string>();
}

Nullable<std::u16string> ElementImp::getAttributeNS(std::u16string namespaceURI, std::u16string localName)
{
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getNamespaceURI().hasValue() && attr.getNamespaceURI().value() == namespaceURI && attr.getLocalName() == localName)
            return attr.getValue();
    }
    return Nullable<std::u16string>();
}

void ElementImp::setAttribute(std::u16string name, std::u16string value)
{
    // TODO: If qualifiedName does not match the Name production in XML, raise an INVALID_CHARACTER_ERR exception and terminate these steps.
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
        toLower(name);
    // TODO: If qualifiedName starts with "xmlns", raise a NAMESPACE_ERR and terminate these steps.
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == name) {
            std::u16string prevValue = attr.getValue();
            attr.setValue(value);

            events::MutationEvent event = new(std::nothrow) MutationEventImp;
            event.initMutationEvent(u"DOMAttrModified",
                                    true, false, attr, prevValue, value, name, events::MutationEvent::MODIFICATION);
            this->dispatchEvent(event);
            return;
        }
    }
    Attr attr = new(std::nothrow) AttrImp(Nullable<std::u16string>(), Nullable<std::u16string>(), name, value);
    if (attr)
        attributes.push_back(attr);
}

void ElementImp::setAttributeNS(std::u16string namespaceURI, std::u16string qualifiedName, std::u16string value)
{
    // TODO:
    // TODO:
    Nullable<std::u16string> prefix;
    std::u16string localName;
    size_t pos = qualifiedName.find(u':');
    if (pos != std::u16string::npos) {
        prefix = qualifiedName.substr(0, pos);
        localName = qualifiedName.substr(pos + 1);
    } else
        localName = qualifiedName;
    if (prefix.hasValue()) {
        if (namespaceURI.length() == 0)
            throw DOMException(DOMException::NAMESPACE_ERR);
        if (prefix.value() == u"xml" && namespaceURI != u"http://www.w3.org/XML/1998/namespace")
            throw DOMException(DOMException::NAMESPACE_ERR);
    }
    if ((qualifiedName == u"xmlns" || prefix.hasValue() && prefix.value() == u"xmlns") &&
        namespaceURI != u"http://www.w3.org/2000/xmlns") {
        throw DOMException(DOMException::NAMESPACE_ERR);
    }
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getNamespaceURI().hasValue() && attr.getNamespaceURI().value() == namespaceURI && attr.getLocalName() == localName) {
            attr.setValue(value);
            // TODO: set prefix, too.
            return;
        }
    }
    Attr attr = new(std::nothrow) AttrImp(namespaceURI, prefix, localName, value);
    if (attr)
        attributes.push_back(attr);
}

void ElementImp::removeAttribute(std::u16string name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
        toLower(name);
    for (auto i = attributes.begin(); i != attributes.end();) {
        Attr attr = *i;
        if (attr.getName() == name)
            i = attributes.erase(i);
        else
            ++i;
    }
}

void ElementImp::removeAttributeNS(std::u16string namespaceURI, std::u16string localName)
{
    for (auto i = attributes.begin(); i != attributes.end();) {
        Attr attr = *i;
        if (attr.getNamespaceURI().hasValue() && attr.getNamespaceURI().value() == namespaceURI && attr.getLocalName() == localName)
            i = attributes.erase(i);
        else
            ++i;
    }
}

bool ElementImp::hasAttribute(std::u16string name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
        toLower(name);
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == name)
            return true;
    }
    return false;
}

bool ElementImp::hasAttributeNS(std::u16string namespaceURI, std::u16string localName)
{
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getNamespaceURI().hasValue() && attr.getNamespaceURI().value() == namespaceURI && attr.getLocalName() == localName)
            return true;
    }
    return false;
}

html::HTMLCollection ElementImp::getChildren()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList ElementImp::getElementsByTagName(std::u16string qualifiedName)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList ElementImp::getElementsByTagNameNS(std::u16string _namespace, std::u16string localName)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList ElementImp::getElementsByClassName(std::u16string classNames)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Element ElementImp::getFirstElementChild()
{
    for (NodeImp* n = this->firstChild; n; n = n->nextSibling) {
        if (dynamic_cast<ElementImp*>(n))
            return n;
    }
    return 0;
}

Element ElementImp::getLastElementChild()
{
    for (NodeImp* n = this->lastChild; n; n = n->previousSibling) {
        if (dynamic_cast<ElementImp*>(n))
            return n;
    }
    return 0;
}

Element ElementImp::getPreviousElementSibling()
{
    NodeImp* n = this;
    while (n = n->previousSibling) {
        if (dynamic_cast<ElementImp*>(n))
            return n;
    }
    return 0;
}

Element ElementImp::getNextElementSibling()
{
    NodeImp* n = this;
    while (n = n->nextSibling) {
        if (dynamic_cast<ElementImp*>(n))
            return n;
    }
    return 0;
}

unsigned int ElementImp::getChildElementCount()
{
    unsigned int count = 0;
    for (NodeImp* n = this->firstChild; n; n = n->nextSibling) {
        if (dynamic_cast<ElementImp*>(n))
            ++count;
    }
    return count;
}

views::ClientRectList ElementImp::getClientRects()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

views::ClientRect ElementImp::getBoundingClientRect()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void ElementImp::scrollIntoView(bool top)
{
    // TODO: implement me!
}

int ElementImp::getScrollTop()
{
    // TODO: implement me!
    return 0;
}

void ElementImp::setScrollTop(int scrollTop)
{
    // TODO: implement me!
}

int ElementImp::getScrollLeft()
{
    // TODO: implement me!
    return 0;
}

void ElementImp::setScrollLeft(int scrollLeft)
{
    // TODO: implement me!
}

int ElementImp::getScrollWidth()
{
    // TODO: implement me!
    return 0;
}

int ElementImp::getScrollHeight()
{
    // TODO: implement me!
    return 0;
}

int ElementImp::getClientTop()
{
    // TODO: implement me!
    return 0;
}

int ElementImp::getClientLeft()
{
    // TODO: implement me!
    return 0;
}

int ElementImp::getClientWidth()
{
    // TODO: implement me!
    return 0;
}

int ElementImp::getClientHeight()
{
    // TODO: implement me!
    return 0;
}

Element ElementImp::querySelector(std::u16string selectors)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

NodeList ElementImp::querySelectorAll(std::u16string selectors)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

ElementImp::ElementImp(ElementImp* org, bool deep) :
    ObjectMixin(org, deep) {
    namespaceURI = org->namespaceURI;
    prefix = org->prefix;
    localName = org->localName;
    for (auto i = org->attributes.begin(); i != org->attributes.end(); ++i) {
        if (Attr attr = new(std::nothrow) AttrImp(*dynamic_cast<AttrImp*>((*i).self())))
            attributes.push_back(attr);
    }
}

}}}}  // org::w3c::dom::bootstrap
