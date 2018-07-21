//
// Created by gen on 16/6/21.
//

#include "Class.h"
#include "StringName.h"
#include "Ref.h"
#include "script/ScriptClass.h"
#include "script/ScriptInstance.h"
#include "script/Script.h"
#include "Variant.h"
#include "FixType.h"
#include "String.h"
#include "Reference.h"
#include "Method.h"
#include "MethodImp.h"
#include "Property.h"
#include "Object.h"
#include "Callback.h"
#include "script/Script.h"
#include "Array.h"
#include "Hash.h"
#include "Pointer.h"
#include "Data.h"
#include "Map.h"
#include <core/math/Math.hpp>
#include <sys/time.h>
#include <cstring>

using namespace gcore;

const void *Data::getBuffer() {
    long size = getSize();
    if (!buffer && size) {
        buffer = malloc(size);
        gets(buffer, 0, size);
    }
    return buffer;
}

const char *Data::text() {
    return (const char *)getBuffer();
}


Ref<Data> Data::fromString(const string &str) {
    return new_t(BufferData, (void*)str.data(), str.size(), BufferData::Copy);
}

bool BufferData::gets(void *chs, long start, long len) {
    if (start + len < getSize()) {
        const char *buffer = (const char *)getBuffer();
        memcpy(chs, buffer + start, len);
        return true;
    }else {
        LOG(e, "Range out of bound.");
        return false;
    }
}

void BufferData::initialize(void* buffer, long size, RetainType retain) {
    this->size = size;
    switch (retain) {
        case Retain: {
            this->b_buffer = buffer;
            this->retain = true;
        }
            break;
        case Copy: {
            this->b_buffer = malloc(size);
            memcpy(this->b_buffer, buffer, size);
            this->retain = true;
        }
        case Ref: {
            this->b_buffer = buffer;
            this->retain = false;
        }

        default:
            break;
    }
}

long FileData::getSize() const {
    FileData *that = const_cast<FileData*>(this);
    if (!file) that->loadFile();
    if (file) {
        size_t s = ftell(file);
        fseek(that->file,0,SEEK_END);
        that->size = ftell(file);
        fseek(file, s, SEEK_SET);
    }
    return size;
}

bool FileData::gets(void *chs, long start, long len) {
    if (!file) loadFile();
    if (file) {
        fseek(file,start,SEEK_SET);
        size_t ret = fread(chs, len, 1, file);
        if (ret == 1) {
            return true;
        }
    }
    return false;
}

const void *FileData::getBuffer() {
    if (!file) loadFile();
    if (file) {
        if (!buffer) {
            long size = getSize();
            buffer = malloc(size + 1);
            gets(buffer, 0, size);
            ((char*)buffer)[size] = 0;
        }
        return buffer;
    }
    return NULL;
}

struct HObject::Scripts  {
    list<ScriptInstance*> scripts;
    Scripts() {}
    ~Scripts() {
        for (auto it = scripts.begin(), _e = scripts.end(); it != _e; ++it) {
            (*it)->setTarget(NULL, false);
        }
    }
};

const HClass *const Variant::ref_class(Reference::getClass());
const HClass *const Variant::pointer_class(Pointer::getClass());

ClassDB* ClassDB::instance = NULL;
mutex ClassDB::mtx;

Reference Reference::nullRef;

const Variant Variant::_null;

void HObject::addScript(ScriptInstance *ins) {
    if (!scripts_container) {
        scripts_container = new Scripts;
    }
    scripts_container->scripts.push_back(ins);
}

void HObject::removeScript(ScriptInstance *instance) {
    if (scripts_container) {
        auto it = scripts_container->scripts.begin();
        while (it != scripts_container->scripts.end()) {
            if (*it == instance) {
                it = scripts_container->scripts.erase(it);
            }else ++it;
        }
    }
}

void HObject::clearScripts() {
    if (scripts_container) {
        delete scripts_container;
        scripts_container = nullptr;
    }
}

