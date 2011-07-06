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

#ifndef DOCUMENTTYPE_IMP_H
#define DOCUMENTTYPE_IMP_H

#include <Object.h>
#include <org/w3c/dom/DocumentType.h>

#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class DocumentTypeImp : public ObjectMixin<DocumentTypeImp, NodeImp>
{
    std::u16string publicId;
    std::u16string systemId;

public:
    DocumentTypeImp(std::u16string qualifiedName, Nullable<std::u16string> publicId, Nullable<std::u16string> systemId) :
        ObjectMixin(static_cast<DocumentImp*>(0)),  // TODO: use nullptr with gcc 4.6
        publicId(publicId.hasValue() ? publicId.value() : u""),
        systemId(systemId.hasValue() ? systemId.value() : u"") {
        nodeName = qualifiedName;
    }

    // Node - override
    virtual unsigned short getNodeType();

    // DocumentType
    virtual std::u16string getName() __attribute__((weak));
    virtual std::u16string getPublicId() __attribute__((weak));
    virtual std::u16string getSystemId() __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DocumentType::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DocumentType::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // DOCUMENTTYPE_IMP_H
