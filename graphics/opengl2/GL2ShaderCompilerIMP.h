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
using namespace gr;


namespace higraphics {

    CLASS_BEGIN_N(GL2ShaderCompilerIMP, ShaderCompilerIMP)

        void processCall(ShaderParser::CallItem *call_item, string &shader);
        void processValue(ShaderParser::Item *item, string &shader, int last_level = -1);
        void processConstantItem(ShaderParser::ConstantItem *const_item, string &shader);
        void processBlockItems(const pointer_vector &items, const variant_map &compile_argvs, string &shader, int indent);
        void processOperatorItem(ShaderParser::OperatorItem *item, string &shader, int last_level = -1);
        const char *processVariantType(ShaderParser::AttributeItem *item);
        const char *processVariantType(ShaderParser::VariantType);
        void processAttributeItem(ShaderParser::AttributeItem *item, const variant_map *compile_argvs, string &shader, bool read_default);

    protected:
        virtual void compile(const pointer_vector &items, const variant_map &compile_argvs, string &shader);

    CLASS_END

}

#endif /* GLShaderCompilerIMP_hpp */
