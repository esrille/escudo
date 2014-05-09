/*
 * Copyright 2013, 2014 Esrille Inc.
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

#include "DocumentFragmentImp.h"

#include "DocumentImp.h"
#include "NodeListImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

// Node

unsigned short DocumentFragmentImp::getNodeType()
{
    return Node::DOCUMENT_FRAGMENT_NODE;
}

Nullable<std::u16string> DocumentFragmentImp::getTextContent()
{
    std::u16string content;
    for (Node child = getFirstChild(); child; child = child.getNextSibling()) {
        switch (child.getNodeType()) {
        case Node::ELEMENT_NODE:
        case Node::TEXT_NODE:
        case Node::DOCUMENT_FRAGMENT_NODE: {
            Nullable<std::u16string> text = child.getTextContent();
            if (text.hasValue())
                content += text.value();
            break;
        }
        default:
            break;
        }
    }
    return content;
}

void DocumentFragmentImp::setTextContent(const Nullable<std::u16string>& textContent)
{
    while (hasChildNodes())
        removeChild(getFirstChild());
    std::u16string content = static_cast<std::u16string>(textContent);
    if (!content.empty()) {
        if (auto owner = getOwnerDocumentImp()) {
            org::w3c::dom::Text text = owner->createTextNode(content);
            appendChild(text);
        }
    }
}

Element DocumentFragmentImp::querySelector(const std::u16string& selectors)
{
    // TODO: implement me!
    return nullptr;
}

NodeList DocumentFragmentImp::querySelectorAll(const std::u16string& selectors)
{
    // TODO: implement me!
    return std::make_shared<NodeListImp>();
}

}
}
}
}
