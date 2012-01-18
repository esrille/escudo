/*
 * Copyright 2011, 2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLIFRAMEELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLIFRAMEELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLIFrameElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/Document.h>
#include <org/w3c/dom/DOMSettableTokenList.h>
#include <org/w3c/dom/html/Window.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLIFrameElementImp : public ObjectMixin<HTMLIFrameElementImp, HTMLElementImp>
{
    html::Window window;

public:
    HTMLIFrameElementImp(DocumentImp* ownerDocument);
    HTMLIFrameElementImp(HTMLIFrameElementImp* org, bool deep);
    ~HTMLIFrameElementImp();

    void eval();

    // HTMLIFrameElement
    std::u16string getSrc();
    void setSrc(std::u16string src);
    std::u16string getSrcdoc();
    void setSrcdoc(std::u16string srcdoc);
    std::u16string getName();
    void setName(std::u16string name);
    DOMSettableTokenList getSandbox();
    void setSandbox(std::u16string sandbox);
    bool getSeamless();
    void setSeamless(bool seamless);
    std::u16string getWidth();
    void setWidth(std::u16string width);
    std::u16string getHeight();
    void setHeight(std::u16string height);
    Document getContentDocument();
    html::Window getContentWindow();
    // HTMLIFrameElement-16
    std::u16string getAlign();
    void setAlign(std::u16string align);
    std::u16string getFrameBorder();
    void setFrameBorder(std::u16string frameBorder);
    std::u16string getLongDesc();
    void setLongDesc(std::u16string longDesc);
    std::u16string getMarginHeight();
    void setMarginHeight(std::u16string marginHeight);
    std::u16string getMarginWidth();
    void setMarginWidth(std::u16string marginWidth);
    std::u16string getScrolling();
    void setScrolling(std::u16string scrolling);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLIFrameElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLIFrameElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLIFRAMEELEMENTIMP_H_INCLUDED
