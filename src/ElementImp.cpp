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

#include "ElementImp.h"

#include <algorithm>
#include <memory>
#include <new>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "utf.h"
#include "Test.util.h"

#include "AttrImp.h"
#include "DocumentImp.h"
#include "DOMTokenListImp.h"
#include "MutationEventImp.h"
#include "NodeListImp.h"
#include "ObjectArrayImp.h"
#include "XMLDocumentImp.h"
#include "WindowProxy.h"
#include "css/CSSSerialize.h"
#include "html/HTMLCollectionImp.h"
#include "html/HTMLParser.h"
#include "html/HTMLTokenizer.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void ElementImp::setAttributes(const std::deque<Attr>& attributes)
{
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        setAttributeNS(attr.getNamespaceURI(), attr.getName(), attr.getValue());
    }
}

ElementPtr ElementImp::getNextElement(const ElementPtr& root)
{
    auto n = std::dynamic_pointer_cast<NodeImp>(self());
    for (auto i = n->firstChild; i; i = i->nextSibling) {
        if (i->getNodeType() == Node::ELEMENT_NODE)
            return std::dynamic_pointer_cast<ElementImp>(i);
    }
    while (n != root) {
        while (n->nextSibling) {
            n = n->nextSibling;
            if (n->getNodeType() == Node::ELEMENT_NODE)
                return std::dynamic_pointer_cast<ElementImp>(n);
        }
        n = n->getParent();
    }
    return nullptr;
}

// Node

unsigned short ElementImp::getNodeType()
{
    return Node::ELEMENT_NODE;
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

void ElementImp::setTextContent(const Nullable<std::u16string>& textContent)
{
    while (hasChildNodes())
        removeChild(getFirstChild());
    std::u16string content = static_cast<std::u16string>(textContent);
    if (!content.empty()) {
        if (auto owner = getOwnerDocumentImp()) {
            org::w3c::dom::Text text = owner->createTextNode(content);
            appendChild(text);
        }
    }
}

bool ElementImp::isEqualNode(Node arg)
{
    auto element = std::dynamic_pointer_cast<ElementImp>(arg.self());
    if (element.get() == this)
        return true;
    if (!element)
        return false;
    if (namespaceURI != element->namespaceURI)
        return false;
    if (prefix != element->prefix)
        return false;
    if (localName != element->localName)
        return false;
    if (attributes.size() != element->attributes.size())
        return false;
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        bool found = false;
        for (auto j = element->attributes.begin(); j != element->attributes.end(); ++j) {
            if (i->getLocalName() == j->getLocalName()) {
                if (i->getNamespaceURI() != j->getNamespaceURI())
                    break;
                if (i->getValue() != j->getValue())
                    break;
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return NodeImp::isEqualNode(arg);
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
    return nodeName;
}

std::u16string ElementImp::getId()
{
    return getAttribute(u"id");
}

void ElementImp::setId(const std::u16string& id)
{
    setAttribute(u"id", id);
}

std::u16string ElementImp::getClassName()
{
    return getAttribute(u"class");
}

void ElementImp::setClassName(const std::u16string& className)
{
    setAttribute(u"class", className);
}

DOMTokenList ElementImp::getClassList()
{
    return std::make_shared<DOMTokenListImp>(this, u"class");
}

dom::ObjectArray<Attr> ElementImp:: getAttributes()
{
    return std::make_shared<ObjectArrayImp<ElementImp, Attr, &ElementImp::attributes>>(std::static_pointer_cast<ElementImp>(self()));
}

Nullable<std::u16string> ElementImp::getAttribute(const std::u16string& name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
    std::u16string n(name);
        toLower(n);
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == n)
            return attr.getValue();
    }
    return Nullable<std::u16string>();
}

Nullable<std::u16string> ElementImp::getAttributeNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& localName)
{
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (static_cast<std::u16string>(attr.getNamespaceURI()) == static_cast<std::u16string>(namespaceURI) && attr.getLocalName() == localName)
            return attr.getValue();
    }
    return Nullable<std::u16string>();
}

