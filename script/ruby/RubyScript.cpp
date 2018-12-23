//
//  RubyScript.cpp
//  hirender_iOS
//
//  Created by Gen on 16/9/28.
//  Copyright © 2016年 gen. All rights reserved.
//

#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/error.h>
#include <core/Array.h>
#include <core/runtime.h>
#include <core/script/NativeObject.h>
#include <core/String.h>
#include <core/FixType.h>
#include <core/Map.h>
#include "RubyScript.h"

#define SCRIPT ((RubyScript*)getScript())

using namespace gscript;
using namespace std;

mrb_sym sym_native_class;
mrb_sym sym_native_instance;

const char debug_string[] = "begin\n"
"  load('{{path}}')\n"
"rescue Exception => e\n"
"  p e.message\n"
"  print e.backtrace.join(\"\\n\")\n"
"end";

mrb_data_type *ruby_data_type = NULL;

void ruby_instance_free(mrb_state *mrb, void *data) {
    delete (RubyInstance*)data;
}

const mrb_data_type *ruby_type() {
    if (!ruby_data_type) {
        ruby_data_type = (mrb_data_type *)malloc(sizeof(mrb_data_type));
        ruby_data_type->struct_name = "RubyInstance";
        ruby_data_type->dfree = ruby_instance_free;
    }
    return ruby_data_type;
}

mrb_value ruby_native_initialize(mrb_state *mrb, mrb_value obj) {
    mrb_value val = mrb_iv_get(mrb, mrb_obj_value(mrb_class(mrb, obj)), sym_native_class);
    if (!mrb_nil_p(val)) {
        RubyClass *mcls = (RubyClass *)mrb_cptr(val);
        RubyInstance *mins = (RubyInstance *)mcls->newInstance(NULL, 0);
        mins->setScriptInstance(mrb_obj_ptr(obj));
        struct RData* data = mrb_data_object_alloc(mrb, mrb->object_class, mins, ruby_type());
        mrb_iv_set(mrb, obj, sym_native_instance, mrb_obj_value(data));
    }
    return obj;
}


mrb_value ruby_h2r(mrb_state *mrb, const gcore::Variant &v) {
    gcore::Variant::Type type = v.getType();
    switch (type) {
        case gcore::Variant::TypeNull: {
            return mrb_nil_value();
        }
        case gcore::Variant::TypeBool: {
            return mrb_bool_value((bool)v);
        }
        case gcore::Variant::TypeChar:
        case gcore::Variant::TypeShort:
        case gcore::Variant::TypeInt:
        case gcore::Variant::TypeLong:
        case gcore::Variant::TypeLongLong: {
            return mrb_fixnum_value(v);
        }
        case gcore::Variant::TypeFloat:
        case gcore::Variant::TypeDouble: {
            return mrb_float_value(mrb, v);
        }
        case gcore::Variant::TypeStringName: {
            return mrb_symbol_value(mrb_intern_cstr(mrb, v));
        }
        case gcore::Variant::TypeObject:
        case gcore::Variant::TypeReference: {
            const gcore::Class *typeclass = v.getTypeClass();
            RubyScript *script = (RubyScript*)mrb->ud;
            RubyClass *mcls = (RubyClass*)script->find(typeclass->getFullname());
            if (mcls) {
                RubyInstance *mins = (RubyInstance*)mcls->get(v);
                if (mins) {
                    return mrb_obj_value(mins->getScriptInstance());
                }
            }else {
                struct RClass *cls = mrb_define_class(mrb, (string("GClass") + typeclass->getName()).c_str(), mrb->object_class);
                mcls = (RubyClass*)script->reg_class(cls, typeclass->getFullname());
            }
            struct RClass *rcls = mcls->getScriptClass();
            mrb_value obj = mrb_obj_new(mrb, rcls, 0, NULL);
            RubyInstance *mins = (RubyInstance*)mcls->createVariant(v);
            mins->setScriptInstance(mrb_obj_ptr(obj));

            struct RData* data = mrb_data_object_alloc(mrb, mrb->object_class, mins, ruby_type());
            mrb_iv_set(mrb, obj, sym_native_instance, mrb_obj_value(data));
            return obj;
        }
        case gcore::Variant::TypePointer: {
            return mrb_cptr_value(mrb, v);
        }
        case gcore::Variant::TypeMemory: {
            //TODO Something for memory type
        }
    }
    return mrb_nil_value();
}

