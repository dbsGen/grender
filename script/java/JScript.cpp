//
// Created by gen on 16/7/5.
//

#ifdef __ANDROID__

#include <core/script/ScriptInstance.h>
#include <core/script/NativeObject.h>
#include <core/FixType.h>
#include <android/asset_manager_jni.h>
#include "JScript.h"
#include "JInstance.h"
#include "JClass.h"
#include <script/Utils.h>
#include <core/math/Type.h>
#include <core/String.h>
#include <core/Array.h>
#include <core/Map.h>
#include <thread>

#define HELPER_CLASS "com/gr/Helper"

using namespace gscript;

JScript *JScript::_instance = NULL;
jclass JScript::helper_class = NULL;
jmethodID JScript::check_method = NULL;
jmethodID JScript::new_class_method = NULL;
JavaVM *JScript::vm = NULL;
pointer_map JScript::envs;

JNIEnvWrap::JNIEnvWrap(JNIEnv *env, jclass helper_class, bool new_thread) : env(env),
                                                                            helper_class(helper_class),
                                                                            new_thread(new_thread),
                                                                            to_int(NULL),
                                                                            to_long(NULL),
                                                                            to_float(NULL),
                                                                            to_double(NULL),
                                                                            to_native(NULL),
                                                                            from_int(NULL),
                                                                            from_long(NULL),
                                                                            from_float(NULL),
                                                                            from_double(NULL),
                                                                            from_native(NULL),
                                                                            new_dic(NULL),
                                                                            set_dic_value(NULL),
                                                                            dic_keys(NULL),
                                                                            get_dic_value(NULL),
                                                                            check_type(NULL)
{ }

JNIEnvWrap::~JNIEnvWrap() {
    if (new_thread) JScript::getVM()->DetachCurrentThread();
}

jmethodID JNIEnvWrap::getToInt() {
    if (!to_int) {
        to_int = env->GetStaticMethodID(helper_class, "toInt", "(Ljava/lang/Object;)I");
    }
    return to_int;
}

jmethodID JNIEnvWrap::getToLong() {
    if (!to_long) {
        to_long = env->GetStaticMethodID(helper_class, "toLong", "(Ljava/lang/Object;)J");
    }
    return to_long;
}

jmethodID JNIEnvWrap::getToFloat() {
    if (!to_float) {
        to_float = env->GetStaticMethodID(helper_class, "toFloat", "(Ljava/lang/Object;)F");
    }
    return to_float;
}

jmethodID JNIEnvWrap::getToDouble() {
    if (!to_double) {
        to_double = env->GetStaticMethodID(helper_class, "toDouble", "(Ljava/lang/Object;)D");
    }
    return to_double;
}

jmethodID JNIEnvWrap::getToBoolean() {
    if (!to_boolean) {
        to_boolean = env->GetStaticMethodID(helper_class, "toBoolean", "(Ljava/lang/Object;)Z");
    }
    return to_boolean;
}

jmethodID JNIEnvWrap::getToNative() {
    if (!to_native) {
        to_native = env->GetStaticMethodID(helper_class, "toNative", "(Ljava/lang/Object;)J");
    }
    return to_native;
}

jmethodID JNIEnvWrap::getFromInt() {
    if (!from_int) {
        from_int = env->GetStaticMethodID(helper_class, "fromInt", "(I)Ljava/lang/Object;");
    }
    return from_int;
}

jmethodID JNIEnvWrap::getFromLong() {
    if (!from_long) {
        from_long = env->GetStaticMethodID(helper_class, "fromLong", "(J)Ljava/lang/Object;");
    }
    return from_long;
}

jmethodID JNIEnvWrap::getFromFloat() {
    if (!from_float) {
        from_float = env->GetStaticMethodID(helper_class, "fromFloat", "(F)Ljava/lang/Object;");
    }
    return from_float;
}

jmethodID JNIEnvWrap::getFromDouble() {
    if (!from_double) {
        from_double = env->GetStaticMethodID(helper_class, "fromDouble", "(D)Ljava/lang/Object;");
    }
    return from_double;
}

jmethodID JNIEnvWrap::getFromBoolean() {
    if (!from_boolean) {
        from_boolean = env->GetStaticMethodID(helper_class, "fromBoolean", "(Z)Ljava/lang/Object;");
    }
    return from_boolean;
}

jmethodID JNIEnvWrap::getFromNative() {
    if (!from_native) {
        from_native = env->GetStaticMethodID(helper_class, "fromNative", "(Ljava/lang/String;J)Ljava/lang/Object;");
    }
    return from_native;
}

