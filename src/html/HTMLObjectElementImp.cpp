/*
 * Copyright 2011-2013 Esrille Inc.
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

#include <boost/bind.hpp>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "DocumentImp.h"
#include "HTMLUtil.h"
#include "css/Box.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLObjectElementImp::HTMLObjectElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"object")
{
}

HTMLObjectElementImp::HTMLObjectElementImp(HTMLObjectElementImp* org, bool deep) :
    ObjectMixin(org, deep)
{
}

void HTMLObjectElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    // Styles
    case Intern(u"border"):
        handleMutationBorder(mutation);
        break;
    case Intern(u"height"):
        if (mapToDimension(value))
            style.setProperty(u"height", value, u"non-css");
        break;
    case Intern(u"hspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-left", value, u"non-css");
            style.setProperty(u"margin-right", value, u"non-css");
        }
        break;
    case Intern(u"vspace"):
        if (mapToDimension(value)) {
            style.setProperty(u"margin-top", value, u"non-css");
            style.setProperty(u"margin-bottom", value, u"non-css");
        }
        break;
    case Intern(u"width"):
        if (mapToDimension(value))
            style.setProperty(u"width", value, u"non-css");
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

void HTMLObjectElementImp::eval()
{
    if (!active)
        return;

    std::u16string classid = getAttribute(u"classid");
    if (!classid.empty()) {
        active = false;
        return;
    }
    std::u16string data = getAttribute(u"data");
    if (data.empty()) {
        active = false;
        return;
    }
    std::u16string type = getAttribute(u"type");
    // TODO: Check type is a supported one.

    DocumentImp* document = getOwnerDocumentImp();
    request = new(std::nothrow) HttpRequest(document->getDocumentURI());
    if (request) {
        request->open(u"GET", data);
        request->setHandler(boost::bind(&HTMLObjectElementImp::notify, this));
        document->incrementLoadEventDelayCount();
        request->send();
    } else
        active = false;
}

void HTMLObjectElementImp::notify()
{
    if (request->getStatus() != 200)
        active = false;
    else {
        // TODO: Check type
        image = new(std::nothrow) BoxImage;
        if (!image)
            active = false;
        else {
            if (FILE* file = request->openFile()) {
                image->open(file);
                fclose(file);
            }
            if (image->getState() != BoxImage::CompletelyAvailable) {
                active = false;
                delete image;
                image = 0;
            }
        }
    }
    // TODO: fire 'load' or 'error' event
    if (Box* box = getBox()) {
        box->setFlags(Box::NEED_REFLOW);
        Box* ancestor = box->getParentBox();
        if (ancestor && !dynamic_cast<Block*>(ancestor)) {
            // Update inline image
            ancestor = ancestor->getParentBox();
            while (ancestor && !dynamic_cast<Block*>(ancestor))
                ancestor = ancestor->getParentBox();
            if (ancestor)
                ancestor->setFlags(Box::NEED_REFLOW);
        }
    }
    DocumentImp* document = getOwnerDocumentImp();
    document->decrementLoadEventDelayCount();
}

std::u16string HTMLObjectElementImp::getData()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setData(const std::u16string& data)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getType()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setType(const std::u16string& type)
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

void HTMLObjectElementImp::setName(const std::u16string& name)
{
    setAttribute(u"name", name);
}

std::u16string HTMLObjectElementImp::getUseMap()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setUseMap(const std::u16string& useMap)
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

void HTMLObjectElementImp::setWidth(const std::u16string& width)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getHeight()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setHeight(const std::u16string& height)
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

void HTMLObjectElementImp::setCustomValidity(const std::u16string& error)
{
    // TODO: implement me!
}

Any HTMLObjectElementImp::operator() (Variadic<Any> arguments)
{
    // TODO: implement me!
    return 0;
}

std::u16string HTMLObjectElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getArchive()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setArchive(const std::u16string& archive)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getCode()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCode(const std::u16string& code)
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

void HTMLObjectElementImp::setStandby(const std::u16string& standby)
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

std::u16string HTMLObjectElementImp::getCodeBase()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCodeBase(const std::u16string& codeBase)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getCodeType()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setCodeType(const std::u16string& codeType)
{
    // TODO: implement me!
}

std::u16string HTMLObjectElementImp::getBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLObjectElementImp::setBorder(const std::u16string& border)
{
    // TODO: implement me!
}

}
}
}
}