gcore::Variant ruby_r2h(mrb_state *mrb, mrb_value v) {
    switch (v.tt) {
        case MRB_TT_FALSE:
            return gcore::Variant(false);
        case MRB_TT_FREE:
            return gcore::Variant::null();
        case MRB_TT_TRUE:
            return gcore::Variant(true);
        case MRB_TT_FIXNUM:
            return gcore::Variant(mrb_int(mrb, v));
        case MRB_TT_SYMBOL:
            return gcore::Variant(mrb_sym2name(mrb, mrb_symbol(v)));
        case MRB_TT_FLOAT:
            return gcore::Variant(mrb_float(v));
        case MRB_TT_CPTR:
            return gcore::Variant((void*)mrb_cptr(v));
        case MRB_TT_OBJECT:
        {
            mrb_value val = mrb_attr_get(mrb, v, sym_native_instance);
            if (mrb_nil_p(val)) {
                RubyNativeObject *no = new RubyNativeObject(mrb_obj_ptr(v));
                return gcore::Variant(gcore::Reference(no));
            }else {
                RubyInstance *cinst = (RubyInstance *)DATA_PTR(val);
                if (cinst)
                    return cinst->getTarget();
                return gcore::Variant::null();
            }
        }
        case MRB_TT_CLASS:
        case MRB_TT_MODULE:
        case MRB_TT_ICLASS:
        case MRB_TT_SCLASS:
        {
            mrb_value val = mrb_iv_get(mrb, v, sym_native_class);
            if (mrb_nil_p(val)) {
                RubyNativeObject *no = new RubyNativeObject(mrb_obj_ptr(v));
                return gcore::Variant(gcore::Reference(no));
            }else {
                RubyClass *ccls = (RubyClass *)mrb_cptr(val);
                return gcore::Variant(ccls);
            }
        }
        case MRB_TT_ARRAY:
        {
            mrb_int l = ARY_LEN(mrb_ary_ptr(v));
            vector<gcore::Variant> vs;
            vs.reserve(l);
            for (int i = 0; i < l; ++i) {
                vs.push_back(ruby_r2h(mrb, mrb_ary_ref(mrb, v, i)));
            }
            return gcore::Variant(gcore::RArray(vs));
        }
        case MRB_TT_STRING:
        {
            return gcore::Variant(mrb_str_to_cstr(mrb, v));
        }
        case MRB_TT_HASH:
        {
            mrb_value keys = mrb_hash_keys(mrb, v);
            mrb_int l = ARY_LEN(mrb_ary_ptr(keys));
            gcore::Map map;
            for (mrb_int i = 0; i < l; ++i) {
                mrb_value key = mrb_ary_ref(mrb, keys, i);
                mrb_value value = mrb_hash_get(mrb, v, key);
                if (key.tt != MRB_TT_STRING) {
                    key = mrb_str_to_str(mrb, key);
                }
                map->set(mrb_str_to_cstr(mrb, key), ruby_r2h(mrb, value));
            }
            return map;
        }
            
        default:
            // TODO hash proc
            return gcore::Variant((void*)mrb_ptr(v));
    }
}

mrb_value ruby_native_class(mrb_state *mrb, mrb_value cls) {
    mrb_value name;
    mrb_get_args(mrb, "S", &name);
    RubyScript *script = (RubyScript*)mrb->ud;
    if (script) {
        struct RClass *rcls = mrb_class_ptr(cls);
        script->reg_class(rcls, RSTRING_PTR(name));
    }
    return name;
}

