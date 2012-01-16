/*
 * Copyright 2010-2012 Esrille Inc.
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
#include "DocumentImp.h"
#include "DocumentTypeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

bool DOMImplementationImp::hasFeature(std::u16string feature, std::u16string version)
{
    // TODO: implement me!
    return 0;
}

DocumentType DOMImplementationImp::createDocumentType(std::u16string qualifiedName, std::u16string publicId, std::u16string systemId)
{
    return new(std::nothrow) DocumentTypeImp(qualifiedName, publicId, systemId);
}

Document DOMImplementationImp::createDocument(std::u16string namespaceURI, std::u16string qualifiedName, DocumentType doctype)
{
    return new(std::nothrow) DocumentImp();
}

Document DOMImplementationImp::createHTMLDocument(std::u16string title)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

css::CSSStyleSheet DOMImplementationImp::createCSSStyleSheet(std::u16string title, std::u16string media) throw(DOMException)
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

DOMImplementationImp::DOMImplementationImp() :
    defaultCSSStyleSheet(0),
    userCSSStyleSheet(0)
{
}

DOMImplementationImp* getDOMImplementation()
{
    static Retained<DOMImplementationImp> domImplementation;
    return &domImplementation;
}

}}}}  // org::w3c::dom::bootstrap