#ifdef USING_SCRIPT
void HObject::apply(const StringName &name, Variant *result, const Variant **params, int count) {
    if (scripts_container) {
        list<ScriptInstance*> scripts = scripts_container->scripts;
        for (auto it = scripts.begin(), _e = scripts.end(); it != _e; ++it) {
            if (result && !*result)
                *result = (*it)->apply(name, params, count);
            else (*it)->apply(name, params, count);
        }
        return;
    }
    if (result)
        *result = Variant::null();
}
#endif

void HObject::call(const StringName &name, Variant *result, const Variant **params, int count) {
    if (result)
        *result = getInstanceClass()->getMethod(name)->call(this, params, count);
}

bool HObject::copy(const HObject *other) {
    if (other->getInstanceClass()->isTypeOf(getInstanceClass())) {
        _copy(other);
        return true;
    }
    return false;
}

Variant HObject::var()  {
    Pointer p(this);
    return Variant(&p);
}


void HObject::pushOnDestroy(ActionCallback callback, void *data) {
    if (!on_destroy) on_destroy = new ActionManager;
    on_destroy->push(callback, data);
}
void HObject::removeOnDestroy(ActionCallback callback, void *data) {
    if (on_destroy)
        on_destroy->remove(callback, data);
}

HObject::~HObject() {
    if (on_destroy) {
        on_destroy->operator()(this);
        delete on_destroy;
    }
    if (scripts_container) {
        delete scripts_container;
    }
}

template <typename ...A>
struct _typeset {
};
template <typename T,typename C>
struct _typeset<T, C> {
    typedef HObject TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return NULL;
    }
};
template <typename C>
struct _typeset<char, C> {
    typedef Char TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Char::getClass();
    }
};
template <typename C>
struct _typeset<short, C> {
    typedef Short TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Short::getClass();
    }
};
template <typename C>
struct _typeset<int, C> {
    typedef Integer TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Integer::getClass();
    }
};
template <typename C>
struct _typeset<long, C> {
    typedef Long TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Long::getClass();
    }
};
template <typename C>
struct _typeset<long long, C> {
    typedef LongLong TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return LongLong::getClass();
    }
};
template <typename C>
struct _typeset<float, C> {
    typedef Float TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Float::getClass();
    }
};
template <typename C>
struct _typeset<double, C> {
    typedef Double TYPE;
    _FORCE_INLINE_ static const HClass *type() {
        return Double::getClass();
    }
};
template <class T_> struct typeset : _typeset<T_, T_> {};

//template <typename T>
//T trans_target(void *target, const HClass *type) {
//    if (type && type->isTypeOf(HObject::getClass())) {
//        Number *num = ((HObject*)target)->cast_to<Number>();
//        if (num)
//            return num->operator T();
//    }
//    return (T)(long)target;
//}

template <typename T>
T trans_target2(void *target, const HClass *type) {
#define CHECK_TYPE(TYPE) if (type == TYPE::getClass()) { \
    return (T)*((TYPE*)target); \
}
    if (typeset<T>::type() == type) {
        return (T)*((typename typeset<T>::TYPE*) target);
    }
    CHECK_TYPE(Char)
    else CHECK_TYPE(Short)
    else CHECK_TYPE(Integer)
    else CHECK_TYPE(Long)
    else CHECK_TYPE(LongLong)
    else CHECK_TYPE(Float)
    else CHECK_TYPE(Double)
    else CHECK_TYPE(Boolean)

#undef CHECK_TYPE

    return (T)(long)target;
}

bool Variant::operator==(const Variant &other) const {
    const HClass *type = getType();
    if (type == other.getType()) {
        if (type == Char::getClass()) {
            return operator char () == (char)other;
        }else if (type == Short::getClass()) {
            return operator short () == (short)other;
        }else if (type == Integer::getClass()) {
            return operator int() == (int)other;
        }else if (type == Long::getClass()) {
            return operator long() == (long)other;
        }else if (type == LongLong::getClass()) {
            return operator long long() == (long long)other;
        }else if (type == Float::getClass()) {
            return operator float() == (float)other;
        }else if (type == Double::getClass()) {
            return operator float() == (float)other;
        }else if (type == String::getClass()) {
            return strcmp(operator const char *(), (const char *)other) == 0;
        }else if (type == StringName::getClass()) {
            StringName sn = operator StringName();
            return sn == (&other)->operator StringName();
        }else {
            return get() == other.get();
        }
    }else {
        return false;
    }
}

