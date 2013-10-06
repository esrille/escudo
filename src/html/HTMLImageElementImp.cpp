/*
 * Copyright 2010-2013 Esrille Inc.
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

#include "HTMLImageElementImp.h"

#include <boost/bind.hpp>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "DocumentImp.h"
#include "HTMLUtil.h"
#include "css/Box.h"

namespace org { namespace w3c { namespace dom { namespace bootstrap {

HTMLImageElementImp::HTMLImageElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"img")
{
}

void HTMLImageElementImp::handleMutation(events::MutationEvent mutation)
{
    std::u16string value = mutation.getNewValue();
    css::CSSStyleDeclaration style(getStyle());

    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"src"):
        if (DocumentPtr document = getOwnerDocumentImp()) {
            if (current)
                current->cancel();
            current = std::make_shared<HttpRequest>(document->getDocumentURI());
            if (current) {
                current->open(u"GET", getSrc());
                current->setHandler(boost::bind(&HTMLImageElementImp::notify, this, current));
                document->incrementLoadEventDelayCount(current->getURL());
                current->send();
            } else
                active = false;
        }
        break;
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

void HTMLImageElementImp::notify(const HttpRequestPtr& request)
{
    if (current != request)
        return;

    if (current->getStatus() != 200)
        active = false;
    else {
        // TODO: Check type
        delete image;
        image = new(std::nothrow) BoxImage;
        if (!image)
            active = false;
        else {
            if (FILE* file = current->openFile()) {
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
    if (DocumentPtr document = getOwnerDocumentImp())
        document->decrementLoadEventDelayCount(request->getURL());
}

// HTMLImageElement
std::u16string HTMLImageElementImp::getAlt()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setAlt(const std::u16string& alt)
{
    // TODO: implement me!
}


std::u16string HTMLImageElementImp::getSrc()
{
    return getAttributeAsURL(u"src");
}

void HTMLImageElementImp::setSrc(const std::u16string& src)
{
    setAttribute(u"src", src);
}

std::u16string HTMLImageElementImp::getCrossOrigin()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setCrossOrigin(const std::u16string& crossOrigin)
{
    // TODO: implement me!
}

std::u16string HTMLImageElementImp::getUseMap()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setUseMap(const std::u16string& useMap)
{
    // TODO: implement me!
}

bool HTMLImageElementImp::getIsMap()
{
    // TODO: implement me!
    return 0;
}

void HTMLImageElementImp::setIsMap(bool isMap)
{
    // TODO: implement me!
}

unsigned int HTMLImageElementImp::getWidth()
{
    // TODO: implement me!
    return 0;
}

void HTMLImageElementImp::setWidth(unsigned int width)
{
    // TODO: implement me!
}

unsigned int HTMLImageElementImp::getHeight()
{
    // TODO: implement me!
    return 0;
}

void HTMLImageElementImp::setHeight(unsigned int height)
{
    // TODO: implement me!
}

unsigned int HTMLImageElementImp::getNaturalWidth()
{
    // TODO: implement me!
    return 0;
}

unsigned int HTMLImageElementImp::getNaturalHeight()
{
    // TODO: implement me!
    return 0;
}

bool HTMLImageElementImp::getComplete()
{
    // TODO: implement me!
    return 0;
}

std::u16string HTMLImageElementImp::getName()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setName(const std::u16string& name)
{
    // TODO: implement me!
}

std::u16string HTMLImageElementImp::getAlign()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setAlign(const std::u16string& align)
{
    // TODO: implement me!
}

std::u16string HTMLImageElementImp::getBorder()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setBorder(const std::u16string& border)
{
    // TODO: implement me!
}

unsigned int HTMLImageElementImp::getHspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLImageElementImp::setHspace(unsigned int hspace)
{
    // TODO: implement me!
}

std::u16string HTMLImageElementImp::getLongDesc()
{
    // TODO: implement me!
    return u"";
}

void HTMLImageElementImp::setLongDesc(const std::u16string& longDesc)
{
    // TODO: implement me!
}

unsigned int HTMLImageElementImp::getVspace()
{
    // TODO: implement me!
    return 0;
}

void HTMLImageElementImp::setVspace(unsigned int vspace)
{
    // TODO: implement me!
}

}  // org::w3c::dom::bootstrap

namespace html {

namespace {

class Constructor : public Object
{
public:
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        bootstrap::HTMLImageElementPtr img;
        switch (argc) {
        case 0:
            img = std::make_shared<bootstrap::HTMLImageElementImp>(nullptr);
            break;
        case 1:
            img = std::make_shared<bootstrap::HTMLImageElementImp>(nullptr);
            if (img)
                img->setWidth(argv[0]);
            break;
        case 2:
            img = std::make_shared<bootstrap::HTMLImageElementImp>(nullptr);
            if (img) {
                img->setWidth(argv[0]);
                img->setHeight(argv[1]);
            }
            break;
        default:
            break;
        }
        return img;
    }
    Constructor() :
        Object(this) {
    }
};

}  // namespace

Object HTMLImageElement::getConstructor()
{
    static Constructor constructor;
    return constructor.self();
}

}

}}}  // org::w3c::dom
