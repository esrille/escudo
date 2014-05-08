/*
 * Copyright 2014 Esrille Inc.
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

#ifndef ORG_W3C_DOM_BOOTSTRAP_DOMRECTREADONLYIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_DOMRECTREADONLYIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/DOMRectReadOnly.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class DOMRectReadOnlyImp : public ObjectMixin<DOMRectReadOnlyImp>
{
protected:
    double x;
    double y;
    double width;
    double height;
public:
    // DOMRectReadOnly
    double getX() {
        return x;
    }
    double getY() {
        return y;
    }
    double getWidth() {
        return width;
    }
    double getHeight() {
        return height;
    }
    double getTop() {
        return (0.0 <= height) ? y : (y + height);
    }
    double getRight() {
        return (0.0 <= width) ? (x + width) : x;
    }
    double getBottom() {
        return (0.0 <= height) ? (y + height) : y;
    }
    double getLeft() {
        return (0.0 <= width) ? x : (x + width);
    }
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return DOMRectReadOnly::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return DOMRectReadOnly::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_DOMRECTREADONLYIMP_H_INCLUDED
