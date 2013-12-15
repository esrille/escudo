/*
 * Copyright 2013 Esrille Inc.
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

#include "HTMLOptionElementImp.h"

#include <org/w3c/dom/html/HTMLOptGroupElement.h>
#include <org/w3c/dom/html/HTMLSelectElement.h>

#include "one_at_a_time.hpp"

constexpr auto Intern = &one_at_a_time::hash<char16_t>;

#include "utf.h"

#include "DocumentImp.h"
#include "ECMAScript.h"
#include "WindowProxy.h"
#include "HTMLSelectElementImp.h"

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{

HTMLOptionElementImp::HTMLOptionElementImp(DocumentImp* ownerDocument) :
    ObjectMixin(ownerDocument, u"option"),
    disabled(false),
    selected(false)
{
}

HTMLOptionElementImp::HTMLOptionElementImp(const HTMLOptionElementImp& org) :
    ObjectMixin(org),
    disabled(org.disabled),
    selected(org.selected)
{
}

void HTMLOptionElementImp::handleMutation(events::MutationEvent mutation)
{
    switch (Intern(mutation.getAttrName().c_str())) {
    case Intern(u"disabled"):
        if (mutation.getAttrChange() == events::MutationEvent::REMOVAL)
            disabled = false;
        else
            disabled = true;
        break;
    case Intern(u"selected"):
        if (mutation.getAttrChange() == events::MutationEvent::REMOVAL)
            selected = false;
        else
            selected = true;
        break;
    default:
        HTMLElementImp::handleMutation(mutation);
        break;
    }
}

HTMLSelectElementPtr HTMLOptionElementImp::getSelect()
{
    Element parent = getParentElement();
    if (!parent)
        return nullptr;
    if (html::HTMLOptGroupElement::hasInstance(parent))
        parent = parent.getParentElement();
    if (!html::HTMLSelectElement::hasInstance(parent))
        return nullptr;
    return std::dynamic_pointer_cast<HTMLSelectElementImp>(parent.self());
}

bool HTMLOptionElementImp::getDisabled()
{
    return disabled;
}

void HTMLOptionElementImp::setDisabled(bool disabled)
{
    setAttributeAsBoolean(u"disabled", disabled);
}

html::HTMLFormElement HTMLOptionElementImp::getForm()
{
    if (HTMLSelectElementPtr select = getSelect())
        return select->getForm();
    return nullptr;
}

std::u16string HTMLOptionElementImp::getLabel()
{
    Nullable<std::u16string> label(getAttribute(u"label"));
    if (label.hasValue())
        return label.value();
    return getText();
}

void HTMLOptionElementImp::setLabel(const std::u16string& label)
{
    setAttribute(u"label", label);
}

bool HTMLOptionElementImp::getDefaultSelected()
{
    return getSelected();
}

void HTMLOptionElementImp::setDefaultSelected(bool defaultSelected)
{
    setSelected(defaultSelected);
}

bool HTMLOptionElementImp::getSelected()
{
    return selected;
}

void HTMLOptionElementImp::setSelected(bool selected)
{
    setAttributeAsBoolean(u"selected", selected);
}

std::u16string HTMLOptionElementImp::getValue()
{
    Nullable<std::u16string> value(getAttribute(u"value"));
    if (value.hasValue())
        return value.value();
    return getText();
}

void HTMLOptionElementImp::setValue(const std::u16string& value)
{
    setAttribute(u"value", value);
}

std::u16string HTMLOptionElementImp::getText()
{
    std::u16string text = getTextContent();
    return stripLeadingAndTrailingWhitespace(text);
}

void HTMLOptionElementImp::setText(const std::u16string& text)
{
    setTextContent(text);
}

int HTMLOptionElementImp::getIndex()
{
    if (HTMLSelectElementPtr select = getSelect())
        return select->getIndex(std::static_pointer_cast<HTMLOptionElementImp>(self()));
    return 0;
}

}  // org::w3c::dom::bootstrap

namespace html {

namespace {

class Constructor : public Object
{
public:
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv) {
        auto window = dynamic_cast<bootstrap::WindowProxy*>(ECMAScriptContext::getCurrent());
        if (!window)
            return nullptr;
        auto owner = static_cast<bootstrap::DocumentImp*>(window->getDocument().self().get());
        if (!owner)
            return nullptr;
        bootstrap::HTMLOptionElementPtr option;
        switch (argc) {
        case 0:
            option = std::make_shared<bootstrap::HTMLOptionElementImp>(owner);
            break;
        case 1:
            option = std::make_shared<bootstrap::HTMLOptionElementImp>(owner);
            option->setText(argv[0].toString());
            break;
        case 2:
            option = std::make_shared<bootstrap::HTMLOptionElementImp>(owner);
            option->setText(argv[0].toString());
            option->setValue(argv[1].toString());
            break;
        case 3:
            option = std::make_shared<bootstrap::HTMLOptionElementImp>(owner);
            option->setText(argv[0].toString());
            option->setValue(argv[1].toString());
            option->setDefaultSelected(argv[2].toBoolean());
            break;
        case 4:
            option = std::make_shared<bootstrap::HTMLOptionElementImp>(owner);
            option->setText(argv[0].toString());
            option->setValue(argv[1].toString());
            option->setDefaultSelected(argv[2].toBoolean());
            option->setSelected(argv[3].toBoolean());
            break;
        default:
            break;
        }
        return option;
    }
    Constructor() :
        Object(this) {
    }
};

}  // namespace

Object HTMLOptionElement::getConstructor()
{
    static Constructor constructor;
    return constructor.self();
}

}

}}}  // org::w3c::dom::bootstrap
