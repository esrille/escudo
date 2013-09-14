/*
 * Copyright 2011-2013 Esrille Inc.
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

#include "HTMLAnchorElementImp.h"

#include <iostream>
#include <boost/bind.hpp>

#include <org/w3c/dom/events/MouseEvent.h>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "utf.h"

#include "DocumentImp.h"
#include "DOMTokenListImp.h"
#include "HTMLUtil.h"
#include "WindowProxy.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLAnchorElementImp::HTMLAnchorElementImp(HTMLAnchorElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    clickListener(boost::bind(&HTMLAnchorElementImp::handleClick, this, _1, _2))
{
    tabIndex = -1;
    addEventListener(u"click", &clickListener, false, EventTargetImp::UseDefault);
}

HTMLAnchorElementImp::HTMLAnchorElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"a"),
    clickListener(boost::bind(&HTMLAnchorElementImp::handleClick, this, _1, _2))
{
    addEventListener(u"click", &clickListener, false, EventTargetImp::UseDefault);
}

HTMLAnchorElementImp::~HTMLAnchorElementImp()
{
}

void HTMLAnchorElementImp::handleClick(EventListenerImp* listener, events::Event event)
{
    events::MouseEvent mouse = interface_cast<events::MouseEvent>(event);
    std::u16string href = getHref();
    if (!href.empty() && mouse.getButton() == 0) {
        // TODO: Add more details
        DocumentImp* document = getOwnerDocumentImp();
        assert(document);
        WindowProxy* window = document->getDefaultWindow();
        assert(window);
        window->open(href, getTarget());
    }
}

void HTMLAnchorElementImp::handleMutation(events::MutationEvent mutation)
{
    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"href"):
        handleMutationHref(mutation);
        break;
    case Intern(u"tabindex"):
        if (hasAttribute(u"href") && !toInteger(mutation.getNewValue(), tabIndex))
            tabIndex = 0;
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

std::u16string HTMLAnchorElementImp::getHref()
{
    return getAttribute(u"href");
}

void HTMLAnchorElementImp::setHref(const std::u16string& href)
{
    setAttribute(u"href", href);
}

std::u16string HTMLAnchorElementImp::getTarget()
{
    return getAttribute(u"target");
}

void HTMLAnchorElementImp::setTarget(const std::u16string& target)
{
    setAttribute(u"target", target);
}

std::u16string HTMLAnchorElementImp::getRel()
{
    return getAttribute(u"rel");
}

void HTMLAnchorElementImp::setRel(const std::u16string& rel)
{
    setAttribute(u"rel", rel);
}

DOMTokenList HTMLAnchorElementImp::getRelList()
{
    return new(std::nothrow) DOMTokenListImp(this, u"rel");
}

std::u16string HTMLAnchorElementImp::getMedia()
{
    return getAttribute(u"media");
}

void HTMLAnchorElementImp::setMedia(const std::u16string& media)
{
   setAttribute(u"media", media);
}

std::u16string HTMLAnchorElementImp::getHreflang()
{
    return getAttribute(u" hreflang");
}

void HTMLAnchorElementImp::setHreflang(const std::u16string& hreflang)
{
   setAttribute(u" hreflang",  hreflang);
}

std::u16string HTMLAnchorElementImp::getType()
{
    return getAttribute(u"type");
}

void HTMLAnchorElementImp::setType(const std::u16string& type)
{
   setAttribute(u"type",  type);
}

std::u16string HTMLAnchorElementImp::getText()
{
    return getTextContent();
}

void HTMLAnchorElementImp::setText(const std::u16string& text)
{
    setTextContent(text);
}

std::u16string HTMLAnchorElementImp::getProtocol()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setProtocol(const std::u16string& protocol)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHost()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHost(const std::u16string& host)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHostname()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHostname(const std::u16string& hostname)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getPort()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setPort(const std::u16string& port)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getPathname()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setPathname(const std::u16string& pathname)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getSearch()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setSearch(const std::u16string& search)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHash()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHash(const std::u16string& hash)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getCoords()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setCoords(const std::u16string& coords)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getCharset()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setCharset(const std::u16string& charset)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getRev()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setRev(const std::u16string& rev)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getShape()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setShape(const std::u16string& shape)
{
    // TODO: implement me!
}

}
}
}
}
