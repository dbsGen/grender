//
// Created by gen on 16/9/6.
//

#ifdef __ANDROID__

#include <core/Ref.h>
#include "JInstance.h"
#include "JScript.h"

using namespace gscript;

jmethodID JInstance::apply_method = NULL;
jclass JInstance::object_class = NULL;
jcharArray JInstance::jsigs = NULL;

Variant JInstance::apply(const StringName &name, const Variant **params, int count) {
    Ref<JNIEnvWrap> _env = JScript::getEnv();
    JNIEnv *env = ***_env;
    if (object) {
        if (!apply_method) {
            jclass base_class = env->GetObjectClass(object);
            apply_method = env->GetMethodID(base_class, "apply",
                                            "(L" JSTRING_CLASS ";[L" JOBJECT_CLASS ";[C)L" JOBJECT_CLASS ";");
        }
        if (apply_method) {
            if (!jsigs) {
                jsigs = (jcharArray)env->NewGlobalRef(env->NewCharArray(1));
            }
            jstring jname = env->NewStringUTF(name.str());
            if (object_class == NULL) {
                object_class = (jclass)env->NewGlobalRef(env->FindClass(JOBJECT_CLASS));
            }
            jobjectArray jarray = (jobjectArray)env->NewObjectArray(count, object_class, NULL);
            JScript *spt = JScript::instance();
            for (int i = 0; i < count; ++i) {
                env->SetObjectArrayElement(jarray, i, spt->process(_env, *params[i]));
            }
            jobject obj = env->CallObjectMethod(object, apply_method, jname, jarray, jsigs);
            jchar *chs = env->GetCharArrayElements(jsigs, NULL);
            Variant ret = spt->process(_env, chs[0], obj);
            env->DeleteLocalRef(jarray);
            env->ReleaseCharArrayElements(jsigs, chs, 0);
            env->DeleteLocalRef(jname);
            return ret;
        }
    }
    return Variant::null();
}

extern "C" {

JNIEXPORT void JNICALL Java_com_gr_Helper_00024GInstance_initializer(JNIEnv * env, jobject jobj,
                                                                            jlong jptr) {
    JInstance *cls = (JInstance *)jptr;
    cls->setJObject(jobj, env);
}

JNIEXPORT void JNICALL Java_com_gr_Helper_00024GInstance_delete(JNIEnv * env, jclass obj, jlong jptr) {
    ScriptInstance *cls = (ScriptInstance *)jptr;
    delete cls;
}

JNIEXPORT jobject JNICALL Java_com_gr_Helper_00024GInstance_call(JNIEnv * env, jclass jcls,
                                                                        jlong jptr, jstring jname,
                                                                        jstring jsignature, jobjectArray jarr) {
    ScriptInstance *obj = (ScriptInstance *)jptr;
    jint size = env->GetArrayLength(jarr);
    const char *cname = env->GetStringUTFChars(jname, NULL);
    JScript *jsc = JScript::instance();
    Ref<JNIEnvWrap> _env = JScript::getEnv(env);
    vector<Variant> vs = jsc->process(_env, jsignature, jarr);
    Variant ret;
    if (size > 0) {
        const Variant **arr = (const Variant **)malloc(size * sizeof(Variant*));
        for (int i = 0; i < size; ++i) {
            arr[i] = &vs[i];
        }
        ret = obj->call(cname, arr, size);
        free(arr);
    }else {
        ret = obj->call(cname, NULL, size);
    }
    return jsc->process(_env, ret);
}
JNIEXPORT jobject JNICALL Java_com_gr_Helper_00024GInstance_apply(JNIEnv * env, jclass jcls,
                                                                             jlong jptr, jstring jname,
                                                                             jstring jsignature, jobjectArray jarr) {
    ScriptInstance *obj = (ScriptInstance *)jptr;
    jint size = env->GetArrayLength(jarr);
    const char *cname = env->GetStringUTFChars(jname, NULL);
    JScript *jsc = JScript::instance();
    Ref<JNIEnvWrap> _env = JScript::getEnv(env);
    vector<Variant> vs = jsc->process(_env, jsignature, jarr);
    Variant ret;
    Variant target = obj->getTarget();
    if (target && target.getType()->isTypeOf(HObject::getClass())) {
        if (size > 0) {
            const Variant **arr = (const Variant **)malloc(size * sizeof(Variant*));
            for (int i = 0; i < size; ++i) {
                arr[i] = &vs[i];
            }
            target.get()->apply(cname, &ret, arr, size);
            free(arr);
        }else {
            target.get()->apply(cname, &ret, NULL, size);
        }
        return jsc->process(_env, ret);
    }
    return NULL;
}

}

#endif //__ANDROID__