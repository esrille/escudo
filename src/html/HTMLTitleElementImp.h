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

#ifndef HTMLTITLEELEMENT_IMP_H
#define HTMLTITLEELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLTitleElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLTitleElementImp : public ObjectMixin<HTMLTitleElementImp, HTMLElementImp>
{
public:
    // Node
    virtual Node cloneNode(bool deep);

    // HTMLTitleElement
    virtual std::u16string getText();
    virtual void setText(std::u16string text);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLTitleElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLTitleElement::getMetaData();
    }

    HTMLTitleElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"title") {
    }
    HTMLTitleElementImp(HTMLTitleElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLTITLEELEMENT_IMP_H
