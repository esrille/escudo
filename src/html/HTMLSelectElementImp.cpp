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

#include "HTMLSelectElementImp.h"

#include <boost/bind.hpp>

#include <org/w3c/dom/events/MutationEvent.h>
#include <org/w3c/dom/html/HTMLOptionElement.h>

#include "HTMLCollectionImp.h"
#include "HTMLFormElementImp.h"
#include "HTMLOptGroupElementImp.h"
#include "HTMLOptionElementImp.h"
#include "HTMLOptionsCollectionImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLSelectElementImp::HTMLSelectElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"select"),
    mutationListener(boost::bind(&HTMLSelectElementImp::handleMutation, this, _1, _2)),
    mute(false)
{
    addEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
}

HTMLSelectElementImp::HTMLSelectElementImp(const HTMLSelectElementImp& org) :
    ObjectMixin(org),
    mutationListener(boost::bind(&HTMLSelectElementImp::handleMutation, this, _1, _2)),
    mute(false)
{
    addEventListener(u"DOMAttrModified", mutationListener, false, EventTargetImp::UseDefault);
}

void HTMLSelectElementImp::handleMutation(EventListenerImp* listener, events::Event event)
{
    if (mute || getMultiple())
        return;
    events::MutationEvent mutation(interface_cast<events::MutationEvent>(event));
    if (mutation.getAttrName() != u"selected")
        return;
    HTMLOptionElementPtr option = std::static_pointer_cast<HTMLOptionElementImp>(mutation.getTarget().self());
    if (!option)
        return;
    bool set = (mutation.getAttrChange() != events::MutationEvent::REMOVAL);
    mute = true;
    HTMLOptionElementPtr first;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (auto o = std::dynamic_pointer_cast<HTMLOptionElementImp>(e.self())) {
            if (option != o)
                o->setSelected(false);
            if (!o->getDisabled()) {
                if (!first)
                    first = o;
            } else if (option == o && set) {
                set = false;
                option->setSelected(false);
            }
        } else if (auto group = std::dynamic_pointer_cast<HTMLOptGroupElementImp>(e.self())) {
            for (Element f = group->getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (auto o = std::dynamic_pointer_cast<HTMLOptionElementImp>(f.self())) {
                    if (option != o)
                        o->setSelected(false);
                    if (!o->getDisabled() && !group->getDisabled()) {
                        if (!first)
                            first = o;
                    } else if (option == o && set) {
                        set = false;
                        option->setSelected(false);
                    }
                }
            }
        }
    }
    if (!set && first)
        first->setSelected(true);
    mute = false;
}

int HTMLSelectElementImp::getIndex(const HTMLOptionElementPtr& option)
{
    int index = -1;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (html::HTMLOptionElement::hasInstance(e)) {
            ++index;
            if (e == option)
                return index;
        } else if (html::HTMLOptGroupElement::hasInstance(e)) {
            for (Element f = e.getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (html::HTMLOptionElement::hasInstance(f)) {
                    ++index;
                    if (f == option)
                        return index;
                }
            }
        }
    }
    return index;
}

bool HTMLSelectElementImp::getAutofocus()
{
    return getAttributeAsBoolean(u"autofocus");
}

void HTMLSelectElementImp::setAutofocus(bool autofocus)
{
    setAttributeAsBoolean(u"autofocus", autofocus);
}

bool HTMLSelectElementImp::getDisabled()
{
    return getAttributeAsBoolean(u"disabled");
}

void HTMLSelectElementImp::setDisabled(bool disabled)
{
    setAttributeAsBoolean(u"disabled", disabled);
}

html::HTMLFormElement HTMLSelectElementImp::getForm()
{
    if (!form.expired())
        return form.lock();
    for (Element parent = getParentElement(); parent; parent = parent.getParentElement()) {
        if (html::HTMLFormElement::hasInstance(parent)) {
            form = std::dynamic_pointer_cast<HTMLFormElementImp>(parent.self());
            return form.lock();
        }
    }
    return nullptr;
}

bool HTMLSelectElementImp::getMultiple()
{
    return getAttributeAsBoolean(u"multiple");
}

void HTMLSelectElementImp::setMultiple(bool multiple)
{
    setAttributeAsBoolean(u"multiple", multiple);
}

std::u16string HTMLSelectElementImp::getName()
{
    return getAttribute(u"name");
}

void HTMLSelectElementImp::setName(const std::u16string& name)
{
    setAttribute(u"name", name);
}

bool HTMLSelectElementImp::getRequired()
{
    return getAttributeAsBoolean(u"required");
}

void HTMLSelectElementImp::setRequired(bool required)
{
    setAttributeAsBoolean(u"required", required);
}

unsigned int HTMLSelectElementImp::getSize()
{
    // For historical reasons, the default value of the size IDL attribute
    // does not return the actual size used.
    // cf. http://www.w3.org/html/wg/drafts/html/CR/forms.html#dom-select-size
    return getAttributeAsUnsigned(u"size", 0);
}

void HTMLSelectElementImp::setSize(unsigned int size)
{
    setAttributeAsUnsigned(u"size", size);
}

std::u16string HTMLSelectElementImp::getType()
{
    return getMultiple() ? u"select-multiple" : u"select-one";
}

html::HTMLOptionsCollection HTMLSelectElementImp::getOptions()
{
    if (options)
        return options;
    try {
        return std::make_shared<HTMLOptionsCollectionImp>(std::dynamic_pointer_cast<HTMLSelectElementImp>(self()));
    } catch (...) {
        return nullptr;
    }
}

