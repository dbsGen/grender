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
    jmethodID HTTPClient_add_header(NULL);
    jmethodID HTTPClient_add_params(NULL);
    jmethodID HTTPClient_get_response_headers(NULL);
    jmethodID HTTPClient_set_timeout(NULL);
    jmethodID HTTPClient_set_body(NULL);

    jclass HTTPClient_listener_class(NULL);
    jfieldID  HTTPClient_method(NULL);
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
        jclass jcls = env->FindClass("com/gr/Request");
        HTTPClient_class = (jclass)env->NewGlobalRef(jcls);
        env->DeleteLocalRef(jcls);
        HTTPClient_creator = env->GetStaticMethodID(HTTPClient_class, "get", "(Ljava/lang/String;Ljava/lang/String;)Lcom/gr/Request;");
        HTTPClient_start = env->GetMethodID(HTTPClient_class, "start", "()V");
        HTTPClient_cancel = env->GetMethodID(HTTPClient_class, "cancel", "()V");
        HTTPClient_start_delay = env->GetMethodID(HTTPClient_class, "start", "(F)V");
        HTTPClient_set_listener = env->GetMethodID(HTTPClient_class, "setListener", "(Lcom/gr/Request$OnStatusListener;)V");
        HTTPClient_get_path = env->GetMethodID(HTTPClient_class, "getPath", "()Ljava/lang/String;");
        HTTPClient_set_timeout = env->GetMethodID(HTTPClient_class, "setTimeout", "(I)V");

        HTTPClient_add_header = env->GetMethodID(HTTPClient_class, "addHeader", "(Ljava/lang/String;Ljava/lang/String;)V");
        HTTPClient_add_params = env->GetMethodID(HTTPClient_class, "addParams", "(Ljava/lang/String;Ljava/lang/Object;)V");

        HTTPClient_set_body = env->GetMethodID(HTTPClient_class, "setBody", "([B)V");

        HTTPClient_get_response_headers = env->GetMethodID(HTTPClient_class, "getResponseHeaders", "()Ljava/util/Hashtable;");
        HTTPClient_method = env->GetFieldID(HTTPClient_class, "method", "Ljava/lang/String;");

        HTTPClient_listener_class = (jclass)env->NewGlobalRef(env->FindClass("com/gr/RequestResultListener"));
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

void HTTPClient::event(const gc::StringName &name, void *data) {
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

    jstring jstr = env->NewStringUTF(method.str());
    env->SetObjectField(jrequest, HTTPClient_method, jstr);
    env->DeleteLocalRef(jstr);
    for (auto it = headers.begin(), _e = headers.end(); it != _e; ++it) {
        jstring jname = env->NewStringUTF(it->first.c_str());
        jstring jcontent = env->NewStringUTF(it->second.c_str());
        env->CallVoidMethod(jrequest, HTTPClient_add_header, jname, jcontent);
        env->DeleteLocalRef(jname);
        env->DeleteLocalRef(jcontent);
    }
    for (auto it = parameters.begin(), _e = parameters.end(); it != _e; ++it) {
        jstring jname = env->NewStringUTF(it->first.c_str());
        jstring jcontent = env->NewStringUTF((const char *)it->second);
        env->CallVoidMethod(jrequest, HTTPClient_add_params, jname, jcontent);
        env->DeleteLocalRef(jname);
        env->DeleteLocalRef(jcontent);
    }
    if (body) {
        jbyteArray jbody = env->NewByteArray(body->getSize());
        env->SetByteArrayRegion(jbody, 0, body->getSize(), (const jbyte*)body->getBuffer());
        env->CallVoidMethod(jrequest, HTTPClient_set_body, jbody);
        env->DeleteLocalRef(jbody);
    }

    jint jtimeout = (jint)(timeout * 1000);
    env->CallVoidMethod(jrequest, HTTPClient_set_timeout, jtimeout);

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
JNIEXPORT void JNICALL Java_com_gr_RequestResultListener_process(JNIEnv * env, jclass jcls, jlong ptr, jfloat percent) {
    HTTPClient *client = (HTTPClient*)ptr;
    client->event(event_process, &percent);
}
JNIEXPORT void JNICALL Java_com_gr_RequestResultListener_complete(JNIEnv * env, jclass jcls, jlong ptr, jobject request) {
    HTTPClient *client = (HTTPClient*)ptr;
    jstring jpath = (jstring)env->CallObjectMethod(request, HTTPClient_get_path);
    const char *chs = env->GetStringUTFChars(jpath, NULL);

    jobject dic = env->CallObjectMethod(request, HTTPClient_get_response_headers);
    if (dic) {
        Ref<JNIEnvWrap> _env = JScript::getEnv(env);
        jobjectArray jkeys = _env->dicKeys(dic);
        jsize len = env->GetArrayLength(jkeys);
        Map map;
        for (int i = 0; i < len; ++i) {
            jobject jkey = env->GetObjectArrayElement(jkeys, i);
            const char *key = env->GetStringUTFChars((jstring)jkey, NULL);
            jobject jvalue = _env->getDicValue(dic, (jstring)jkey);
            const char *value = env->GetStringUTFChars((jstring)jvalue, NULL);
            map->set(key, value);
            env->ReleaseStringUTFChars((jstring)jkey, key);
            env->ReleaseStringUTFChars((jstring)jvalue, value);
            env->DeleteLocalRef(jkey);
            env->DeleteLocalRef(jvalue);
        }
        client->setResponseHeaders(map);
    }

    client->event(event_complete, (void*)chs);
    env->ReleaseStringUTFChars(jpath, chs);
}
JNIEXPORT void JNICALL Java_com_gr_RequestResultListener_failed(JNIEnv * env, jclass jcls, jlong ptr) {
    HTTPClient *client = (HTTPClient*)ptr;
    char *error = "net error";
    client->event(event_failed, error);
}
JNIEXPORT void JNICALL Java_com_gr_RequestResultListener_cancel(JNIEnv * env, jclass jcls, jlong ptr) {

}
}

#endif