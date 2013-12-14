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

#ifndef HTMLSPANELEMENT_IMP_H
#define HTMLSPANELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLSpanElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLSpanElementImp : public ObjectMixin<HTMLSpanElementImp, HTMLElementImp>
{
public:
    HTMLSpanElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"span")
    {
    }

    // Node - override
    virtual Node cloneNode(bool deep = true) {
        auto node = std::make_shared<HTMLSpanElementImp>(*this);
        node->cloneAttributes(this);
        if (deep)
            node->cloneChildren(this);
        return node;
    }

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLSpanElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLSpanElement::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLSPANELEMENT_IMP_H
