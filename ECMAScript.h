/*
 * Copyright 2012 Esrille Inc.
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

#ifndef ES_ECMASCRIPT_H
#define ES_ECMASCRIPT_H

#include <boost/scoped_ptr.hpp>
#include <Object.h>
#include <org/w3c/dom/html/Function.h>
#include <org/w3c/dom/html/BeforeUnloadEvent.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventTarget.h>

Any callFunction(Object thisObject, Object functionObject, int argc, Any* argv);

class ECMAScriptContext
{
    class Impl;
    boost::scoped_ptr<Impl> pimpl;

    // ECMAScriptContext is not copyable.
    ECMAScriptContext(const ECMAScriptContext&);
    ECMAScriptContext& operator=(const ECMAScriptContext&);

public:
    ECMAScriptContext();
    ~ECMAScriptContext();

    void activate(ObjectImp* window);
    void evaluate(const std::u16string& script);
    Object* compileFunction(const std::u16string& body);
    Any callFunction(Object thisObject, Object functionObject, int argc, Any* argv);

    void callFunction(org::w3c::dom::html::Function function, org::w3c::dom::events::Event event)
    {
        Any arg(event);
        Any result = callFunction(event.getCurrentTarget(), function, 1, &arg);
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

    static void shutDown();
};

#endif  // ES_ECMASCRIPT_H
