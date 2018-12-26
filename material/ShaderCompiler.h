//
// Created by gen on 20/04/2017.
//

#ifndef NEXTLAUNCHER_GIT_SHADERCOMPILER_H
#define NEXTLAUNCHER_GIT_SHADERCOMPILER_H

#include "ShaderParser.h"
#include <core/Object.h>
#include <IMP.h>
#include "../render_define.h"

namespace gr {
    class ShaderCompilerIMP;
    class ShaderContext;
    CLASS_BEGIN_0_N(ShaderCompiler)
    
    pointer_vector includes;
    
    ShaderCompilerIMP *imp;
    ShaderContext *context;
    
public:
    void setMainName(const gc::StringName &name);
    const gc::StringName &getMainName();
    
    void compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader);
    
    ShaderCompiler();
    
    CLASS_END
    
    
    CLASS_BEGIN_TNV(ShaderCompilerIMP, Imp, 1, ShaderCompiler)
    
protected:
        gc::StringName main_name;
    
    virtual void compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader) = 0;
    
    friend class ShaderCompiler;
public:
    _FORCE_INLINE_ ShaderCompilerIMP() : main_name("main") {}
    
    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_SHADERCOMPILER_H
