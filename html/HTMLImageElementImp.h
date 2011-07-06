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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLImageElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/html/HTMLImageElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLImageElementImp : public ObjectMixin<HTMLImageElementImp, HTMLElementImp>
{
public:
    HTMLImageElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"img") {
    }
    HTMLImageElementImp(HTMLImageElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }

    // Node
    virtual Node cloneNode(bool deep);

    // HTMLImageElement
    std::u16string getAlt() __attribute__((weak));
    void setAlt(std::u16string alt) __attribute__((weak));
    std::u16string getSrc() __attribute__((weak));
    void setSrc(std::u16string src) __attribute__((weak));
    std::u16string getCrossOrigin() __attribute__((weak));
    void setCrossOrigin(std::u16string crossOrigin) __attribute__((weak));
    std::u16string getUseMap() __attribute__((weak));
    void setUseMap(std::u16string useMap) __attribute__((weak));
    bool getIsMap() __attribute__((weak));
    void setIsMap(bool isMap) __attribute__((weak));
    unsigned int getWidth() __attribute__((weak));
    void setWidth(unsigned int width) __attribute__((weak));
    unsigned int getHeight() __attribute__((weak));
    void setHeight(unsigned int height) __attribute__((weak));
    unsigned int getNaturalWidth() __attribute__((weak));
    unsigned int getNaturalHeight() __attribute__((weak));
    bool getComplete() __attribute__((weak));
    // HTMLImageElement-20
    std::u16string getName() __attribute__((weak));
    void setName(std::u16string name) __attribute__((weak));
    std::u16string getAlign() __attribute__((weak));
    void setAlign(std::u16string align) __attribute__((weak));
    std::u16string getBorder() __attribute__((weak));
    void setBorder(std::u16string border) __attribute__((weak));
    unsigned int getHspace() __attribute__((weak));
    void setHspace(unsigned int hspace) __attribute__((weak));
    std::u16string getLongDesc() __attribute__((weak));
    void setLongDesc(std::u16string longDesc) __attribute__((weak));
    unsigned int getVspace() __attribute__((weak));
    void setVspace(unsigned int vspace) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLImageElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLImageElement::getMetaData();
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLIMAGEELEMENT_IMP_H
