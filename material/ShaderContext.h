//
//  ShaderContext.hpp
//  hirender_iOS
//
//  Created by mac on 2017/4/25.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef ShaderContext_hpp
#define ShaderContext_hpp

#include <core/Ref.h>
#include <core/Data.h>
#include <vector>

namespace gr {
    class ShaderCompiler;
    class ShaderContextItem;
    CLASS_BEGIN_N(ShaderContext, gc::RefObject)
    
public:
    struct Result {
        pointer_vector attributes;
        pointer_vector uniforms;
        pointer_vector compiles;
        
        _FORCE_INLINE_ Result() : attributes(NULL), uniforms(NULL), compiles(NULL) {}
    };
    
private:
    
    pointer_map items;
    std::vector<std::string> paths;
    
    ShaderContextItem *findItem(const gc::StringName &name);
    void _compile(ShaderCompiler &compiler,
                  ShaderContextItem *item,
                  const variant_map &compile_argvs,
                  std::string &shader,
                  bool is_function);
    void _parse(ShaderContextItem *item,
                Result &result);
    
public:
    ShaderContext() {}
    ~ShaderContext();
    
    void addData(const gc::StringName &name, const gc::Ref<gc::Data> &data, bool replase = false);
    void addPath(const std::string &path);
    
    void compile(const gc::StringName &name, const variant_map &compile_argvs, std::string &shader);
    void parse(const gc::StringName &name, Result &result);
    
    CLASS_END
}

#endif /* ShaderContext_hpp */
