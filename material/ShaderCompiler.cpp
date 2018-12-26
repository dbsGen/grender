//
// Created by gen on 20/04/2017.
//

#include "ShaderCompiler.h"
#include "ShaderParser.h"
#include "ShaderContext.h"
#include "../graphics/Factory.h"

using namespace gr;

void ShaderCompiler::setMainName(const gc::StringName &name) {
    imp->main_name = name;
}

const gc::StringName &ShaderCompiler::getMainName() {
    return imp->main_name;
}

void ShaderCompiler::compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader) {
    imp->compile(items, compile_argvs, shader);
}

ShaderCompiler::ShaderCompiler() {
    imp = (ShaderCompilerIMP*)gg::Factory::getInstance()->create(ShaderCompilerIMP::getClass());
}
