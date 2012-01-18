/*
 * Copyright 2011 Esrille Inc.
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

#include "HTMLIFrameElementImp.h"

#include "WindowImp.h"
#include "DocumentImp.h"


namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLIFrameElementImp::HTMLIFrameElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"iframe"),
    window(0)
{
    if (ownerDocument)
        window = new WindowImp(dynamic_cast<WindowImp*>(ownerDocument->getDefaultView().self()));  // TODO set JS global
}

HTMLIFrameElementImp::HTMLIFrameElementImp(HTMLIFrameElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    window(org->window)
{
}

HTMLIFrameElementImp::~HTMLIFrameElementImp()
{
}

void HTMLIFrameElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalHeight(this);
    HTMLElementImp::evalWidth(this);
    HTMLElementImp::evalHspace(this);
    HTMLElementImp::evalVspace(this);
    HTMLElementImp::evalMarginHeight(this);
    HTMLElementImp::evalMarginWidth(this);
}

//
// HTMLIFrameElement
//

std::u16string HTMLIFrameElementImp::getSrc()
{
    return getAttribute(u"src");
}

void HTMLIFrameElementImp::setSrc(std::u16string src)
{
    window.open(src, u"_self", u"", true);
}

std::u16string HTMLIFrameElementImp::getSrcdoc()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setSrcdoc(std::u16string srcdoc)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setName(std::u16string name)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLIFrameElementImp::getSandbox()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLIFrameElementImp::setSandbox(std::u16string sandbox)
{
    // TODO: implement me!
}

bool HTMLIFrameElementImp::getSeamless()
{
    // TODO: implement me!
    return 0;
}

void HTMLIFrameElementImp::setSeamless(bool seamless)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getWidth()
{
    return getAttribute(u"width");
}

void HTMLIFrameElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getHeight()
{
    return getAttribute(u"height");
}

void HTMLIFrameElementImp::setHeight(std::u16string height)
{
    // TODO: implement me!
}

Document HTMLIFrameElementImp::getContentDocument()
{
    return window.getDocument();
}

html::Window HTMLIFrameElementImp::getContentWindow()
{
    return window;
}

std::u16string HTMLIFrameElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getFrameBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setFrameBorder(std::u16string frameBorder)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getLongDesc()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setLongDesc(std::u16string longDesc)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getMarginHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setMarginHeight(std::u16string marginHeight)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getMarginWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setMarginWidth(std::u16string marginWidth)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getScrolling()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setScrolling(std::u16string scrolling)
{
    // TODO: implement me!
}

}
}
}
}
