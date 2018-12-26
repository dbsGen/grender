//
//  GLShaderCompilerIMP.cpp
//  hirender_iOS
//
//  Created by gen on 24/04/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "GL2ShaderCompilerIMP.h"
#include <math/Type.h>
#include <core/String.h>

using namespace gg;
using namespace gc;
using namespace gr;

const StringName main_word("main");

const char *GL2ShaderCompilerIMP::processVariantType(ShaderParser::AttributeItem *item) {
    if (item->var_type == ShaderParser::Sampler) {
        return item->var_string.str();
    }else {
        return processVariantType(item->var_type);
    }
}

const char *GL2ShaderCompilerIMP::processVariantType(ShaderParser::VariantType type) {
    switch (type) {
        case ShaderParser::Vector2:
            return "vec2";
        case ShaderParser::Vector3:
            return "vec3";
        case ShaderParser::Vector4:
            return "vec4";
        case ShaderParser::Int:
            return "int";
        case ShaderParser::Long:
            return "long";
        case ShaderParser::Float:
            return "float";
        case ShaderParser::Double:
            return "double";
        case ShaderParser::Matrix2:
            return "mat2";
        case ShaderParser::Matrix3:
            return "mat3";
        case ShaderParser::Matrix4:
            return "mat4";
        case ShaderParser::Void:
            return "void";
        case ShaderParser::Sampler:
            return "sampler2D";
            
        default:
            break;
    }
    return NULL;
}

void GL2ShaderCompilerIMP::processAttributeItem(ShaderParser::AttributeItem *item,
                                                const variant_map *compile_argvs,
                                                string &shader, bool read_default) {
    if (item->attr_type == ShaderParser::AttributeItem::Compile) {
        shader += "const ";
        bool has_name = false;
        if (compile_argvs != nullptr) {
            auto it = compile_argvs->find(item->name);
            if (it != compile_argvs->end()) {
                const gc::Variant &var = it->second;
                
                switch (item->var_type) {
                    case ShaderParser::Vector2: {
                        shader += "vec2 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Vector2f::getClass())) {
                            shader += '=';
                            Vector2f v2 = var;
                            char str[44];
                            sprintf(str, "vec2(%f,%f)", v2.x(), v2.y());
                            shader += str;
                            return;
                        }
                        break;
                    }
                    
                    case ShaderParser::Vector3: {
                        shader += "vec3 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Vector3f::getClass())) {
                            shader += '=';
                            Vector3f v3 = var;
                            char str[64];
                            sprintf(str, "vec3(%f,%f,%f)", v3.x(), v3.y(), v3.z());
                            shader += str;
                            return;
                        }
                        break;
                    }
                    case ShaderParser::Vector4:{
                        shader += "vec4 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Vector4f::getClass())) {
                            shader += '=';
                            Vector4f v4 = var;
                            char str[64];
                            sprintf(str, "vec4(%f,%f,%f,%f)", v4.x(), v4.y(), v4.z(), v4.w());
                            shader += str;
                            return;
                        }
                        break;
                    }
                    case ShaderParser::Int: {
                        shader += "int ";
                        shader += item->name.str();
                        shader += '=';
                        char str[20];
                        sprintf(str, "%i", (int)var);
                        shader += str;
                        return;
                    }
                    case ShaderParser::Long: {
                        shader += "long ";
                        shader += item->name.str();
                        shader += '=';
                        char str[20];
                        sprintf(str, "%li", (long)var);
                        shader += str;
                        return;
                    }
                    case ShaderParser::Float: {
                        shader += "float ";
                        shader += item->name.str();
                        shader += '=';
                        char str[20];
                        sprintf(str, "%f", (float)var);
                        shader += str;
                        return;
                    }
                    case ShaderParser::Double: {
                        shader += "double ";
                        shader += item->name.str();
                        shader += '=';
                        char str[20];
                        sprintf(str, "%f", (double)var);
                        shader += str;
                        return;
                    }
                    case ShaderParser::Matrix2: {
                        shader += "mat2 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Matrix2::getClass())) {
                            Matrix2 mat2 = var;
                            shader += "=mat2(";
                            for (int i = 0; i < 4; ++i) {
                                char str[20];
                                sprintf(str, "%f", mat2.m[i]);
                                shader += str;
                                if (i != 3) {
                                    shader += ',';
                                }
                            }
                            shader += ')';
                            return;
                        }
                        break;
                    }
                    case ShaderParser::Matrix3: {
                        shader += "mat3 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Matrix2::getClass())) {
                            Matrix3 mat3 = var;
                            shader += "=mat3(";
                            for (int i = 0; i < 9; ++i) {
                                char str[20];
                                sprintf(str, "%f", mat3.m[i]);
                                shader += str;
                                if (i != 8) {
                                    shader += ',';
                                }
                            }
                            shader += ')';
                            return;
                        }
                        break;
                    }
                    case ShaderParser::Matrix4:{
                        shader += "mat4 ";
                        shader += item->name.str();
                        has_name = true;
                        if (var.getTypeClass()->isTypeOf(Matrix2::getClass())) {
                            Matrix4 mat4 = var;
                            shader += "=mat4(";
                            for (int i = 0; i < 16; ++i) {
                                char str[20];
                                sprintf(str, "%.4f", mat4.m[i]);
                                shader += str;
                                if (i != 15) {
                                    shader += ',';
                                }
                            }
                            shader += ')';
                            return;
                        }
                        break;
                    }
                    
                    default:
                        break;
                }
            }
        }
        if (!has_name) {
            const char *var = processVariantType(item);
            if (var) {
                shader += var;
            }
            shader += ' ';
            shader += item->name.str();
        }
        if (read_default &&
            item->def_value) {
            shader.push_back('=');
            processValue(item->def_value, shader);
        }
    }else {
        switch (item->attr_type) {
            case ShaderParser::AttributeItem::Attr:
                shader += "attribute ";
                break;
            case ShaderParser::AttributeItem::Varying:
                shader += "varying ";
                break;
            case ShaderParser::AttributeItem::Uniform:
                shader += "uniform ";
                break;
            
            default:
            break;
        }
        switch (item->prefix) {
            case ShaderParser::AttributeItem::Const:
            {
                shader += "const ";
            }
            break;
            case ShaderParser::AttributeItem::In:
            {
                shader += "in ";
            }
            break;
            case ShaderParser::AttributeItem::Out:
            {
                shader += "out ";
            }
            break;
            case ShaderParser::AttributeItem::Inout:
            {
                shader += "inout ";
            }
            break;
            
            default:
            break;
        }
        const char *var = processVariantType(item);
        if (var) {
            shader += var;
        }
        if (item->name) {
            shader += ' ';
            shader += item->name.str();
            if (read_default &&
                item->attr_type == ShaderParser::AttributeItem::Normal &&
                item->def_value) {
                shader.push_back('=');
                processValue(item->def_value, shader);
            }
        }
    }
}

