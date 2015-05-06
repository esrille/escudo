/*
 * Copyright 2010-2015 Esrille Inc.
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

#include "CharacterDataImp.h"
#include "MutationEventImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

void CharacterDataImp::dispatchMutationEvent(const std::u16string& prev)
{
    events::MutationEvent event = std::make_shared<MutationEventImp>();
    event.initMutationEvent(u"DOMCharacterDataModified",
                            true, false, getParentNode(), prev, data, u"", 0);
    dispatchEvent(event);
}


// Node
Nullable<std::u16string> CharacterDataImp::getTextContent()
{
    return getData();
}

void CharacterDataImp::setTextContent(const Nullable<std::u16string>& textContent)
{
    if (textContent.hasValue())
        setData(textContent.value());
}

bool CharacterDataImp::isEqualNode(Node arg)
{
    auto characterData = std::dynamic_pointer_cast<CharacterDataImp>(arg.self());
    if (self() == characterData)
        return true;
    if (!characterData)
        return false;
    if (data != characterData->data)
        return false;
    return NodeImp::isEqualNode(arg);
}

// CharacterData
std::u16string CharacterDataImp::getData()
{
    return data;
}

void CharacterDataImp::setData(const std::u16string& data)
{
    std::u16string prev = this->data;
    this->data = data;
    dispatchMutationEvent(prev);
}

unsigned int CharacterDataImp::getLength()
{
    return data.length();
}

std::u16string CharacterDataImp::substringData(unsigned int offset, unsigned int count)
{
    if (data.length() <= offset)    // TODO: Check html4/table-anonymous-objects-159.htm
        return u"";
    return data.substr(offset, count);
}

void CharacterDataImp::appendData(const std::u16string& arg)
{
    std::u16string prev = this->data;
    data += arg;
    dispatchMutationEvent(prev);
}

void CharacterDataImp::insertData(unsigned int offset, const std::u16string& arg)
{
    std::u16string prev = this->data;
    data.insert(offset, arg);
    dispatchMutationEvent(prev);
}

void CharacterDataImp::deleteData(unsigned int offset, unsigned int count)
{
    std::u16string prev = this->data;
    data.erase(offset, count);
    dispatchMutationEvent(prev);
}

void CharacterDataImp::replaceData(unsigned int offset, unsigned int count, const std::u16string& arg)
{
    std::u16string prev = this->data;
    data.replace(offset, count, arg);
    dispatchMutationEvent(prev);
}

}}}}  // org::w3c::dom::bootstrap