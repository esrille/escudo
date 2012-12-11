/*
 * Copyright 2010-2012 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_HTMLMETERELEMENTIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_HTMLMETERELEMENTIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/html/HTMLMeterElement.h>
#include "HTMLElementImp.h"

#include <org/w3c/dom/html/HTMLElement.h>
#include <org/w3c/dom/NodeList.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class HTMLMeterElementImp : public ObjectMixin<HTMLMeterElementImp, HTMLElementImp>
{
public:
    // HTMLMeterElement
    double getValue();
    void setValue(double value);
    double getMin();
    void setMin(double min);
    double getMax();
    void setMax(double max);
    double getLow();
    void setLow(double low);
    double getHigh();
    void setHigh(double high);
    double getOptimum();
    void setOptimum(double optimum);
    NodeList getLabels();
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return html::HTMLMeterElement::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return html::HTMLMeterElement::getMetaData();
    }
    HTMLMeterElementImp(DocumentImp* ownerDocument) :
        ObjectMixin(ownerDocument, u"meter") {
    }
    HTMLMeterElementImp(HTMLMeterElementImp* org, bool deep) :
        ObjectMixin(org, deep) {
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_HTMLMETERELEMENTIMP_H_INCLUDED
