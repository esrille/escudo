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

#include "HTMLFrameSetElementImp.h"

#include <org/w3c/dom/html/Window.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

std::u16string HTMLFrameSetElementImp::getCols()
{
    // TODO: implement me!
    return u"";
}

void HTMLFrameSetElementImp::setCols(const std::u16string& cols)
{
    // TODO: implement me!
}

std::u16string HTMLFrameSetElementImp::getRows()
{
    // TODO: implement me!
    return u"";
}

void HTMLFrameSetElementImp::setRows(const std::u16string& rows)
{
    // TODO: implement me!
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnafterprint()
{
    return getOwnerDocument().getDefaultView().getOnafterprint();
}

void HTMLFrameSetElementImp::setOnafterprint(events::EventHandlerNonNull onafterprint)
{
    getOwnerDocument().getDefaultView().setOnafterprint(onafterprint);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnbeforeprint()
{
    return getOwnerDocument().getDefaultView().getOnbeforeprint();
}

void HTMLFrameSetElementImp::setOnbeforeprint(events::EventHandlerNonNull onbeforeprint)
{
    getOwnerDocument().getDefaultView().setOnbeforeprint(onbeforeprint);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnbeforeunload()
{
    return getOwnerDocument().getDefaultView().getOnbeforeunload();
}

void HTMLFrameSetElementImp::setOnbeforeunload(events::EventHandlerNonNull onbeforeunload)
{
    getOwnerDocument().getDefaultView().setOnbeforeunload(onbeforeunload);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnblur()
{
    return getOwnerDocument().getDefaultView().getOnblur();
}

void HTMLFrameSetElementImp::setOnblur(events::EventHandlerNonNull onblur)
{
    getOwnerDocument().getDefaultView().setOnblur(onblur);
}

events::OnErrorEventHandlerNonNull HTMLFrameSetElementImp::getOnerror()
{
    return getOwnerDocument().getDefaultView().getOnerror();
}

void HTMLFrameSetElementImp::setOnerror(events::OnErrorEventHandlerNonNull onerror)
{
    getOwnerDocument().getDefaultView().setOnerror(onerror);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnfocus()
{
    return getOwnerDocument().getDefaultView().getOnfocus();
}

void HTMLFrameSetElementImp::setOnfocus(events::EventHandlerNonNull onfocus)
{
    getOwnerDocument().getDefaultView().setOnfocus(onfocus);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnhashchange()
{
    return getOwnerDocument().getDefaultView().getOnhashchange();
}

void HTMLFrameSetElementImp::setOnhashchange(events::EventHandlerNonNull onhashchange)
{
    getOwnerDocument().getDefaultView().setOnhashchange(onhashchange);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnload()
{
    return getOwnerDocument().getDefaultView().getOnload();
}

void HTMLFrameSetElementImp::setOnload(events::EventHandlerNonNull onload)
{
    getOwnerDocument().getDefaultView().setOnload(onload);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnmessage()
{
    return getOwnerDocument().getDefaultView().getOnmessage();
}

void HTMLFrameSetElementImp::setOnmessage(events::EventHandlerNonNull onmessage)
{
    getOwnerDocument().getDefaultView().setOnmessage(onmessage);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnoffline()
{
    return getOwnerDocument().getDefaultView().getOnoffline();
}

void HTMLFrameSetElementImp::setOnoffline(events::EventHandlerNonNull onoffline)
{
    getOwnerDocument().getDefaultView().setOnoffline(onoffline);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnonline()
{
    return getOwnerDocument().getDefaultView().getOnonline();
}

void HTMLFrameSetElementImp::setOnonline(events::EventHandlerNonNull ononline)
{
    getOwnerDocument().getDefaultView().setOnonline(ononline);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnpagehide()
{
    return getOwnerDocument().getDefaultView().getOnpagehide();
}

void HTMLFrameSetElementImp::setOnpagehide(events::EventHandlerNonNull onpagehide)
{
    getOwnerDocument().getDefaultView().setOnpagehide(onpagehide);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnpageshow()
{
    return getOwnerDocument().getDefaultView().getOnpageshow();
}

void HTMLFrameSetElementImp::setOnpageshow(events::EventHandlerNonNull onpageshow)
{
    getOwnerDocument().getDefaultView().setOnpageshow(onpageshow);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnpopstate()
{
    return getOwnerDocument().getDefaultView().getOnpopstate();
}

void HTMLFrameSetElementImp::setOnpopstate(events::EventHandlerNonNull onpopstate)
{
    getOwnerDocument().getDefaultView().setOnpopstate(onpopstate);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnresize()
{
    return getOwnerDocument().getDefaultView().getOnresize();
}

void HTMLFrameSetElementImp::setOnresize(events::EventHandlerNonNull onresize)
{
    getOwnerDocument().getDefaultView().setOnresize(onresize);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnscroll()
{
    return getOwnerDocument().getDefaultView().getOnscroll();
}

void HTMLFrameSetElementImp::setOnscroll(events::EventHandlerNonNull setOnscroll)
{
    getOwnerDocument().getDefaultView().setOnscroll(setOnscroll);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnstorage()
{
    return getOwnerDocument().getDefaultView().getOnstorage();
}

void HTMLFrameSetElementImp::setOnstorage(events::EventHandlerNonNull onstorage)
{
    getOwnerDocument().getDefaultView().setOnstorage(onstorage);
}

events::EventHandlerNonNull HTMLFrameSetElementImp::getOnunload()
{
    return getOwnerDocument().getDefaultView().getOnunload();
}

void HTMLFrameSetElementImp::setOnunload(events::EventHandlerNonNull onunload)
{
    getOwnerDocument().getDefaultView().setOnunload(onunload);
}

}
}
}
}
