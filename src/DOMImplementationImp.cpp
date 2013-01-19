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

#include <new>

#include "DOMImplementationImp.h"
#include "DocumentTypeImp.h"
#include "XMLDocumentImp.h"
#include "css/CSSStyleSheetImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

DOMImplementationImp::DOMImplementationImp() :
    defaultStyleSheet(0),
    presHintsStyleSheet(0),
    userStyleSheet(0)
{
}

CSSStyleSheetImp* DOMImplementationImp::getDefaultStyleSheet() const
{
    return dynamic_cast<CSSStyleSheetImp*>(defaultStyleSheet.self());
}

void DOMImplementationImp::setDefaultStyleSheet(css::CSSStyleSheet sheet)
{
    defaultStyleSheet = sheet;
}

CSSStyleSheetImp* DOMImplementationImp::getPresentationalHints() const
{
    return dynamic_cast<CSSStyleSheetImp*>(presHintsStyleSheet.self());
}

void DOMImplementationImp::setPresentationalHints(css::CSSStyleSheet sheet)
{
    presHintsStyleSheet = sheet;
}

CSSStyleSheetImp* DOMImplementationImp::getUserStyleSheet() const
{
    return dynamic_cast<CSSStyleSheetImp*>(userStyleSheet.self());
}

void DOMImplementationImp::setUserStyleSheet(css::CSSStyleSheet sheet)
{
    userStyleSheet = sheet;
}

DocumentType DOMImplementationImp::createDocumentType(const std::u16string& qualifiedName, const std::u16string& publicId, const std::u16string& systemId)
{
    return new(std::nothrow) DocumentTypeImp(qualifiedName, publicId, systemId); // TODO: set node document
}

XMLDocument DOMImplementationImp::createDocument(const Nullable<std::u16string>& _namespace, const std::u16string& qualifiedName, DocumentType doctype)
{
    return new(std::nothrow) XMLDocumentImp();
}

Document DOMImplementationImp::createHTMLDocument()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

Document DOMImplementationImp::createHTMLDocument(const std::u16string& title)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

bool DOMImplementationImp::hasFeature(const std::u16string& feature, const std::u16string& version)
{
    // TODO: implement me!
    return 0;
}

css::CSSStyleSheet DOMImplementationImp::createCSSStyleSheet(const std::u16string& title, const std::u16string& media) throw(DOMException)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

DOMImplementationImp* getDOMImplementation()
{
    static Retained<DOMImplementationImp> domImplementation;
    return &domImplementation;
}

}}}}  // org::w3c::dom::bootstrap
