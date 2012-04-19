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

#include "Script.h"

#include <assert.h>

#include <iostream>

namespace {

// a public domain hash function.
// cf. http://burtleburtle.net/bob/hash/doobs.html
inline uint32_t one_at_a_time(const char* key, size_t len)
{
    uint32_t hash, i;
    for (hash = 0, i = 0; i < len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// returns zero if key contains any non-ascii character.
inline uint32_t uc_one_at_a_time(const char16_t* key, size_t len)
{
    uint32_t hash, i;
    for (hash = 0, i = 0; i < len; ++i) {
        if (127 < key[i])
            return 0;
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

Object* convert(JSContext* cx, JSObject* obj)
{
    JSClass* cls = JS_GET_CLASS(cx, obj);
    if (cls && NativeClass::isNativeClass(cls))
        return static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
    // obj is a JavaScript object. Create a proxy for obj.
    return new(std::nothrow) ProxyObject(cx, obj);
}

Any convert(JSContext* cx, jsval& v)
{
    if (JSVAL_IS_VOID(v))
        return Any();
    if (JSVAL_IS_NULL(v))
        return Any(static_cast<Object*>(0));
    if (JSVAL_IS_INT(v))
        return JSVAL_TO_INT(v);
    if (JSVAL_IS_DOUBLE(v))
        return JSVAL_TO_DOUBLE(v);
    if (JSVAL_IS_BOOLEAN(v))
        return JSVAL_TO_BOOLEAN(v);
    if (JSVAL_IS_STRING(v)) {
        JSString* s = JSVAL_TO_STRING(v);
        size_t l;
        const jschar* b = JS_GetStringCharsAndLength(cx, s, &l);
        return std::u16string(reinterpret_cast<const char16_t*>(b), l);
    }
    if (JSVAL_IS_OBJECT(v))
        return convert(cx, JSVAL_TO_OBJECT(v));
    return Any();
}

JSObject* convert(JSContext* cx, Object* obj)
{
    JSObject* jsobj = 0;
    if (obj) {
        if (ProxyObject* p = dynamic_cast<ProxyObject*>(obj->self()))
            jsobj = p->getJSObject();
        else if (ObjectImp* imp = dynamic_cast<ObjectImp*>(obj->self()))
            jsobj = static_cast<NativeClass*>(imp->getStaticPrivate())->createJSObject(cx, imp);
    }
    return jsobj;
}

jsval convert(JSContext* cx, Any& v)
{
    switch (v.getType()) {
    case Any::Undefined:
        return JSVAL_VOID;
    case Any::Null:
        return JSVAL_NULL;
    case Any::Bool:
        return BOOLEAN_TO_JSVAL(static_cast<bool>(v));
    case Any::Int32:
        return INT_TO_JSVAL(static_cast<int32_t>(v));
    case Any::Uint32:
        return INT_TO_JSVAL(static_cast<uint32_t>(v));
    case Any::Int64:
    case Any::Uint64:
    case Any::Float32:
    case Any::Float64:
        return DOUBLE_TO_JSVAL(static_cast<double>(v));
    default:
        break;
    }
    if (v.isString()) {
        std::u16string s = v.toString();
        JSString* j = JS_NewUCStringCopyN(cx, reinterpret_cast<const jschar*>(s.c_str()), s.length());
        return STRING_TO_JSVAL(j);
    }
    if (v.isObject())
        return OBJECT_TO_JSVAL(convert(cx, v.toObject()));
    return JSVAL_VOID;
}

JSBool caller(JSContext* cx, uintN argc, jsval* vp)
{
    JSObject* obj = JSVAL_TO_OBJECT(JS_CALLEE(cx, vp));
    ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
    if (native) {
        Any arguments[argc];
        for (unsigned i = 0; i < argc; ++i)
            arguments[i] = convert(cx, JS_ARGV(cx, vp)[i]);
        Any result = native->message_(0, 0, argc, arguments);
        JS_SET_RVAL(cx, vp, convert(cx, result));
        return JS_TRUE;
    }
    return JS_FALSE;
}

template <int N>
JSBool operation(JSContext* cx, uintN argc, jsval* vp)
{
    if (JSObject* obj = JS_THIS_OBJECT(cx, vp)) {
        ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
        Any arguments[argc];
        for (unsigned i = 0; i < argc; ++i)
            arguments[i] = convert(cx, JS_ARGV(cx, vp)[i]);
        Any result = native->message_(NativeClass::getHash(cx, vp, N), 0, argc, arguments);
        JS_SET_RVAL(cx, vp, convert(cx, result));
        return JS_TRUE;
    }
    return JS_FALSE;
}

bool hasProperty(ObjectImp* native, const std::u16string& name)
{
    uint32_t hash = uc_one_at_a_time(name.c_str(), name.length());
    if (!hash)
        return false;
    // It seems specialGetter, etc. are not called for predefined properties; so just check operatons.
    Any result = native->message_(hash, 0, Object::HAS_OPERATION_, 0);
    return result.toBoolean();
}

std::u16string toString(JSContext* cx, jsid id)
{
    JSString* jsstring = JSID_TO_STRING(id);
    size_t length;
    const char16_t* string = reinterpret_cast<const char16_t*>(JS_GetStringCharsAndLength(cx, jsstring, &length));
    return std::u16string(string, length);
}

JSBool specialOp(JSContext* cx, JSObject* obj, jsid id, jsval* vp, int argc)
{
    ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
    if (!native)
        return JS_PropertyStub(cx, obj, id, vp);
    Any argument;
    if (JSID_IS_INT(id))
        argument = JSID_TO_INT(id);
    else if (JSID_IS_STRING(id)) {
        std::u16string name = toString(cx, id);
        if (hasProperty(native, name))
            return JS_PropertyStub(cx, obj, id, vp);
        argument = name;
    } else
        return JS_PropertyStub(cx, obj, id, vp);
    Any result = native->message_(0, 0, argc, &argument);
    JS_SET_RVAL(cx, vp, convert(cx, result));
    return JS_TRUE;
}

JSBool specialGetter(JSContext* cx, JSObject* obj, jsid id, jsval* vp)
{
    return specialOp(cx, obj, id, vp, Object::SPECIAL_GETTER_);
}

JSBool specialDeleter(JSContext* cx, JSObject* obj, jsid id, jsval* vp)
{
    return specialOp(cx, obj, id, vp, Object::SPECIAL_DELETER_);
}

JSBool specialSetter(JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
    ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
    if (!native)
        return JS_StrictPropertyStub(cx, obj, id, strict, vp);
    Any arguments[2];
    if (JSID_IS_INT(id))
        arguments[0] = JSID_TO_INT(id);
    else if (JSID_IS_STRING(id)) {
        std::u16string name = toString(cx, id);
        if (hasProperty(native, name))
            return JS_StrictPropertyStub(cx, obj, id, strict, vp);
        arguments[0] = name;
    } else
        return JS_StrictPropertyStub(cx, obj, id, strict, vp);
    arguments[1] = convert(cx, *vp);
    Any result = native->message_(0, 0, Object::SPECIAL_SETTER_, arguments);
    JS_SET_RVAL(cx, vp, convert(cx, result));
    return JS_TRUE;
}

void finalize(JSContext* cx, JSObject* obj)
{
    ObjectImp* imp = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
    if (imp && imp->getPrivate()) {
        imp->setPrivate(0);
        imp->release_();
    }
}

}  // namespace

// This one works only with a Function.
uint32_t NativeClass::getHash(JSContext* cx, jsval* vp, int n)
{
    JSObject* callee = JSVAL_TO_OBJECT(JS_CALLEE(cx, vp));
    assert(callee);
    JSObject* parent = JS_GetParent(cx, callee);
    assert(parent);
    JSClass* jsclass = JS_GET_CLASS(cx, parent);
    assert(jsclass);
    NativeClass* nc = reinterpret_cast<NativeClass*>(reinterpret_cast<char*>(jsclass) - offsetof(NativeClass, jsclass));
    return nc->getHash(n);
}

template <int N>
JSBool NativeClass::staticOperation(JSContext* cx, uintN argc, jsval* vp)
{
    Object (*getConstructor)() = NativeClass::constructorGetters[N];
    if (getConstructor) {
        uint32_t hash = 0;
        jsval val = JS_CALLEE(cx, vp);
        if (JSFunction* f = JS_ValueToFunction(cx, val)) {
            JSString* s = JS_GetFunctionId(f);
            size_t l;
            const jschar* b = JS_GetStringCharsAndLength(cx, s, &l);
            std::u16string name(reinterpret_cast<const char16_t*>(b), l);
            hash = uc_one_at_a_time(name.c_str(), name.length());
        }
        if (!hash)
            return JS_FALSE;
        Any arguments[argc];
        for (unsigned i = 0; i < argc; ++i)
            arguments[i] = convert(cx, JS_ARGV(cx, vp)[i]);
        Any result = getConstructor().message_(hash, "", argc, arguments).toObject();
        JS_SET_RVAL(cx, vp, convert(cx, result));
        return JS_TRUE;
    }
    return JS_FALSE;
}

template <int N>
JSBool NativeClass::constructor(JSContext* cx, uintN argc, jsval* vp)
{
    Object (*getConstructor)() = NativeClass::constructorGetters[N];
    if (getConstructor) {
        Any arguments[argc];
        for (unsigned i = 0; i < argc; ++i)
            arguments[i] = convert(cx, JS_ARGV(cx, vp)[i]);
        Any result = getConstructor().message_(0, "", argc, arguments).toObject();
        if (ObjectImp* imp = dynamic_cast<ObjectImp*>(result.toObject()->self())) {
            JSObject* obj = static_cast<NativeClass*>(imp->getStaticPrivate())->createJSObject(cx, imp);
            if (obj) {
                JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
                return JS_TRUE;
            }
        }
    }
    return JS_FALSE;
}

template <int R>
JSBool NativeClass::getter(JSContext* cx, JSObject* obj, jsid id, jsval* vp)
{
    if (JSID_IS_INT(id)) {
        ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
        NativeClass* nc = static_cast<NativeClass*>(native->getStaticPrivate());
        while (nc->protoRank != R)
            nc = nc->proto;
        uint32_t hash = nc->getHash(JSID_TO_INT(id));
        Any result = native->message_(hash, 0, Object::GETTER_, 0);
        JS_SET_RVAL(cx, vp, convert(cx, result));
        return JS_TRUE;
    }
    return JS_FALSE;
}

template <int R>
JSBool NativeClass::setter(JSContext* cx, JSObject* obj, jsid id, JSBool strict, jsval* vp)
{
    if (JSID_IS_INT(id)) {
        ObjectImp* native = static_cast<ObjectImp*>(JS_GetPrivate(cx, obj));
        NativeClass* nc = static_cast<NativeClass*>(native->getStaticPrivate());
        while (nc->protoRank != R)
            nc = nc->proto;
        uint32_t hash = nc->getHash(JSID_TO_INT(id));
        Any argument = convert(cx, *vp);
        Any result = native->message_(hash, 0, Object::SETTER_, &argument);
        JS_SET_RVAL(cx, vp, convert(cx, result));
        return JS_TRUE;
    }
    return JS_FALSE;
}

JSObject* NativeClass::createJSObject(JSContext* cx, ObjectImp* self)
{
    assert(self);
    JSObject* obj = static_cast<JSObject*>(self->getPrivate());
    if (!obj) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (obj) {
            self->retain_();
            self->setPrivate(obj);
            JS_SetPrivate(cx, obj, self);
        }
    }
    return obj;
}

bool NativeClass::isNativeClass(JSClass* jsclass)
{
    return jsclass->finalize == finalize;
}

JSNative NativeClass::operations[MAX_METHOD_COUNT] =
{
    &operation<0>, &operation<1>, &operation<2>, &operation<3>, &operation<4>,
    &operation<5>, &operation<6>, &operation<7>, &operation<8>, &operation<9>,
    &operation<10>, &operation<11>, &operation<12>, &operation<13>, &operation<14>,
    &operation<15>, &operation<16>, &operation<17>, &operation<18>, &operation<19>,
    &operation<20>, &operation<21>, &operation<22>, &operation<23>, &operation<24>,
    &operation<25>, &operation<26>, &operation<27>, &operation<28>, &operation<29>,
    &operation<30>, &operation<31>, &operation<32>, &operation<33>, &operation<34>,
    &operation<35>, &operation<36>, &operation<37>, &operation<38>, &operation<39>,
    &operation<40>, &operation<41>, &operation<42>, &operation<43>, &operation<44>,
    &operation<45>, &operation<46>, &operation<47>, &operation<48>, &operation<49>,
    &operation<50>, &operation<51>, &operation<52>, &operation<53>, &operation<54>,
    &operation<55>, &operation<56>, &operation<57>, &operation<58>, &operation<59>,
    &operation<60>, &operation<61>, &operation<62>, &operation<63>, &operation<64>,
    &operation<65>, &operation<66>, &operation<67>, &operation<68>, &operation<69>,
    &operation<70>, &operation<71>, &operation<72>, &operation<73>, &operation<74>,
    &operation<75>, &operation<76>, &operation<77>, &operation<78>, &operation<79>,
    &operation<80>, &operation<81>, &operation<82>, &operation<83>, &operation<84>,
    &operation<85>, &operation<86>, &operation<87>, &operation<88>, &operation<89>,
    &operation<90>, &operation<91>, &operation<92>, &operation<93>, &operation<94>,
    &operation<95>, &operation<96>, &operation<97>, &operation<98>, &operation<99>,
    &operation<100>, &operation<101>, &operation<102>, &operation<103>, &operation<104>,
    &operation<105>, &operation<106>, &operation<107>, &operation<108>, &operation<109>,
    &operation<110>, &operation<111>, &operation<112>, &operation<113>, &operation<114>,
    &operation<115>, &operation<116>, &operation<117>, &operation<118>, &operation<119>,
    &operation<120>, &operation<121>, &operation<122>, &operation<123>, &operation<124>,
    &operation<125>, &operation<126>, &operation<127>, &operation<128>, &operation<129>,
    &operation<130>, &operation<131>, &operation<132>, &operation<133>, &operation<134>,
    &operation<135>, &operation<136>, &operation<137>, &operation<138>, &operation<139>,
    &operation<140>, &operation<141>, &operation<142>, &operation<143>, &operation<144>,
    &operation<145>, &operation<146>, &operation<147>, &operation<148>, &operation<149>,
    &operation<150>, &operation<151>, &operation<152>, &operation<153>, &operation<154>,
    &operation<155>, &operation<156>, &operation<157>, &operation<158>, &operation<159>,
    &operation<160>, &operation<161>, &operation<162>, &operation<163>, &operation<164>,
    &operation<165>, &operation<166>, &operation<167>, &operation<168>, &operation<169>,
    &operation<170>, &operation<171>, &operation<172>, &operation<173>, &operation<174>,
    &operation<175>, &operation<176>, &operation<177>, &operation<178>, &operation<179>,
    &operation<180>, &operation<181>, &operation<182>, &operation<183>, &operation<184>,
    &operation<185>, &operation<186>, &operation<187>, &operation<188>, &operation<189>,
    &operation<190>, &operation<191>, &operation<192>, &operation<193>, &operation<194>,
    &operation<195>, &operation<196>, &operation<197>, &operation<198>, &operation<199>,
    &operation<200>, &operation<201>, &operation<202>, &operation<203>, &operation<204>,
    &operation<205>, &operation<206>, &operation<207>, &operation<208>, &operation<209>,
    &operation<210>, &operation<211>, &operation<212>, &operation<213>, &operation<214>,
    &operation<215>, &operation<216>, &operation<217>, &operation<218>, &operation<219>,
    &operation<220>, &operation<221>, &operation<222>, &operation<223>, &operation<224>,
    &operation<225>, &operation<226>, &operation<227>, &operation<228>, &operation<229>,
    &operation<230>, &operation<231>, &operation<232>, &operation<233>, &operation<234>,
    &operation<235>, &operation<236>, &operation<237>, &operation<238>, &operation<239>,
    &operation<240>, &operation<241>, &operation<242>, &operation<243>, &operation<244>,
    &operation<245>, &operation<246>, &operation<247>, &operation<248>, &operation<249>,
    &operation<250>, &operation<251>, &operation<252>, &operation<253>, &operation<254>,
    &operation<255>
};

JSNative NativeClass::staticOperations[MAX_CONSTRUCTOR_COUNT] =
{
    &staticOperation<0>, &staticOperation<1>, &staticOperation<2>, &staticOperation<3>, &staticOperation<4>,
    &staticOperation<5>, &staticOperation<6>, &staticOperation<7>, &staticOperation<8>, &staticOperation<9>,
    &staticOperation<10>, &staticOperation<11>, &staticOperation<12>, &staticOperation<13>, &staticOperation<14>,
    &staticOperation<15>, &staticOperation<16>, &staticOperation<17>, &staticOperation<18>, &staticOperation<19>,
    &staticOperation<20>, &staticOperation<21>, &staticOperation<22>, &staticOperation<23>, &staticOperation<24>,
    &staticOperation<25>, &staticOperation<26>, &staticOperation<27>, &staticOperation<28>, &staticOperation<29>,
    &staticOperation<30>, &staticOperation<31>, &staticOperation<32>, &staticOperation<33>, &staticOperation<34>,
    &staticOperation<35>, &staticOperation<36>, &staticOperation<37>, &staticOperation<38>, &staticOperation<39>,
    &staticOperation<40>, &staticOperation<41>, &staticOperation<42>, &staticOperation<43>, &staticOperation<44>,
    &staticOperation<45>, &staticOperation<46>, &staticOperation<47>, &staticOperation<48>, &staticOperation<49>,
    &staticOperation<50>, &staticOperation<51>, &staticOperation<52>, &staticOperation<53>, &staticOperation<54>,
    &staticOperation<55>, &staticOperation<56>, &staticOperation<57>, &staticOperation<58>, &staticOperation<59>,
    &staticOperation<60>, &staticOperation<61>, &staticOperation<62>, &staticOperation<63>
};

JSNative NativeClass::constructors[MAX_CONSTRUCTOR_COUNT] =
{
    &constructor<0>, &constructor<1>, &constructor<2>, &constructor<3>, &constructor<4>,
    &constructor<5>, &constructor<6>, &constructor<7>, &constructor<8>, &constructor<9>,
    &constructor<10>, &constructor<11>, &constructor<12>, &constructor<13>, &constructor<14>,
    &constructor<15>, &constructor<16>, &constructor<17>, &constructor<18>, &constructor<19>,
    &constructor<20>, &constructor<21>, &constructor<22>, &constructor<23>, &constructor<24>,
    &constructor<25>, &constructor<26>, &constructor<27>, &constructor<28>, &constructor<29>,
    &constructor<30>, &constructor<31>, &constructor<32>, &constructor<33>, &constructor<34>,
    &constructor<35>, &constructor<36>, &constructor<37>, &constructor<38>, &constructor<39>,
    &constructor<40>, &constructor<41>, &constructor<42>, &constructor<43>, &constructor<44>,
    &constructor<45>, &constructor<46>, &constructor<47>, &constructor<48>, &constructor<49>,
    &constructor<50>, &constructor<51>, &constructor<52>, &constructor<53>, &constructor<54>,
    &constructor<55>, &constructor<56>, &constructor<57>, &constructor<58>, &constructor<59>,
    &constructor<60>, &constructor<61>, &constructor<62>, &constructor<63>
};

JSPropertyOp NativeClass::getters[MAX_RANK] =
{
    &getter<0>, &getter<1>, &getter<2>, &getter<3>, &getter<4>,
    &getter<5>, &getter<6>, &getter<7>, &getter<8>, &getter<9>,
    &getter<10>, &getter<11>, &getter<12>, &getter<13>, &getter<14>,
    &getter<15>
};

JSStrictPropertyOp NativeClass::setters[MAX_RANK] =
{
    &setter<0>, &setter<1>, &setter<2>, &setter<3>, &setter<4>,
    &setter<5>, &setter<6>, &setter<7>, &setter<8>, &setter<9>,
    &setter<10>, &setter<11>, &setter<12>, &setter<13>, &setter<14>,
    &setter<15>
};

Object (*NativeClass::constructorGetters[MAX_CONSTRUCTOR_COUNT])();
int NativeClass::constructorCount;

NativeClass::NativeClass(JSContext* cx, JSObject* global, const char* metadata, Object (*getConstructor)()) :
    proto(0),
    protoRank(0),
    jsclass {
        name,
        JSCLASS_HAS_PRIVATE,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize
    }
{
    Reflect::Interface meta(metadata);

    std::string identifier = Reflect::getIdentifier(meta.getName());
    assert(identifier.length() + 1 < sizeof(name));
    strcpy(name, identifier.c_str());

    JSBool found;
    JS_HasProperty(cx, global, name, &found);
    if (found) {
        std::cerr << "error: Interface " << name << " has already been registered.\n";
        return;
    }

    JSObject* parentProto = 0;
    std::string parentName = Reflect::getIdentifier(meta.getExtends());
    if (0 < parentName.length()) {
        // Look up parentProto
        jsval val;
        if (JS_GetProperty(cx, global, parentName.c_str(), &val) && JSVAL_IS_OBJECT(val)) {
            JSObject* parent = JSVAL_TO_OBJECT(val);
            if (JS_GetProperty(cx, parent, "prototype", &val) && JSVAL_IS_OBJECT(val))
                parentProto = JSVAL_TO_OBJECT(val);
                JSClass* jsclass = JS_GET_CLASS(cx, parentProto);
                assert(jsclass);
                proto = reinterpret_cast<NativeClass*>(reinterpret_cast<char*>(jsclass) - offsetof(NativeClass, jsclass));
        } else {
            std::cerr << "error: Interface " << parentName << " has not been initialized.\n";
            return;
        }
    }
    for (auto chain = proto; chain; chain = chain->proto)
        ++protoRank;

    const int attributeCount = meta.getAttributeCount() + 1;
    const int operationCount = meta.getOperationCount() + 2;  // +1 for stringifier
    const int staticOperationCount = meta.getStaticOperationCount() + 1;
    std::unique_ptr<JSPropertySpec[]> ps(new JSPropertySpec[attributeCount]);
    std::unique_ptr<JSFunctionSpec[]> fs(new JSFunctionSpec[operationCount]);
    std::unique_ptr<JSFunctionSpec[]> sfs(new JSFunctionSpec[staticOperationCount]);
    std::unique_ptr<char[]> stringHeap(new char[meta.getStringSize()]);
    size_t propertyCount = attributeCount + operationCount + staticOperationCount;
    if (0 < propertyCount) {
        auto table = std::unique_ptr<uint32_t[]>(new uint32_t[propertyCount]);
        hashTable = std::move(table);
    }

    char* heap = stringHeap.get();
    JSPropertySpec* pps = ps.get();
    JSFunctionSpec* pfs = fs.get();
    JSFunctionSpec* psfs = sfs.get();
    size_t propertyNumber = 0;

    JSNative constructor = constructors[0];
    JSNative sop = staticOperations[0];
    if (meta.hasConstructor() || 0 < meta.getStaticOperationCount()) {
        constructor = constructors[++constructorCount];
        sop = staticOperations[constructorCount];
        constructorGetters[constructorCount] = getConstructor;
    }

    Reflect::Property prop(meta);
    while (prop.getType()) {
        switch (prop.getType()) {
        case Reflect::kOperation:
            if (!prop.isOmittable() && 0 < prop.getName().length()) {
                hashTable.get()[propertyNumber] = one_at_a_time(prop.getName().c_str(), prop.getName().length());
                if (prop.isStatic()) {
                    psfs->name = heap;
                    psfs->call = sop;
                    psfs->nargs = 0;
                    psfs->flags = JSPROP_ENUMERATE | JSPROP_PERMANENT;
                    heap = storeName(heap, prop);
                    ++psfs;
                } else {
                    pfs->name = heap;
                    pfs->call = operations[propertyNumber];
                    pfs->nargs = 0;
                    pfs->flags = JSPROP_ENUMERATE | JSPROP_PERMANENT;
                    heap = storeName(heap, prop);
                    ++pfs;
                    ++propertyNumber;
                }
            }
            if (prop.isGetter())
                jsclass.getProperty = specialGetter;
            if (prop.isSetter() || prop.isCreator())
                jsclass.setProperty = specialSetter;
            if (prop.isDeleter())
                jsclass.delProperty = specialDeleter;
            if (prop.isCaller())
                jsclass.call = caller;
            if (prop.isStringifier() && prop.getName() != "toString") {
                hashTable.get()[propertyNumber] = one_at_a_time(prop.getName().c_str(), prop.getName().length());
                pfs->name = "toString";
                pfs->call = operations[propertyNumber];
                pfs->nargs = 0;
                pfs->flags = JSPROP_ENUMERATE | JSPROP_PERMANENT;
                ++pfs;
                ++propertyNumber;
            }
            break;
        case Reflect::kAttribute:
            hashTable.get()[propertyNumber] = one_at_a_time(prop.getName().c_str(), prop.getName().length());
            pps->name = heap;
            pps->tinyid = propertyNumber++;
            pps->flags = JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED;
            if (prop.isReadOnly())
                pps->flags |= JSPROP_READONLY;
            pps->getter = getters[protoRank];
            pps->setter = setters[protoRank];
            heap = storeName(heap, prop);
            ++pps;

            if (prop.isStringifier()) {
                hashTable.get()[propertyNumber] = one_at_a_time(prop.getName().c_str(), prop.getName().length());
                pfs->name = "toString";
                pfs->call = operations[propertyNumber];
                pfs->nargs = 0;
                pfs->flags = JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_SHARED;
                ++pfs;
                ++propertyNumber;
            }
            break;
        default:
            break;
        }
        prop.next();
    }
    pfs->name = 0;
    pfs->call = 0;
    pfs->nargs = 0;
    pfs->flags = 0;
    pps->name = 0;
    pps->tinyid = 0;
    pps->flags = 0;
    pps->getter = 0;
    pps->setter = 0;
    psfs->name = 0;
    psfs->call = 0;
    psfs->nargs = 0;
    psfs->flags = 0;

    assert(propertyNumber <= propertyCount);
    assert(pps - ps.get() <= attributeCount);
    assert(pfs - fs.get() <= operationCount);
    assert(heap - stringHeap.get() <= meta.getStringSize());
    assert(psfs - sfs.get() <= staticOperationCount);
    JS_InitClass(cx, global,
                 parentProto,
                 &jsclass,
                 constructor, 0,
                 ps.get(), fs.get(),
                 NULL, sfs.get());

    // Define constants
    jsval val;
    if (0 < meta.getConstantCount() && JS_GetProperty(cx, global, name, &val)) {
        assert(JSVAL_IS_OBJECT(val));
        JSObject* ctor = JSVAL_TO_OBJECT(val);
        Reflect::Property prop(meta);
        while (prop.getType()) {
            Any k;
            switch (prop.getType()) {
            case Reflect::kConstant:
                k = prop.getValue();
                JS_DefineProperty(cx, ctor, prop.getName().c_str(),
                                  convert(cx, k),
                                  0, 0,
                                  JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT);
                break;
            default:
                break;
            }
            prop.next();
        }
    }
}

Any ProxyObject::message_(uint32_t selector, const char* id, int argc, Any* argv)
{
    bool callback = (CALLBACK_ <= argc);
    if (callback)
        argc -= CALLBACK_;
    jsval result;
    JSBool found;
    std::u16string name;
    switch (argc) {
    case GETTER_:
        if (JS_GetProperty(jscontext, jsobject, id, &result))
            return convert(jscontext, result);
        break;
    case SETTER_:
        result = convert(jscontext, argv[0]);
        if (JS_SetProperty(jscontext, jsobject, id, &result))
            return convert(jscontext, result);
        break;
    case HAS_PROPERTY_:
    case HAS_OPERATION_:  // TODO: refine HAS_OPERATION_ path
        if (JS_HasProperty(jscontext, jsobject, id, &found))
            return (found == JS_TRUE);
        break;
    case SPECIAL_GETTER_:
        name = argv[0].toString();
        if (JS_GetUCProperty(jscontext, jsobject, reinterpret_cast<const jschar*>(name.c_str()), name.length(), &result))
            return convert(jscontext, result);
        break;
    case SPECIAL_SETTER_:
    case SPECIAL_CREATOR_:
    case SPECIAL_SETTER_CREATOR_:
        name = argv[0].toString();
        result = convert(jscontext, argv[1]);
        if (JS_GetUCProperty(jscontext, jsobject, reinterpret_cast<const jschar*>(name.c_str()), name.length(), &result))
            return convert(jscontext, result);
        break;
    case SPECIAL_DELETER_:
        name = argv[0].toString();
        if (JS_DeleteUCProperty2(jscontext, jsobject, reinterpret_cast<const jschar*>(name.c_str()), name.length(), &result))
            return result == JS_TRUE;
        break;
    case STRINGIFY_:
        if (JS_CallFunctionName(jscontext, jsobject, "toString", 0, 0, &result) == JS_TRUE)
            return convert(jscontext, result);
        break;
    default: {
        assert(0 <= argc);
        jsval arguments[0 < argc ? argc : 1];
        for (int i = 0; i < argc; ++i)
            arguments[i] = convert(jscontext, argv[i]);
        if (callback &&
            JS_CallFunctionValue(jscontext, JS_GetGlobalObject(jscontext), OBJECT_TO_JSVAL(jsobject), argc, arguments, &result) == JS_TRUE)
            return convert(jscontext, result);
        if (JS_CallFunctionName(jscontext, jsobject, id, argc, arguments, &result) == JS_TRUE)
            return convert(jscontext, result);
        }
        break;
    }
    return Any();
}

Any ECMAScriptContext::Impl::evaluate(const std::u16string& script)
{
    jsval rval;
    const char* filename = "";
    int lineno = 0;
    JS_EvaluateUCScript(getContext(), JS_GetGlobalObject(getContext()),
                        reinterpret_cast<const jschar*>(script.c_str()), script.length(),
                        filename, lineno, &rval);
    return convert(getContext(), rval);
}

Object* ECMAScriptContext::Impl::compileFunction(const std::u16string& body)
{
    static const char* argname = "event";
    JSFunction* fun = JS_CompileUCFunction(getContext(), 0, 0, 1, &argname, reinterpret_cast<const jschar*>(body.c_str()), body.length(), 0, 0);
    if (!fun)
        return 0;
    return convert(getContext(), JS_GetFunctionObject(fun));
}

Any ECMAScriptContext::Impl::callFunction(Object thisObject, Object functionObject, int argc, Any* argv)
{
    assert(0 <= argc);
    if (!thisObject || !functionObject)
        return Any();
    JSObject* funcObj = convert(getContext(), functionObject.self());
    jsval oval = OBJECT_TO_JSVAL(funcObj);
    jsval fval;
    if (!JS_ConvertValue(getContext(), oval, JSTYPE_FUNCTION, &fval))
        return Any();

    JSObject* thisObj = convert(getContext(), thisObject.self());

    jsval arguments[0 < argc ? argc : 1];
    for (int i = 0; i < argc; ++i)
        arguments[i] = convert(getContext(), argv[i]);

    jsval result;
    if (!JS_CallFunctionValue(getContext(), thisObj, fval, argc, arguments, &result))
        return Any();
    return convert(getContext(), result);
}

Object* ECMAScriptContext::Impl::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    JSObject* imp = convert(getContext(), object.self());

    if (prototype) {
        JSObject* p = convert(getContext(), prototype.self());
        JS_SetPrototype(getContext(), p, JS_GetPrototype(getContext(), imp));
        JS_SetPrototype(getContext(), imp, p);
    }

    // TODO: Create an external object.

    jsval val;
    val = OBJECT_TO_JSVAL(convert(getContext(), boundElement.self()));
    JS_SetProperty(getContext(), imp, "boundElement", &val);
    val = OBJECT_TO_JSVAL(convert(getContext(), shadowTree.self()));
    JS_SetProperty(getContext(), imp, "shadowTree", &val);

    return new(std::nothrow) ProxyObject(getContext(), imp);
}
