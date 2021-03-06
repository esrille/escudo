// Generated by esidl 0.4.0.
// This file is expected to be modified for the Web IDL interface
// implementation.  Permission to use, copy, modify and distribute
// this file in any software license is hereby granted.

#ifndef ORG_W3C_DOM_BOOTSTRAP_DOMPOINTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_DOMPOINTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/DOMPoint.h>
#include "DOMPointReadOnlyImp.h"

#include <org/w3c/dom/DOMPointReadOnly.h>
#include <org/w3c/dom/DOMPoint.h>
#include <org/w3c/dom/DOMPointInit.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class DOMPointImp : public ObjectMixin<DOMPointImp, DOMPointReadOnlyImp>
{
public:
    // DOMPoint
    double getX();
    void setX(double x);
    double getY();
    void setY(double y);
    double getZ();
    void setZ(double z);
    double getW();
    void setW(double w);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DOMPoint::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DOMPoint::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_DOMPOINTIMP_H_INCLUDED