Variant::operator char() const {
    return trans_target2<char>(mem, getType());
}
Variant::operator short() const {
    return trans_target2<short>(mem, getType());
}
Variant::operator int() const {
    return trans_target2<int>(mem, getType());
}
Variant::operator long() const {
    return trans_target2<long>(mem, getType());
}
Variant::operator long long() const {
    return trans_target2<long long>(mem, getType());
}
Variant::operator float() const {
    return trans_target2<float>(mem, getType());
}
Variant::operator double() const {
    return trans_target2<double>(mem, getType());
}

Variant::operator void *() const {
    const HClass *t = type;
    if (t) {
        if (t->isTypeOf(Pointer::getClass())) {
            return (void*)*_get<Pointer>();
        }else if (t->isTypeOf(String::getClass())) {
            return (void*)_get<_String>()->c_str();
        }
    }
    return _get<void>();
}

const Pointer *Variant::pointer() const {
    if (type == Pointer::getClass()) {
        return (const Pointer*)mem;
    }
    return NULL;
}

Variant::operator StringName() const {
    const HClass *type = getType();
    if (type->isTypeOf(StringName::getClass())) {
        return *get<StringName>();
    }else if (type->isTypeOf(_String::getClass())){
        StringName ret(str().c_str());
        return ret;
    }
    return StringName::null();
}

const HClass* Variant::getType() const {
    if (isRef()) {
        return ((Reference*)mem)->getType();
    }else {
        if (type == Pointer::getClass()) {
            const HClass *cls = ((const Pointer*)mem)->getType();
            if (cls) return cls;
        }
        return type;
    }
}

void Variant::release() {
    if (type) {
        if (isRef())
            delete ((Reference*)mem);
        else free(mem);
        mem = NULL;
        type = NULL;
    }
}

void Variant::retain(void *buffer, const HClass *cls) {
    if (cls) {
        if (type) {
            if (type == ref_class && cls->isTypeOf(ref_class)) {
                ((Reference*)mem)->ref((Reference*)buffer);
            }else if (type == ref_class) {
                delete ((Reference*)mem);
                mem = malloc(cls->getSize());
                goto copy_f;
            }else if (cls->isTypeOf(ref_class)) {
                free(mem);
                goto ref_f;
            }else {
                if (type->getSize() == cls->getSize()) {
                    goto copy_f;
                }else  {
                    mem = realloc(mem, cls->getSize());
                    goto copy_f;
                }
            }
        }else {
            if (cls->isTypeOf(ref_class)) {
                goto ref_f;
            }else {
                mem = malloc(cls->getSize());
                goto copy_f;
            }
        }
    }else {
        release();
        return;
    }
copy_f:
    memcpy(mem, buffer, cls->getSize());
    type = cls;
    return;
ref_f:
    mem = new Reference(*(Reference*)buffer);
    type = cls;
    return;
}

Variant::Variant(char n) : Variant() {
    Char v(n);
    const HClass *cls = Char::getClass();
    retain(&v, cls);
}
Variant::Variant(short n) : Variant() {
    Short v(n);
    const HClass *cls = Short::getClass();
    retain(&v, cls);
}
Variant::Variant(int n) : Variant() {
    Integer v(n);
    const HClass *cls = Integer::getClass();
    retain(&v, cls);
}
Variant::Variant(long n) : Variant() {
    Long v(n);
    const HClass *cls = Long::getClass();
    retain(&v, cls);
}
Variant::Variant(long long n) : Variant() {
    LongLong v(n);
    const HClass *cls = LongLong::getClass();
    retain(&v, cls);
}
Variant::Variant(float n) : Variant() {
    Float v(n);
    const HClass *cls = Float::getClass();
    retain(&v, cls);
}
Variant::Variant(double n) : Variant() {
    Double v(n);
    const HClass *cls = Double::getClass();
    retain(&v, cls);
}
Variant::Variant(bool n) : Variant()  {
    Boolean v(n);
    const HClass *cls = Boolean::getClass();
    retain(&v, cls);
}

