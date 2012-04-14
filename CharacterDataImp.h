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

#ifndef CHARACTERDATA_IMP_H
#define CHARACTERDATA_IMP_H

#include <Object.h>
#include <org/w3c/dom/CharacterData.h>

#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class CharacterDataImp : public ObjectMixin<CharacterDataImp, NodeImp>
{
    std::u16string data;

public:
    CharacterDataImp(DocumentImp* ownerDocument, const std::u16string& data) :
        ObjectMixin(ownerDocument),
        data(data) {
    }
    CharacterDataImp(CharacterDataImp* org, bool deep) :
        ObjectMixin(org, deep),
        data(org->data)
    {}

    // Node - override
    virtual Node cloneNode(bool deep) {
        return new(std::nothrow) CharacterDataImp(this, deep);
    }
    virtual Nullable<std::u16string> getTextContent();
    virtual void setTextContent(Nullable<std::u16string> textContent);

    // CharacterData
    virtual std::u16string getData();
    virtual void setData(std::u16string data);
    virtual unsigned int getLength();
    virtual std::u16string substringData(unsigned int offset, unsigned int count);
    virtual void appendData(std::u16string data);
    virtual void insertData(unsigned int offset, std::u16string data);
    virtual void deleteData(unsigned int offset, unsigned int count);
    virtual void replaceData(unsigned int offset, unsigned int count, std::u16string data);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return CharacterData::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return CharacterData::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // CHARACTERDATA_IMP_H
