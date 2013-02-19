/*
 * Copyright 2013 Esrille Inc.
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

#include "HTMLMarqueeElementImp.h"

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "DocumentImp.h"
#include "HTMLUtil.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLMarqueeElementImp::HTMLMarqueeElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"marquee")
{
}

HTMLMarqueeElementImp::HTMLMarqueeElementImp(HTMLMarqueeElementImp* org, bool deep) :
    ObjectMixin(org, deep)
{
}

HTMLMarqueeElementImp::~HTMLMarqueeElementImp()
{
}

void HTMLMarqueeElementImp::handleMutation(events::MutationEvent mutation)
{
    ECMAScriptContext* context = getOwnerDocumentImp()->getContext();
    css::CSSStyleDeclaration style(getStyle());
    std::u16string value = mutation.getNewValue();
    bool compile = false;
    if (!value.empty()) {
        switch (mutation.getAttrChange()) {
        case events::MutationEvent::MODIFICATION:
        case events::MutationEvent::ADDITION:
            compile = true;
            break;
        default:
            break;
        }
    }
    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"height"):
        if (mapToDimension(value = mutation.getNewValue()))
            style.setProperty(u"height", value, u"non-css");
        break;
    case Intern(u"hspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-left", value, u"non-css");
            style.setProperty(u"margin-right", value, u"non-css");
        }
        break;
    case Intern(u"vspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-top", value, u"non-css");
            style.setProperty(u"margin-bottom", value, u"non-css");
        }
        break;
    case Intern(u"width"):
        if (mapToDimension(value = mutation.getNewValue()))
            style.setProperty(u"width", value, u"non-css");
        break;
    // Event handlers
    case Intern(u"onbounce"):
        setOnbounce(compile ? context->compileFunction(value) : 0);
        break;
    case Intern(u"onfinish"):
        setOnfinish(compile ? context->compileFunction(value) : 0);
        break;
    case Intern(u"onstart"):
        setOnstart(compile ? context->compileFunction(value) : 0);
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

std::u16string HTMLMarqueeElementImp::getBehavior()
{
    // TODO: implement me!
    return u"";
}

void HTMLMarqueeElementImp::setBehavior(const std::u16string& behavior)
{
    // TODO: implement me!
}

std::u16string HTMLMarqueeElementImp::getBgColor()
{
    // TODO: implement me!
    return u"";
}

void HTMLMarqueeElementImp::setBgColor(const std::u16string& bgColor)
{
    // TODO: implement me!
}

std::u16string HTMLMarqueeElementImp::getDirection()
{
    // TODO: implement me!
    return u"";
}

void HTMLMarqueeElementImp::setDirection(const std::u16string& direction)
{
    // TODO: implement me!
}

std::u16string HTMLMarqueeElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLMarqueeElementImp::setHeight(const std::u16string& height)
{
    // TODO: implement me!
}

unsigned int HTMLMarqueeElementImp::getHspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setHspace(unsigned int hspace)
{
    // TODO: implement me!
}

int HTMLMarqueeElementImp::getLoop()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setLoop(int loop)
{
    // TODO: implement me!
}

unsigned int HTMLMarqueeElementImp::getScrollAmount()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setScrollAmount(unsigned int scrollAmount)
{
    // TODO: implement me!
}

unsigned int HTMLMarqueeElementImp::getScrollDelay()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setScrollDelay(unsigned int scrollDelay)
{
    // TODO: implement me!
}

bool HTMLMarqueeElementImp::getTrueSpeed()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setTrueSpeed(bool trueSpeed)
{
    // TODO: implement me!
}

unsigned int HTMLMarqueeElementImp::getVspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLMarqueeElementImp::setVspace(unsigned int vspace)
{
    // TODO: implement me!
}

std::u16string HTMLMarqueeElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLMarqueeElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLMarqueeElementImp::getOnbounce()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"bounce"));
}

void HTMLMarqueeElementImp::setOnbounce(events::EventHandlerNonNull onbounce)
{
    setEventHandler(u"bounce", onbounce);
}

events::EventHandlerNonNull HTMLMarqueeElementImp::getOnfinish()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"finish"));
}

void HTMLMarqueeElementImp::setOnfinish(events::EventHandlerNonNull onfinish)
{
    setEventHandler(u"finish", onfinish);
}

events::EventHandlerNonNull HTMLMarqueeElementImp::getOnstart()
{
    return interface_cast<events::EventHandlerNonNull>(getEventHandler(u"start"));
}

void HTMLMarqueeElementImp::setOnstart(events::EventHandlerNonNull onstart)
{
    setEventHandler(u"start", onstart);
}

void HTMLMarqueeElementImp::start()
{
    // TODO: implement me!
}

void HTMLMarqueeElementImp::stop()
{
    // TODO: implement me!
}

}
}
}
}