mrb_value ruby_initializer(mrb_state *mrb, mrb_value ins) {
    mrb_value val = mrb_iv_get(mrb, mrb_obj_value(mrb_class(mrb, ins)), sym_native_class);
    if (!mrb_nil_p(val)) {
        RubyClass *mcls = (RubyClass *)mrb_cptr(val);
        RubyInstance *mins = (RubyInstance *)mcls->newInstance(NULL, 0);
        mins->setScriptInstance(mrb_obj_ptr(ins));
        struct RData* data = mrb_data_object_alloc(mrb, mrb->object_class, mins, ruby_type());
        mrb_iv_set(mrb, ins, sym_native_instance, mrb_obj_value(data));
        
        const gcore::Method *init = mcls->getNativeClass()->getInitializer();
        if (init) {
            int count = mrb_get_argc(mrb);
            if (count > 0) {
                gcore::Variant *vs = new gcore::Variant[count];
                const gcore::Variant **ps = (const gcore::Variant **)malloc(count * sizeof(gcore::Variant *));
                for (int i = 0; i < count; ++i) {
                    vs[i] = ruby_r2h(mrb, mrb->c->stack[1 + i]);
                    ps[i] = &vs[i];
                }
                mrb_value ret = ruby_h2r(mrb, init->call(mins->getTarget().get(), ps, count));
                delete [] vs;
                free(ps);
                return ret;
            }else {
                return ruby_h2r(mrb, init->call(mins->getTarget().get(), NULL, 0));
            }
        }
    }
    return mrb_nil_value();
}

mrb_value ruby_call_i(mrb_state *mrb, mrb_value ins) {
    mrb_value val = mrb_iv_get(mrb, ins, sym_native_instance);
    if (mrb_bool(val)) {
        RubyInstance *mins = (RubyInstance*)DATA_PTR(val);
        if (mins && mins->getTarget()) {
            mrb_value m_name = mrb->c->stack[1];
            
            const char* name = mrb_sym2name(mrb, mrb_obj_to_sym(mrb, m_name));
            
            const gcore::Class *ncls = mins->getMiddleClass()->getNativeClass();
            const gcore::Method *method = ncls->getMethod(name);
            if (!method) {
                string std_name(name);
                const gcore::Property *property = ncls->getProperty(std_name.c_str());
                if (property) {
                    method = property->getGetter();
                }
                if (method == NULL && std_name[std_name.size()-1] == '=') {
                    std_name.pop_back();
                    const gcore::Property *property = ncls->getProperty(std_name.c_str());
                    if (property) {
                        method = property->getSetter();
                    }
                }
            }
            
            if (method) {
                mrb_value array = mrb->c->stack[2];
                int len = ARY_LEN(mrb_ary_ptr(array));
                if (len) {
                    gcore::Variant *vs = new gcore::Variant[len];
                    const gcore::Variant **ps = (const gcore::Variant **)malloc(len * sizeof(gcore::Variant *));
                    for (int i = 0; i < len; ++i) {
                        vs[i] = ruby_r2h(mrb, mrb_ary_ref(mrb, array, i));
                        ps[i] = &vs[i];
                    }
                    mrb_value ret = ruby_h2r(mrb, method->call(mins->getTarget().get(), ps, len));
                    delete [] vs;
                    free(ps);
                    return ret;
                }else {
                    return ruby_h2r(mrb, method->call(mins->getTarget().get(), NULL, 0));
                }
            }
        }
    }
    return mrb_nil_value();
}

mrb_value ruby_call_c(mrb_state *mrb, mrb_value cls) {
//    struct RClass *rcls = mrb_class(mrb, cls);
    mrb_value val = mrb_iv_get(mrb, cls, sym_native_class);
    if (mrb_bool(val)) {
        RubyClass *mcls = (RubyClass *)mrb_cptr(val);
        if (mcls) {
            mrb_value m_name = mrb->c->stack[1];
            const char* name = mrb_sym2name(mrb, mrb_obj_to_sym(mrb, m_name));
            mrb_value array = mrb->c->stack[2];
            int len = ARY_LEN(mrb_ary_ptr(array));
            if (len) {
                gcore::Variant *vs = new gcore::Variant[len];
                const gcore::Variant **ps = (const gcore::Variant **)malloc((len) * sizeof(gcore::Variant *));
                for (int i = 0; i < len; ++i) {
                    vs[i] = ruby_r2h(mrb, mrb_ary_ref(mrb, array, i));
                    ps[i] = &vs[i];
                }
                mrb_value ret = ruby_h2r(mrb, mcls->call(name, ps, len));
                delete [] vs;
                free(ps);
                return ret;
            }else {
                return ruby_h2r(mrb, mcls->call(name, NULL, 0));
            }
        }
    }
    return mrb_nil_value();
}

