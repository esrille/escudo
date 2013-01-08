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

#ifndef ORG_W3C_DOM_BOOTSTRAP_ELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_ELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Element.h>
#include "NodeImp.h"

#include <org/w3c/dom/css/CSSStyleDeclaration.h>
#include <org/w3c/dom/views/ClientRectList.h>
#include <org/w3c/dom/views/ClientRect.h>
#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/Attr.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/DOMTokenList.h>
#include <org/w3c/dom/xbl2/XBLImplementationList.h>

#include <deque>

#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLCollectionImp;

class ElementImp : public ObjectMixin<ElementImp, NodeImp>
{
    friend class AttrArray;
    friend class ViewCSSImp;

private:
    std::deque<Attr> attributes;
    std::u16string namespaceURI;
    std::u16string prefix;
    std::u16string localName;

public:
    ElementImp(DocumentImp* ownerDocument, const std::u16string& localName, const std::u16string& namespaceURI, const std::u16string& prefix = u"");
    ElementImp(ElementImp* org, bool deep);

    void setAttributes(const std::deque<Attr>& attributes);
    ElementImp* getNextElement(ElementImp* root = 0);

    // Node
    virtual unsigned short getNodeType();
    virtual Node cloneNode(bool deep = true);
    virtual Nullable<std::u16string> getTextContent();
    virtual void setTextContent(const std::u16string& textContent);
    virtual bool isEqualNode(Node arg);

    // Element
    virtual Nullable<std::u16string> getNamespaceURI();
    virtual Nullable<std::u16string> getPrefix();
    virtual std::u16string getLocalName();
    virtual std::u16string getTagName();
    virtual std::u16string getId();
    virtual void setId(const std::u16string& id);
    virtual std::u16string getClassName();
    virtual void setClassName(const std::u16string& className);
    virtual DOMTokenList getClassList();
    virtual ObjectArray<Attr> getAttributes();
    virtual Nullable<std::u16string> getAttribute(const std::u16string& qualifiedName);
    virtual Nullable<std::u16string> getAttributeNS(const std::u16string& _namespace, const std::u16string& localName);
    virtual void setAttribute(const std::u16string& qualifiedName, const std::u16string& value);
    virtual void setAttributeNS(const std::u16string& _namespace, const std::u16string& qualifiedName, const std::u16string& value);
    virtual void removeAttribute(const std::u16string& qualifiedName);
    virtual void removeAttributeNS(const std::u16string& _namespace, const std::u16string& localName);
    virtual bool hasAttribute(const std::u16string& qualifiedName);
    virtual bool hasAttributeNS(const std::u16string& _namespace, const std::u16string& localName);
    virtual html::HTMLCollection getElementsByTagName(const std::u16string& localName);
    virtual html::HTMLCollection getElementsByTagNameNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName);
    virtual html::HTMLCollection getElementsByClassName(const std::u16string& classNames);
    virtual html::HTMLCollection getChildren();
    virtual Element getFirstElementChild();
    virtual Element getLastElementChild();
    virtual Element getPreviousElementSibling();
    virtual Element getNextElementSibling();
    virtual unsigned int getChildElementCount();
    // ElementCSSInlineStyle
    virtual css::CSSStyleDeclaration getStyle();
    // Element-49
    virtual views::ClientRectList getClientRects();
    virtual views::ClientRect getBoundingClientRect();
    virtual void scrollIntoView(bool top = true);
    virtual int getScrollTop();
    virtual void setScrollTop(int scrollTop);
    virtual int getScrollLeft();
    virtual void setScrollLeft(int scrollLeft);
    virtual int getScrollWidth();
    virtual int getScrollHeight();
    virtual int getClientTop();
    virtual int getClientLeft();
    virtual int getClientWidth();
    virtual int getClientHeight();
    // NodeSelector
    virtual Element querySelector(const std::u16string& selectors);
    virtual NodeList querySelectorAll(const std::u16string& selectors);
    // ElementXBL
    virtual xbl2::XBLImplementationList getXblImplementations();
    virtual void addBinding(const std::u16string& bindingURI);
    virtual void removeBinding(const std::u16string& bindingURI);
    virtual bool hasBinding(const std::u16string& bindingURI);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Element::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Element::getMetaData();
    }

    static HTMLCollectionImp* getElementsByTagName(ElementImp* element, const std::u16string& qualifiedName);
    static HTMLCollectionImp* getElementsByClassName(ElementImp* element, const std::u16string& classNames);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_ELEMENTIMP_H_INCLUDED
