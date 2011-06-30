// Generated by esidl (r1745).
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLSOURCEELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLSOURCEELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLSourceElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLSourceElementImp : public ObjectMixin<HTMLSourceElementImp, HTMLElementImp>
{
public:
    // HTMLSourceElement
    std::u16string getSrc() __attribute__((weak));
    void setSrc(std::u16string src) __attribute__((weak));
    std::u16string getType() __attribute__((weak));
    void setType(std::u16string type) __attribute__((weak));
    std::u16string getMedia() __attribute__((weak));
    void setMedia(std::u16string media) __attribute__((weak));
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLSourceElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLSourceElement::getMetaData();
    }
    HTMLSourceElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"source") {
    }
    HTMLSourceElementImp(HTMLSourceElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLSOURCEELEMENTIMP_H_INCLUDED
