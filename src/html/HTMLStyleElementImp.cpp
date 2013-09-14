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

#include "HTMLStyleElementImp.h"

#include "css/CSSParser.h"
#include "css/CSSStyleSheetImp.h"

#include <boost/bind.hpp>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "TextImp.h"
#include "DocumentImp.h"
#include "WindowProxy.h"

#include "Test.util.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLStyleElementImp::HTMLStyleElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"style"),
    mutationListener(boost::bind(&HTMLStyleElementImp::handleMutation, this, _1, _2)),
    type(u"text/css"),
    scoped(false),
    styleSheet(0)
{
    addEventListener(u"DOMNodeInserted", &mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMNodeRemoved", &mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMCharacterDataModified", &mutationListener, false, EventTargetImp::UseDefault);
}

HTMLStyleElementImp::HTMLStyleElementImp(HTMLStyleElementImp* org, bool deep) :
    ObjectMixin(org, deep),
    mutationListener(boost::bind(&HTMLStyleElementImp::handleMutation, this, _1, _2)),
    type(org->type),
    scoped(org->scoped),
    styleSheet(org->styleSheet) // TODO: make a clone sheet, too?
{
    addEventListener(u"DOMNodeInserted", &mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMNodeRemoved", &mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMCharacterDataModified", &mutationListener, false, EventTargetImp::UseDefault);
}

void HTMLStyleElementImp::handleMutation(EventListenerImp* listener, events::Event event)
{
    // TODO: update type, media, and scoped. Then check type.

    events::MutationEvent mutation(interface_cast<events::MutationEvent>(event));

    DocumentImp* document = getOwnerDocumentImp();
    if (!document)
        return;

    if (mutation.getType() == u"DOMNodeRemoved" && event.getTarget().self() == this)
        styleSheet = 0;
    else {
        std::u16string content;
        for (Node node = getFirstChild(); node; node = node.getNextSibling()) {
            if (TextImp* text = dynamic_cast<TextImp*>(node.self()))  // TODO better to avoid imp call?
                content += text->getData();
        }
        CSSParser parser(getBaseURI());
        styleSheet = parser.parse(document, content);
        if (auto imp = dynamic_cast<CSSStyleSheetImp*>(styleSheet.self())) {
            imp->setOwnerNode(this);
            imp->setHref(u"");
            imp->setMedia(getMedia());
            if (4 <= getLogLevel())
                dumpStyleSheet(std::cerr, imp);
        }
    }
    if (WindowProxy* view = document->getDefaultWindow())
        view->setViewFlags(Box::NEED_SELECTOR_REMATCHING);
    document->resetStyleSheets();
}

void HTMLStyleElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();

    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"media"):
        if (styleSheet) {
            styleSheet.setMedia((mutation.getAttrChange() != events::MutationEvent::REMOVAL) ? value : u"");
            if (DocumentImp* document = getOwnerDocumentImp()) {
                // TODO: Optimize the following steps later
                if (WindowProxy* view = document->getDefaultWindow())
                    view->setViewFlags(Box::NEED_SELECTOR_REMATCHING);
            }
        }
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

// Node
Node HTMLStyleElementImp::cloneNode(bool deep)
{
    return new(std::nothrow) HTMLStyleElementImp(this, deep);
}

// HTMLStyleElement
bool HTMLStyleElementImp::getDisabled()
{
    if (!styleSheet)
        return false;
    return styleSheet.getDisabled();
}

void HTMLStyleElementImp::setDisabled(bool disabled)
{
    if (styleSheet)
        styleSheet.setDisabled(disabled);
}

std::u16string HTMLStyleElementImp::getMedia()
{
    return getAttribute(u"media");
}

void HTMLStyleElementImp::setMedia(const std::u16string& media)
{
    setAttribute(u"media", media);
}

std::u16string HTMLStyleElementImp::getType()
{
    return type;
}

void HTMLStyleElementImp::setType(const std::u16string& type)
{
    this->type = type;
}

bool HTMLStyleElementImp::getScoped()
{
    return scoped;
}

void HTMLStyleElementImp::setScoped(bool scoped)
{
    this->scoped = scoped;
}

// LinkStyle
stylesheets::StyleSheet HTMLStyleElementImp::getSheet()
{
    return styleSheet;
}

}}}}  // org::w3c::dom::bootstrap
