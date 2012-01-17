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

#ifndef HTMLBODYELEMENT_IMP_H
#define HTMLBODYELEMENT_IMP_H

#include <Object.h>
#include <org/w3c/dom/html/HTMLBodyElement.h>

#include "HTMLElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

class HTMLBodyElementImp : public ObjectMixin<HTMLBodyElementImp, HTMLElementImp>
{
public:
    virtual void eval();

    // Node
    virtual Node cloneNode(bool deep);
    // HTMLBodyElement
    virtual html::Function getOnafterprint();
    virtual void setOnafterprint(html::Function onafterprint);
    virtual html::Function getOnbeforeprint();
    virtual void setOnbeforeprint(html::Function onbeforeprint);
    virtual html::Function getOnbeforeunload();
    virtual void setOnbeforeunload(html::Function onbeforeunload);
    virtual html::Function getOnblur();
    virtual void setOnblur(html::Function onblur);
    virtual html::Function getOnerror();
    virtual void setOnerror(html::Function onerror);
    virtual html::Function getOnfocus();
    virtual void setOnfocus(html::Function onfocus);
    virtual html::Function getOnhashchange();
    virtual void setOnhashchange(html::Function onhashchange);
    virtual html::Function getOnload();
    virtual void setOnload(html::Function onload);
    virtual html::Function getOnmessage();
    virtual void setOnmessage(html::Function onmessage);
    virtual html::Function getOnoffline();
    virtual void setOnoffline(html::Function onoffline);
    virtual html::Function getOnonline();
    virtual void setOnonline(html::Function ononline);
    virtual html::Function getOnpopstate();
    virtual void setOnpopstate(html::Function onpopstate);
    virtual html::Function getOnpagehide();
    virtual void setOnpagehide(html::Function onpagehide);
    virtual html::Function getOnpageshow();
    virtual void setOnpageshow(html::Function onpageshow);
    virtual html::Function getOnredo();
    virtual void setOnredo(html::Function onredo);
    virtual html::Function getOnresize();
    virtual void setOnresize(html::Function onresize);
    virtual html::Function getOnstorage();
    virtual void setOnstorage(html::Function onstorage);
    virtual html::Function getOnundo();
    virtual void setOnundo(html::Function onundo);
    virtual html::Function getOnunload();
    virtual void setOnunload(html::Function onunload);
    // HTMLBodyElement-6
    virtual std::u16string getText();
    virtual void setText(std::u16string text);
    virtual std::u16string getBgColor();
    virtual void setBgColor(std::u16string bgColor);
    virtual std::u16string getBackground();
    virtual void setBackground(std::u16string background);
    virtual std::u16string getLink();
    virtual void setLink(std::u16string link);
    virtual std::u16string getVLink();
    virtual void setVLink(std::u16string vLink);
    virtual std::u16string getALink();
    virtual void setALink(std::u16string aLink);

    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        return html::HTMLBodyElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLBodyElement::getMetaData();
    }

    HTMLBodyElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"body") {
    }
    HTMLBodyElementImp(HTMLBodyElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}}}}  // org::w3c::dom::bootstrap

#endif  // HTMLBODYELEMENT_IMP_H
