//
//  ObjcClass.m
//  hirender_iOS
//
//  Created by gen on 16/9/17.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifdef __OBJC__

#include "ObjcClass.h"
#include "ObjcInstance.h"
#import <Foundation/Foundation.h>
#import "ObjcUtils.h"

using namespace hiscript;

ScriptInstance *ObjcClass::makeInstance() const {
    return new ObjcInstance;
}

Variant ObjcClass::apply(const StringName &name, const Variant **params, int count) const {
    if (clz) {
        const char *chs = name.str();
        size_t len = strlen(chs);
        char *sel_cs = (char *)malloc((len + count + 1) * sizeof(char));
        strcpy(sel_cs, chs);
        for (int i = 0; i < count; ++i) {
            sel_cs[len + i] = ':';
        }
        sel_cs[len + count] = 0;
        SEL sel = sel_getUid(sel_cs);
        Variant ret = [ObjcUtils apply:clz :sel :params :count];
        free(sel_cs);
        return ret;
    }
    
    return Variant::null();
}

#endif