Variant::Variant(const HObject *obj) : Variant() {
    if (obj) {
        Pointer v(obj);
        retain(&v, Pointer::getClass());
    }
}

Variant::Variant(void *ptr) : Variant() {
    if (ptr) {
        Pointer v(ptr);
        retain(&v, Pointer::getClass());
    }
}

Variant::Variant(const StringName &name) : Variant() {
    StringName cn = name;
    retain(&cn, StringName::getClass());
}

Variant::operator bool() const {
    if (type) {
        if (isRef()) {
            return *(Reference*)mem;
        }else {
            if (type->isTypeOf(Boolean::getClass())) {
                return *get<Boolean>();
            }
            if (type->isTypeOf(Integer::getClass())) {
                return (bool) (int)*get<Integer>();
            }else if (type->isTypeOf(Long::getClass())){
                return (bool) (long)*get<Long>();
            }else if (type->isTypeOf(Float::getClass())) {
                return (bool) (float)*get<Float>();
            }else if (type->isTypeOf(Double::getClass())) {
                return (bool) (double)*get<Double>();
            }else return (bool) mem;
        }
    }
    return false;
}

Variant::operator string() const {
    if (type) {
        return str();
    }else {
        return "";
    }
}

Variant::Variant(const string &str) : Variant() {
    String v(str);
    const HClass *cls = String::getClass();
    retain(&v, cls);
}

string Variant::str() const {
    if (mem) {
        if (isRef()) {
            return ((Reference*)mem)->str();
        }else {
            if (type->isTypeOf(Integer::getClass())) {
                return get<Integer>()->str();
            }else if (type->isTypeOf(Long::getClass())){
                return get<Long>()->str();
            }else if (type->isTypeOf(LongLong::getClass())){
                return get<LongLong>()->str();
            }else if (type->isTypeOf(Float::getClass())) {
                return get<Float>()->str();
            }else if (type->isTypeOf(Double::getClass())) {
                return get<Double>()->str();
            }else if (type->isTypeOf(Boolean::getClass())) {
                return get<Boolean>()->str();
            }
            HObject *obj = get<HObject>();
            if (obj) return obj->str();
        }
    }
    return "(NULL)";
}

bool Variant::isRef(const HClass *cls) {
    if (!cls) return false;
    return cls == Reference::getClass() || cls->isSubclassOf(Reference::getClass());
}

Variant::operator const char *() const {
    if (getType() == _String::getClass()) {
        _String *str = (_String*)**(Reference*)mem;
        return str->c_str();
    }else if (getType() == StringName::getClass()) {
        StringName *name = get<StringName>();
        return name->str();
    }
    return NULL;
}

Variant::operator HObject *() const {
    return get<HObject>();
}

Variant::Variant(const char *str) : Variant() {
    String v(str);
    const HClass *cls = String::getClass();
    retain(&v, cls);
}

Variant ptr(void *pointer) {
    Pointer p(pointer);
    return Variant(&p);
}

bool Reference::isRefObject() {
    if (ptr) {
        return ptr->getInstanceClass()->isTypeOf(RefObject::getClass());
    }
    return false;
}

void Reference::release() {
    if (ptr) {
        if (isRefObject()) {
            if (ptr->cast_to<RefObject>()->release() == 0)
                goto r_del;
        }else if (ref_count && ref_count->release() == 0) goto r_del;
    }
    goto r_end;
    r_del:
    delete ptr;
    delete ref_count;
    ptr = NULL;
    ref_count = NULL;
    r_end:
    return;
}

