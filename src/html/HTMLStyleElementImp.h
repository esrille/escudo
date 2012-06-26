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

#ifndef HTMLSTYLEELEMENT_IMP_H
#define HTMLSTYLEELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLStyleElement.h>

#include <org/w3c/dom/stylesheets/StyleSheet.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLStyleElementImp : public ObjectMixin<HTMLStyleElementImp, HTMLElementImp>
{
    std::u16string type;
    std::u16string media;
    bool scoped;
    stylesheets::StyleSheet styleSheet;
public:
    virtual void eval();

    // Node
    virtual Node cloneNode(bool deep);

    // HTMLStyleElement
    virtual bool getDisabled();
    virtual void setDisabled(bool disabled);
    virtual std::u16string getMedia();
    virtual void setMedia(std::u16string media);
    virtual std::u16string getType();
    virtual void setType(std::u16string type);
    virtual bool getScoped();
    virtual void setScoped(bool scoped);

    // LinkStyle
    virtual stylesheets::StyleSheet getSheet();

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLStyleElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLStyleElement::getMetaData();
    }

    HTMLStyleElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"style"),
        type(u"text/css"),
        media(u"all"),
        scoped(false),
        styleSheet(0) {
    }
    HTMLStyleElementImp(HTMLStyleElementImp* org, bool deep) :
        ObjectMixin(org, deep),
        type(org->type),
        media(org->media),
        scoped(org->scoped),
        styleSheet(org->styleSheet) {  // TODO: make a clone sheet, too?
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLSTYLEELEMENT_IMP_H
