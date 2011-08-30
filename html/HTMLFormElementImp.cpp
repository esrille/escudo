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
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setAcceptCharset(std::u16string acceptCharset)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getAction()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setAction(std::u16string action)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getAutocomplete()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setAutocomplete(std::u16string autocomplete)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getEnctype()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setEnctype(std::u16string enctype)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getEncoding()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setEncoding(std::u16string encoding)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getMethod()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setMethod(std::u16string method)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setName(std::u16string name)
{
    // TODO: implement me!
}

bool HTMLFormElementImp::getNoValidate()
{
    // TODO: implement me!
    return 0;
}

void HTMLFormElementImp::setNoValidate(bool noValidate)
{
    // TODO: implement me!
}

std::u16string HTMLFormElementImp::getTarget()
{
    // TODO: implement me!
    return u"";
}

void HTMLFormElementImp::setTarget(std::u16string target)
{
    // TODO: implement me!
}

html::HTMLFormControlsCollection HTMLFormElementImp::getElements()
{
    if (!elements)
        elements = new(std::nothrow) HTMLFormControlsCollectionImp(this);
    return elements;
}

int HTMLFormElementImp::getLength()
{
    // TODO: implement me!
    return 0;
}

Any HTMLFormElementImp::getElement(unsigned int index)
{
    // TODO: implement me!
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
