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

#include "NodeListImp.h"
#include "NodeImp.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

Node NodeListImp::item(unsigned int index)
{
    // TODO: a more faster version might be necessary.
    if (NodeImp* imp = dynamic_cast<NodeImp*>(node.self())) {
        if (imp->getChildCount() <= index)
            return 0;
        NodeImp* item;
        for (item = imp->firstChild; 0 < index && item; --index)
            item = item->nextSibling;
        return item;
    }
    return 0;
}

unsigned int NodeListImp::getLength()
{
    if (NodeImp* imp = dynamic_cast<NodeImp*>(node.self()))
        return imp->getChildCount();
    return 0;
}

NodeListImp::NodeListImp(Node node) :
    node(node)
{
}

}}}}  // org::w3c::dom::bootstrap