mrb_value ruby_call_class(mrb_state *mrb, mrb_value cls) {
//    struct RClass *rcls = mrb_class(mrb, cls);
    mrb_value val = mrb_iv_get(mrb, cls, sym_native_class);
    if (mrb_bool(val)) {
        
        RubyClass *scls = (RubyClass *)mrb_cptr(val);
        int count = mrb_get_argc(mrb);
        if (count > 0) {
            gcore::Variant *vs = new gcore::Variant[count];
            const gcore::Variant **ps = (const gcore::Variant **)malloc(count * sizeof(gcore::Variant *));
            for (int i = 0; i < count; ++i) {
                vs[i] = ruby_r2h(mrb, mrb->c->stack[1 + i]);
                ps[i] = &vs[i];
            }
            mrb_value ret = ruby_h2r(mrb, scls->call(mrb_sym2name(mrb, mrb_get_mid(mrb)), ps, count));
            delete [] vs;
            free(ps);
            return ret;
        }else {
            return ruby_h2r(mrb, scls->call(mrb_sym2name(mrb, mrb_get_mid(mrb)), NULL, 0));
        }
    }
    return mrb_nil_value();
}
mrb_value ruby_call_instance(mrb_state *mrb, mrb_value ins) {
    mrb_value val = mrb_iv_get(mrb, ins, sym_native_instance);
    if (mrb_bool(val)) {
        RubyInstance *mins = (RubyInstance*)DATA_PTR(val);
        if (mins) {
            int count = mrb_get_argc(mrb);
            if (count == 0) {
                return ruby_h2r(mrb, mins->call(mrb_sym2name(mrb, mrb_get_mid(mrb)), NULL, 0));
            }else {
                gcore::Variant *vs = new gcore::Variant[count];
                const gcore::Variant **ps = (const gcore::Variant **)malloc(count * sizeof(gcore::Variant *));
                for (int i = 0; i < count; ++i) {
                    vs[i] = ruby_r2h(mrb, mrb->c->stack[1 + i]);
                    ps[i] = &vs[i];
                }
                mrb_value ret = ruby_h2r(mrb, mins->call(mrb_sym2name(mrb, mrb_get_mid(mrb)), ps, count));
                delete [] vs;
                free(ps);
                return ret;
            }
        }
    }
    return mrb_nil_value();
}

mrb_value ruby_get_instance(mrb_state *mrb, mrb_value ins) {
    mrb_value val = mrb_iv_get(mrb, ins, sym_native_instance);
    RubyInstance *mins = (RubyInstance*)DATA_PTR(val);
    if (mins) {
        return ruby_h2r(mrb, mins->get(mrb_sym2name(mrb, mrb_get_mid(mrb))));
    }
    return mrb_nil_value();
}

mrb_value ruby_set_instance(mrb_state *mrb, mrb_value ins) {
    mrb_value val = mrb_iv_get(mrb, ins, sym_native_instance);
    RubyInstance *mins = (RubyInstance*)DATA_PTR(val);
    string name(mrb_sym2name(mrb, mrb_get_mid(mrb)));
    if (mrb_bool(val)) {
        name.pop_back();
        if (mins) {
            mins->set(name.c_str(), ruby_r2h(mrb, mrb->c->stack[1]));
        }
    }else {
        LOG(w, "No native instance found.");
    }
    // 添加一个引用.
    mrb_iv_set(mrb, ins, mrb_intern_cstr(mrb, name.c_str()), mrb->c->stack[1]);
    return mrb->c->stack[1];
}

RubyScript::RubyScript() : Script("ruby"), mrb(NULL) {
    reset();
}

RubyScript::~RubyScript() {
    mrb_close(mrb);
}

