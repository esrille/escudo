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

#include "HTMLOListElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLOListElementImp::eval()
{
    Nullable<std::u16string> attr = getAttribute(u"start");
    if (attr.hasValue()) {
        getStyle().setProperty(u"counter-reset", u"list-item " + attr.value(), u"non-css");
        getStyle().setProperty(u"counter-increment", u"list-item -1");
    }
}

bool HTMLOListElementImp::getReversed()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setReversed(bool reversed)
{
    // TODO: implement me!
}

int HTMLOListElementImp::getStart()
{
    // TODO: implement me!
    return 0;
}

void HTMLOListElementImp::setStart(int start)
{
    // TODO: implement me!
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

void HTMLOListElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

}
}
}
}
