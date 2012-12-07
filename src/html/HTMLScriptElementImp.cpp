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

#include "HTMLBindingElementImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLScriptElementImp::HTMLScriptElementImp(DocumentImp* ownerDocument, const std::u16string& localName) :
    ObjectMixin(ownerDocument, localName),
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
    request(0)
{
}

HTMLScriptElementImp::~HTMLScriptElementImp()
{
    delete request;
}

// cf. http://www.whatwg.org/specs/web-apps/current-work/multipage/scripting-1.html#prepare-a-script
bool HTMLScriptElementImp::prepare()
{
    if (alreadyStarted)
        return false;
    if (parserInserted) {
        wasParserInserted = true;
        parserInserted = false;
    } else
        wasParserInserted = false;
    if (wasParserInserted && !hasAttribute(u"async"))
        forceAsync = true;

    // TODO 4. - 7.

    if (wasParserInserted) {
        parserInserted = true;
        forceAsync = false;
    }
    alreadyStarted = true;

    // TODO 10. - 13.
    DocumentImp* document = getOwnerDocumentImp();

    if (hasAttribute(u"src")) {
        std::u16string src = getSrc();
        if (src.empty()) {
            // TODO: fire the error event
            return false;
        }

        request = new(std::nothrow) HttpRequest(document->getDocumentURI());
        if (request) {
            request->open(u"GET", src);
            request->setHandler(boost::bind(&HTMLScriptElementImp::notify, this));
            document->incrementLoadEventDelayCount();
            if (parserInserted)
                document->setPendingParsingBlockingScript(this);
            // TODO: deal with 'defer' and 'async'
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
        if (parserInserted)
            readyToBeParserExecuted = true;
        // TODO: deal with 'defer' and 'async'
    } else {
        if (document->getPendingParsingBlockingScript() == this)
            document->setPendingParsingBlockingScript(0);
    }
    document->decrementLoadEventDelayCount();
}

bool HTMLScriptElementImp::execute()
{
    std::u16string script;
    if (request) {
        assert(request->getStatus() == 200);
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::never_close_handle);
        U16InputStream u16stream(stream, "utf-8");  // TODO detect encode
        script = u16stream;
    } else {
        Nullable<std::u16string> content = getTextContent();
        if (!content.hasValue())
            return false;
        script = content.value();
        stripLeadingAndTrailingWhitespace(script);
        if (script.compare(0, 4, u"<!--") == 0)
            script.erase(0, 4);
        if (3 <= script.length() && script.compare(script.length() - 3, 3, u"-->") == 0)
            script.erase(script.length() - 3);
    }
    DocumentWindowPtr window = getOwnerDocumentImp()->activate();
    Any result = window->getContext()->evaluate(script);
    if (auto binding = dynamic_cast<HTMLBindingElementImp*>(getParentElement().self())) {
        if (result.isObject() && !binding->getImplementation())
            binding->setImplementation(result.toObject());
    }
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
    setAttribute(u"src", src);
}

bool HTMLScriptElementImp::getAsync()
{
    if (forceAsync)
        return true;
    Nullable<std::u16string> value = getAttribute(u"async");
    if (!value.hasValue())
        return false;
    return value.value().empty() || !compareIgnoreCase(value.value(), u"async");
}

void HTMLScriptElementImp::setAsync(bool async)
{
    forceAsync = false;
    if (async)
        setAttribute(u"async", u"");
    else
        removeAttribute(u"async");
}

bool HTMLScriptElementImp::getDefer()
{
    Nullable<std::u16string> value = getAttribute(u"defer");
    if (!value.hasValue())
        return false;
    return value.value().empty() || !compareIgnoreCase(value.value(), u"defer");
}

void HTMLScriptElementImp::setDefer(bool defer)
{
    if (defer)
        setAttribute(u"defer", u"");
    else
        removeAttribute(u"defer");
}

std::u16string HTMLScriptElementImp::getType()
{
    return getAttribute(u"type");
}

void HTMLScriptElementImp::setType(std::u16string type)
{
    setAttribute(u"type", type);
}

std::u16string HTMLScriptElementImp::getCharset()
{
    return getAttribute(u"charset");
}

void HTMLScriptElementImp::setCharset(std::u16string charset)
{
    setAttribute(u"charset", charset);
}

std::u16string HTMLScriptElementImp::getText()
{
    return getTextContent();
}

void HTMLScriptElementImp::setText(std::u16string text)
{
    setTextContent(text);
}

std::u16string HTMLScriptElementImp::getEvent()
{
    return u"";
}

void HTMLScriptElementImp::setEvent(std::u16string event)
{
}

std::u16string HTMLScriptElementImp::getHtmlFor()
{
    return u"";
}

void HTMLScriptElementImp::setHtmlFor(std::u16string htmlFor)
{
}

}}}}  // org::w3c::dom::bootstrap
