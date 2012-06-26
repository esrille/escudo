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

#include <boost/bind.hpp>

#include "utf.h"

#include "DocumentImp.h"
#include "EventImp.h"
#include "url/URL.h"

#include "html/HTMLButtonElementImp.h"
#include "html/HTMLInputElementImp.h"
#include "html/HTMLKeygenElementImp.h"
#include "html/HTMLObjectElementImp.h"
#include "html/HTMLSelectElementImp.h"
#include "html/HTMLTextAreaElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

namespace {

bool isSubmittableElements(const std::u16string& tag)
{
    if (tag == u"button" ||     // is submittable elements?
        tag == u"input" ||
        tag == u"keygen" ||
        tag == u"object" ||
        tag == u"select" ||
        tag == u"textarea")
        return true;
    return false;
}

bool hasDatalistAncestor(ElementImp* field)
{
    // TODO: implement me!
    return false;
}

bool isDisabled(ElementImp* field)
{
    // TODO: implement me!
    return false;
}

void urlEncode(std::u16string& result, const std::u16string& value)
{
    std::string u;
    char buffer[8];

    const char16_t* b = value.c_str();
    while (*b) {
        char32_t utf32;
        b = utf16to32(b, &utf32);
        if (!b)
            break;
        if (char* p = utf32to8(utf32, buffer)) {  // TODO: support other encoding types
            *p = 0;
            for (char* s = buffer; *s; ++s) {
                unsigned char c = *s;
                if (c == ' ') {
                    result += '+';
                    continue;
                }
                if (isalnum(c) || strchr("*-._", c)) {
                    result += c;
                    continue;
                }
                result += '%';
                result += "0123456789ABCDEF"[c >> 4];
                result += "0123456789ABCDEF"[c & 0x0f];
            }
        }
    }
}

}  // namespace

void HTMLFormElementImp::
enumFormDataSet(ElementImp* submitter,
                boost::function<void (const std::u16string&, const std::u16string&, const std::u16string&)> callback)
{
    std::u16string name;
    std::u16string value;
    std::u16string type;

    ElementImp* field = this;
    while (field = field->getNextElement(this)) {
        std::u16string tag = field->getTagName();
        if (hasDatalistAncestor(field))
            continue;
        if (isDisabled(field))
            continue;
        if (HTMLButtonElementImp* button = dynamic_cast<HTMLButtonElementImp*>(field)) {
            type = button->getType();
            if (field != submitter)
                continue;
            name = button->getName();
            value = button->getValue();
        } else if (HTMLInputElementImp* input = dynamic_cast<HTMLInputElementImp*>(field)) {
            if (input->isButton() && field != submitter)
                continue;
            type = input->getType();
            if (type == u"checkbox" || type == u"radio") {
                if (!input->getChecked())
                    continue;
                // TODO: 6.
            } else if (type == u"image") {
                Nullable<std::u16string> name = input->getAttribute(u"name");
                if (!name.hasValue() || name.value().empty())
                    continue;
                // TODO: 3.
            } else if (type == u"file") {
                // TODO: 7.
            }
            name = input->getName();
            value = input->getValue();
        } else if (HTMLKeygenElementImp* keygen = dynamic_cast<HTMLKeygenElementImp*>(field)) {
            type = keygen->getType();
            name = keygen->getName();
            // TODO: The 'value' attribute seems to be missing in the HTMLKeygenElement interface in the spec.
        } else if (HTMLObjectElementImp* object = dynamic_cast<HTMLObjectElementImp*>(field)) {
            type = object->getType();
            // TODO: check plugin
            continue;
            // TODO: 8.
            name = object->getName();
            // TODO: The 'value' attribute seems to be missing in the HTMLObjectElement interface in the spec.
        } else if (HTMLSelectElementImp* select = dynamic_cast<HTMLSelectElementImp*>(field)) {
            type = select->getType();
            name = select->getName();
            // TODO: 5.
            value = select->getValue();
        } else if (HTMLTextAreaElementImp* textarea = dynamic_cast<HTMLTextAreaElementImp*>(field)) {
            type = textarea->getType();
            name = textarea->getName();
            value = textarea->getValue();
        } else
            continue;   // Not a submittable element
        // TODO: 10
        callback(name, value, type);
    }
}

void HTMLFormElementImp::appendEncodedFormData(std::u16string* result, const std::u16string& name, const std::u16string& value, const std::u16string& type)
{
    if (!(*result).empty())
        *result += '&';
    urlEncode(*result, name);
    *result += '=';
    urlEncode(*result, value);
}

std::u16string HTMLFormElementImp::getEncodedFormData(ElementImp* submitter)
{
    std::u16string result;
    enumFormDataSet(submitter, boost::bind(&HTMLFormElementImp::appendEncodedFormData, this, &result, _1, _2, _3));
    return result;
}

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

void HTMLFormElementImp::submit(ElementImp* submitter)
{
    // TODO: implement me!
    DocumentImp* document = getOwnerDocumentImp();
    if (!document)
        return;
    // TODO: 3. check document's browsing context.
    // TODO: 4. check submission more than once.
    // TODO: 5.
    // 6.
    events::Event event = new(std::nothrow) EventImp;
    event.initEvent(u"submit", true, true);
    this->dispatchEvent(event);
    if (event.getDefaultPrevented())
        return;
    // TODO: 7.
    // 8.
    std::u16string action = getAction();
    // 9.
    if (action.empty())
        action = document->getDocumentURI();
    // 10.
    URL url(document->getDocumentURI(), action);
    if (url.isEmpty())
        return;

    // 13.
    std::u16string method = getMethod();
    if (method.empty())
        method = u"GET";
    // 17.
    if (!compareIgnoreCase(method, u"GET") && !compareIgnoreCase(url.getProtocol(), u"http:")) {
        std::u16string query = getEncodedFormData(submitter);
        // TODO: set query to url!!
        url.setSearch(query);
        std::cerr << __func__ << ": " << static_cast<std::u16string>(url) << '\n';
        document->open(url, u"_self", u"", false); // TODO
    }
    // TODO: else
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
