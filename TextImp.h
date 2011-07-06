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

#ifndef TEXT_IMP_H
#define TEXT_IMP_H

#include <Object.h>
#include <org/w3c/dom/Text.h>

#include "CharacterDataImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class TextImp : public ObjectMixin<TextImp, CharacterDataImp>
{
public:
    TextImp(DocumentImp* ownerDocument, const std::u16string& data) :
        ObjectMixin(ownerDocument, data) {
        nodeName = u"#text";
    }

    // Node - override
    virtual unsigned short getNodeType();

    // Text
    virtual Text splitText(unsigned int offset) __attribute__((weak));
    virtual std::u16string getWholeText() __attribute__((weak));
    virtual Text replaceWholeText(std::u16string data) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Text::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Text::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // TEXT_IMP_H
