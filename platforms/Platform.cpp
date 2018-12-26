//
//  Platform.cpp
//  hirender_iOS
//
//  Created by Gen on 16/9/24.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Platform.h"

using namespace gr;
using namespace gc;
using namespace std;

Platform *Platform::shared_platform(NULL);

Platform *Platform::sharedPlatform() {
    return shared_platform;
}

Platform::~Platform() {
    if (shared_platform == this) {
        shared_platform = NULL;
    }
}

void Platform::reg(Platform *platform) {
    shared_platform = platform;
}

void Platform::startInput(const string &text, const string &placeholder, const RCallback &callback) {
    Variant vs[3];
    vs[0] = text;
    vs[1] = placeholder;
    vs[2] = callback;
    apply("_startInput", NULL, (const Variant**)&vs, 3);
}

void Platform::endInput() {
    apply("_endInput");
}

string Platform::persistencePath() {
    Variant ret;
    apply("_persistencePath", &ret);
    return ret;
}

METHOD Ref<Data> Platform::getFontData() {
    Variant ret;
    apply("_getFontData", &ret);
    return ret;
}
METHOD void Platform::setFontData(const Ref<Data> &data) {
    Variant vs[1];
    vs[0] = data;
    apply("_setFontData", NULL, (const Variant**)&vs, 1);
}
