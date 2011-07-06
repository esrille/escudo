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

#ifndef ELEMENT_IMP_H
#define ELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/views/ClientRectList.h>
#include <org/w3c/dom/views/ClientRect.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Attr.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>

#include <deque>

#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class ElementImp : public ObjectMixin<ElementImp, NodeImp>
{
    friend class AttrArray;

private:
    std::deque<Attr> attributes;
    std::u16string namespaceURI;
    std::u16string prefix;
    std::u16string localName;
public:
    ElementImp(DocumentImp* ownerDocument, const std::u16string& localName, std::u16string namespaceURI, std::u16string prefix = u"") :
        ObjectMixin(ownerDocument),
        namespaceURI(namespaceURI),
        prefix(prefix),
        localName(localName) {
        nodeName = getTagName();
    }
    ElementImp(ElementImp* org, bool deep);

    void setAttributes(const std::deque<Attr>& attributes) {
        this->attributes = attributes;
    }

    ElementImp* getNextElement();

    // Node
    virtual unsigned short getNodeType();
    virtual Node cloneNode(bool deep);
    virtual Nullable<std::u16string> getTextContent();
    virtual void setTextContent(std::u16string textContent);

    // Element
    virtual Nullable<std::u16string> getNamespaceURI() __attribute__((weak));
    virtual Nullable<std::u16string> getPrefix() __attribute__((weak));
    virtual std::u16string getLocalName() __attribute__((weak));
    virtual std::u16string getTagName() __attribute__((weak));
    virtual ObjectArray<Attr> getAttributes() __attribute__((weak));
    virtual Nullable<std::u16string> getAttribute(std::u16string qualifiedName) __attribute__((weak));
    virtual Nullable<std::u16string> getAttributeNS(std::u16string _namespace, std::u16string localName) __attribute__((weak));
    virtual void setAttribute(std::u16string qualifiedName, std::u16string value) __attribute__((weak));
    virtual void setAttributeNS(std::u16string _namespace, std::u16string qualifiedName, std::u16string value) __attribute__((weak));
    virtual void removeAttribute(std::u16string qualifiedName) __attribute__((weak));
    virtual void removeAttributeNS(std::u16string _namespace, std::u16string localName) __attribute__((weak));
    virtual bool hasAttribute(std::u16string qualifiedName) __attribute__((weak));
    virtual bool hasAttributeNS(std::u16string _namespace, std::u16string localName) __attribute__((weak));
    virtual NodeList getElementsByTagName(std::u16string qualifiedName) __attribute__((weak));
    virtual NodeList getElementsByTagNameNS(std::u16string _namespace, std::u16string localName) __attribute__((weak));
    virtual NodeList getElementsByClassName(std::u16string classNames) __attribute__((weak));
    virtual html::HTMLCollection getChildren() __attribute__((weak));
    virtual Element getFirstElementChild() __attribute__((weak));
    virtual Element getLastElementChild() __attribute__((weak));
    virtual Element getPreviousElementSibling() __attribute__((weak));
    virtual Element getNextElementSibling() __attribute__((weak));
    virtual unsigned int getChildElementCount() __attribute__((weak));
    // Element-3
    virtual views::ClientRectList getClientRects() __attribute__((weak));
    virtual views::ClientRect getBoundingClientRect() __attribute__((weak));
    virtual int getScrollTop() __attribute__((weak));
    virtual void setScrollTop(int scrollTop) __attribute__((weak));
    virtual int getScrollLeft() __attribute__((weak));
    virtual void setScrollLeft(int scrollLeft) __attribute__((weak));
    virtual int getScrollWidth() __attribute__((weak));
    virtual int getScrollHeight() __attribute__((weak));
    virtual int getClientTop() __attribute__((weak));
    virtual int getClientLeft() __attribute__((weak));
    virtual int getClientWidth() __attribute__((weak));
    virtual int getClientHeight() __attribute__((weak));
    // NodeSelector
    virtual Element querySelector(std::u16string selectors) __attribute__((weak));
    virtual NodeList querySelectorAll(std::u16string selectors) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Element::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Element::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ELEMENT_IMP_H
