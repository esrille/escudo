/*
 * Copyright 2011-2013 Esrille Inc.
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

#ifndef ESJSAPI_H_INCLUDED
#define ESJSAPI_H_INCLUDED

#include <js/jsapi.h>

#include <list>
#include <memory>

#include "Object.h"
#include "Reflect.h"

class ObjectImp;

class NativeClass
{
    static const size_t MAX_METHOD_COUNT = 256;
    static const size_t MAX_CONSTRUCTOR_COUNT = 64;
    static const size_t MAX_RANK = 16;

    static std::list<NativeClass*> nativeClassList;

    static JSNative operations[MAX_METHOD_COUNT];
    static JSNative staticOperations[MAX_CONSTRUCTOR_COUNT];
    static JSNative constructors[MAX_CONSTRUCTOR_COUNT];
    static JSPropertyOp getters[MAX_RANK];
    static JSStrictPropertyOp setters[MAX_RANK];
    static Object (*constructorGetters[MAX_CONSTRUCTOR_COUNT])();
    static int constructorCount;

    const char* metadata;
    NativeClass* proto;
    int protoRank;
    char name[48];
    JSClass jsclass;
    std::unique_ptr<uint32_t[]> hashTable;

    std::unique_ptr<JSPropertySpec[]> ps;
    std::unique_ptr<JSFunctionSpec[]> fs;
    std::unique_ptr<JSFunctionSpec[]> sfs;
    std::unique_ptr<char[]> stringHeap;
    JSNative ctor;

    char* storeName(char* heap, const Reflect::Property& prop)
    {
        std::string name = prop.getName();
        strcpy(heap, name.c_str());
        return heap + name.length() + 1;
    }

public:
    NativeClass(const char* metadata, Object (*getConstructor)() = 0);

    JSObject* registerClass(JSContext* cx, JSObject* global);

    JSObject* createInstance(JSContext* cx, Object* other);

    uint32_t getHash(int n) const {
        uint32_t* table = hashTable.get();
        return table ? table[n] : 0;
    }

    JSClass* getJSClass() {
        return &jsclass;
    }

    JSObject* createJSObject(JSContext* cx, ObjectImp* self);

    static bool isNativeClass(JSClass* jsclass);

    template <int N>
    static JSBool staticOperation(JSContext* cx, uintN argc, jsval* vp);

    template <int N>
    static JSBool constructor(JSContext* cx, uintN argc, jsval* vp);

    template <int R>
    static JSBool getter(JSContext* cx, JSObject* obj, jsid id, jsval* vp);
    template <int R>
    static JSBool setter(JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp);

    static uint32_t getHash(JSContext* cx, jsval* vp, int n);

    static NativeClass* lookupNativeClass(const char* name) {
        for (auto i = nativeClassList.begin(); i != nativeClassList.end(); ++i) {
            if (!std::strcmp((*i)->name, name))
                return *i;
        }
        return 0;
    }
    static void registerClasses(JSContext* cx, JSObject* global) {
        for (auto i = nativeClassList.begin(); i != nativeClassList.end(); ++i)
            (*i)->registerClass(cx, global);
    }
};

class ProxyObject : public Imp
{
    JSContext* jscontext;
    JSObject* jsobject;
public:
    ProxyObject(JSContext* jscontext, JSObject* obj) :
        jscontext(jscontext),
        jsobject(obj)
    {
        JS_AddObjectRoot(jscontext, &jsobject);
    }
    ~ProxyObject()
    {
        JS_RemoveObjectRoot(jscontext, &jsobject);
    }
    JSObject* getJSObject() const {
        return jsobject;
    }
    virtual Any message_(uint32_t selector, const char* id, int argc, Any* argv);
};

#endif  // ESJSAPI_H_INCLUDED