void GL2ShaderCompilerIMP::processConstantItem(ShaderParser::ConstantItem *const_item, string &shader) {
    if (const_item->is_str) {
        shader.push_back('"');
        shader += const_item->variant;
        shader.push_back('"');
    }else {
        shader += const_item->variant;
    }
}

void GL2ShaderCompilerIMP::processCall(ShaderParser::CallItem *call_item, string &shader) {
    shader += call_item->name.str();
    shader.push_back('(');
    for (size_t i = 0, t = call_item->argvs.size(); i < t; ++i) {
        ShaderParser::Item *item = (ShaderParser::Item *)call_item->argvs[i];
        processValue(item, shader);
        if (i < t - 1) {
            shader.push_back(',');
        }
    }
    shader.push_back(')');
}

void GL2ShaderCompilerIMP::processValue(ShaderParser::Item *item, string &shader, int last_level) {
    switch (item->type) {
        case ShaderParser::Operator:
        {
            ShaderParser::OperatorItem *op_item = (ShaderParser::OperatorItem *)item;
            processOperatorItem(op_item, shader, last_level);
            break;
        }
        case ShaderParser::Constant:
        {
            ShaderParser::ConstantItem *const_item = (ShaderParser::ConstantItem *)item;
            processConstantItem(const_item, shader);
            break;
        }
        case ShaderParser::Word:
        {
            ShaderParser::WordItem *word_item = (ShaderParser::WordItem *)item;
            shader += word_item->value.str();
            break;
        }
        case ShaderParser::Index:
        {
            ShaderParser::IndexItem *index_item = (ShaderParser::IndexItem*)item;
            processValue(index_item->item, shader, ShaderParser::OperatorItem::priorityLevel(ShaderParser::OperatorItem::Dot));
            shader += '[';
            processValue(index_item->index_item, shader);
            shader += ']';
            break;
        }
        case ShaderParser::Call:
        {
            ShaderParser::CallItem *call_item = (ShaderParser::CallItem*)item;
            processCall(call_item, shader);
            break;
        }
        case ShaderParser::OperatorValue:
        {
            ShaderParser::OperatorValueItem *value_item = (ShaderParser::OperatorValueItem*)item;
            switch (value_item->operator_type) {
                case ShaderParser::OperatorValueItem::Minus:
                {
                    shader += '-';
                    break;
                }
                case ShaderParser::OperatorValueItem::MinusMinus:
                {
                    shader += "--";
                    break;
                }
                case ShaderParser::OperatorValueItem::PlusPlus:
                {
                    shader += "++";
                    break;
                }
                case ShaderParser::OperatorValueItem::Not:
                {
                    shader += '!';
                    break;
                }
                
                default:
                break;
            }
            if (value_item->value->type == ShaderParser::Operator) shader += '(';
            processValue(value_item->value, shader, last_level);
            if (value_item->value->type == ShaderParser::Operator) shader += ')';
            break;
        }
        case ShaderParser::Attribute: {
            processAttributeItem((ShaderParser::AttributeItem *)item, NULL, shader, true);
            break;
        }
        
        default:
            break;
    }
}

