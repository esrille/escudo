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

#ifndef ORG_W3C_DOM_BOOTSTRAP_UINT8CLAMPEDARRAYIMP_H_INCLUDED
#define ORG_W3C_DOM_BOOTSTRAP_UINT8CLAMPEDARRAYIMP_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <org/w3c/dom/Uint8ClampedArray.h>
#include "Uint8ArrayImp.h"

#include <org/w3c/dom/ArrayBuffer.h>
#include <org/w3c/dom/Uint8Array.h>
#include <org/w3c/dom/Uint8ClampedArray.h>

namespace org
{
namespace w3c
{
namespace dom
{
namespace bootstrap
{
class Uint8ClampedArrayImp : public ObjectMixin<Uint8ClampedArrayImp, Uint8ArrayImp>
{
public:
    // Uint8Array
    virtual void set(Uint8Array array);
    virtual void set(Uint8Array array, unsigned int offset);

    // Uint8ClampedArray
    virtual void set(unsigned int index, unsigned char value);
    void set(Uint8ClampedArray array);
    void set(Uint8ClampedArray array, unsigned int offset);
    virtual void set(ObjectArray<unsigned char> array);
    virtual void set(ObjectArray<unsigned char> array, unsigned int offset);
    virtual Uint8Array /* Uint8ClampedArray */ subarray(int start, int end);
    // Object
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv)
    {
        return Uint8ClampedArray::dispatch(this, selector, id, argc, argv);
    }
    static const char* const getMetaData()
    {
        return Uint8ClampedArray::getMetaData();
    }
};

}
}
}
}

#endif  // ORG_W3C_DOM_BOOTSTRAP_UINT8CLAMPEDARRAYIMP_H_INCLUDED