unsigned int HTMLSelectElementImp::getLength()
{
    unsigned int length = 0;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (html::HTMLOptionElement::hasInstance(e))
            ++length;
        else if (html::HTMLOptGroupElement::hasInstance(e)) {
            for (Element f = e.getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (html::HTMLOptionElement::hasInstance(f))
                    ++length;
            }
        }
    }
    return length;
}

void HTMLSelectElementImp::setLength(unsigned int length)
{
    // TODO: implement me!
}

Element HTMLSelectElementImp::item(unsigned int index)
{
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (html::HTMLOptionElement::hasInstance(e)) {
            if (index-- == 0)
                return e;
        } else if (html::HTMLOptGroupElement::hasInstance(e)) {
            for (Element f = e.getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (html::HTMLOptionElement::hasInstance(f)) {
                    if (index-- == 0)
                        return f;
                }
            }
        }
    }
    return nullptr;
}

Object HTMLSelectElementImp::namedItem(const std::u16string& name)
{
    if (name.empty())
        return nullptr;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (html::HTMLOptionElement::hasInstance(e)) {
            if (e.getId() == name || e.getAttribute(u"name") == name)
                return e;
        } else if (html::HTMLOptGroupElement::hasInstance(e)) {
            for (Element f = e.getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (html::HTMLOptionElement::hasInstance(f)) {
                    if (f.getId() == name || f.getAttribute(u"name") == name)
                        return f;
                }
            }
        }
    }
    return nullptr;
}

void HTMLSelectElementImp::add(Any element)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::add(Any element, Any before)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::remove(int index)
{
    // TODO: implement me!
}

void HTMLSelectElementImp::setElement(unsigned int index, html::HTMLOptionElement option)
{
    // TODO: implement me!
}

html::HTMLCollection HTMLSelectElementImp::getSelectedOptions()
{
    try {
        HTMLCollectionPtr collection = std::make_shared<HTMLCollectionImp>();
        for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
            if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(e.self())) {
                if (option->getSelected() && !option->getDisabled())
                    collection->addItem(option);
            } else if (auto group = std::dynamic_pointer_cast<HTMLOptGroupElementImp>(e.self())) {
                if (!group->getDisabled()) {
                    for (Element f = group->getFirstElementChild(); f; f = f.getNextElementSibling()) {
                        if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(f.self())) {
                            if (option->getSelected() && !option->getDisabled())
                                collection->addItem(option);
                        }
                    }
                }
            }
        }
        return collection;
    } catch (...) {
        return nullptr;
    }
}

int HTMLSelectElementImp::getSelectedIndex()
{
    HTMLOptionElementPtr first;
    int firstIndex = -1;
    int index = -1;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(e.self())) {
            ++index;
            if (!option->getDisabled()) {
                if (!first) {
                    first = option;
                    firstIndex = index;
                }
                if (option->getSelected())
                    return index;
            }
        } else if (auto group = std::dynamic_pointer_cast<HTMLOptGroupElementImp>(e.self())) {
            for (Element f = e.getFirstElementChild(); f; f = f.getNextElementSibling()) {
                if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(f.self())) {
                    ++index;
                    if (!option->getDisabled() && !group->getDisabled()) {
                        if (!first) {
                            first = option;
                            firstIndex = index;
                        }
                        if (option->getSelected())
                            return index;
                    }
                }
            }
        }
    }
    if (!getMultiple() && first) {
        first->setSelected(true);
        return firstIndex;
    }
    return -1;
}

void HTMLSelectElementImp::setSelectedIndex(int selectedIndex)
{
    html::HTMLOptionElement option(interface_cast<html::HTMLOptionElement>(item(selectedIndex)));
    if (option)
        option.setSelected(true);
}

std::u16string HTMLSelectElementImp::getValue()
{
    html::HTMLOptionElement option;
    for (Element e = getFirstElementChild(); e; e = e.getNextElementSibling()) {
        if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(e.self())) {
            if (option->getSelected() && !option->getDisabled())
                return option->getValue();
        } else if (auto group = std::dynamic_pointer_cast<HTMLOptGroupElementImp>(e.self())) {
            if (!group->getDisabled()) {
                for (Element f = group->getFirstElementChild(); f; f = f.getNextElementSibling()) {
                    if (auto option = std::dynamic_pointer_cast<HTMLOptionElementImp>(f.self())) {
                        if (option->getSelected() && !option->getDisabled())
                            return option->getValue();
                    }
                }
            }
        }
    }
    return u"";
}

void HTMLSelectElementImp::setValue(const std::u16string& value)
{
    // TODO: implement me!
}

bool HTMLSelectElementImp::getWillValidate()
{
    // TODO: implement me!
    return 0;
}

html::ValidityState HTMLSelectElementImp::getValidity()
{
    // TODO: implement me!
    return nullptr;
}

std::u16string HTMLSelectElementImp::getValidationMessage()
{
    // TODO: implement me!
    return u"";
}

bool HTMLSelectElementImp::checkValidity()
{
    // TODO: implement me!
    return 0;
}

void HTMLSelectElementImp::setCustomValidity(const std::u16string& error)
{
    // TODO: implement me!
}

NodeList HTMLSelectElementImp::getLabels()
{
    // TODO: implement me!
    return nullptr;
}

}
}
}
}