void GL2ShaderCompilerIMP::processOperatorItem(ShaderParser::OperatorItem *item, string &shader, int last_level) {
    int current_level = ShaderParser::OperatorItem::priorityLevel(item->operator_type);
    bool has_p = current_level<last_level;
    if (has_p) shader.push_back('(');
    for (size_t i = 0, t = item->argvs.size(); i < t; ++i) {
        ShaderParser::Item *sub_item = (ShaderParser::Item*)item->argvs[i];
        processValue(sub_item, shader, current_level);
        if (i < t - 1) {
            switch (item->operator_type) {
                case ShaderParser::OperatorItem::Equal:
                    shader.push_back('=');
                    break;
                case ShaderParser::OperatorItem::PlusEqual:
                    shader += "+=";
                    break;
                case ShaderParser::OperatorItem::MinusEqual:
                    shader += "-=";
                    break;
                case ShaderParser::OperatorItem::MultiEqual:
                    shader += "*=";
                    break;
                case ShaderParser::OperatorItem::DivisionEqual:
                    shader += "/=";
                    break;
                case ShaderParser::OperatorItem::Plus:
                    shader.push_back('+');
                    break;
                case ShaderParser::OperatorItem::Minus:
                    shader.push_back('-');
                    break;
                case ShaderParser::OperatorItem::Multi:
                    shader.push_back('*');
                    break;
                case ShaderParser::OperatorItem::Division:
                    shader.push_back('/');
                    break;
                case ShaderParser::OperatorItem::Dot:
                    shader.push_back('.');
                    break;
                case ShaderParser::OperatorItem::EqualEqual:
                    shader += "==";
                    break;
                case ShaderParser::OperatorItem::NotEqual:
                    shader += "!=";
                    break;
                case ShaderParser::OperatorItem::And:
                    shader.push_back('&');
                    break;
                case ShaderParser::OperatorItem::Or:
                    shader.push_back('|');
                    break;
                case ShaderParser::OperatorItem::AndEqual:
                    shader += "&=";
                    break;
                case ShaderParser::OperatorItem::OrEqual:
                    shader += "|=";
                    break;
                case ShaderParser::OperatorItem::Exclusive:
                    shader += '^';
                    break;
                case ShaderParser::OperatorItem::AndAnd:
                    shader += "&&";
                    break;
                case ShaderParser::OperatorItem::OrOr:
                    shader += "||";
                    break;
                case ShaderParser::OperatorItem::Greater:
                    shader += '>';
                    break;
                case ShaderParser::OperatorItem::Lesser:
                    shader += '<';
                    break;
                case ShaderParser::OperatorItem::GreaterEqual:
                    shader += ">=";
                    break;
                case ShaderParser::OperatorItem::LesserEqual:
                    shader += "<=";
                    break;
                case ShaderParser::OperatorItem::LeftShift:
                    shader += "<<";
                    break;
                case ShaderParser::OperatorItem::RightShift:
                    shader += ">>";
                    break;
                case ShaderParser::OperatorItem::Remainder:
                    shader += '%';
                    break;
                
                default:
                    break;
            }
        }
    }
    if (has_p) shader.push_back(')');
}

void GL2ShaderCompilerIMP::processBlockItems(const std::vector<void *> &items,
                                             const variant_map &compile_argvs,
                                             string &shader, int indent) {
#define BEGIN_LINE  {for (int i = 0; i < indent; ++i) { \
        shader.push_back('\t'); \
    }}
