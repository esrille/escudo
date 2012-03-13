/*
 * Copyright 2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLOLISTELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLOLISTELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLOListElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

class HTMLOListElementImp : public ObjectMixin<HTMLOListElementImp, HTMLElementImp>
{
public:
    HTMLOListElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"ol")
    {
    }
    HTMLOListElementImp(HTMLOListElementImp* org, bool deep) :
        ObjectMixin(org, deep)
    {
    }

    virtual void eval();

    // HTMLOListElement
    bool getReversed();
    void setReversed(bool reversed);
    int getStart();
    void setStart(int start);
    // HTMLOListElement-25
    bool getCompact();
    void setCompact(bool compact);
    std::u16string getType();
    void setType(std::u16string type);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLOListElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLOListElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLOLISTELEMENTIMP_H_INCLUDED
