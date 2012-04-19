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

#include "esv8api.h"

#include <assert.h>
#include <iostream>

#include "ScriptV8.h"

std::map<std::string, v8::Persistent<v8::FunctionTemplate>> NativeClass::interfaceMap;
std::map<ObjectImp*, v8::Persistent<v8::Object>> NativeClass::wrapperMap;

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

Object* convertObject(v8::Handle<v8::Object> obj)
{
    // TODO: We would need to check more strictly whether obj is an ObjectImp.
    if (obj->InternalFieldCount() == 1) {
        auto wrap = v8::Handle<v8::External>::Cast(obj->GetInternalField(0));
        return static_cast<ObjectImp*>(wrap->Value());
    }
    // obj is a JavaScript object. Create a proxy for obj.
    return new(std::nothrow) ProxyObject(obj);
}

Any convert(v8::Handle<v8::Value> v)
{
    if (v.IsEmpty() || v->IsUndefined())
        return Any();
    if (v->IsNull())
        return Any(static_cast<Object*>(0));
    if (v->IsInt32())
        return v->Int32Value();
    if (v->IsUint32())
        return v->Uint32Value();
    if (v->IsNumber())
        return v->NumberValue();
    if (v->IsBoolean())
        return v->BooleanValue();
    if (v->IsString()) {
        v8::String::Value value(v);
        return std::u16string(*value ? reinterpret_cast<const char16_t*>(*value) : u"");
    }
#if 0
    if (v->IsDate()) {
        return; // TODO: Convert to long long
    }
#endif
    if (v->IsObject())
        return convertObject(v->ToObject());
    // TODO: Check IsFunction, IsArray, IsExternal, IsRegExp
    return Any();
}

Any convert(v8::Handle<v8::String> property)
{
    v8::String::Value value(property);
    return std::u16string(*value ? reinterpret_cast<const char16_t*>(*value) : u"");
}

v8::Handle<v8::Object> convertObject(Object* obj)
{
    if (obj) {
        if (ProxyObject* proxy = dynamic_cast<ProxyObject*>(obj->self()))
            return proxy->getJSObject();
        if (ObjectImp* imp = dynamic_cast<ObjectImp*>(obj->self()))
            return static_cast<NativeClass*>(imp->getStaticPrivate())->createJSObject(imp);
    }
    return v8::Handle<v8::Object>();
}

v8::Handle<v8::Value> convert(Any& v)
{
    switch (v.getType()) {
    case Any::Undefined:
        return v8::Undefined();
    case Any::Null:
        return v8::Null();
    case Any::Bool:
        return v8::Boolean::New(static_cast<bool>(v));
    case Any::Int32:
        return v8::Integer::New(static_cast<int32_t>(v));
    case Any::Uint32:
        return v8::Integer::NewFromUnsigned(static_cast<uint32_t>(v));
    case Any::Int64:
    case Any::Uint64:
    case Any::Float32:
    case Any::Float64:
        return v8::Number::New(static_cast<double>(v));
    default:
        break;
    }
    if (v.isString()) {
        std::u16string s = v.toString();
        return v8::String::New(reinterpret_cast<const uint16_t*>(s.c_str()), s.length());
    }
    if (v.isObject())
        return convertObject(v.toObject());
    return v8::Undefined();
}

v8::Handle<v8::Value> caller(const v8::Arguments& args)
{
    assert(!args.IsConstructCall());
    int argc = args.Length();
    v8::Local<v8::Object> self = args.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Any arguments[(0 < argc) ? argc : 1];
    for (int i = 0; i < argc; ++i)
        arguments[i] = convert(args[i]);
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any result = imp->message_(0, 0, argc, arguments);
    return convert(result);
}

v8::Handle<v8::Value> operation(const v8::Arguments& args)
{
    int argc = args.Length();
    v8::Local<v8::Object> self = args.This();
    v8::Local<v8::Value> data = args.Data();
    uint32_t hash = data->Uint32Value();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any arguments[(0 < argc) ? argc : 1];
    for (int i = 0; i < argc; ++i)
        arguments[i] = convert(args[i]);
    Any result = imp->message_(hash, 0, argc, arguments);
    return convert(result);
}