#define END_LINE shader += "\n";
    for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
        ShaderParser::Item *item = (ShaderParser::Item *)*it;
        switch (item->type) {
            case ShaderParser::Action:
            {
                ShaderParser::ActionItem *action_item = (ShaderParser::ActionItem*)item;
                BEGIN_LINE
                switch (action_item->action_type) {
                    case ShaderParser::ActionItem::Include:{
                        shader += "#include \"";
                        ShaderParser::ConstantItem *item = (ShaderParser::ConstantItem *)action_item->argvs[0];
                        shader += item->variant;
                        shader += '\"';
                        break;
                    }
                        
                    default:
                        break;
                }
                END_LINE
            }
                break;
            case ShaderParser::Attribute:
            {
                BEGIN_LINE
                ShaderParser::AttributeItem *attrbute_item = (ShaderParser::AttributeItem *)item;
                processAttributeItem(attrbute_item, &compile_argvs, shader, true);
                shader.push_back(';');
                
                END_LINE
                break;
            }
            case ShaderParser::Function: {
                BEGIN_LINE
                
                ShaderParser::FunctionItem *function_item = (ShaderParser::FunctionItem*)item;
                const char *var = processVariantType(function_item->ret_type);
                if (var) {
                    shader += var;
                }
                shader += ' ';
                shader += function_item->name == main_word ? main_name.str() : function_item->name.str();
                shader += '(';
                
                for (size_t i = 0, t = function_item->argvs.size();
                     i < t; ++i) {
                    ShaderParser::AttributeItem *attrbute_item = (ShaderParser::AttributeItem *)function_item->argvs[i];
                    processAttributeItem(attrbute_item, nullptr, shader, false);
                    if (i < t - 1) {
                        shader.push_back(',');
                    }
                }
                shader.push_back(')');
                shader.push_back('{');
                
                END_LINE
                
                processBlockItems(function_item->block->sub_items,
                                  compile_argvs,
                                  shader, indent + 1);
                
                BEGIN_LINE
                shader.push_back('}');
                END_LINE
                
                break;
            }
            case ShaderParser::Operator: {
                BEGIN_LINE
                ShaderParser::OperatorItem *operator_item = (ShaderParser::OperatorItem*)item;
                processOperatorItem(operator_item, shader);
                shader.push_back(';');
                
                END_LINE
                break;
            }
            case ShaderParser::FlowControl: {
                BEGIN_LINE
                ShaderParser::FlowControlItem *flow_item = (ShaderParser::FlowControlItem*)item;
                switch (flow_item->flow_type) {
                    case ShaderParser::FlowControlItem::If:
                    {
                    if_begin:
                        if (flow_item->controls.size() == 0) {
                            LOG(e, "Wrong if control.");
                            break;
                        }
                        shader += "if(";
                        processValue((ShaderParser::Item*)flow_item->controls[0], shader);
                        shader += ')';
                    block_begin:
                        shader += '{';
                        END_LINE
                        processBlockItems(flow_item->block->sub_items,
                                          compile_argvs,
                                          shader, indent + 1);
                        
                        BEGIN_LINE
                        shader.push_back('}');
                        if (flow_item->else_flow) {
                            shader += "else ";
                            if (flow_item->else_flow->controls.size() > 0) {
                                flow_item = flow_item->else_flow;
                                goto if_begin;
                            }else {
                                flow_item = flow_item->else_flow;
                                goto block_begin;
                            }
                        }
                        
                        break;
                    }
                    case ShaderParser::FlowControlItem::For: {
                        if (flow_item->controls.size() != 3) {
                            LOG(e, "Wrong for control.");
                            break;
                        }
                        shader += "for(";
                        processValue((ShaderParser::Item*)flow_item->controls[0], shader);
                        shader += ';';
                        processValue((ShaderParser::Item*)flow_item->controls[1], shader);
                        shader += ';';
                        processValue((ShaderParser::Item*)flow_item->controls[2], shader);
                        shader += "){";
                        END_LINE
                        processBlockItems(flow_item->block->sub_items,
                                          compile_argvs,
                                          shader, indent + 1);
                        
                        BEGIN_LINE
                        shader.push_back('}');
                        break;
                    }
                    case ShaderParser::FlowControlItem::While: {
                        if (flow_item->controls.size() != 1) {
                            LOG(e, "Wrong for control.");
                            break;
                        }
                        shader += "while(";
                        processValue((ShaderParser::Item*)flow_item->controls[0], shader);
                        shader += "){";
                        END_LINE
                        processBlockItems(flow_item->block->sub_items,
                                          compile_argvs,
                                          shader, indent + 1);
                        
                        BEGIN_LINE
                        shader.push_back('}');
                        break;
                    }
                    
                    default:
                        break;
                }
                END_LINE
                break;
            }
                
            default: {
                BEGIN_LINE
                processValue(item, shader);
                shader.push_back(';');
                END_LINE
            }
                break;
        }
    }
}

void GL2ShaderCompilerIMP::compile(const std::vector<void *> &items, const variant_map &compile_argvs, string &shader) {
    processBlockItems(items, compile_argvs, shader, 0);
}


