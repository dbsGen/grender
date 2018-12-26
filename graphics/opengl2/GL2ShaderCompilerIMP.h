//
//  GLShaderCompilerIMP.hpp
//  hirender_iOS
//
//  Created by gen on 24/04/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#ifndef GLShaderCompilerIMP_hpp
#define GLShaderCompilerIMP_hpp

#include <material/ShaderCompiler.h>
#include "../graphics_define.h"


namespace gg {

    CLASS_BEGIN_N(GL2ShaderCompilerIMP, gr::ShaderCompilerIMP)

    void processCall(gr::ShaderParser::CallItem *call_item, string &shader);
    void processValue(gr::ShaderParser::Item *item, string &shader, int last_level = -1);
    void processConstantItem(gr::ShaderParser::ConstantItem *const_item, string &shader);
        void processBlockItems(const pointer_vector &items, const variant_map &compile_argvs, string &shader, int indent);
    void processOperatorItem(gr::ShaderParser::OperatorItem *item, string &shader, int last_level = -1);
    const char *processVariantType(gr::ShaderParser::AttributeItem *item);
    const char *processVariantType(gr::ShaderParser::VariantType);
    void processAttributeItem(gr::ShaderParser::AttributeItem *item, const variant_map *compile_argvs, string &shader, bool read_default);

    protected:
        virtual void compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader);

    CLASS_END

}

#endif /* GLShaderCompilerIMP_hpp */