v8::Handle<v8::Integer> namedPropertyQuery(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::String::Value value(property);
    std::u16string name(*value ? reinterpret_cast<const char16_t*>(*value) : u"");
    uint32_t hash = uc_one_at_a_time(name.c_str(), name.length());
    if (!hash)
        return v8::Handle<v8::Integer>();

    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any result = imp->message_(hash, 0, Object::HAS_OPERATION_, 0);
    if (!static_cast<bool>(result))
        return v8::Handle<v8::Integer>();
    return v8::Integer::New(v8::None);  // TODO:
}

v8::Handle<v8::Value> namedPropertyGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::String::Value value(property);
    std::u16string name(*value ? reinterpret_cast<const char16_t*>(*value) : u"");
    uint32_t hash = uc_one_at_a_time(name.c_str(), name.length());
    if (!hash)
        return v8::Handle<v8::Value>();

    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    // TODO: Check [NamedPropertiesObject]
    v8::Local<v8::Value> realProperty = self->GetRealNamedProperty(property);
    if (!realProperty.IsEmpty())
        return realProperty;
    Any argument(convert(property));
    Any result = imp->message_(0, 0, Object::SPECIAL_GETTER_, &argument);
    return convert(result);
}

v8::Handle<v8::Value> indexedPropertyGetter(uint32_t index, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any argument(index);
    Any result = imp->message_(0, 0, Object::SPECIAL_GETTER_, &argument);
    return convert(result);
}

v8::Handle<v8::Boolean> namedPropertyDeleter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any argument(convert(property));
    imp->message_(0, 0, Object::SPECIAL_DELETER_, &argument);
    return v8::True();
}

v8::Handle<v8::Boolean> indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any argument(index);
    imp->message_(0, 0, Object::SPECIAL_DELETER_, &argument);
    return v8::True();
}

v8::Handle<v8::Value> namedPropertySetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any arguments[2];
    arguments[0] = convert(property);
    arguments[1] = convert(value);
    imp->message_(0, 0, Object::SPECIAL_SETTER_, arguments);
    return value;
}

v8::Handle<v8::Value> indexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any arguments[2];
    arguments[0] = index;
    arguments[1] = convert(value);
    imp->message_(0, 0, Object::SPECIAL_DELETER_, arguments);
    return value;
}

}  // namespace

v8::Handle<v8::Value> NativeClass::staticOperation(const v8::Arguments& args)
{
    int argc = args.Length();

    // TODO: It is better if we can get 'hash' through args.Data() and get
    //       'getConstructor' by other means.

    auto data = v8::Handle<v8::External>::Cast(args.Data());
    Object (*getConstructor)() = reinterpret_cast<Object (*)()>(data->Value());

    auto function = v8::Local<v8::Function>::Cast(args.Callee());
    if (function.IsEmpty())
        return v8::Null();
    v8::Handle<v8::Value> val = function->GetName();
    if (!val->IsString())
        return v8::Null();
    v8::String::Value value(val);
    if (!*value)
        return v8::Null();
    std::u16string name(reinterpret_cast<const char16_t*>(*value));
    uint32_t hash = uc_one_at_a_time(name.c_str(), name.length());

    Any arguments[(0 < argc) ? argc : 1];
    for (int i = 0; i < argc; ++i)
        arguments[i] = convert(args[i]);
    if (getConstructor) {
        Any result = getConstructor().message_(hash, 0, argc, arguments);
        return convert(result);
    }
    return v8::Null();
}

void NativeClass::finalize(v8::Persistent<v8::Value> object, void* parameter)
{
    assert(parameter);
    ObjectImp* imp = static_cast<ObjectImp*>(parameter);
    wrapperMap.erase(imp);
    if (imp && imp->getPrivate()) {
        imp->setPrivate(0);
        imp->release_();
    }
    object.Dispose();
    object.Clear();
}