RubyClass *RubyScript::reg_class(struct RClass *rcls, const gcore::StringName &name) {
    bool create;
    RubyClass *scls = (RubyClass*)find(name, create);
    if (create) {
        scls->script_class = rcls;
        mrb_obj_iv_set(mrb, (struct RObject *)rcls, sym_native_class, mrb_cptr_value(mrb, scls));
        mrb_define_method(mrb, rcls, "native_initialize", ruby_native_initialize, MRB_ARGS_NONE());
        const gcore::Class *hcls = scls->getNativeClass();
        const pointer_map &methods = hcls->getMethods();
        for (auto it = methods.begin(), _e = methods.end(); it != _e; ++it) {
            gcore::StringName name(it->first);
            const gcore::Method *method = (const gcore::Method *)it->second;
            switch (method->getType()) {
                case gcore::Method::Static:
                {
                    mrb_define_class_method(mrb, rcls, name.str(), &ruby_call_class, MRB_ARGS_REQ(method->getParamsCount()));
                }
                    break;
                    
                default:
                {
                    mrb_define_method(mrb, rcls, name.str(), &ruby_call_instance, MRB_ARGS_REQ(method->getParamsCount()));
                }
                    break;
            }
        }
        const pointer_map &properties = hcls->getProperties();
        for (auto it = properties.begin(), _e = properties.end(); it != _e; ++it) {
            const gcore::Property *property = (const gcore::Property *)it->second;
            string name(property->getName().str());
            if (property->getGetter()) {
                mrb_define_method(mrb, rcls, name.c_str(), &ruby_get_instance, MRB_ARGS_NONE());
            }
            if (property->getSetter()) {
                name.push_back('=');
                mrb_define_method(mrb, rcls, name.c_str(), &ruby_set_instance, MRB_ARGS_REQ(1));
            }
        }
        const gcore::Method *init = hcls->getInitializer();
        if (init) {
            mrb_define_method(mrb, rcls, "initialize", &ruby_initializer, MRB_ARGS_REQ(init->getParamsCount()));
        }
    }
    return scls;
}

void RubyScript::_setup(const char *root) const {
    string str("$LOAD_PATH = ['");
    str += root;
    str += "','";
    str += root;
    str += "/gr";
    str += "']";
    mrb_load_string(mrb, str.c_str());
}

void RubyScript::setup(const char *root) {
    context_root = root;
    _setup(root);
}

void RubyScript::reset() {
    if (mrb)
        mrb_close(mrb);
        
    mrb = mrb_open();
    mrb->ud = this;
    sym_native_class = mrb_intern_cstr(mrb, "NATIVE_CLASS");
    sym_native_instance = mrb_intern_cstr(mrb, "native_instance");
    mrb_define_class_method(mrb, mrb->object_class, "native", &ruby_native_class, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, mrb->object_class, "native_call", &ruby_call_i, MRB_ARGS_REQ(2));
    if (!context_root.empty())
        _setup(context_root.c_str());
    reg_class(mrb->object_class, gcore::Object::getClass()->getFullname());
}

void RubyScript::addEnvPath(const char *path) {
    string script = "$LOAD_PATH |= []\n$LOAD_PATH << '";
    script += path;
    script += "'";
    mrb_load_string(mrb, script.c_str());
}

gcore::Variant RubyScript::run(const char *filepath) const {
#ifndef DEBUG
    FILE *file = fopen(filepath, "rb");
    mrb_value val = mrb_load_file(mrb, file);
    fclose(file);
#else
    string script = debug_string;
    char path_str[] = "{{path}}";
    script = script.replace(script.find(path_str), sizeof(path_str) - 1, filepath);
    mrb_value val = mrb_load_string(mrb, script.c_str());
#endif
    return ruby_r2h(mrb, val);
}

gcore::Variant RubyScript::runScript(const char *script) const {
    return ruby_r2h(mrb, mrb_load_string(mrb, script));
}

gcore::ScriptClass *RubyScript::makeClass() const {
    return new RubyClass;
}

gcore::ScriptInstance *RubyScript::newBuff(const string &cls_name, gcore::Object *target, const gcore::Variant **params, int count)const {
    return newBuff(mrb_class_get(mrb, cls_name.c_str()), target, params, count);
}


