//
//  ScriptTransport.cpp
//  hirender_iOS
//
//  Created by Gen on 16/10/5.
//  Copyright © 2016年 gen. All rights reserved.
//

#include <core/Callback.h>
#include "ScriptTransport.h"

using namespace hiscript;

ref_map ScriptTransport::callbacks;

void ScriptTransport::reg(const hicore::StringName &name, const hicore::Reference &callback) {
    callbacks[name] = callback;
}

void ScriptTransport::send(const hicore::StringName &name, const hicore::Reference &object) {
    auto it = callbacks.find(name);
    if (it != callbacks.end()) {
        Callback *callback = (*it->second)->cast_to<Callback>();
        if (callback)
            callback->invoke(Array(vector<Variant>{{object}}));
    }
}