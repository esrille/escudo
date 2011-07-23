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

#include "HTMLScriptElementImp.h"

#include <iostream>

#include "utf.h"
#include "DocumentImp.h"
#include "HTMLScriptElementImp.h"
#include "js/esjsapi.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLScriptElementImp::HTMLScriptElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"script"),
    alreadyStarted(false),
    parserInserted(false),
    wasParserInserted(false),
    forceAsync(true),
    readyToBeParserExecuted(false)
{
}

HTMLScriptElementImp::HTMLScriptElementImp(HTMLScriptElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    alreadyStarted(false),
    parserInserted(false),
    wasParserInserted(false),
    forceAsync(true),
    readyToBeParserExecuted(false)
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

    return execute();
}

bool HTMLScriptElementImp::execute()
{
    if (!jscontext)
        return false;
    jsval rval;
    const char* filename = "";
    int lineno = 0;
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
    return JS_EvaluateUCScript(jscontext, static_cast<JSObject*>(getOwnerDocumentImp()->getGlobal()),
                               reinterpret_cast<const jschar*>(script.c_str() + pos), length,
                               filename, lineno, &rval);
}

// Node
Node HTMLScriptElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLScriptElementImp(this, deep);
}

// HTMLScriptElement
std::u16string HTMLScriptElementImp::getSrc()
{
    // TODO: implement me!
    return u"";
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