void ElementImp::setAttribute(const std::u16string& name, const std::u16string& value)
{
    // TODO: If qualifiedName does not match the Name production in XML, raise an INVALID_CHARACTER_ERR exception and terminate these steps.
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
    std::u16string n(name);
        toLower(n);
    // TODO: If qualifiedName starts with "xmlns", raise a NAMESPACE_ERR and terminate these steps.
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == n) {
            std::u16string prevValue = attr.getValue();
            if (prevValue != value) {
                attr.setValue(value);
                events::MutationEvent event = std::make_shared<MutationEventImp>();
                event.initMutationEvent(u"DOMAttrModified",
                                        true, false, attr, prevValue, value, n, events::MutationEvent::MODIFICATION);
                this->dispatchEvent(event);
            }
            return;
        }
    }
    if (Attr attr = std::make_shared<AttrImp>(Nullable<std::u16string>(), Nullable<std::u16string>(), n, value)) {
        attributes.push_back(attr);
        events::MutationEvent event = std::make_shared<MutationEventImp>();
        event.initMutationEvent(u"DOMAttrModified",
                                true, false, attr, u"", value, n, events::MutationEvent::ADDITION);
        this->dispatchEvent(event);
    }
}

void ElementImp::setAttributeNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& name, const std::u16string& value)
{
    // TODO:
    // TODO:
    Nullable<std::u16string> prefix;
    std::u16string localName;
    size_t pos = name.find(u':');
    if (pos != std::u16string::npos) {
        prefix = name.substr(0, pos);
        localName = name.substr(pos + 1);
    } else
        localName = name;
/* TODO: The following code is only necessary in XML.
 *       cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/elements.html#global-attributes
    if (prefix.hasValue()) {
        if (static_cast<std::u16string>(namespaceURI).empty())
            throw DOMException{DOMException::NAMESPACE_ERR};
        if (prefix.value() == u"xml" && namespaceURI != u"http://www.w3.org/XML/1998/namespace")
            throw DOMException{DOMException::NAMESPACE_ERR};
    }
    if ((name == u"xmlns" || prefix.hasValue() && prefix.value() == u"xmlns") && namespaceURI != u"http://www.w3.org/2000/xmlns")
        throw DOMException{DOMException::NAMESPACE_ERR};
 */
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (static_cast<std::u16string>(attr.getNamespaceURI()) == static_cast<std::u16string>(namespaceURI) && attr.getLocalName() == localName) {
            std::u16string prevValue = attr.getValue();
            if (prevValue != value) {
                attr.setValue(value);
                // TODO: set prefix, too.

                events::MutationEvent event = std::make_shared<MutationEventImp>();
                event.initMutationEvent(u"DOMAttrModified",
                                        true, false, attr, prevValue, value, localName, events::MutationEvent::MODIFICATION);
                dispatchEvent(event);
            }
            return;
        }
    }
    if (Attr attr = std::make_shared<AttrImp>(namespaceURI, prefix, localName, value)) {
        attributes.push_back(attr);
        events::MutationEvent event = std::make_shared<MutationEventImp>();
        event.initMutationEvent(u"DOMAttrModified",
                                true, false, attr, u"", value, localName, events::MutationEvent::ADDITION);
        dispatchEvent(event);
    }
}

void ElementImp::removeAttribute(const std::u16string& name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
    std::u16string n(name);
        toLower(n);
    for (auto i = attributes.begin(); i != attributes.end();) {
        Attr attr = *i;
        if (attr.getName() == n) {
            events::MutationEvent event = std::make_shared<MutationEventImp>();
            event.initMutationEvent(u"DOMAttrModified",
                                    true, false, attr, attr.getValue(), u"", n, events::MutationEvent::REMOVAL);
            this->dispatchEvent(event);
            i = attributes.erase(i);
        } else
            ++i;
    }
}

void ElementImp::removeAttributeNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& localName)
{
    for (auto i = attributes.begin(); i != attributes.end();) {
        Attr attr = *i;
        if (static_cast<std::u16string>(attr.getNamespaceURI()) == static_cast<std::u16string>(namespaceURI) && attr.getLocalName() == localName) {
            events::MutationEvent event = std::make_shared<MutationEventImp>();
            event.initMutationEvent(u"DOMAttrModified",
                                    true, false, attr, attr.getValue(), u"", localName, events::MutationEvent::REMOVAL);
            this->dispatchEvent(event);
            i = attributes.erase(i);
        } else
            ++i;
    }
}

bool ElementImp::hasAttribute(const std::u16string& name)
{
    // TODO: If the context node is in the HTML namespace and its ownerDocument is an HTML document
    std::u16string n(name);
        toLower(n);
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (attr.getName() == n)
            return true;
    }
    return false;
}

bool ElementImp::hasAttributeNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& localName)
{
    for (auto i = attributes.begin(); i != attributes.end(); ++i) {
        Attr attr = *i;
        if (static_cast<std::u16string>(attr.getNamespaceURI()) == static_cast<std::u16string>(namespaceURI) && attr.getLocalName() == localName)
            return true;
    }
    return false;
}

