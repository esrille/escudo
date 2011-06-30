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

#include "HTMLLinkElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

// Node
Node HTMLLinkElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLLinkElementImp(this, deep);
}

// HTMLLinkElement
bool HTMLLinkElementImp::getDisabled()
{
    // TODO: implement me!
    return 0;
}

void HTMLLinkElementImp::setDisabled(bool disabled)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getHref()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setHref(std::u16string href)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getRel()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setRel(std::u16string rel)
{
    // TODO: implement me!
}

DOMTokenList HTMLLinkElementImp::getRelList()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLLinkElementImp::getMedia()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setMedia(std::u16string media)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getHreflang()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setHreflang(std::u16string hreflang)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLLinkElementImp::getSizes()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLLinkElementImp::setSizes(std::u16string sizes)
{
    // TODO: implement me!
}

stylesheets::StyleSheet HTMLLinkElementImp::getSheet()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLLinkElementImp::getCharset()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setCharset(std::u16string charset)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getRev()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setRev(std::u16string rev)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setTarget(std::u16string target)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
