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

#ifndef DOMIMPLEMENTATION_IMP_H
#define DOMIMPLEMENTATION_IMP_H

#include <Object.h>
#include <org/w3c/dom/DOMImplementation.h>
#include <org/w3c/dom/css/CSSStyleSheet.h>
#include <org/w3c/dom/DOMException.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/DocumentType.h>

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DOMImplementationImp : public ObjectMixin<DOMImplementationImp>
{
    css::CSSStyleSheet defaultCSSStyleSheet;

public:
    DOMImplementationImp();

    css::CSSStyleSheet getDefaultCSSStyleSheet() {
        return defaultCSSStyleSheet;
    }
    void setDefaultCSSStyleSheet(css::CSSStyleSheet sheet) {
        defaultCSSStyleSheet = sheet;
    }

    // DOMImplementation
    virtual bool hasFeature(std::u16string feature, std::u16string version) __attribute__((weak));
    virtual DocumentType createDocumentType(std::u16string qualifiedName, std::u16string publicId, std::u16string systemId) __attribute__((weak));
    virtual Document createDocument(std::u16string _namespace, std::u16string qualifiedName, DocumentType doctype) __attribute__((weak));
    virtual Document createHTMLDocument(std::u16string title) __attribute__((weak));
    // DOMImplementationCSS
    virtual css::CSSStyleSheet createCSSStyleSheet(std::u16string title, std::u16string media) throw(DOMException) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DOMImplementation::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DOMImplementation::getMetaData();
    }
};

DOMImplementationImp* getDOMImplementation();

}}}}  // org::w3c::dom::bootstrap

#endif  // DOMIMPLEMENTATION_IMP_H
