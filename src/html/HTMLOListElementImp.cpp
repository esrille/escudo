/*
 * Copyright 2012, 2013 Esrille Inc.
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

#include "HTMLOListElementImp.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "HTMLLIElementImp.h"
#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLOListElementImp::HTMLOListElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"ol"),
    mutationListener(boost::bind(&HTMLOListElementImp::handleMutation, this, _1, _2))
{
    addEventListener(u"DOMNodeInserted", mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMNodeRemoved", mutationListener, false, EventTargetImp::UseDefault);
}

HTMLOListElementImp::HTMLOListElementImp(const HTMLOListElementImp& org) :
    ObjectMixin(org),
    mutationListener(boost::bind(&HTMLOListElementImp::handleMutation, this, _1, _2))
{
    addEventListener(u"DOMNodeInserted", mutationListener, false, EventTargetImp::UseDefault);
    addEventListener(u"DOMNodeRemoved", mutationListener, false, EventTargetImp::UseDefault);
}

int HTMLOListElementImp::getStart(const std::u16string& value)
{
    int start;
    if (toInteger(value, start))
        return start;
    if (!getReversed())
        return 1;
    // return the number of child li elements
    start = 0;
    for (auto i = getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (std::dynamic_pointer_cast<HTMLLIElementImp>(i.self()))
            ++start;
    }
    return start;
}

void HTMLOListElementImp::handleMutation(EventListenerImp* listener, events::Event event)
{
    events::MutationEvent mutation(interface_cast<events::MutationEvent>(event));
    if (this != mutation.getRelatedNode().self().get())
        return;
    Node child = interface_cast<Node>(event.getTarget());
    if (!Element::hasInstance(child))
        return;
    if (!getReversed())
        return;

    int start = getStart();
    if (mutation.getType() == u"DOMNodeRemoved" && interface_cast<Element>(child).getLocalName() == u"li")
        --start;
    getStyle().setProperty(u"counter-reset", u"list-item " + boost::lexical_cast<std::u16string>(start), u"non-css");
}

void HTMLOListElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"reversed"):
        style.setProperty(u"counter-reset", u"list-item " + boost::lexical_cast<std::u16string>(getStart()), u"non-css");
        style.setProperty(u"counter-increment", (mutation.getAttrChange() != events::MutationEvent::REMOVAL) ? u"list-item" : u"list-item -1", u"non-css");
        break;
    case Intern(u"start"):
        style.setProperty(u"counter-reset", u"list-item " + boost::lexical_cast<std::u16string>(getStart(value)), u"non-css");
        style.setProperty(u"counter-increment", getReversed() ? u"list-item" : u"list-item -1", u"non-css");
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

bool HTMLOListElementImp::getReversed()
{
    return getAttributeAsBoolean(u"reversed");
}

void HTMLOListElementImp::setReversed(bool reversed)
{
    setAttributeAsBoolean(u"reversed", reversed);
}

int HTMLOListElementImp::getStart()
{
    return getStart(getAttribute(u"start"));
}

void HTMLOListElementImp::setStart(int start)
{
    setAttributeAsInteger(u"start", start);
}

bool HTMLOListElementImp::getCompact()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setCompact(bool compact)
{
    // TODO: implement me!
}

std::u16string HTMLOListElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLOListElementImp::setType(const std::u16string& type)
{
    // TODO: implement me!
}

}
}
}
}