RubyInstance *RubyScript::newBuff(struct RClass *scls, gcore::Object *target, const gcore::Variant **params, int count) const {
    mrb_value obj;
    if (count) {
        mrb_value *argv = (mrb_value*)malloc(sizeof(mrb_value) * count);
        for (int i = 0; i < count; ++i) {
            argv[i] = ruby_h2r(mrb, *params[i]);
        }
        obj = mrb_obj_new(mrb, scls, count, argv);
        free(argv);
    }else {
        obj = mrb_obj_new(mrb, scls, 0, NULL);
    }
    
    mrb_define_class_method(mrb, scls, "native_class_call", &ruby_call_c, MRB_ARGS_REQ(2));
    
    RubyClass *mcls = (RubyClass*)makeClass();
    mcls->setNativeClass(target->getInstanceClass());
    mcls->setScript(this);
    mcls->script_class = scls;
    mrb_obj_iv_set(mrb, (struct RObject *)scls, sym_native_class, mrb_cptr_value(mrb, mcls));
    
    RubyInstance *mins = (RubyInstance*)mcls->create(target);
    mins->script_class = scls;
    mins->setScriptInstance(mrb_obj_ptr(obj));
    mins->setSingleClass(true);
    
    struct RData* data = mrb_data_object_alloc(mrb, mrb->object_class, mins, ruby_type());
    mrb_iv_set(mrb, obj, sym_native_instance, mrb_obj_value(data));
    
    target->addScript(mins);
    return mins;
}

gcore::ScriptInstance *RubyClass::makeInstance() const {
    return new RubyInstance;
}

gcore::Variant RubyClass::apply(const gcore::StringName &name, const gcore::Variant **params, int count) const {
    mrb_state *mrb = ((RubyScript*)getScript())->getMRB();
    if (mrb->exc) mrb->exc = NULL;
    if (!getScriptClass()) {
        if (mrb_respond_to(mrb, mrb_obj_value(getScriptClass()), mrb_intern_cstr(mrb, name.str()))) {
            mrb_value *vs = (mrb_value *)malloc(count * sizeof(mrb_value));
            for (int i = 0; i < count; ++i) {
                vs[i] = ruby_h2r(mrb, *params[i]);
            }
            mrb_value ret = mrb_funcall_argv(mrb, mrb_obj_value(getScriptClass()), mrb_intern_cstr(mrb, name.str()), count, vs);
            free(vs);
            if (ret.tt == MRB_TT_EXCEPTION) {
                mrb_p(mrb, ret);
                mrb->exc = NULL;
            }
            return ruby_r2h(mrb, ret);
        }else {
            LOG(w, "can not applay %s", name.str());
        }
    }
    return gcore::Variant::null();
}

gcore::Variant RubyInstance::apply(const gcore::StringName &name, const gcore::Variant **params, int count) {
    mrb_state *mrb = ((RubyScript*)getScript())->getMRB();
    if (mrb->exc) mrb->exc = NULL;
    if (mrb_respond_to(mrb, mrb_obj_value(getScriptInstance()), mrb_intern_cstr(mrb, name.str()))) {
        mrb_value *vs = (mrb_value *)malloc(count * sizeof(mrb_value));
        for (int i = 0; i < count; ++i) {
            vs[i] = ruby_h2r(mrb, *params[i]);
        }
        mrb_value ret = mrb_funcall_argv(mrb, mrb_obj_value(getScriptInstance()), mrb_intern_cstr(mrb, name.str()), count, vs);
        free(vs);
        if (ret.tt == MRB_TT_EXCEPTION) {
            mrb_p(mrb, ret);
            mrb->exc = NULL;
        }
        return ruby_r2h(mrb, ret);
    }else {
        LOG(w, "can not applay %s", name.str());
    }
    return gcore::Variant::null();
}

void RubyNativeObject::setNative(void *native) {
    RubyScript *script = (RubyScript*)gcore::Script::get("ruby");
    if (script) {
        if (getNative()) {
            mrb_gc_register(script->getMRB(), mrb_obj_value(getNative()));
        }
        NativeObject::setNative(native);
        if (native) {
            mrb_gc_unregister(script->getMRB(), mrb_obj_value(native));
        }
    }
}

RubyNativeObject::RubyNativeObject(void *native) {
    setNative(native);
}

RubyNativeObject::~RubyNativeObject() {
    if (getNative()) {
        RubyScript *script = (RubyScript*)gcore::Script::get("ruby");
        if (script) {
            mrb_gc_register(script->getMRB(), mrb_obj_value(getNative()));
        }
    }
}

