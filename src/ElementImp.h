/*
 * Copyright 2010-2014 Esrille Inc.
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
#include <org/w3c/dom/DOMRectList.h>
#include <org/w3c/dom/DOMRect.h>
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

class CSSSelectorsGroup;
class HTMLCollectionImp;
class NodeListImp;
class ViewCSSImp;

typedef std::shared_ptr<HTMLCollectionImp> HTMLCollectionPtr;
typedef std::shared_ptr<NodeListImp> NodeListPtr;

class ElementImp;
typedef std::shared_ptr<ElementImp> ElementPtr;

class ElementImp : public ObjectMixin<ElementImp, NodeImp>
{
    friend class AttrArray;
    friend class ViewCSSImp;

    std::u16string namespaceURI;
    std::u16string prefix;
    std::u16string localName;
    std::deque<Attr> attributes;

    Element querySelector(CSSSelectorsGroup* selectorsGroup, ViewCSSImp* view);
    void querySelectorAll(NodeListPtr nodeList, CSSSelectorsGroup* selectorsGroup, ViewCSSImp* view);

protected:
    void cloneAttributes(const ElementImp* org);

public:
    ElementImp(DocumentImp* ownerDocument, const std::u16string& localName, const std::u16string& namespaceURI, const std::u16string& prefix = u"");
    ElementImp(const ElementImp& org);

    void setAttributes(const std::deque<Attr>& attributes);
    ElementPtr getNextElement(const ElementPtr& root = nullptr);

    // notify() is called when conditions that are not handled by DOM events
    // but still needed be processed occur; e.g., the element is popped off
    // the stack of open elements of an HTML parser.
    enum class NotificationType {
        Popped, // popped off the stack of open elements
    };
    virtual void notify(NotificationType type) {}

    // Node
    virtual unsigned short getNodeType();
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<ElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }
    virtual Nullable<std::u16string> getTextContent();
    virtual void setTextContent(const Nullable<std::u16string>& textContent);
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
    virtual Nullable<std::u16string> getAttribute(const std::u16string& name);
    virtual Nullable<std::u16string> getAttributeNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName);
    virtual void setAttribute(const std::u16string& name, const std::u16string& value);
    virtual void setAttributeNS(const Nullable<std::u16string>& _namespace, const std::u16string& name, const std::u16string& value);
    virtual void removeAttribute(const std::u16string& name);
    virtual void removeAttributeNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName);
    virtual bool hasAttribute(const std::u16string& name);
    virtual bool hasAttributeNS(const Nullable<std::u16string>& _namespace, const std::u16string& localName);
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
    virtual DOMRectList getClientRects();
    virtual DOMRect getBoundingClientRect();
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
    // Element-50
    virtual std::u16string getInnerHTML();
    virtual void setInnerHTML(const std::u16string& innerHTML);
    virtual std::u16string getOuterHTML();
    virtual void setOuterHTML(const std::u16string& outerHTML);
    virtual void insertAdjacentHTML(const insertAdjacentHTMLPosition& position, const std::u16string& text);
    // NodeSelector
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

    static HTMLCollectionPtr getElementsByTagName(const ElementPtr& element, const std::u16string& localName);
    static HTMLCollectionPtr getElementsByClassName(const ElementPtr& element, const std::u16string& classNames);
};

}}}}  // org::w3c::dom::bootstrap

#endif  // ORG_W3C_DOM_BOOTSTRAP_ELEMENTIMP_H_INCLUDED