string Reference::str() const {
    if (ptr) {
        return operator*()->str();
    }else return "";
}

void Reference::retain() {
    if (ptr) {
        if (isRefObject()) {
            ptr->cast_to<RefObject>()->retain();
        }else if (ref_count){
            ref_count->retain();
        }else ref_count = new __ref_count(1);
    }
}


Reference::Reference(const Variant &other) {
    if (other.isRef()) {
        const Reference &r = other.ref();
        ptr = r.ptr;
        ref_count = r.ref_count;
        retain();
    }else {
        ptr = NULL;
        ref_count = NULL;
        const Pointer *p = other.pointer();
        if (p && p->getType() && p->getType()->isTypeOf(RefObject::getClass())) {
            this->operator=((RefObject*)p->operator void *());
        }
    }
}

Reference& Reference::operator=(HObject *p) {
    release();
    ptr = p;
    if (ptr) {
        if (isRefObject()) {
            ptr->cast_to<RefObject>()->retain();
        }else {
            ref_count = ptr ? new __ref_count(1) : NULL;
        }
    }else {
        ref_count = NULL;
    }
    return *this;
}

//void Variant::operator=(const HObject *object) {
//    if (object) {
//        const HClass *cls = object->getInstanceClass();
//        if (isRef(cls)) ref((const Reference *) object, cls);
//        else {
//            setMem((void *) object, cls);
//            ((HObject *)mem)->scripts_container->scripts_ref_count++;
//        }
//    }else {
//        if (isRef()) reference = NULL;
//        else if (mem) {
//            free(mem);
//            mem = NULL;
//        }
//        type = NULL;
//    }
//}

namespace gcore {
    const StringName HMethodInitialer("initialize");
}

const HMethod *HClass::addMethod(const HMethod *method) {
    if (method->getName() == HMethodInitialer) {
        setInitializer(method);
    }else
        methods[method->getName()] = (void*)method;
    return method;
}

const Property *HClass::addProperty(const Property *property) {
    properties[property->getName()] = (void*)property;
    return property;
}

Property::Property(const HClass *clazz, const char *name, const HMethod *getter,
                   const HMethod *setter) {
    if ((getter && getter->getType() == HMethod::Static) || (setter && setter->getType() == HMethod::Static)) {
        LOG(e, "Setter and Getter must be a member method.");
    }else {
        this->getter = getter;
        this->setter = setter;
    }
    this->clazz = clazz;
    this->name = new StringName(name);
}

Property::Property(const HClass *clazz, const char *name, const HMethod *getter,
                   const HMethod *setter, const variant_map &labels) : Property(clazz, name, getter, setter) {
    this->labels = new variant_map;
    this->labels->operator=(labels);
}

Property::~Property() {
    if (labels) delete labels;
    delete name;
}

BASE_CLASS_IMPLEMENT(Reference)
BASE_CLASS_IMPLEMENT_V(HMethod)

Variant HMethod::call(void *obj, const Variant **params, int count) const {
    int pc = getParamsCount();
    int dc = getDefaultCount();
    if (count < pc) {
        if (count + dc < pc) {
            LOG(e, "(%s) The variant count mush be greater than %d.", getName().str(), pc - dc);
            return Variant();
        }else {
            const Variant **vs = (const Variant **)malloc(sizeof(const Variant *) * pc);
            int i = 0;
            for (; i < count; ++i) {
                vs[i] = params[i];
            }
            for (; i < pc; i++) {
                vs[i] = &default_values[dc - (i - count)];
            }
            Variant ret = _call(obj, vs);
            free(vs);
            return ret;
        }
    }else {
        return _call(obj, params);
    }
}

HMethod::HMethod(const char *name) : return_type(NULL), params_types(NULL), labels(NULL), default_values(NULL) {
    *(uint32_t*)(&params) = 0;
    this->name = name;
}

HMethod::~HMethod() {
    if (params_types) free(params_types);
    if (labels) free(labels);
    if (default_values) delete []default_values;
    delete name;
}

