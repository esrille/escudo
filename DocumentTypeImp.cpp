/*
 * Copyright 2010 Esrille Inc.
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

#include "DocumentTypeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// DocumentType
std::u16string DocumentTypeImp::getName()
{
    return nodeName;
}

std::u16string DocumentTypeImp::getPublicId()
{
    return publicId;
}

std::u16string DocumentTypeImp::getSystemId()
{
    return systemId;
}

// Node - override
unsigned short DocumentTypeImp::getNodeType()
{
    return Node::DOCUMENT_TYPE_NODE;
}

}}}}  // org::w3c::dom::bootstrap
