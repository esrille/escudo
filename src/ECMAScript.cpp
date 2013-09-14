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

#include "ECMAScript.h"

#include "WindowProxy.h"

#include "Test.util.h"

void ECMAScriptContext::dispatchEvent(org::w3c::dom::bootstrap::EventListenerImp* listener, org::w3c::dom::events::Event event)
{
    if (!isMainThread())
        return;

    assert(listener);
    Object functionObject(listener->getEventHandler());
    if (!functionObject)
        return;
    Any arg(event);
    Any result = callFunction(event.getCurrentTarget(), functionObject, 1, &arg);
    if (event.getType() == u"mouseover") {
        if (result.toBoolean())
            event.preventDefault();
    } else if (org::w3c::dom::html::BeforeUnloadEvent::hasInstance(event)) {
        org::w3c::dom::html::BeforeUnloadEvent unloadEvent = interface_cast<org::w3c::dom::html::BeforeUnloadEvent>(event);
        if (unloadEvent.getReturnValue().empty() && result.isString())
            unloadEvent.setReturnValue(result.toString());
    } else if (!result.toBoolean())
        event.preventDefault();
}

ECMAScriptContext* ECMAScriptContext::getCurrentContext()
{
    auto window = dynamic_cast<org::w3c::dom::bootstrap::WindowProxy*>(current);
    if (!window)
        return 0;
    return window->getWindowPtr()->getContext();
}