v8::Handle<v8::Value> NativeClass::constructor(const v8::Arguments& args)
{
    v8::HandleScope handleScope;

    assert(args.IsConstructCall());
    int argc = args.Length();
    if (argc == 1 && args[0]->IsExternal())
        return args.This();
    auto data = v8::Handle<v8::External>::Cast(args.Data());
    Object (*getConstructor)() = reinterpret_cast<Object (*)()>(data->Value());
    if (getConstructor) {
        Any arguments[argc];
        for (int i = 0; i < argc; ++i)
            arguments[i] = convert(args[i]);
        Any result = getConstructor().message_(0, "", argc, arguments).toObject();
        if (auto imp = dynamic_cast<ObjectImp*>(result.toObject()->self())) {
            v8::Persistent<v8::Object> obj = v8::Persistent<v8::Object>::New(args.This());
            obj.MakeWeak(imp, finalize);
            wrapperMap[imp] = obj;
            imp->retain_();
            imp->setPrivate(static_cast<void*>(*args.This()));
            args.This()->SetInternalField(0, v8::External::New(imp));
            return args.This();
        }
    }
    return v8::Null();
}

namespace {

v8::Handle<v8::Value> getter(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::Value> data = info.Data();
    uint32_t hash = data->Uint32Value();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any result = imp->message_(hash, 0, Object::GETTER_, 0);
    return convert(result);
}

void setter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::Value> data = info.Data();
    uint32_t hash = data->Uint32Value();
    auto wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    ObjectImp* imp = static_cast<ObjectImp*>(wrap->Value());
    Any argument = convert(value);
    imp->message_(hash, 0, Object::SETTER_, &argument);
}

Any call(v8::Handle<v8::Object> self, v8::Handle<v8::Function> func, int argc, Any* argv)
{
    assert(0 <= argc);
    v8::Handle<v8::Value> arguments[0 < argc ? argc : 1];
    for (int i = 0; i < argc; ++i)
        arguments[i] = convert(argv[i]);
    return convert(func->Call(self, argc, arguments));
}

}   // namespace

v8::Handle<v8::Object> NativeClass::createJSObject(ObjectImp* imp)
{
    v8::HandleScope handleScope;

    assert(imp);
    if (0 < wrapperMap.count(imp))
        return wrapperMap[imp];

    v8::Handle<v8::Function> ctor = classTemplate->GetFunction();
    v8::Handle<v8::Value> external = v8::External::New(imp);
    v8::Persistent<v8::Object> obj = v8::Persistent<v8::Object>::New(ctor->NewInstance(1, &external));
    obj->SetInternalField(0, v8::External::New(imp));
    obj.MakeWeak(imp, finalize);
    wrapperMap[imp] = obj;
    imp->retain_();
    imp->setPrivate(static_cast<void*>(*obj));
    return handleScope.Close(obj);
}

NativeClass::~NativeClass()
{
    classTemplate.Dispose();
    classTemplate.Clear();
}

