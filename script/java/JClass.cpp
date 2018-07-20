//
// Created by gen on 16/9/6.
//

#ifdef __ANDROID__
#include "JClass.h"
#include "JInstance.h"
#include "JScript.h"

#include <jni.h>
#include <core/FixType.h>
#include <core/Ref.h>

using namespace gscript;

ScriptInstance *JClass::makeInstance() const {
    return new JInstance;
}

Variant JClass::apply(const StringName &name, const Variant **params, int count) const {
    return Variant::null();
}

void JClass::setJavaClass(jobject new_cls, JNIEnv *env) {
    if (clz) env->DeleteGlobalRef(clz);
    clz = new_cls ? env->NewGlobalRef(new_cls) : new_cls;
}

JClass::~JClass() {
    if (clz) {
        bool attach = false;
        Ref<JNIEnvWrap> env = JScript::getEnv();
        (**env)->DeleteGlobalRef(clz);
    }
}

extern "C" {

JNIEXPORT void JNICALL Java_com_gr_Helper_00024GClass_delete(JNIEnv * env, jclass obj, jlong jptr) {
    ScriptClass *cls = (ScriptClass *)jptr;
    delete cls;
}

JNIEXPORT jlong JNICALL Java_com_gr_Helper_00024GClass_newi(JNIEnv * env, jclass jcls, jlong jptr,
                                                                  jstring jsignature, jobjectArray jarr) {
    ScriptClass *cls = (ScriptClass *)jptr;
    jint size = env->GetArrayLength(jarr);
    JScript *jsc = JScript::instance();
    Ref<JNIEnvWrap> _env = JScript::getEnv(env);
    vector<Variant> vs = jsc->process(_env, jsignature, jarr);
    jlong ret;
    if (size > 0) {
        const Variant **arr = (const Variant **)malloc(size * sizeof(Variant*));
        for (int i = 0; i < size; ++i) {
            arr[i] = &vs[i];
        }
        ret = (jlong)cls->newInstance(arr, size);
        free(arr);
    }else {
        ret = (jlong)cls->newInstance(NULL, size);
    }
    return ret;
}

JNIEXPORT jobject JNICALL Java_com_gr_Helper_00024GClass_call(JNIEnv * env, jclass jcls, jlong jptr, jstring jname,
                                                                  jstring jsignature, jobjectArray jarr) {
    ScriptClass *cls = (ScriptClass *)jptr;
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
        ret = cls->call(cname, arr, size);
        free(arr);
    }else {
        ret = cls->call(cname, NULL, size);
    }
    return jsc->process(_env, ret);
}

}
#endif //__ANDROID__
