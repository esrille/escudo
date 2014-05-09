/*
 * Copyright 2013, 2014 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_DOCUMENTFRAGMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_DOCUMENTFRAGMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/DocumentFragment.h>
#include "NodeImp.h"

#include <org/w3c/dom/Node.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/NodeList.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class DocumentImp;
class NodeImp;

typedef std::shared_ptr<NodeImp> NodePtr;
typedef std::shared_ptr<DocumentImp> DocumentPtr;

class DocumentFragmentImp : public ObjectMixin<DocumentFragmentImp, NodeImp>
{
public:
    DocumentFragmentImp(const DocumentPtr& ownerDocument) :
        ObjectMixin(ownerDocument.get())
    {}
    DocumentFragmentImp(const DocumentFragmentImp& org) :
        ObjectMixin(org)
    {}

    // Node
    virtual unsigned short getNodeType();
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<DocumentFragmentImp>(*this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }
    virtual Nullable<std::u16string> getTextContent();
    virtual void setTextContent(const Nullable<std::u16string>& textContent);

    // DocumentFragment
    // NodeSelector
    Element querySelector(const std::u16string& selectors);
    NodeList querySelectorAll(const std::u16string& selectors);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DocumentFragment::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DocumentFragment::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_DOCUMENTFRAGMENTIMP_H_INCLUDED
