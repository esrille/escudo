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

#include "HTMLFormElementImp.h"

#include "HTMLFormControlsCollectionImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLFormElementImp::~HTMLFormElementImp()
{
    pastNamesMap.clear();
}

std::u16string HTMLFormElementImp::getAcceptCharset()
{
    return getAttribute(u"accept-charset");
}

void HTMLFormElementImp::setAcceptCharset(std::u16string acceptCharset)
{
    setAttribute(u"accept-charset", acceptCharset);
}

std::u16string HTMLFormElementImp::getAction()
{
    return getAttribute(u"action");
}

void HTMLFormElementImp::setAction(std::u16string action)
{
    setAttribute(u"action", action);
}

std::u16string HTMLFormElementImp::getAutocomplete()
{
    return getAttribute(u"autocomplete");
}

void HTMLFormElementImp::setAutocomplete(std::u16string autocomplete)
{
    setAttribute(u"autocomplete", autocomplete);
}

std::u16string HTMLFormElementImp::getEnctype()
{
    return getAttribute(u"enctype");
}

void HTMLFormElementImp::setEnctype(std::u16string enctype)
{
    setAttribute(u"enctype", enctype);
}

std::u16string HTMLFormElementImp::getEncoding()
{
    return getEnctype();
}

void HTMLFormElementImp::setEncoding(std::u16string encoding)
{
    setEnctype(encoding);
}

std::u16string HTMLFormElementImp::getMethod()
{
    return getAttribute(u"method");
}

void HTMLFormElementImp::setMethod(std::u16string method)
{
    setAttribute(u"method", method);
}

std::u16string HTMLFormElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLFormElementImp::setName(std::u16string name)
{
    setAttribute(u"name", name);
}

bool HTMLFormElementImp::getNoValidate()
{
    return hasAttribute(u"novalidate");
}

void HTMLFormElementImp::setNoValidate(bool noValidate)
{
    if (noValidate)
        setAttribute(u"novalidate", u"");
    else
        removeAttribute(u"novalidate");
}

std::u16string HTMLFormElementImp::getTarget()
{
    return getAttribute(u"target");
}

void HTMLFormElementImp::setTarget(std::u16string target)
{
    setAttribute(u"target", target);
}

html::HTMLFormControlsCollection HTMLFormElementImp::getElements()
{
    if (!elements)
        elements = new(std::nothrow) HTMLFormControlsCollectionImp(this);
    return elements;
}

int HTMLFormElementImp::getLength()
{
    getElements();
    if (elements)
        return elements.getLength();
    return 0;
}

Any HTMLFormElementImp::getElement(unsigned int index)
{
    getElements();
    if (elements)
        return elements.item(index);
    return 0;
}

Any HTMLFormElementImp::getElement(std::u16string name)
{
    getElements();
    if (elements) {
        Object candidate = elements.namedItem(name);
        if (Element::hasInstance(candidate))
            pastNamesMap.insert(std::pair<const std::u16string, Element>(name, interface_cast<Element>(candidate)));
        return candidate;
    }
    auto found = pastNamesMap.find(name);
    if (found != pastNamesMap.end())
        return found->second;
    return 0;
}

void HTMLFormElementImp::submit()
{
    // TODO: implement me!
}

void HTMLFormElementImp::reset()
{
    // TODO: implement me!
}

bool HTMLFormElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

}
}
}
}
