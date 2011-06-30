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

#include "HTMLAnchorElementImp.h"

#include <iostream>
#include <boost/bind.hpp>

#include "utf.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLAnchorElementImp::handleClick(events::Event event)
{
    std::u16string href = getHref();
    if (!href.empty())
        getOwnerDocument().setLocation(href);
}

HTMLAnchorElementImp::HTMLAnchorElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"a"),
    clickListener(boost::bind(&HTMLAnchorElementImp::handleClick, this, _1))
{
    addEventListener(u"click", &clickListener);
}

HTMLAnchorElementImp::HTMLAnchorElementImp(HTMLAnchorElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    clickListener(boost::bind(&HTMLAnchorElementImp::handleClick, this, _1))
{
    addEventListener(u"click", &clickListener);
}

HTMLAnchorElementImp::~HTMLAnchorElementImp()
{
}

std::u16string HTMLAnchorElementImp::getHref()
{
    Nullable<std::u16string> src = getAttribute(u"href");
    if (src.hasValue())
        return src.value();
    return u"";
}

void HTMLAnchorElementImp::setHref(std::u16string href)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setTarget(std::u16string target)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getRel()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setRel(std::u16string rel)
{
    // TODO: implement me!
}

DOMTokenList HTMLAnchorElementImp::getRelList()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLAnchorElementImp::getMedia()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setMedia(std::u16string media)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHreflang()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHreflang(std::u16string hreflang)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getText()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setText(std::u16string text)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getProtocol()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setProtocol(std::u16string protocol)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHost()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHost(std::u16string host)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHostname()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHostname(std::u16string hostname)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getPort()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setPort(std::u16string port)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getPathname()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setPathname(std::u16string pathname)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getSearch()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setSearch(std::u16string search)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getHash()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setHash(std::u16string hash)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getCoords()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setCoords(std::u16string coords)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getCharset()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setCharset(std::u16string charset)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setName(std::u16string name)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getRev()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setRev(std::u16string rev)
{
    // TODO: implement me!
}

std::u16string HTMLAnchorElementImp::getShape()
{
    // TODO: implement me!
    return u"";
}

void HTMLAnchorElementImp::setShape(std::u16string shape)
{
    // TODO: implement me!
}

}
}
}
}
