// Generated by esidl 0.3.0.
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLDIALOGELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLDIALOGELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLDialogElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/events/MouseEvent.h>
#include <org/w3c/dom/Element.h>
#include <org/w3c/dom/html/HTMLElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLDialogElementImp : public ObjectMixin<HTMLDialogElementImp, HTMLElementImp>
{
public:
    // HTMLDialogElement
    bool getOpen();
    void setOpen(bool open);
    std::u16string getReturnValue();
    void setReturnValue(const std::u16string& returnValue);
    void show();
    void show(Any anchor);
    void showModal();
    void showModal(Any anchor);
    void close();
    void close(const std::u16string& returnValue);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLDialogElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLDialogElement::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLDIALOGELEMENTIMP_H_INCLUDED
