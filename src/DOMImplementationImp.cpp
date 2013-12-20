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

CSSStyleSheetPtr DOMImplementationImp::getDefaultStyleSheet() const
{
    return std::dynamic_pointer_cast<CSSStyleSheetImp>(defaultStyleSheet.self());
}

void DOMImplementationImp::setDefaultStyleSheet(css::CSSStyleSheet sheet)
{
    defaultStyleSheet = sheet;
}

CSSStyleSheetPtr DOMImplementationImp::getPresentationalHints() const
{
    return std::dynamic_pointer_cast<CSSStyleSheetImp>(presHintsStyleSheet.self());
}

void DOMImplementationImp::setPresentationalHints(css::CSSStyleSheet sheet)
{
    presHintsStyleSheet = sheet;
}

CSSStyleSheetPtr DOMImplementationImp::getUserStyleSheet() const
{
    return std::dynamic_pointer_cast<CSSStyleSheetImp>(userStyleSheet.self());
}

void DOMImplementationImp::setUserStyleSheet(css::CSSStyleSheet sheet)
{
    userStyleSheet = sheet;
}

DocumentType DOMImplementationImp::createDocumentType(const std::u16string& qualifiedName, const std::u16string& publicId, const std::u16string& systemId)
{
    return std::make_shared<DocumentTypeImp>(qualifiedName, publicId, systemId);    // TODO: set node document
}

XMLDocument DOMImplementationImp::createDocument(const Nullable<std::u16string>& _namespace, const std::u16string& qualifiedName, DocumentType doctype)
{
    return std::make_shared<DocumentImp>();  // TODO: Create XMLDocumentImp once createHTMLDocument is supported
}

Document DOMImplementationImp::createHTMLDocument()
{
    return createHTMLDocument(u"");
}

Document DOMImplementationImp::createHTMLDocument(const std::u16string& title)
{
    try {
        DocumentPtr document = std::make_shared<DocumentImp>();
        document->setContentType(u"text/html");
        DocumentType doctype = createDocumentType(u"html", u"", u"");    // TODO: set node document
        document->appendChild(doctype);
        Element html = document->createElement(u"html");
        document->appendChild(html);
        Element head = document->createElement(u"head");
        html.appendChild(head);
        if (!title.empty()) {
            Element t = document->createElement(u"title");
            head.appendChild(t);
            Text text = document->createTextNode(title);
            t.appendChild(text);
        }
        Element body = document->createElement(u"body");
        html.appendChild(body);
        // TODO: Step 8.
        return document;
    } catch (...) {
        return nullptr;
    }
}

bool DOMImplementationImp::hasFeature(const std::u16string& feature, const std::u16string& version)
{
    // TODO: implement me!
    return 0;
}

css::CSSStyleSheet DOMImplementationImp::createCSSStyleSheet(const std::u16string& title, const std::u16string& media) throw(DOMException)
{
    // TODO: implement me!
    return nullptr;
}

DOMImplementationPtr getDOMImplementation()
{
    static Retained<DOMImplementationImp> domImplementation;
    return domImplementation;
}

}}}}  // org::w3c::dom::bootstrap