jmethodID JNIEnvWrap::getNewDic() {
    if (!new_dic) {
        new_dic = env->GetStaticMethodID(helper_class, "newDic", "()Ljava/lang/Object;");
    }
    return new_dic;
}

jmethodID JNIEnvWrap::getSetDicValue() {
    if (!set_dic_value) {
        set_dic_value = env->GetStaticMethodID(helper_class, "setDicValue", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/Object;)V");
    }
    return set_dic_value;
}

jmethodID JNIEnvWrap::getDicKeys() {
    if (!dic_keys) {
        dic_keys = env->GetStaticMethodID(helper_class, "dicKeys", "(Ljava/lang/Object;)[Ljava/lang/String;");
    }
    return dic_keys;
}

jmethodID JNIEnvWrap::getGetDicValue() {
    if (!get_dic_value) {
        get_dic_value = env->GetStaticMethodID(helper_class, "getDicValue", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;");
    }
    return get_dic_value;
}

jmethodID JNIEnvWrap::getCheckType() {
    if (!check_type) {
        check_type = env->GetStaticMethodID(helper_class, "checkType", "(Ljava/lang/Object;)C");
    }
    return check_type;
}

ScriptInstance* JNIEnvWrap::toNative(jobject jobj) {
    return (ScriptInstance*)env->CallStaticLongMethod(helper_class, getToNative(), jobj);
}
int JNIEnvWrap::toInt(jobject jobj) {
    return env->CallStaticIntMethod(helper_class, getToInt(), jobj);
}
long long JNIEnvWrap::toLong(jobject jobj) {
    return env->CallStaticLongMethod(helper_class, getToLong(), jobj);
}
float JNIEnvWrap::toFloat(jobject jobj) {
    return env->CallStaticFloatMethod(helper_class, getToFloat(), jobj);
}
double JNIEnvWrap::toDouble(jobject jobj) {
    return env->CallStaticDoubleMethod(helper_class, getToDouble(), jobj);
}
bool JNIEnvWrap::toBoolean(jobject jobj) {
    return env->CallStaticBooleanMethod(helper_class, getToBoolean(), jobj);
}
jobject JNIEnvWrap::fromNative(jstring class_name, jlong ptr) {
    return env->CallStaticObjectMethod(helper_class, getFromNative(), class_name, ptr);
}
jobject JNIEnvWrap::fromInt(int i) {
    return env->CallStaticObjectMethod(helper_class, getFromInt(), (jint)i);
}
jobject JNIEnvWrap::fromLong(long long l) {
    return env->CallStaticObjectMethod(helper_class, getFromLong(), (jlong)l);
}
jobject JNIEnvWrap::fromFloat(float f) {
    return env->CallStaticObjectMethod(helper_class, getFromFloat(), (jfloat)f);
}
jobject JNIEnvWrap::fromDouble(double d) {
    return env->CallStaticObjectMethod(helper_class, getFromDouble(), (jdouble)d);
}
jobject JNIEnvWrap::fromBoolean(bool b) {
    return env->CallStaticObjectMethod(helper_class, getFromBoolean(), (jboolean)b);
}

jobject JNIEnvWrap::newDic() {
    return env->CallStaticObjectMethod(helper_class, getNewDic());
}
void JNIEnvWrap::setDicValue(jobject dic, jstring key, jobject value) {
    env->CallStaticVoidMethod(helper_class, getSetDicValue(), dic, key, value);
}

jobjectArray JNIEnvWrap::dicKeys(jobject dic) {
    return (jobjectArray)env->CallStaticObjectMethod(helper_class, getDicKeys(), dic);
}

jobject JNIEnvWrap::getDicValue(jobject dic, jstring key) {
    return env->CallStaticObjectMethod(helper_class, getGetDicValue(), dic, key);
}

jchar JNIEnvWrap::checkType(jobject jobj) {
    return env->CallCharMethod(helper_class, getCheckType(), jobj);
}

void JScript::loadVM(JavaVM *_vm) {
    vm = _vm;
    Ref<JNIEnvWrap> env = getEnv();
    JNIEnv *_env = ***env;
    jclass jcls = _env->FindClass(HELPER_CLASS);
    helper_class = (jclass)_env->NewGlobalRef(jcls);
    _env->DeleteLocalRef(jcls);

    check_method = _env->GetStaticMethodID(helper_class, "hasMethod", "(L" JCLASS_CLASS ";L" JSTRING_CLASS ";)Z");
    new_class_method = _env->GetStaticMethodID(helper_class, "newClass", "(L" JSTRING_CLASS ";J)L" JOBJECT_CLASS ";");
}

jclass JScript::getHelperClass() {
    return helper_class;
}


Variant JScript::process(const Ref<JNIEnvWrap> &env, char type, jobject jobj) {
    Variant ret;
    switch (type) {
        case 'i': {
            ret = env->toInt(jobj);
            break;
        }
        case 'b': {
            ret = env->toBoolean(jobj);
            break;
        }
        case 'l': {
            ret = env->toLong(jobj);
            break;
        }
        case 'f': {
            ret = env->toFloat(jobj);
            break;
        }
        case 'd': {
            ret = env->toDouble(jobj);
            break;
        }
        case 'n': {
            ScriptInstance *obj = env->toNative(jobj);
            if (obj) {
                ret = obj->getTarget();
            }
            break;
        }
        case 'z': {
            jclass jsize_class = (**env)->GetObjectClass(jobj);
            jmethodID jget_width = (**env)->GetMethodID(jsize_class, "getWidth", "()F");
            jmethodID jget_height = (**env)->GetMethodID(jsize_class, "getHeight", "()F");
            float w = (**env)->CallFloatMethod(jobj, jget_width);
            float h = (**env)->CallFloatMethod(jobj, jget_height);
            ret = HSize(w, h);
            break;
        }
        case 's': {
            JNIEnv *_env = ***env;
            const char *chs = _env->GetStringUTFChars((jstring)jobj, NULL);
            ret = Variant(chs);
            _env->ReleaseStringUTFChars((jstring)jobj, chs);
            break;
        }
        case 'o': {
            NativeObject obj(jobj);
            ret = Variant(&obj);
            break;
        }
        case 'm': {
            JNIEnv *_env = ***env;
            jobjectArray jkeys = env->dicKeys(jobj);
            size_t count = _env->GetArrayLength(jkeys);
            Map map;
            for (size_t i = 0; i < count; ++i) {
                jstring key = (jstring)_env->GetObjectArrayElement(jkeys, i);
                jobject jval = (jobject)env->getDicValue(jobj, key);
                const char *kchs = _env->GetStringUTFChars(key, NULL);
                map->set(kchs, process(env, env->checkType(jval), jval));
                _env->ReleaseStringUTFChars(key, kchs);
                _env->DeleteLocalRef(key);
                _env->DeleteLocalRef(jval);
            }
            ret = map;
            break;
        }
        case 'a': {
            JNIEnv *_env = ***env;
            jobjectArray jobjs = (jobjectArray)jobj;
            jsize count = _env->GetArrayLength(jobjs);
            Array arr;
            for (jsize i = 0; i < count; ++i) {
                jobject jsubobj = _env->GetObjectArrayElement(jobjs, i);
                arr << process(env, env->checkType(jsubobj), jsubobj);
                _env->DeleteLocalRef(jsubobj);
            }
            ret = arr;
            break;
        }
        default:
            break;
    }
    return ret;
}

vector<Variant> JScript::process(const Ref<JNIEnvWrap> &env, jstring jsignature, jobjectArray jarr) {
    jint size = (**env)->GetArrayLength(jarr);
    JNIEnv *_env = ***env;
    vector<Variant> vs = vector<Variant>(size);
    const char *sig = (**env)->GetStringUTFChars(jsignature, NULL);
    for (int i = 0; i < size; ++i) {
        jobject jobj = _env->GetObjectArrayElement(jarr, i);
        vs[i] = process(env, sig[i], jobj);
        _env->DeleteLocalRef(jobj);
    }
    return vs;
}

jobject JScript::process(const Ref<JNIEnvWrap> &env, const Variant &variant) {
    if (!variant.empty()) {
        const HClass *cls = variant.getType();
        if (cls->isTypeOf(Integer::getClass())) {
            return env->fromInt((int)variant);
        }else if (cls->isTypeOf(Long::getClass()) | cls->isTypeOf(LongLong::getClass())) {
            return env->fromLong((long long)variant);
        }else if (cls->isTypeOf(Float::getClass())) {
            return env->fromFloat((float)variant);
        }else if (cls->isTypeOf(Double::getClass())) {
            return env->fromDouble((double)variant);
        }else if (cls->isTypeOf(NativeObject::getClass())) {
            return (jobject)(long)variant;
        }else if (cls->isTypeOf(_String::getClass()) ||
                cls->isTypeOf(StringName::getClass())) {
            return (***env)->NewStringUTF((const char *) variant);
        }else if (cls->isTypeOf(Boolean::getClass())) {
            return env->fromBoolean((bool)variant);
//        }else if (cls->isTypeOf(_Array::getClass())){
//            Array arr = variant;
//            int size = arr.size();
//            JNIEnv *_env = (***env);
//
//            jobjectArray arrObjects = _env->NewObjectArray(size, _env->FindClass("java/lang/Object"), NULL);
//            for (int i = 0; i < size; ++i) {
//                jobject obj = process(env, arr.at(i));
//                _env->SetObjectArrayElement(arrObjects, i, obj);
//                _env->DeleteLocalRef(obj);
//            }
//            return arrObjects;
//        }else if (cls->isTypeOf(Map::getClass())) {
//            Map *map = variant.get<Map>();
//            JNIEnv *_env = (***env);
//            jobject jdic = env->newDic();
//            for (auto it = (*map)->begin(), _e = (*map)->end(); it != _e; ++it) {
//                jstring jkey = _env->NewStringUTF(it->first.c_str());
//                jobject jvalue = process(env, it->second);
//                env->setDicValue(jdic, jkey, jvalue);
//                _env->DeleteLocalRef(jkey);
//                _env->DeleteLocalRef(jvalue);
//            }
//            return jdic;
        } else if (variant) {
            JNIEnv *_env = (***env);
            HObject *obj = variant.get();
            bool created = false;
            JClass *mcls = (JClass*)find(obj->getInstanceClass()->getFullname(), created);
            JInstance *ins = (JInstance*)mcls->get(obj);
            if (!ins) {
                ins = (JInstance*)mcls->createVariant(obj);
                const char *chs = obj->getInstanceClass()->getFullname().str();
                jstring jname = _env->NewStringUTF(chs);
                jobject ret = env->fromNative(jname, (jlong)ins);
                ins->setJObject(ret, _env);
                _env->DeleteLocalRef(jname);
                return ret;
            }else {
                return _env->NewLocalRef(ins->getJObject());
            }
        }
    }
    return NULL;
}

Ref<JNIEnvWrap> JScript::getEnv(JNIEnv *env) {
    if (env == NULL) {
        JavaVM *vm = getVM();
        if (vm) {
            int status;

            status = vm->GetEnv((void **) &env, JNI_VERSION_1_6);
            if(status < 0) {
                pthread_t p = pthread_self();
                auto it = envs.find((void*)p);
                if (it != envs.end()) {
                    return *(Ref<JNIEnvWrap>*)it->second;
                }
                status = vm->AttachCurrentThread(&env, NULL);
                if(status < 0) {
                    return NULL;
                }
                return new JNIEnvWrap(env, helper_class, true);
            }
            return new JNIEnvWrap(env, helper_class, false);
        }
        return NULL;
    }else {
        return new JNIEnvWrap(env, helper_class, false);
    }
}

void JScript::registerThread() {
    pthread_t p = pthread_self();
    envs[(void*)p] = new Ref<JNIEnvWrap>(getEnv());
}

void JScript::unregisterThread() {
    pthread_t p = pthread_self();
    auto it = envs.find((void*)p);
    if (it != envs.end()) {
        delete (Ref<JNIEnvWrap>*)it->second;
        envs.erase(it);
    }
}

ScriptClass *JScript::makeClass() const {
    return new JClass;
}

JScript *JScript::instance() {
    return _instance;
}

jobject JScript::newHiClass(JNIEnv *env, jstring name, long ptr) {
    return env->CallStaticObjectMethod(helper_class, new_class_method, name, ptr);
}

extern "C" {

char *stpcpy(char *dst, char const *src)
{
    size_t src_len = strlen(src);
    return (char*)memcpy(dst, src, src_len) + src_len;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    if (!JScript::instance()) new JScript;
    JScript::loadVM(vm);
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_gr_Helper_getParams(JNIEnv * env, jobject jobj,
                                                                  jobjectArray jarr, jlong jptr) {
    JScript *spt = JScript::instance();
    Ref<JNIEnvWrap> _env = JScript::getEnv(env);
    jsize count = env->GetArrayLength(jarr);
    const Variant **params = (const Variant **)jptr;
    for (int i = 0; i < count; ++i) {
        jobject obj = spt->process(_env, *params[i]);
        env->SetObjectArrayElement(jarr, i, obj);
        env->DeleteLocalRef(obj);
    }
}

JNIEXPORT jobject JNICALL Java_com_gr_Helper_findc(JNIEnv * env, jclass jcls, jstring jname) {
    const char *cname = env->GetStringUTFChars(jname, NULL);
    JScript *jsc = JScript::instance();
    bool create = false;
    JClass *cls = (JClass*)jsc->find(cname, create);
    if (cls) {
        if (create) {
            cls->setJavaClass(JScript::newHiClass(env, jname, (long)cls), env);
        }
        env->ReleaseStringUTFChars(jname, cname);
        return cls->getJavaClass();
    }
    env->ReleaseStringUTFChars(jname, cname);
    return nullptr;
}

}

#endif

