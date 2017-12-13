//
// Created by gen on 16/9/3.
//

#include "Plugin.h"

using namespace gr;

const StringName &Plugin::getName() {
    if (name.empty()) {
        return getInstanceClass()->getFullname();
    }
    return name;
}