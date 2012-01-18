/*
 * Copyright 2011, 2012 Esrille Inc.
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

#include "HTMLObjectElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

void HTMLObjectElementImp::eval()
{
    HTMLElementImp::eval();
    HTMLElementImp::evalBorder(this);
    HTMLElementImp::evalHeight(this);
    HTMLElementImp::evalWidth(this);
    HTMLElementImp::evalHspace(this);
    HTMLElementImp::evalVspace(this);
}

std::u16string HTMLObjectElementImp::getData()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setData(std::u16string data)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setType(std::u16string type)
{
    // TODO: implement me!
}

bool HTMLObjectElementImp::getTypeMustMatch()
{
    // TODO: implement me!
    return 0;
}

void HTMLObjectElementImp::setTypeMustMatch(bool typeMustMatch)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLObjectElementImp::setName(std::u16string name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLObjectElementImp::getUseMap()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setUseMap(std::u16string useMap)
{
    // TODO: implement me!
}

html::HTMLFormElement HTMLObjectElementImp::getForm()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLObjectElementImp::getWidth()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setWidth(std::u16string width)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setHeight(std::u16string height)
{
    // TODO: implement me!
}

Document HTMLObjectElementImp::getContentDocument()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

html::Window HTMLObjectElementImp::getContentWindow()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

bool HTMLObjectElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLObjectElementImp::getValidity()
{
    // TODO: implement me!
    return static_cast<Object*>(0);
}

std::u16string HTMLObjectElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLObjectElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLObjectElementImp::setCustomValidity(std::u16string error)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setAlign(std::u16string align)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getArchive()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setArchive(std::u16string archive)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setBorder(std::u16string border)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getCode()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCode(std::u16string code)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getCodeBase()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCodeBase(std::u16string codeBase)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getCodeType()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCodeType(std::u16string codeType)
{
    // TODO: implement me!
}

bool HTMLObjectElementImp::getDeclare()
{
    // TODO: implement me!
    return 0;
}

void HTMLObjectElementImp::setDeclare(bool declare)
{
    // TODO: implement me!
}

unsigned int HTMLObjectElementImp::getHspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLObjectElementImp::setHspace(unsigned int hspace)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getStandby()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setStandby(std::u16string standby)
{
    // TODO: implement me!
}

unsigned int HTMLObjectElementImp::getVspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLObjectElementImp::setVspace(unsigned int vspace)
{
    // TODO: implement me!
}

}
}
}
}
