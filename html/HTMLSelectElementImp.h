// Generated by esidl (r1745).
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLSelectElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/NodeList.h>
#include <org/w3c/dom/html/HTMLCollection.h>
#include <org/w3c/dom/html/HTMLOptionsCollection.h>
#include <org/w3c/dom/html/HTMLFormElement.h>
#include <org/w3c/dom/html/ValidityState.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLSelectElementImp : public ObjectMixin<HTMLSelectElementImp, HTMLElementImp>
{
public:
    // HTMLSelectElement
    bool getAutofocus() __attribute__((weak));
    void setAutofocus(bool autofocus) __attribute__((weak));
    bool getDisabled() __attribute__((weak));
    void setDisabled(bool disabled) __attribute__((weak));
    html::HTMLFormElement getForm() __attribute__((weak));
    bool getMultiple() __attribute__((weak));
    void setMultiple(bool multiple) __attribute__((weak));
    std::u16string getName() __attribute__((weak));
    void setName(std::u16string name) __attribute__((weak));
    unsigned int getSize() __attribute__((weak));
    void setSize(unsigned int size) __attribute__((weak));
    std::u16string getType() __attribute__((weak));
    html::HTMLOptionsCollection getOptions() __attribute__((weak));
    unsigned int getLength() __attribute__((weak));
    void setLength(unsigned int length) __attribute__((weak));
    Any item(unsigned int index) __attribute__((weak));
    Any namedItem(std::u16string name) __attribute__((weak));
    void add(html::HTMLElement element) __attribute__((weak));
    void add(html::HTMLElement element, html::HTMLElement before) __attribute__((weak));
    void add(html::HTMLElement element, int before) __attribute__((weak));
    void remove(int index) __attribute__((weak));
    html::HTMLCollection getSelectedOptions() __attribute__((weak));
    int getSelectedIndex() __attribute__((weak));
    void setSelectedIndex(int selectedIndex) __attribute__((weak));
    std::u16string getValue() __attribute__((weak));
    void setValue(std::u16string value) __attribute__((weak));
    bool getWillValidate() __attribute__((weak));
    html::ValidityState getValidity() __attribute__((weak));
    std::u16string getValidationMessage() __attribute__((weak));
    bool checkValidity() __attribute__((weak));
    void setCustomValidity(std::u16string error) __attribute__((weak));
    NodeList getLabels() __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLSelectElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLSelectElement::getMetaData();
    }
    HTMLSelectElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"select") {
    }
    HTMLSelectElementImp(HTMLSelectElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLSELECTELEMENTIMP_H_INCLUDED