BASE_CLASS_IMPLEMENT(Variant)


BASE_CLASS_IMPLEMENT(Property)

const pointer_map &HClass::getMethods() const {
    return methods;
}

const pointer_map &HClass::getProperties() const {
    return properties;
}

void HClass::setLabels(const variant_map &labels) {
    this->labels = labels;
}

const HClass *ClassDB::find(const StringName &fullname) {
    const HClass *result = find_loaded(fullname);
    if (!result) {
        auto it = class_loaders.find(fullname);
        result = it == class_loaders.end() ? NULL : (*(ClassLoader*)it->second)();
    }
    return result;
}

Variant HClass::call(const StringName &name, object_type obj, const Variant **params, int count) const {
    const HMethod *mtd = getMethod(name);
    if (mtd) return mtd->call(obj, params, count);
    return Variant::null();
}

const HClass *ClassDB::find_loaded(const StringName &fullname) {
    auto it = classes_index.find(fullname);
    return (const HClass *) (it == classes_index.end() ? NULL : it->second);
}

const HClass *Reference::getType() const {
    return ptr? ptr->getInstanceClass() : NULL;
}

HClass::~HClass() {
    delete fullname;
}

HClass::HClass(const char *ns, const char *name) : HClass() {
    this->ns = ns;
    this->name = name;
    fullname = new StringName(ns ? (string(ns) + "::" + name).c_str() : name);
}

void Reference::call(const StringName &name, Variant *result, const Variant **params, int count) {
    if (ptr) {
        ptr->call(name, result, params, count);
    }
}

void Reference::ref(const Reference *other) {
    if (ptr != other->ptr)  {
        release();
        ptr = other->ptr;
        ref_count = other->ref_count;
        retain();
    }else {
        ptr = other->ptr;
        ref_count = other->ref_count;
    };
}

Variant Callback::invoke(const Array &params) {
    const Variant *vs[1];
    Variant v1((Variant)params);
    vs[0] = &v1;
    Variant ret;
    apply("_invoke", &ret, vs, 1);
    return ret;
}


ScriptInstance *Script::getScriptInstance(const HObject *target) const {
    if (target->scripts_container) {
        for (auto it = target->scripts_container->scripts.begin(),
             _e = target->scripts_container->scripts.end(); it != _e; ++it) {
            if ((*it)->getScript() == this) {
                return *it;
            }
        }
    }
    return NULL;
}

string _Array::str() const {
    stringstream ss;
    ss << "[";
    for (auto it = variants.begin(), _e = variants.end(); it != _e; ++it) {
        ss << (*it).str();
    }
    ss << "]";
    return ss.str();
}

bool _Array::triger(ArrayEvent event, long idx, const Variant &v) {
    RefCallback *lis = (RefCallback*)listener;
    return (*lis)->invoke(Array{event, idx, v});
}

bool _Array::replace(long idx, const Variant &v1, const Variant &v2) {
    RefCallback *lis = (RefCallback*)listener;
    return (*lis)->invoke(Array{E(Replace), idx, v1, v2});
}

void _Array::push_back(const Variant &var) {
    variants.push_back(var);
    if (listener) triger(E(Insert), variants.size()-1, var);
}


void _Array::insert(long n, const Variant &var) {
    if (n <= variants.size()) {
        auto it = variants.begin() + n;
        if (listener) triger(E(Insert), n, *it);
        variants.insert(it, var);
    }
}

void _Array::erase(long n) {
    if (n < variants.size()) {
        auto it = variants.begin() + n;
        if (listener) triger(E(Remove), n, *it);
        variants.erase(it);
    }
}

void _Array::set(long idx, const Variant &var) {
    if (idx < variants.size()) {
        Variant old = variants[idx];
        variants[idx] = var;
        replace(idx, var, old);
    }
}

void _Array::remove(const Variant &var) {
    long count = 0;
    for (auto it = variants.begin(), _e = variants.end(); it != _e; ++it) {
        if (var == *it) {
            if (listener) triger(E(Remove), count, var);
            it = variants.erase(it);
        }else {
            ++it;
        }
        ++count;
    }
}

