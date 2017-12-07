//
//  ObjcScript.cpp
//  hirender_iOS
//
//  Created by gen on 16/9/17.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifdef __OBJC__
#include "ObjcScript.h"
#include "ObjcClass.h"

using namespace hiscript;

ScriptClass *ObjcScript::makeClass() const {
    return new ObjcClass;
}

ScriptInstance *ObjcScript::newBuff(const string &cls_name, hicore::HObject *target, const hicore::Variant **params, int count) const {
    return NULL;
}

#endif
