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

#include "HTMLBindingElementImp.h"

#include <org/w3c/dom/html/HTMLTemplateElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

html::HTMLTemplateElement HTMLBindingElementImp::cloneTemplate()
{
    html::HTMLTemplateElement t = getTemplate();
    if (!t)
        return 0;
    return interface_cast<html::HTMLTemplateElement>(t.cloneNode(true));
}

// HTMLBindingElement

html::HTMLTemplateElement HTMLBindingElementImp::getTemplate()
{
    for (auto i = getFirstElementChild(); i; i = i.getNextElementSibling()) {
        if (i.getTagName() == u"template")
            return interface_cast<html::HTMLTemplateElement>(i);
    }
    return 0;
}

void HTMLBindingElementImp::setImplementation(Object implementationPrototypeObject)
{
    // TODO: implement me!
}

void HTMLBindingElementImp::addToDocument()
{
    // TODO: implement me!
}

}
}
}
}
