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

#include "HTMLScriptElementImp.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "ECMAScript.h"

#include "utf.h"
#include "DocumentImp.h"
#include "DocumentWindow.h"
#include "U16InputStream.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLScriptElementImp::HTMLScriptElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"script"),
    alreadyStarted(false),
    parserInserted(false),
    wasParserInserted(false),
    forceAsync(true),
    readyToBeParserExecuted(false),
    request(0)
{
}

HTMLScriptElementImp::HTMLScriptElementImp(HTMLScriptElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    alreadyStarted(false),
    parserInserted(false),
    wasParserInserted(false),
    forceAsync(true),
    readyToBeParserExecuted(false),
    request(0)  // TODO XXX
{
}

bool HTMLScriptElementImp::prepare()
{
    if (alreadyStarted)
        return false;
    if (parserInserted) {
        wasParserInserted = true;
        parserInserted = false;
    } else
        parserInserted = true;
    if (wasParserInserted && !hasAttribute(u"async"))
        forceAsync = true;

    // TODO s

    if (wasParserInserted) {
        parserInserted = true;
        forceAsync = false;
    }
    alreadyStarted = true;

    // TODO s

    if (hasAttribute(u"src")) {
        std::u16string src = getSrc();
        if (src.empty()) {
            // TODO: fire the error event
            return false;
        }

        DocumentImp* document = getOwnerDocumentImp();
        request = new(std::nothrow) HttpRequest(document->getDocumentURI());
        if (request) {
            request->open(u"GET", src);
            request->setHanndler(boost::bind(&HTMLScriptElementImp::notify, this));
            document->incrementLoadEventDelayCount();
            request->send();
        }

        return true;
    }

    return execute();
}

void HTMLScriptElementImp::notify()
{
    DocumentImp* document = getOwnerDocumentImp();
    if (request->getStatus() == 200) {
#if 104400 <= BOOST_VERSION
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::never_close_handle);
#else
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), false);
#endif
        U16InputStream u16stream(stream, "utf-8");  // TODO detect encode
        std::u16string script = u16stream;
        DocumentWindowPtr window = document->activate();
        if (window)
            window->getContext()->evaluate(script);
    }
    document->decrementLoadEventDelayCount();
}

bool HTMLScriptElementImp::execute()
{
    Nullable<std::u16string> content = getTextContent();
    if (!content.hasValue())
        return false;
    std::u16string script = content.value();
    stripLeadingAndTrailingWhitespace(script);
    size_t pos = 0;
    size_t length = script.length();
    if (script.compare(0, 4, u"<!--") == 0) {
        pos = 4;
        length -= 4;
    }
    if (3 <= length && script.compare(length - 3, 3, u"-->") == 0)
        length -= 3;
    DocumentWindowPtr window = getOwnerDocumentImp()->activate();
    window->getContext()->evaluate(script);
    return true;
}

// Node
Node HTMLScriptElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLScriptElementImp(this, deep);
}

// HTMLScriptElement
std::u16string HTMLScriptElementImp::getSrc()
{
    return getAttribute(u"src");
}

void HTMLScriptElementImp::setSrc(std::u16string src)
{
    // TODO: implement me!
}

bool HTMLScriptElementImp::getAsync()
{
    // TODO: implement me!
    return 0;
}

void HTMLScriptElementImp::setAsync(bool async)
{
    // TODO: implement me!
}

bool HTMLScriptElementImp::getDefer()
{
    // TODO: implement me!
    return 0;
}

void HTMLScriptElementImp::setDefer(bool defer)
{
    // TODO: implement me!
}

std::u16string HTMLScriptElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLScriptElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

std::u16string HTMLScriptElementImp::getCharset()
{
    // TODO: implement me!
    return u"";
}

void HTMLScriptElementImp::setCharset(std::u16string charset)
{
    // TODO: implement me!
}

std::u16string HTMLScriptElementImp::getText()
{
    // TODO: implement me!
    return u"";
}

void HTMLScriptElementImp::setText(std::u16string text)
{
    // TODO: implement me!
}

std::u16string HTMLScriptElementImp::getEvent()
{
    // TODO: implement me!
    return u"";
}

void HTMLScriptElementImp::setEvent(std::u16string event)
{
    // TODO: implement me!
}

std::u16string HTMLScriptElementImp::getHtmlFor()
{
    // TODO: implement me!
    return u"";
}

void HTMLScriptElementImp::setHtmlFor(std::u16string htmlFor)
{
    // TODO: implement me!
}

}}}}  // org::w3c::dom::bootstrap