html::HTMLCollection ElementImp::getChildren()
{
    // TODO: implement me!
    return nullptr;
}

HTMLCollectionPtr ElementImp::getElementsByTagName(const ElementPtr& element, const std::u16string& localName)
{
    HTMLCollectionPtr list = std::make_shared<HTMLCollectionImp>();
    if (!list)
        return nullptr;

    if (localName == u"*") {
        for (ElementPtr e = element; e; e = e->getNextElement())
            list->addItem(e);
    } else {
        // TODO: Support non HTML document
        for (ElementPtr e = element; e; e = e->getNextElement()) {
            if (e->getLocalName() == localName)
                list->addItem(e);
        }
    }
    return list;
}

html::HTMLCollection ElementImp::getElementsByTagName(const std::u16string& localName)
{
    return getElementsByTagName(std::static_pointer_cast<ElementImp>(self()), localName);
}

html::HTMLCollection ElementImp::getElementsByTagNameNS(const Nullable<std::u16string>& namespaceURI, const std::u16string& localName)
{
    // TODO: implement me!
    return nullptr;
}

HTMLCollectionPtr ElementImp::getElementsByClassName(const ElementPtr& element, const std::u16string& classNames)
{
    HTMLCollectionPtr list = std::make_shared<HTMLCollectionImp>();
    if (!list)
        return nullptr;

    std::vector<std::u16string> classes;
    boost::algorithm::split(classes, classNames, isSpace);
    for (ElementPtr e = element; e; e = e->getNextElement()) {
        std::u16string c = e->getClassName();
        std::vector<std::u16string> v;
        boost::algorithm::split(v, c, isSpace);
        bool notFound = false;
        for (auto i = classes.begin(); i != classes.end(); ++i) {
            if (std::find(v.begin(), v.end(), *i) == v.end()) {
                notFound = true;
                break;
            }
        }
        if (!notFound)
            list->addItem(e);
    }
    return list;
}

html::HTMLCollection ElementImp::getElementsByClassName(const std::u16string& classNames)
{
    return getElementsByClassName(std::static_pointer_cast<ElementImp>(self()), classNames);
}

Element ElementImp::getFirstElementChild()
{
    for (NodePtr n = this->firstChild; n; n = n->nextSibling) {
        if (std::dynamic_pointer_cast<ElementImp>(n))
            return n;
    }
    return nullptr;
}

Element ElementImp::getLastElementChild()
{
    for (NodePtr n = this->lastChild; n; n = n->previousSibling) {
        if (std::dynamic_pointer_cast<ElementImp>(n))
            return n;
    }
    return nullptr;
}

Element ElementImp::getPreviousElementSibling()
{
    NodePtr n = std::static_pointer_cast<ElementImp>(self());
    while (n = n->previousSibling) {
        if (std::dynamic_pointer_cast<ElementImp>(n))
            return n;
    }
    return nullptr;
}

Element ElementImp::getNextElementSibling()
{
    NodePtr n = std::static_pointer_cast<ElementImp>(self());
    while (n = n->nextSibling) {
        if (std::dynamic_pointer_cast<ElementImp>(n))
            return n;
    }
    return nullptr;
}

unsigned int ElementImp::getChildElementCount()
{
    unsigned int count = 0;
    for (NodePtr n = this->firstChild; n; n = n->nextSibling) {
        if (std::dynamic_pointer_cast<ElementImp>(n))
            ++count;
    }
    return count;
}

css::CSSStyleDeclaration ElementImp::getStyle()
{
    // TODO: implement me!
    return nullptr;
}

views::ClientRectList ElementImp::getClientRects()
{
    // TODO: implement me!
    return nullptr;
}

