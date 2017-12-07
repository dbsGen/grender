//
// Created by gen on 20/04/2017.
//

#include "ShaderCompiler.h"
#include "ShaderParser.h"
#include "ShaderContext.h"
#include "../graphics/Factory.h"

using namespace hirender;

void ShaderCompiler::setMainName(const hicore::StringName &name) {
    imp->main_name = name;
}

const hicore::StringName &ShaderCompiler::getMainName() {
    return imp->main_name;
}

void ShaderCompiler::compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader) {
    imp->compile(items, compile_argvs, shader);
}

ShaderCompiler::ShaderCompiler() {
    imp = (ShaderCompilerIMP*)higraphics::Factory::getInstance()->create(ShaderCompilerIMP::getClass());
}
