/*
 * Copyright 2011, 2012 Esrille Inc.
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

#ifndef ESV8API_H_INCLUDED
#define ESV8API_H_INCLUDED

#include <v8.h>

#include <map>

#include "Object.h"
#include "Reflect.h"

class ObjectImp;

class NativeClass
{
    const char* metaData;
    v8::Persistent<v8::FunctionTemplate> classTemplate;
public:
    NativeClass(v8::Handle<v8::ObjectTemplate> global, const char* metadata, Object (*getConstructor)() = 0);
    ~NativeClass();

    v8::Handle<v8::Object> createJSObject(ObjectImp* self);

    static v8::Handle<v8::Value> constructor(const v8::Arguments& args);
    static void finalize(v8::Persistent<v8::Value> object, void* parameter);

private:
    static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> interfaceMap;
    static std::map<ObjectImp*, v8::Persistent<v8::Object>> wrapperMap;
};

class ProxyObject : public Object
{
    v8::Persistent<v8::Object> jsobject;
    unsigned int count;
protected:
    virtual unsigned int retain_() {
        return ++count;
    };
    virtual unsigned int release_() {
        if (0 < count)
            --count;
        if (count == 0) {
            delete this;
            return 0;
        }
        return count;
    };
public:
    ProxyObject(v8::Handle<v8::Object> obj) :
        Object(this),
        jsobject(v8::Persistent<v8::Object>::New(obj)),
        count(1)
    {
    }
    ~ProxyObject()
    {
        jsobject.Dispose();
        jsobject.Clear();
    }
    v8::Persistent<v8::Object>& getJSObject() {
        return jsobject;
    }
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv);
};

#endif  // ESV8API_H_INCLUDED