NativeClass::NativeClass(v8::Handle<v8::ObjectTemplate> global, const char* metadata, Object (*getConstructor)()) :
    metaData(metadata),
    getConstructor(getConstructor)
{
    v8::HandleScope handleScope;

    Reflect::Interface meta(metadata);

    std::string identifier = Reflect::getIdentifier(meta.getName());
    if (interfaceMap.find(identifier) != interfaceMap.end()) {
        std::cerr << "error: Interface " << identifier << " has already been registered.\n";
        return;
    }

    classTemplate = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(meta.hasConstructor() ? constructor : 0, v8::External::New(reinterpret_cast<void*>(getConstructor))));
    classTemplate->SetClassName(v8::String::New(identifier.c_str(), identifier.length()));
    global->Set(v8::String::New(identifier.c_str(), identifier.length()), classTemplate);
    interfaceMap[identifier] = classTemplate;

    // Inheritance
    std::string parentName = Reflect::getIdentifier(meta.getExtends());
    if (!parentName.empty()) {
        auto it = interfaceMap.find(parentName);
        if (it != interfaceMap.end())
            classTemplate->Inherit(it->second);
        else {
            std::cerr << "error: Interface " << parentName << " has not been initialized.\n";
            return;
        }
    }

    v8::Handle<v8::ObjectTemplate> prototypeTemplate = classTemplate->PrototypeTemplate();
    v8::Handle<v8::ObjectTemplate> instanceTemplate = classTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);

    Reflect::Property prop(meta);
    while (prop.getType()) {
        uint32_t hash = one_at_a_time(prop.getName().c_str(), prop.getName().length());

        v8::NamedPropertyGetter namedGetter = 0;
        v8::NamedPropertySetter namedSetter = 0;
        v8::NamedPropertyDeleter namedDeleter = 0;

        v8::IndexedPropertyGetter indexedGetter = 0;
        v8::IndexedPropertySetter indexedSetter = 0;
        v8::IndexedPropertyDeleter indexedDeleter = 0;

        switch (prop.getType()) {
        case Reflect::kOperation:
            if (!prop.isOmittable() && 0 < prop.getName().length()) {
                if (prop.isStatic())
                    classTemplate->Set(v8::String::New(prop.getName().c_str()), v8::FunctionTemplate::New(staticOperation, v8::External::New(reinterpret_cast<void*>(getConstructor))));
                else
                    prototypeTemplate->Set(v8::String::New(prop.getName().c_str()), v8::FunctionTemplate::New(operation, v8::Uint32::New(hash)));
            }

            if (prop.isGetter())
                namedGetter = namedPropertyGetter;
            if (prop.isSetter() || prop.isCreator())
                namedSetter = namedPropertySetter;
            if (prop.isDeleter())
                namedDeleter = namedPropertyDeleter;
            if (namedGetter)
                prototypeTemplate->SetNamedPropertyHandler(namedGetter, namedSetter, 0 /* query */, namedDeleter, 0 /* enumerator */, v8::Uint32::New(hash));

            if (prop.isGetter())
                indexedGetter = indexedPropertyGetter;
            if (prop.isSetter() || prop.isCreator())
                indexedSetter = indexedPropertySetter;
            if (prop.isDeleter())
                indexedDeleter = indexedPropertyDeleter;
            if (indexedGetter)
                prototypeTemplate->SetIndexedPropertyHandler(indexedGetter, indexedSetter, 0 /* query */, indexedDeleter, 0 /* enumerator */, v8::Uint32::New(hash));

            if (prop.isCaller())
                instanceTemplate->SetCallAsFunctionHandler(caller, v8::Uint32::New(hash));

            if (prop.isStringifier() && prop.getName() != "toString")
                prototypeTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(operation, v8::Uint32::New(hash)));
            break;
        case Reflect::kAttribute:
            // TODO: check settings and attribute
            prototypeTemplate->SetAccessor(v8::String::New(prop.getName().c_str()), getter, prop.isReadOnly() ? 0 : setter, v8::Uint32::New(hash));

            if (prop.isStringifier())
                prototypeTemplate->Set(v8::String::New("toString"), v8::FunctionTemplate::New(operation, v8::Uint32::New(hash)));
            break;
        case Reflect::kConstant: {
            // Define constants
            Any k = prop.getValue();
            classTemplate->Set(v8::String::New(prop.getName().c_str()), convert(k), v8::ReadOnly);
            break;
        }
        default:
            break;
        }
        prop.next();
    }
}