long _Array::find(const Variant &var) {
    long i = variants.size() - 1;
    for (; i >= 0; --i) {
        if (variants[i] == var) {
            break;
        }
    }
    return i;
}

Variant _Array::pop_back() {
    if (variants.size()) {
        Variant ret = variants.back();
        if (listener) triger(E(Remove), variants.size() - 1, ret);
        variants.pop_back();
        return ret;
    }else {
        return Variant::null();
    }
}

void _Array::setListener(const RefCallback &callback) {
    RefCallback *lis;
    if (!listener) {
        lis = new RefCallback;
        listener = lis;
    }else {
        lis = (RefCallback*)listener;
    }
    lis->operator=(callback);
}

_Array::~_Array() {
    if (listener) {
        delete (RefCallback*)listener;
    }
}

static const unsigned int seed = 131;
static pointer_map *stringDB;

_FORCE_INLINE_ uint32_t bkdrHash(const char *str) {
    unsigned int hash = 0;
    while (*str) hash = hash * seed + (*str++);
    return (hash & 0x7FFFFFFF);
}

const char *make_cs(const char *cs) {
    if (cs) {
        int size = strlen(cs) + 1;
        char *chs = (char*)malloc(size * sizeof(char));
        strcpy(chs, cs);
        chs[size - 1] = 0;
        return chs;
    }
    return NULL;
}

int getName(const char *str, uint32_t h, const char **res = NULL) {
    if (!stringDB) stringDB = new pointer_map;
    auto f = stringDB->find((void*)h);
    if (f == stringDB->end()) {
        pointer_vector *v = new pointer_vector;
        const char *cname = make_cs(str);
        v->push_back((void*)cname);
        stringDB->operator[]((void*)h) = v;
        if (res) *res = cname;
        return 0;
    } else {
        pointer_vector *v = (pointer_vector *) f->second;
        int count = 0;
        for (auto it = v->begin(), _e = v->end(); it != _e; ++it) {
            const char *cname = (const char *)*it;
            if (strcmp(cname, str) == 0) {
                if (res) *res = cname;
                return count;
            }
            ++count;
        }
        const char *cname = make_cs(str);
        v->push_back((void*)cname);
        if (res) *res = cname;
        return count;
    }
}

//void *hicore::h(const char *chs) {
//    uint32_t h = bkdrHash(chs);
//    return make_key(h, getIndex(chs, h));
//}
//
//void *hicore::h(const string &str) {
//    const char *chs = str.c_str();
//    uint32_t h = bkdrHash(chs);
//    return make_key(h, getIndex(chs, h));
//}

void *gcore::h(const char *chs) {
    uint32_t h = bkdrHash(chs);
    const char *ret;
    getName(chs, h, &ret);
    return (void*)ret;
}

//const char *hicore::s(uint64_t key) {
//    if (stringDB && key > b32_mask) {
//        uint32_t h = key >> 32 & b32_mask;
//        auto f = stringDB->find(h);
//        if (f != stringDB->end()) {
//            pointer_vector *v = (pointer_vector *) f->second;
//            uint32_t i;
//            if (v && (i = key & b32_mask) < v->size()) {
//                return (const char*)v->operator[](i);
//            }
//        }
//    }
//    return NULL;
//}

StringName StringName::_null;

BASE_CLASS_IMPLEMENT(StringName)

BASE_CLASS_IMPLEMENT(Char)
BASE_CLASS_IMPLEMENT(Short)
BASE_CLASS_IMPLEMENT(Integer)
BASE_CLASS_IMPLEMENT(Long)
BASE_CLASS_IMPLEMENT(LongLong)
BASE_CLASS_IMPLEMENT(Float)
BASE_CLASS_IMPLEMENT(Double)
BASE_CLASS_IMPLEMENT(Boolean)


BASE_CLASS_IMPLEMENT(Pointer)
