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

#ifndef HTMLMETAELEMENT_IMP_H
#define HTMLMETAELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLMetaElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLMetaElementImp : public ObjectMixin<HTMLMetaElementImp, HTMLElementImp>
{
public:
    // Node
    virtual Node cloneNode(bool deep);

    // HTMLMetaElement
    virtual std::u16string getName();
    virtual void setName(std::u16string name);
    virtual std::u16string getHttpEquiv();
    virtual void setHttpEquiv(std::u16string httpEquiv);
    virtual std::u16string getContent();
    virtual void setContent(std::u16string content);
    // HTMLMetaElement-23
    virtual std::u16string getScheme();
    virtual void setScheme(std::u16string scheme);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLMetaElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLMetaElement::getMetaData();
    }

    HTMLMetaElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"meta") {
    }
    HTMLMetaElementImp(HTMLMetaElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLMETAELEMENT_IMP_H
