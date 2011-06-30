/*
 * Copyright 2011 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_MUTATIONEVENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_MUTATIONEVENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/events/MutationEvent.h>
#include "EventImp.h"

#include <org/w3c/dom/events/Event.h>
#include <org/w3c/dom/Node.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class MutationEventImp : public ObjectMixin<MutationEventImp, EventImp>
{
    Object         relatedNode;
    std::u16string prevValue;
    std::u16string newValue;
    std::u16string attrName;
    unsigned short attrChange;
public:
    MutationEventImp() :
        relatedNode(0),
        attrChange(0)
    {
    }

    // MutationEvent
    Object getRelatedNode() {
        return relatedNode;
    }
    std::u16string getPrevValue() {
        return prevValue;
    }
    std::u16string getNewValue() {
        return newValue;
    }
    std::u16string getAttrName() {
        return attrName;
    }
    unsigned short getAttrChange() {
        return attrChange;
    }
    void initMutationEvent(std::u16string typeArg, bool canBubbleArg, bool cancelableArg, Object relatedNodeArg, std::u16string prevValueArg, std::u16string newValueArg, std::u16string attrNameArg, unsigned short attrChangeArg);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return events::MutationEvent::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return events::MutationEvent::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_MUTATIONEVENTIMP_H_INCLUDED
