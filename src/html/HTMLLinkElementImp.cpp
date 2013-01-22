/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "DocumentImp.h"
#include "DOMTokenListImp.h"
#include "WindowImp.h"
#include "HTMLUtil.h"
#include "css/BoxImage.h"
#include "css/CSSInputStream.h"
#include "css/CSSParser.h"
#include "css/CSSStyleSheetImp.h"
#include "css/Ico.h"

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

void HTMLLinkElementImp::handleMutation(events::MutationEvent mutation)
{
    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"href"):
        handleMutationHref(mutation);
        break;
    case Intern(u"tabindex"):
        if (hasAttribute(u"href")) {
            if (!toInteger(mutation.getNewValue(), tabIndex))
                tabIndex = 0;
        } else
            tabIndex = -1;
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

void HTMLLinkElementImp::eval()
{
    std::u16string href = getHref();
    if (href.empty())
        return;

    std::u16string rel = getRel();
    if (::contains(rel, u"stylesheet")) {
        // TODO: check "type"

        if (!::contains(rel, u"alternate")) {
            // Check "media"
            std::u16string mediaText = getMedia();
            Retained<MediaListImp> mediaList;
            mediaList.setMediaText(mediaText);
            if (mediaText.empty() || mediaList.hasMedium(MediaListImp::Screen)) {   // TODO: support other mediums, too.
                DocumentImp* document = getOwnerDocumentImp();
                request = new(std::nothrow) HttpRequest(document->getDocumentURI());
                if (request) {
                    request->open(u"GET", href);
                    request->setHandler(boost::bind(&HTMLLinkElementImp::linkStyleSheet, this));
                    document->incrementLoadEventDelayCount();
                    request->send();
                }
            }
        }
    }
    else if (::contains(rel, u"icon")) {
        DocumentImp* document = getOwnerDocumentImp();
        request = new(std::nothrow) HttpRequest(document->getDocumentURI());
        if (request) {
            request->open(u"GET", href);
            request->setHandler(boost::bind(&HTMLLinkElementImp::linkIcon, this));
            document->incrementLoadEventDelayCount();
            request->send();
        }
    }
}

void HTMLLinkElementImp::linkStyleSheet()
{
    DocumentImp* document = getOwnerDocumentImp();
    if (request->getStatus() == 200) {
#if 104400 <= BOOST_VERSION
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::never_close_handle);
#else
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), false);
#endif
        CSSParser parser;
        CSSInputStream cssStream(stream, request->getResponseMessage().getContentCharset(), utfconv(document->getCharacterSet()));
        styleSheet = parser.parse(document, cssStream);
        if (auto imp = dynamic_cast<CSSStyleSheetImp*>(styleSheet.self())) {
            imp->setHref(request->getRequestMessage().getURL());
            imp->setOwnerNode(this);
        }
        if (4 <= getLogLevel())
            dumpStyleSheet(std::cerr, styleSheet.self());
        document->resetStyleSheets();
        if (WindowImp* view = document->getDefaultWindow())
            view->setFlags(Box::NEED_SELECTOR_REMATCHING);
    }
    document->decrementLoadEventDelayCount();
}

void HTMLLinkElementImp::linkIcon()
{
    DocumentImp* document = getOwnerDocumentImp();
    setFavicon(document);
    document->decrementLoadEventDelayCount();
}

bool HTMLLinkElementImp::setFavicon(DocumentImp* document)
{
    std::u16string rel = getRel();
    if (!::contains(rel, u"icon"))
        return false;
    if (!request || request->getStatus() != 200)
        return false;
    bool result = false;
    if (FILE* file = request->openFile()) {
        std::u16string type = getType();
        if (type == u"image/vnd.microsoft.icon" || type.empty()) {
            IcoImage ico;
            if (ico.open(file)) {
                if (WindowImp* view = document->getDefaultWindow()) {
                    view->setFavicon(&ico, file);
                    result = true;
                }
            }
        } else {
            BoxImage image;
            image.open(file);
            if (image.getState() == BoxImage::CompletelyAvailable) {
                if (WindowImp* view = document->getDefaultWindow()) {
                    view->setFavicon(&image);
                    result = true;
                }
            }
        }
        fclose(file);
    }
    return result;
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

void HTMLLinkElementImp::setHref(const std::u16string& href)
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

void HTMLLinkElementImp::setRel(const std::u16string& rel)
{
    // TODO: implement me!
}

DOMTokenList HTMLLinkElementImp::getRelList()
{
    return new(std::nothrow) DOMTokenListImp(this, u"rel");
}

std::u16string HTMLLinkElementImp::getMedia()
{
    return getAttribute(u"media");
}

void HTMLLinkElementImp::setMedia(const std::u16string& media)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getHreflang()
{
    return getAttribute(u"hreflang");
}

void HTMLLinkElementImp::setHreflang(const std::u16string& hreflang)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getType()
{
    return getAttribute(u"type");
}

void HTMLLinkElementImp::setType(const std::u16string& type)
{
    // TODO: implement me!
}

DOMSettableTokenList HTMLLinkElementImp::getSizes()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

void HTMLLinkElementImp::setSizes(const std::u16string& sizes)
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

void HTMLLinkElementImp::setCharset(const std::u16string& charset)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getRev()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setRev(const std::u16string& rev)
{
    // TODO: implement me!
}

std::u16string HTMLLinkElementImp::getTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLLinkElementImp::setTarget(const std::u16string& target)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
