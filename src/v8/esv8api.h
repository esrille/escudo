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
    Object (*getConstructor)();
    v8::Persistent<v8::FunctionTemplate> classTemplate;
public:
    NativeClass(v8::Handle<v8::ObjectTemplate> global, const char* metadata, Object (*getConstructor)() = 0);
    ~NativeClass();

    v8::Handle<v8::Object> createJSObject(ObjectImp* self);

    static v8::Handle<v8::Value> staticOperation(const v8::Arguments& args);
    static v8::Handle<v8::Value> constructor(const v8::Arguments& args);
#ifdef V8_HAVE_ISOLATE
    static void finalize(v8::Isolate*, v8::Persistent<v8::Value> object, void* parameter);
#else
    static void finalize(v8::Persistent<v8::Value> object, void* parameter);
#endif

private:
    static std::map<std::string, v8::Persistent<v8::FunctionTemplate>> interfaceMap;
    static std::map<ObjectImp*, v8::Persistent<v8::Object>> wrapperMap;
};

class ProxyObject : public Imp
{
    v8::Persistent<v8::Object> jsobject;
public:
    ProxyObject(v8::Handle<v8::Object> obj) :
#ifdef V8_HAVE_ISOLATE
        jsobject(v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), obj))
#else
        jsobject(v8::Persistent<v8::Object>::New(obj))
#endif
    {
    }
    ~ProxyObject()
    {
#ifdef V8_HAVE_ISOLATE
        jsobject.Dispose(v8::Isolate::GetCurrent());
#else
        jsobject.Dispose();
#endif
        jsobject.Clear();
    }
    v8::Persistent<v8::Object>& getJSObject() {
        return jsobject;
    }
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv);
};

#endif  // ESV8API_H_INCLUDED
