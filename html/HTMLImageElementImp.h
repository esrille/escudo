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

#ifndef HTMLIMAGEELEMENT_IMP_H
#define HTMLIMAGEELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLImageElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLImageElementImp : public ObjectMixin<HTMLImageElementImp, HTMLElementImp>
{
public:
    // Node
    virtual Node cloneNode(bool deep);
    // HTMLImageElement
    virtual std::u16string getAlt();
    virtual void setAlt(std::u16string alt);
    virtual std::u16string getSrc();
    virtual void setSrc(std::u16string src);
    virtual std::u16string getUseMap();
    virtual void setUseMap(std::u16string useMap);
    virtual bool getIsMap();
    virtual void setIsMap(bool isMap);
    virtual unsigned int getWidth();
    virtual void setWidth(unsigned int width);
    virtual unsigned int getHeight();
    virtual void setHeight(unsigned int height);
    virtual unsigned int getNaturalWidth();
    virtual unsigned int getNaturalHeight();
    virtual bool getComplete();
    // HTMLImageElement-17
    virtual std::u16string getName();
    virtual void setName(std::u16string name);
    virtual std::u16string getAlign();
    virtual void setAlign(std::u16string align);
    virtual std::u16string getBorder();
    virtual void setBorder(std::u16string border);
    virtual unsigned int getHspace();
    virtual void setHspace(unsigned int hspace);
    virtual std::u16string getLongDesc();
    virtual void setLongDesc(std::u16string longDesc);
    virtual unsigned int getVspace();
    virtual void setVspace(unsigned int vspace);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLImageElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLImageElement::getMetaData();
    }

    HTMLImageElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"img") {
    }
    HTMLImageElementImp(HTMLImageElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLIMAGEELEMENT_IMP_H
