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

#include "HTMLScriptElementImp.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/version.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include "ECMAScript.h"

#include "utf.h"
#include "DocumentImp.h"
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
    bool hasAsync = getAsync();
    bool hasDefer = getDefer();

    if (alreadyStarted)
        return false;
    if (parserInserted) {
        wasParserInserted = true;
        parserInserted = false;
    } else
        wasParserInserted = false;
    if (wasParserInserted && !hasAsync)
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
            if (hasDefer && parserInserted && !hasAsync) {
                type = Defer;
                document->addDeferScript(this);
            } else if (parserInserted && !hasAsync) {
                type = Blocking;
                document->setPendingParsingBlockingScript(this);
            } else if (!hasAsync && !forceAsync) {
                type = Ordered;
                document->addOrderedScript(this);
            } else {
                type = Async;
                document->addAsyncScript(this);
            }
            request->send();
        }
        return true;
    }
    // TODO: Check style sheets that is blocking scripts
    return execute();
}

void HTMLScriptElementImp::notify()
{
    DocumentImp* document = getOwnerDocumentImp();
    if (request->getStatus() == 200) {
        readyToBeParserExecuted = true;
        switch (type) {
        case Blocking:
            document->decrementLoadEventDelayCount();
            break;
        case Async:
            execute();
            document->removeAsyncScript(this);
            document->decrementLoadEventDelayCount();
            break;
        case Ordered:
            document->processOrderedScripts();
            break;
        default:
            break;
        }
    } else {
        switch (type) {
        case Blocking:
            assert(document->getPendingParsingBlockingScript() == this);
            document->setPendingParsingBlockingScript(0);
            break;
        case Defer:
            document->removeDeferScript(this);
            break;
        case Async:
            document->removeAsyncScript(this);
            break;
        case Ordered:
            document->removeOrderedScript(this);
            document->processOrderedScripts();
            break;
        default:
            break;
        }
        document->decrementLoadEventDelayCount();
    }
}

bool HTMLScriptElementImp::execute()
{
    std::u16string script;
    if (request) {
        assert(request->getStatus() == 200);
        boost::iostreams::stream<boost::iostreams::file_descriptor_source> stream(request->getContentDescriptor(), boost::iostreams::close_handle);
        U16ConverterInputStream u16stream(stream, "utf-8");  // TODO detect encode
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
    if (ECMAScriptContext* context = getOwnerDocumentImp()->getContext()) {
        Any result = context->evaluate(script);
        if (auto binding = dynamic_cast<HTMLBindingElementImp*>(getParentElement().self())) {
            if (result.isObject() && !binding->getImplementation())
                binding->setImplementation(result.toObject());
        }
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

void HTMLScriptElementImp::setSrc(const std::u16string& src)
{
    setAttribute(u"src", src);
}

bool HTMLScriptElementImp::getAsync()
{
    if (forceAsync)
        return true;
    return getAttributeAsBoolean(u"async");
}

void HTMLScriptElementImp::setAsync(bool async)
{
    forceAsync = false;
    setAttributeAsBoolean(u"async", async);
}

bool HTMLScriptElementImp::getDefer()
{
    return getAttributeAsBoolean(u"defer");
}

void HTMLScriptElementImp::setDefer(bool defer)
{
    setAttributeAsBoolean(u"defer", defer);
}

std::u16string HTMLScriptElementImp::getType()
{
    return getAttribute(u"type");
}

void HTMLScriptElementImp::setType(const std::u16string& type)
{
    setAttribute(u"type", type);
}

std::u16string HTMLScriptElementImp::getCharset()
{
    return getAttribute(u"charset");
}

void HTMLScriptElementImp::setCharset(const std::u16string& charset)
{
    setAttribute(u"charset", charset);
}

std::u16string HTMLScriptElementImp::getText()
{
    return getTextContent();
}

void HTMLScriptElementImp::setText(const std::u16string& text)
{
    setTextContent(text);
}

std::u16string HTMLScriptElementImp::getEvent()
{
    return u"";
}

void HTMLScriptElementImp::setEvent(const std::u16string& event)
{
}

std::u16string HTMLScriptElementImp::getHtmlFor()
{
    return u"";
}

void HTMLScriptElementImp::setHtmlFor(const std::u16string& htmlFor)
{
}

}}}}  // org::w3c::dom::bootstrap