views::ClientRect ElementImp::getBoundingClientRect()
{
    // TODO: implement me!
    return nullptr;
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

std::u16string ElementImp::getInnerHTML()
{
    // TODO: implement me!
    return u"";
}

void ElementImp::setInnerHTML(const std::u16string& innerHTML)
{
    DocumentPtr document(std::make_shared<DocumentImp>());
    if (!document)
        return;
    HTMLParser::parseFragment(document, innerHTML, std::static_pointer_cast<ElementImp>(self()));
    Element root = document->getDocumentElement();
    if (!root)
        return;

    // TODO: Set suppress observers flag
    for (auto i = getFirstChild(); i; i = i.getNextSibling())
        removeChild(i);
    while (root.hasChildNodes()) {
        // Replace all
        auto i = root.getFirstChild();
        getOwnerDocumentImp()->adoptNode(i);
        appendChild(i);
    }
    // TODO: Unset suppress observers flag and queue events
}

std::u16string ElementImp::getOuterHTML()
{
    // TODO: implement me!
    return u"";
}

void ElementImp::setOuterHTML(const std::u16string& outerHTML)
{
    // TODO: implement me!
}

void ElementImp::insertAdjacentHTML(const insertAdjacentHTMLPosition& position, const std::u16string& text)
{
    // TODO: implement me!
}

Element ElementImp::querySelector(CSSSelectorsGroup* selectorsGroup, ViewCSSImp* view)
{
    if (selectorsGroup->evaluate(std::static_pointer_cast<ElementImp>(self()), view))
        return self();
    for (auto i = getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (auto imp = std::dynamic_pointer_cast<ElementImp>(i.self())) {
            if (Element e = imp->querySelector(selectorsGroup, view))
                return e;
        }
    }
    return nullptr;
}

Element ElementImp::querySelector(const std::u16string& selectors)
{
    CSSParser parser;
    std::unique_ptr<CSSSelectorsGroup> selectorsGroup(parser.parseSelectorsGroup(selectors));
    if (!selectorsGroup)
        return nullptr;

    if (!getOwnerDocumentImp())
        return nullptr;
    WindowProxyPtr window = getOwnerDocumentImp()->getDefaultWindow();
    if (!window)
        return nullptr;
    return querySelector(selectorsGroup.get(), window->getView());
}

void ElementImp::querySelectorAll(NodeListPtr nodeList, CSSSelectorsGroup* selectorsGroup, ViewCSSImp* view)
{
    if (selectorsGroup->evaluate(std::static_pointer_cast<ElementImp>(self()), view))
        nodeList->addItem(std::static_pointer_cast<ElementImp>(self()));
    for (auto i = getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (auto imp = std::dynamic_pointer_cast<ElementImp>(i.self()))
            imp->querySelectorAll(nodeList, selectorsGroup, view);
    }
}

NodeList ElementImp::querySelectorAll(const std::u16string& selectors)
{
    NodeListPtr nodeList = std::make_shared<NodeListImp>();
    if (!nodeList)
        return nullptr;

    CSSParser parser;
    std::unique_ptr<CSSSelectorsGroup> selectorsGroup(parser.parseSelectorsGroup(selectors));
    if (!selectorsGroup)
        return nodeList;

    if (!getOwnerDocumentImp())
        return nodeList;
    WindowProxyPtr window = getOwnerDocumentImp()->getDefaultWindow();
    if (!window)
        return nodeList;
    querySelectorAll(nodeList, selectorsGroup.get(), window->getView());
    return nodeList;
}

xbl2::XBLImplementationList ElementImp::getXblImplementations()
{
    // TODO: implement me!
    return nullptr;
}

void ElementImp::addBinding(const std::u16string& bindingURI)
{
    // TODO: implement me!
}

void ElementImp::removeBinding(const std::u16string& bindingURI)
{
    // TODO: implement me!
}

bool ElementImp::hasBinding(const std::u16string& bindingURI)
{
    // TODO: implement me!
    return false;
}

ElementImp::ElementImp(DocumentImp* ownerDocument, const std::u16string& localName, const std::u16string& namespaceURI, const std::u16string& prefix) :
    ObjectMixin(ownerDocument),
    namespaceURI(namespaceURI),
    prefix(prefix),
    localName(localName)
{
    // Set tagName to nodeName; cf. http://dom.spec.whatwg.org/#dom-node-nodename
    if (0 < prefix.length())
        nodeName = prefix + u':' + localName;
    else
        nodeName = localName;
    if (namespaceURI == u"http://www.w3.org/1999/xhtml" && ownerDocument && !dynamic_cast<XMLDocumentImp*>(ownerDocument))
        toUpper(nodeName);
}

ElementImp::ElementImp(const ElementImp& org) :
    ObjectMixin(org),
    namespaceURI(org.namespaceURI),
    prefix(org.prefix),
    localName(org.localName)
{
    for (auto i = org.attributes.begin(); i != org.attributes.end(); ++i) {
        AttrPtr attr = std::dynamic_pointer_cast<AttrImp>((*i).self());
        attributes.push_back(attr); // TODO: dup?
    }
}

}}}}  // org::w3c::dom::bootstrap
