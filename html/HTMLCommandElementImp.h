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

#ifndef HTMLCOMMANDELEMENT_IMP_H
#define HTMLCOMMANDELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLCommandElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLCommandElementImp : public ObjectMixin<HTMLCommandElementImp, HTMLElementImp>
{
public:
    // Node
    virtual Node cloneNode(bool deep);

    // HTMLCommandElement
    virtual std::u16string getType();
    virtual void setType(std::u16string type);
    virtual std::u16string getLabel();
    virtual void setLabel(std::u16string label);
    virtual std::u16string getIcon();
    virtual void setIcon(std::u16string icon);
    virtual bool getDisabled();
    virtual void setDisabled(bool disabled);
    virtual bool getChecked();
    virtual void setChecked(bool checked);
    virtual std::u16string getRadiogroup();
    virtual void setRadiogroup(std::u16string radiogroup);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLCommandElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLCommandElement::getMetaData();
    }

    HTMLCommandElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"command") {
    }
    HTMLCommandElementImp(HTMLCommandElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLCOMMANDELEMENT_IMP_H
