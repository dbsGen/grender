//
// Created by gen on 03/03/2017.
//

#include "AndroidFont.h"
#include <script/java/JScript.h>
#include <android/bitmap.h>

using namespace gr;
using namespace gscript;

#define ANDROID_FONT_CLASS "com/hiar/render/AndroidFont"
#define ANDROID_RECT_CLASS "android/graphics/Rect"
#define BITMAP_SIG      "Landroid/graphics/Bitmap;"
#define TYPEFACE_SIG    "Landroid/graphics/Typeface;"

jclass AndroidFont::android_font_class = NULL;
jmethodID AndroidFont::create_bitmap_method = NULL;
jmethodID AndroidFont::destroy_bitmap_method = NULL;
jmethodID AndroidFont::create_typeface_method = NULL;
jmethodID AndroidFont::draw_text_method = NULL;

jclass AndroidFont::android_rect_class = NULL;
jfieldID AndroidFont::rect_left_field = NULL;
jfieldID AndroidFont::rect_right_field = NULL;
jfieldID AndroidFont::rect_top_field = NULL;
jfieldID AndroidFont::rect_bottom_field = NULL;

AndroidFont::AndroidFont() : Font(12), bitmap(NULL), typeface(NULL) {
}

AndroidFont::AndroidFont(int size) : Font(size), bitmap(NULL), typeface(NULL) {
}

AndroidFont::AndroidFont(string name, int size) : Font(size), bitmap(NULL), typeface(NULL), font_name(name) {
}

AndroidFont::~AndroidFont() {
    Ref<JNIEnvWrap> envWrap;
    JNIEnv *env = nullptr;
    if (bitmap) {
        envWrap = JScript::getEnv();
        env = envWrap->jniEnv();
        jclass cls = getAndroidFontClass(env);
        env->CallStaticVoidMethod(cls, destroy_bitmap_method, bitmap);
        env->DeleteGlobalRef(bitmap);
    }
    if (typeface) {
        if (!env) {
            envWrap = JScript::getEnv();
            env = envWrap->jniEnv();
        }
        env->DeleteGlobalRef(typeface);
    }
}

void AndroidFont::calculateTextureSize(unsigned int &width, unsigned int &height) {
    width = 2048;
    height = 2048;
}

void AndroidFont::drawCharacter(unsigned int ch, const Vector2i &off, const Ref<Texture> &tex,
                                int &width, int &height, Vector2i &corner) {
    Ref<JNIEnvWrap> envWrap = JScript::getEnv();
    JNIEnv *env = envWrap->jniEnv();
    jobject bitmap = getBitmap(env);

    jfloat size = getSize();
    jobject jrect = env->CallStaticObjectMethod(getAndroidFontClass(env), draw_text_method, bitmap, (jchar)ch, size, getTypeface(env));
    jint left = env->GetIntField(jrect, rect_left_field);
    jint top = env->GetIntField(jrect, rect_top_field);
    jint right = env->GetIntField(jrect, rect_right_field);
    jint bottom = env->GetIntField(jrect, rect_bottom_field);
    corner.x(left);
    corner.y(top + size);
    width = right - left;
    height = bottom - top;

    uint8_t *buffer = NULL;
    AndroidBitmap_lockPixels(env, bitmap, (void **) &buffer);
    if (buffer) {
//        char *chs_buf = (char *)buffer;
//        LOG(i, "BEGIN -----------");
//        for (int y = 0; y < size; ++y) {
//            string line;
//            for (int x = 0; x < size; ++x) {
//                line += chs_buf[x + y * (int)size] ? '0':'_';
//            }
//            LOG(i, "%s", line.c_str());
//        }
//        LOG(i, "END -------------");
        tex->render(off.x(), off.y(), size, size, buffer);
        AndroidBitmap_unlockPixels(env, bitmap);
    }
}

jclass AndroidFont::getAndroidFontClass(JNIEnv *env) {
    if (!android_font_class) {
        android_font_class = (jclass)env->NewGlobalRef(env->FindClass(ANDROID_FONT_CLASS));
        create_bitmap_method = env->GetStaticMethodID(android_font_class, "createBitmap", "(II)" BITMAP_SIG);
        destroy_bitmap_method = env->GetStaticMethodID(android_font_class, "destroyBitmap", "(" BITMAP_SIG ")V");
        create_typeface_method = env->GetStaticMethodID(android_font_class, "createTypeface", "(Ljava/lang/String;I)" TYPEFACE_SIG);
        draw_text_method = env->GetStaticMethodID(android_font_class, "drawText", "(" BITMAP_SIG "CF" TYPEFACE_SIG ")L" ANDROID_RECT_CLASS ";");

        android_rect_class = (jclass)env->NewGlobalRef(env->FindClass(ANDROID_RECT_CLASS));
        rect_left_field = env->GetFieldID(android_rect_class, "left", "I");
        rect_right_field = env->GetFieldID(android_rect_class, "right", "I");
        rect_top_field = env->GetFieldID(android_rect_class, "top", "I");
        rect_bottom_field = env->GetFieldID(android_rect_class, "bottom", "I");
    }
    return android_font_class;
}

jobject AndroidFont::getBitmap(JNIEnv *env) {
    if (!bitmap) {
        jclass cls = getAndroidFontClass(env);
        bitmap = env->NewGlobalRef(env->CallStaticObjectMethod(cls, create_bitmap_method, (jint)getSize(), (jint)getSize()));
    }
    return bitmap;
}

jobject AndroidFont::getTypeface(JNIEnv *env) {
    if (!typeface) {
        jclass cls = getAndroidFontClass(env);
        jstring jstr = nullptr;
        if (!font_name.empty()) {
            jstr = env->NewStringUTF(font_name.c_str());
        }
        typeface = env->NewGlobalRef(env->CallStaticObjectMethod(cls, create_typeface_method, jstr, (jint)0));
    }
    return typeface;
}







