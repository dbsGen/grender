//
// Created by mac on 2017/5/25.
//

#ifdef __ANDROID__

#include "HTTPClient.h"
#include <script/java/JScript.h>

using namespace gr;
using namespace gscript;

namespace gr {
    jclass HTTPClient_class(NULL);
    jmethodID HTTPClient_creator(NULL);
    jmethodID HTTPClient_start(NULL);
    jmethodID HTTPClient_start_delay(NULL);
    jmethodID HTTPClient_cancel(NULL);
    jmethodID HTTPClient_set_listener(NULL);
    jmethodID HTTPClient_get_path(NULL);

    jclass HTTPClient_listener_class(NULL);
    jmethodID HTTPClient_listener_creator(NULL);
    const StringName HTTPClient_request("REQUEST");
    const StringName HTTPClient_listener("LISTENER");

    const StringName event_complete("COMPLETE");
    const StringName event_failed("FAILED");
    const StringName event_process("PROCESS");
}


void HTTPClient::_initialize() {
    Ref<JNIEnvWrap> _env = JScript::getEnv();
    JNIEnv *env = ***_env;

    if (HTTPClient_class == NULL) {
        jclass jcls = env->FindClass("com/hiar/render/Request");
        HTTPClient_class = (jclass)env->NewGlobalRef(jcls);
        env->DeleteLocalRef(jcls);
        HTTPClient_creator = env->GetStaticMethodID(HTTPClient_class, "get", "(Ljava/lang/String;Ljava/lang/String;)Lcom/hiar/render/Request;");
        HTTPClient_start = env->GetMethodID(HTTPClient_class, "start", "()V");
        HTTPClient_cancel = env->GetMethodID(HTTPClient_class, "cancel", "()V");
        HTTPClient_start_delay = env->GetMethodID(HTTPClient_class, "start", "(F)V");
        HTTPClient_set_listener = env->GetMethodID(HTTPClient_class, "setListener", "(Lcom/hiar/render/Request$OnStatusListener;)V");
        HTTPClient_get_path = env->GetMethodID(HTTPClient_class, "getPath", "()Ljava/lang/String;");

        HTTPClient_listener_class = (jclass)env->NewGlobalRef(env->FindClass("com/hiar/render/RequestResultListener"));
        HTTPClient_listener_creator = env->GetMethodID(HTTPClient_listener_class, "<init>", "(J)V");
    }
}
void HTTPClient::_finalize() {
    auto it = properties.find((void*)HTTPClient_request);
    if (it != properties.end()) {
        jobject jrequest = (jobject) it->second;
        Ref<JNIEnvWrap> _env = JScript::getEnv();
        JNIEnv *env = ***_env;
        env->DeleteGlobalRef(jrequest);
    }
}

void HTTPClient::event(const StringName &name, void *data) {
    if (name == event_complete) {
        path = (const char *)data;
        error.clear();
        if (on_complete)
            (**on_complete)(this, path);
    }else if (name == event_failed) {
        if (retry_num >= retry_count) {
            error = (const char *)data;
            if (on_complete)
                (**on_complete)(this, error);
        }else {
            retry_num ++;
            if (delay) {
                start();
            }else {
                start();
            }
        }
    }else if (name == event_process) {
        percent = *(float*)data;
        if (on_progress)
            (**on_progress)(this, percent);
    }
}

void HTTPClient::start() {
    auto it = properties.find((void*)HTTPClient_request);
    jobject jrequest;
    Ref<JNIEnvWrap> _env = JScript::getEnv();
    JNIEnv *env = ***_env;
    if (it == properties.end()) {
        jstring jurl = env->NewStringUTF(url.c_str());
        jrequest = env->NewGlobalRef(env->CallStaticObjectMethod(HTTPClient_class, HTTPClient_creator, jurl, jurl));
        properties[(void*)HTTPClient_request] = jrequest;
        env->DeleteLocalRef(jurl);

        jobject jlis = env->NewGlobalRef(env->NewObject(HTTPClient_listener_class, HTTPClient_listener_creator, (jlong)this));
        properties[(void*)HTTPClient_listener] = jlis;
        env->CallVoidMethod(jrequest, HTTPClient_set_listener, jlis);
    }else {
        jrequest = (jobject)it->second;
    }
    if (delay > 0) {
        env->CallVoidMethod(jrequest, HTTPClient_start_delay, delay);
    }else
        env->CallVoidMethod(jrequest, HTTPClient_start);
}
void HTTPClient::cancel() {
    auto it = properties.find((void*)HTTPClient_request);
    Ref<JNIEnvWrap> _env = JScript::getEnv();
    JNIEnv *env = ***_env;
    if (it != properties.end()) {
        env->CallVoidMethod((jobject)it->second, HTTPClient_cancel);
    }
}

extern "C" {
JNIEXPORT void JNICALL Java_com_hiar_render_RequestResultListener_process(JNIEnv * env, jclass jcls, jlong ptr, jfloat percent) {
    HTTPClient *client = (HTTPClient*)ptr;
    client->event(event_process, &percent);
}
JNIEXPORT void JNICALL Java_com_hiar_render_RequestResultListener_complete(JNIEnv * env, jclass jcls, jlong ptr, jobject request) {
    HTTPClient *client = (HTTPClient*)ptr;
    jstring jpath = (jstring)env->CallObjectMethod(request, HTTPClient_get_path);
    const char *chs = env->GetStringUTFChars(jpath, NULL);
    client->event(event_complete, (void*)chs);
    env->ReleaseStringUTFChars(jpath, chs);

}
JNIEXPORT void JNICALL Java_com_hiar_render_RequestResultListener_failed(JNIEnv * env, jclass jcls, jlong ptr) {
    HTTPClient *client = (HTTPClient*)ptr;
    char *error = "net error";
    client->event(event_failed, error);
}
JNIEXPORT void JNICALL Java_com_hiar_render_RequestResultListener_cancel(JNIEnv * env, jclass jcls, jlong ptr) {

}
}

#endif