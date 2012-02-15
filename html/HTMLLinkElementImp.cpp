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

#include "HTMLLinkElementImp.h"

#include <boost/bind.hpp>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "DocumentImp.h"
#include "WindowImp.h"
#include "css/CSSInputStream.h"
#include "css/CSSParser.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLLinkElementImp::HTMLLinkElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"link"),
    request(0),
    styleSheet(0)
{
}

HTMLLinkElementImp::HTMLLinkElementImp(HTMLLinkElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    request(0),  // TODO: XXX
    styleSheet(org->styleSheet) // TODO: make a clone sheet, too?
{
}

HTMLLinkElementImp::~HTMLLinkElementImp()
{
    delete request;
}

void HTMLLinkElementImp::eval()
{
    std::u16string href = getHref();
    if (href.empty())
        return;
    HTMLElementImp::eval();
    std::u16string rel = getRel();
    if (contains(rel, u"stylesheet")) {
        // TODO: check type
        if (!contains(rel, u"alternate")) {
            DocumentImp* document = getOwnerDocumentImp();
            request = new(std::nothrow) HttpRequest(document->getDocumentURI());
            if (request) {
                request->open(u"GET", href);
                request->setHanndler(boost::bind(&HTMLLinkElementImp::notify, this));
                document->incrementLoadEventDelayCount();
                request->send();
            }
        }
    }
}

void HTMLLinkElementImp::notify()
{
    DocumentImp* document = getOwnerDocumentImp();
    if (request->getStatus() == 200) {
#if 104400 <= BOOST_VERSION
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::never_close_handle);
#else
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), false);
#endif
        CSSParser parser;
        CSSInputStream cssStream(stream, request->getResponseMessage().getContentCharset(), utfconv(document->getCharset()));
        styleSheet = parser.parse(document, cssStream);

        if (3 <= getLogLevel())
            dumpStyleSheet(std::cerr, styleSheet.self());

        if (WindowImp* view = document->getDefaultWindow())
            view->setFlagsToBoxTree(1);
    }
    document->decrementLoadEventDelayCount();
}

// Node
Node HTMLLinkElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLLinkElementImp(this, deep);
}

// HTMLLinkElement
bool HTMLLinkElementImp::getDisabled()
{
    if (!styleSheet)
        return false;
    return styleSheet.getDisabled();
}

void HTMLLinkElementImp::setDisabled(bool disabled)
{
    if (styleSheet)
        styleSheet.setDisabled(disabled);
}

std::u16string HTMLLinkElementImp::getHref()
{
    return getAttribute(u"href");
}

void HTMLLinkElementImp::setHref(std::u16string href)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getRel()
{
    // cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/links.html#linkTypes
    std::u16string rel = getAttribute(u"rel");
    toLower(rel);
    return rel;
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
    return getAttribute(u"media");
}

void HTMLLinkElementImp::setMedia(std::u16string media)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getHreflang()
{
    return getAttribute(u"hreflang");
}

void HTMLLinkElementImp::setHreflang(std::u16string hreflang)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getType()
{
    return getAttribute(u"type");
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
    return styleSheet;
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
