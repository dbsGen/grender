//
// Created by gen on 16/9/6.
//

#include "Script.h"
#include "ScriptClass.h"
#include "ScriptInstance.h"
#include "../Object.h"
#include "../runtime.h"
#include "../Ref.h"

using namespace gcore;

pointer_map Script::scripts;

ScriptClass* Script::find(const StringName &fullname, bool &create) {
    auto it = classes.find(fullname);
    if (it == classes.end()) {
        const Class *cls = ClassDB::getInstance()->find(fullname);
        if (cls) {
            ScriptClass *scls = makeClass();
            scls->setNativeClass(cls);
            scls->setScript(this);
            classes[fullname] = (void*)scls;
            create = true;
            return scls;
        }
        create = false;
        return NULL;
    }
    create = false;
    return (ScriptClass *)it->second;
}

Script::~Script() {
    scripts.erase(name);
    for (auto it = classes.begin(), _e = classes.end(); it != _e; ++it) {
        delete (ScriptClass*)it->second;
    }
}

ScriptClass *Script::find(const StringName &fullname) const {
    auto it = classes.find(fullname);
    if (it == classes.end()) {
        return NULL;
    }
    return (ScriptClass*)it->second;
}

ScriptClass *Script::find(const Class *cls) const {
    auto it = classes.find(cls->getFullname());
    const Class *parent = cls->getParent();
    while (it == classes.end() && parent) {
        it = classes.find(parent->getFullname());
        parent = parent->getParent();
    }
    if (it == classes.end()) {
        return NULL;
    }
    return (ScriptClass*)it->second;
}

Script::Script(const StringName &name) : name(name) {
    scripts[name] = this;
}

Variant ScriptClass::call(const StringName &name, const Variant **params, int count) const {
    const Method *mtd = cls->getMethod(name);
    if (mtd && mtd->getType() == Method::Static) return mtd->call(NULL, params, count);
    return Variant::null();
}

ScriptInstance* ScriptClass::newInstance(const Variant **params, int count) const {
    Reference *ref = new Reference((Object*)cls->instance());
    ScriptInstance *sin = makeInstance();
    sin->setScript(script);
    sin->setMiddleClass(const_cast<ScriptClass*>(this));
    if (cls->isTypeOf(Object::getClass())) {
        (*ref)->addScript(sin);
    }
    sin->setTarget(ref, true);
    return sin;
}

ScriptInstance *ScriptClass::newInstance() const {
    ScriptInstance *sin = makeInstance();
    sin->setScript(script);
    sin->setMiddleClass(const_cast<ScriptClass*>(this));
    return sin;
}

ScriptInstance *ScriptClass::get(Object *target) const {
    return script->getScriptInstance(target);
}

ScriptInstance *ScriptClass::create(void *target) const {
    ScriptInstance *ins = makeInstance();
    ins->setScript(script);
    ins->setMiddleClass(const_cast<ScriptClass*>(this));
    ins->setTarget(target, false);
    return ins;
}

ScriptInstance* ScriptClass::createVariant(Object *target) const {
    ScriptInstance *ins = makeInstance();
    ins->setScript(script);
    ins->setMiddleClass(const_cast<ScriptClass*>(this));
    if (target->getInstanceClass()->isTypeOf(RefObject::getClass())) {
        ins->setTarget(new Reference(target), true);
        target->addScript(ins);
    }else {
        ins->setTarget(target, false);
        target->addScript(ins);
    }
    return ins;
}

Variant ScriptInstance::call(const StringName &name, const Variant **params, int count) const {
    if (target) {
        const Method *mtd = cls->getNativeClass()->getMethod(name);
        if (mtd) {
            return mtd->call((void*)getTarget(), params, count);
        }
    }
    return Variant::null();
}

Variant ScriptInstance::get(const StringName &name) const {
    if (target) {
        const Property *pro = cls->getNativeClass()->getProperty(name);
        if (pro) {
            return pro->get(getTarget().get());
        }
    }
    return Variant::null();
}

void ScriptInstance::set(const StringName &name, const Variant &val) const {
    if (target) {
        const Property *pro = cls->getNativeClass()->getProperty(name);
        if (pro) {
            pro->set(getTarget().get(), val);
        }
    }
}

ScriptInstance::~ScriptInstance() {
    if (target) {
        if (target_ref) {
            Reference *ref = (Reference*)target;
            (*ref)->removeScript(this);
            delete ref;
        }else {
            ((Object*)target)->removeScript(this);
        }
    }
    if (single_class && cls) {
        delete cls;
    }
}