Any ProxyObject::message_(uint32_t selector, const char* id, int argc, Any* argv)
{
    bool callback = (CALLBACK_ <= argc);
    if (callback)
        argc -= CALLBACK_;
    v8::Handle<v8::Value> result;
    switch (argc) {
    case GETTER_:
        return convert(jsobject->Get(v8::String::New(id)));
        break;
    case SETTER_:
        result = convert(argv[0]);
        return jsobject->Set(v8::String::New(id), result);
    case HAS_PROPERTY_:
    case HAS_OPERATION_:  // TODO: refine HAS_OPERATION_ path
        return jsobject->Has(v8::String::New(id));
    case SPECIAL_GETTER_:
        if (argv[0].getType() == Any::Uint32) {
            uint32_t index = static_cast<uint32_t>(argv[0]);
            return convert(jsobject->Get(index));
        } else {
            std::u16string name = argv[0].toString();
            return convert(jsobject->Get(v8::String::New(reinterpret_cast<const uint16_t*>(name.c_str()), name.length())));
        }
        break;
    case SPECIAL_SETTER_:
    case SPECIAL_CREATOR_:
    case SPECIAL_SETTER_CREATOR_:
        result = convert(argv[1]);
        if (argv[0].getType() == Any::Uint32) {
            uint32_t index = static_cast<uint32_t>(argv[0]);
            return jsobject->Set(index, result);
        } else {
            std::u16string name = argv[0].toString();
            return jsobject->Set(v8::String::New(reinterpret_cast<const uint16_t*>(name.c_str()), name.length()), result);
        }
        break;
    case SPECIAL_DELETER_:
        if (argv[0].getType() == Any::Uint32) {
            uint32_t index = static_cast<uint32_t>(argv[0]);
            return jsobject->Delete(index);
        } else {
            std::u16string name = argv[0].toString();
            return jsobject->Delete(v8::String::New(reinterpret_cast<const uint16_t*>(name.c_str()), name.length()));
        }
        break;
    case STRINGIFY_: {
        auto functionObject = v8::Local<v8::Function>::Cast(jsobject->Get(v8::String::New("toString")));
        if (!functionObject.IsEmpty()) {
            Any result = call(jsobject, functionObject, 0, 0);
            return convert(result);
        }
        }
        break;
    default: {
        assert(0 <= argc);
        if (callback) {
            v8::Handle<v8::Value> arguments[0 < argc ? argc : 1];
            for (int i = 0; i < argc; ++i)
                arguments[i] = convert(argv[i]);
            return convert(jsobject->CallAsFunction(jsobject, argc, arguments));
        }
        auto functionObject = v8::Local<v8::Function>::Cast(jsobject->Get(v8::String::New(id)));
        if (!functionObject.IsEmpty()) {
            Any result = call(jsobject, functionObject, argc, argv);
            return convert(result);
        }
        }
        break;
    }
    return Any();
}

Any ECMAScriptContext::callFunction(Object thisObject, Object functionObject, int argc, Any* argv)
{
    assert(0 <= argc);
    if (!thisObject || !functionObject)
        return Any();
    v8::Handle<v8::Object> self = convertObject(thisObject.self());
    v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(convertObject(functionObject.self()));
    return call(self, func, argc, argv);
}

Any ECMAScriptContext::evaluate(const std::u16string& source)
{
    v8::HandleScope handleScope;

    v8::Handle<v8::String> string = v8::String::New(reinterpret_cast<const uint16_t*>(source.c_str()), source.length());
    v8::Handle<v8::Script> script = v8::Script::Compile(string);
    return convert(script->Run());
}

Object* ECMAScriptContext::Impl::compileFunction(const std::u16string& body)
{
    v8::HandleScope handleScope;

    v8::Handle<v8::Value> source = v8::String::New(reinterpret_cast<const uint16_t*>(body.c_str()), body.length());
    v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(context->Global()->Get(v8::String::New("Function")));
    return convertObject(function->NewInstance(1, &source));
}

Object* ECMAScriptContext::Impl::xblCreateImplementation(Object object, Object prototype, Object boundElement, Object shadowTree)
{
    v8::Handle<v8::Object> imp = convertObject(object.self());
    if (prototype) {
        v8::Handle<v8::Object> p = convertObject(prototype.self());
        p->SetPrototype(imp->GetPrototype());
        imp->SetPrototype(p);
    }
    // TODO: Create an external object.
    imp->Set(v8::String::New("boundElement"), convertObject(boundElement.self()));
    imp->Set(v8::String::New("shadowTree"), convertObject(shadowTree.self()));
    return new(std::nothrow) ProxyObject(imp);
}
