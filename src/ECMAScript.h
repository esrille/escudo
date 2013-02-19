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

#ifndef ES_ECMASCRIPT_H
#define ES_ECMASCRIPT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/scoped_ptr.hpp>
#include <org/w3c/dom/html/BeforeUnloadEvent.h>
#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/events/EventTarget.h>
#include <org/w3c/dom/events/EventHandlerNonNull.h>

#include "EventListenerImp.h"

Any callFunction(Object thisObject, Object functionObject, int argc, Any* argv);

class ECMAScriptContext
{
    typedef org::w3c::dom::events::EventHandlerNonNull Function;

    class Impl;
    boost::scoped_ptr<Impl> pimpl;
    static ObjectImp* current;

    // ECMAScriptContext is not copyable.
    ECMAScriptContext(const ECMAScriptContext&);
    ECMAScriptContext& operator=(const ECMAScriptContext&);

public:
    ECMAScriptContext();
    ~ECMAScriptContext();

    void enter(ObjectImp* windowProxy);
    void exit(ObjectImp* windowProxy);

    Any evaluate(const std::u16string& script);
    Object* compileFunction(const std::u16string& body);
    Any callFunction(Object thisObject, Object functionObject, int argc, Any* argv);

    void dispatchEvent(org::w3c::dom::bootstrap::EventListenerImp* listener, org::w3c::dom::events::Event event);

    Object* xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree);

    static ObjectImp* getCurrent() {
        return current;
    }
    static ECMAScriptContext* getCurrentContext();
    static void shutDown();
};

#endif  // ES_ECMASCRIPT_H
