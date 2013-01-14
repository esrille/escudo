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

#include "HTMLIFrameElementImp.h"

#include <boost/bind.hpp>

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
    window(0),
    blurListener(boost::bind(&HTMLIFrameElementImp::handleBlur, this, _1, _2))
{
    if (ownerDocument)
        window = new WindowImp(dynamic_cast<WindowImp*>(ownerDocument->getDefaultView().self()), this);
}

HTMLIFrameElementImp::HTMLIFrameElementImp(HTMLIFrameElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    window(org->window),
    blurListener(boost::bind(&HTMLIFrameElementImp::handleBlur, this, _1, _2))
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
    setTabIndex(0);
    addEventListener(u"blur", &blurListener, false, EventTargetImp::UseDefault);
    setSrc(getSrc());
}

void HTMLIFrameElementImp::handleBlur(EventListenerImp* listener, events::Event event)
{
    if (auto imp = dynamic_cast<DocumentImp*>(window.getDocument().self()))
        imp->setFocus(0);
}

//
// HTMLIFrameElement
//

std::u16string HTMLIFrameElementImp::getSrc()
{
    return getAttribute(u"src");
}

void HTMLIFrameElementImp::setSrc(const std::u16string& src)
{
    std::u16string s(src);
    if (!s.empty()) {
        if (DocumentImp* document = getOwnerDocumentImp()) {
            URL base(document->getDocumentURI());
            URL url(base, s);
            s = url;
        } else
            s.clear();
        // TODO: Check what should be done if s is still empty (i.e. invalid)
    }
    window.open(s, u"_self");
}

std::u16string HTMLIFrameElementImp::getSrcdoc()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setSrcdoc(const std::u16string& srcdoc)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLIFrameElementImp::getSandbox()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLIFrameElementImp::setSandbox(const std::u16string& sandbox)
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

void HTMLIFrameElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getHeight()
{
    return getAttribute(u"height");
}

void HTMLIFrameElementImp::setHeight(const std::u16string& height)
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

void HTMLIFrameElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getFrameBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setFrameBorder(const std::u16string& frameBorder)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getLongDesc()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setLongDesc(const std::u16string& longDesc)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getMarginHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setMarginHeight(const std::u16string& marginHeight)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getMarginWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setMarginWidth(const std::u16string& marginWidth)
{
    // TODO: implement me!
}

std::u16string HTMLIFrameElementImp::getScrolling()
{
    // TODO: implement me!
    return u"";
}

void HTMLIFrameElementImp::setScrolling(const std::u16string& scrolling)
{
    // TODO: implement me!
}

}
}
}
}
