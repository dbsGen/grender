//
//  GL2Parser.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_SHADER_PARSE_H
#define HIGRAPHICS_SHADER_PARSE_H

#include <graphics/graphics_define.h>
#include <core/StringName.h>
#include <core/Variant.h>
#include "ShaderTokenizer.h"
#include "../render_define.h"

namespace hirender {
    CLASS_BEGIN_0_N(ShaderParser)

    public:
        enum ItemType {
            Attribute,
            Block,
            Action,
            Constant,
            Operator,
            Word,
            Call,
            Other,
            Index,
            Function,
            FlowControl,
            OperatorValue,
        };

        struct Item {
            ItemType type;

            virtual ~Item() {}
        };
        enum {
            Vector2 = 0,
            Vector3,
            Vector4,
            
            Void,
            Int,
            Long,
            Float,
            Double,

            Matrix2,
            Matrix3,
            Matrix4,
            Sampler,
        };

        typedef int VariantType;
        static const VariantType VARIANT_IDX = 16;
        _FORCE_INLINE_ static void setVariantCount(VariantType &type, int count) {
            type = (type & 0xffff) + ((count - 1) << VARIANT_IDX);
        }
        _FORCE_INLINE_ static int getVariantCount(VariantType type) {
            return (type >> VARIANT_IDX) + 1;
        }
        _FORCE_INLINE_ static VariantType getVariantType(VariantType type) {
            return type & 0xffff;
        }

        struct AttributeItem : public Item {
            
            enum AttributeType {
                Normal,
                Attr,
                Varying,
                Uniform,
                Compile,
            };
            
            enum Prefix {
                None,
                Const,
                In,
                Out,
                Inout,
            };
            AttributeType   attr_type;
            VariantType     var_type;
            Prefix          prefix;
            hicore::StringName name;
            Item            *def_value;
            hicore::StringName  var_string;

            _FORCE_INLINE_ AttributeItem() : prefix(None), def_value(NULL) {
                type = ItemType::Attribute;attr_type=Normal;
            }
            _FORCE_INLINE_ ~AttributeItem() {
                if (def_value) delete def_value;
            }
        };
    
        struct IndexItem : public Item {
            Item *item;
            Item *index_item;
            
            _FORCE_INLINE_ IndexItem() : item(NULL), index_item(NULL) {
                type = ItemType::Index;
            }
            _FORCE_INLINE_ ~IndexItem() {
                if (item) delete item;
                if (index_item) delete index_item;
            }
        };

        struct BlockItem : public Item {
            enum BlockType {
                Root,
                Main,
                Other
            };
            pointer_vector  sub_items;
            BlockType   block_type;

            void clear();
            _FORCE_INLINE_ BlockItem() {type = ItemType::Block;block_type=Other;}
            _FORCE_INLINE_ ~BlockItem() {clear();}
        };
    
        struct FlowControlItem : public Item {
            enum FlowType {
                If,
                For,
                While
            };
            FlowType flow_type;
            pointer_vector controls;
            BlockItem *block;
            FlowControlItem *else_flow;
        
            void clear();
            _FORCE_INLINE_ FlowControlItem() : block(new BlockItem), else_flow(NULL) {type = FlowControl;flow_type=If;}
            _FORCE_INLINE_ ~FlowControlItem() { clear(); }
        };
    
        struct WordItem : public Item {
            hicore::StringName value;
        
            _FORCE_INLINE_ WordItem() {type = Word;}
            _FORCE_INLINE_ WordItem(const hicore::StringName &value) : WordItem() {this->value = value;}
            _FORCE_INLINE_ ~WordItem() {}
        };
    
        struct ConstantItem : public Item {
            string variant;
            bool is_str;
            
            _FORCE_INLINE_ ConstantItem() {type = Constant;}
            _FORCE_INLINE_ ConstantItem(bool is_string, const string &v) : ConstantItem() {variant = v;is_str = is_string;}
            _FORCE_INLINE_ ~ConstantItem() {}
        };
    
        struct OperatorItem : public Item {
            enum OperatorType {
                Equal,
                PlusEqual,
                MinusEqual,
                MultiEqual,
                DivisionEqual,
                
                EqualEqual,
                NotEqual,
                Plus,
                Minus,
                Multi,
                Division,
                Dot,
                
                And,
                Or,
                AndEqual,
                OrEqual,
                Exclusive,
                AndAnd,
                OrOr,
                Greater,
                Lesser,
                GreaterEqual,
                LesserEqual,
                LeftShift,
                RightShift,
                Remainder,
            };
            OperatorType operator_type;
            pointer_vector argvs;
            
            void clear();
            static int priorityLevel(OperatorType op_t);
            _FORCE_INLINE_ OperatorItem() {type = Operator; operator_type = Equal;}
            _FORCE_INLINE_ ~OperatorItem() {clear();}
        };
    
        struct OperatorValueItem : public Item {
            enum OperatorType {
                Minus,
                PlusPlus,
                MinusMinus,
                Not
            };
            OperatorType operator_type;
            Item *value;
            _FORCE_INLINE_ OperatorValueItem(OperatorType ot) : value(NULL) {type = OperatorValue;operator_type=ot;}
            _FORCE_INLINE_ ~OperatorValueItem() {if (value) delete value;}
        };
    
        struct FunctionItem : public Item {
            VariantType ret_type;
            //AttributeItem*
            pointer_vector argvs;
            BlockItem *block;
            hicore::StringName name;
            
            void clear();
            _FORCE_INLINE_ FunctionItem() : block(NULL), ret_type(Void) {
                type = Function;
            }
            _FORCE_INLINE_ ~FunctionItem() {clear();}
        };

        struct ActionItem : public Item {
            enum ActionType {
                Include,
                Extend
            };
            ActionType      action_type;
            pointer_vector  argvs;

            void clear();
            _FORCE_INLINE_ ActionItem() {type = ItemType::Action; action_type = Include;}
            _FORCE_INLINE_ ~ActionItem() {clear();}
        };

        struct CallItem : public Item {
            hicore::StringName name;
            pointer_vector  argvs;
        
            void clear();
            _FORCE_INLINE_ CallItem() {type = ItemType::Call;}
            _FORCE_INLINE_ ~CallItem() {clear();}
        };
    
        struct Error {
            string resean;
            int line;

            _FORCE_INLINE_ Error(int l, const string &r) {line = l;resean = r;}
        };

    private:
        typedef int Status;

        ShaderTokenizer *tokenizer;
        BlockItem       root;
        pointer_list    block_stack;
        pointer_vector  errors;
        pointer_vector  attributes;
        pointer_vector  functions;
        pointer_vector  actions;
        bool            processed;
        Status          status;

        _FORCE_INLINE_ BlockItem *currentBlock() {
            return (BlockItem*)block_stack.back();
        }
    
        void printErrors();
        bool parseAction();
        bool parseVariant(AttributeItem::AttributeType type = AttributeItem::Normal);
        bool parseFunction();
        bool parseBlock(bool root);
        Item *parseException(bool root);
        AttributeItem *parseAttribute(int t = -1);
        FlowControlItem *parseIfControl();
        FlowControlItem *parseForControl();
        FlowControlItem *parseWhileControl();
    
        void clearErrors();

    public:
        ShaderParser() {
            processed=false;
            root.block_type = BlockItem::Root;
        }
        _FORCE_INLINE_ ~ShaderParser() {
            root.clear();
            clearErrors();
            delete tokenizer;
        }
        INITIALIZE(ShaderParser, const char *shader,
                   processed = false;
                           tokenizer = new ShaderTokenizer;
                           tokenizer->process(shader);
                   )
    
        void process();
        _FORCE_INLINE_ const pointer_vector &getAttributes() {
            if (!processed) process();
            return attributes;
        }
        _FORCE_INLINE_ const pointer_vector &getFunctions() {
            if (!processed) process();
            return functions;
        }
        _FORCE_INLINE_ const pointer_vector &getActions() {
            if (!processed) process();
            return actions;
        }

    CLASS_END
}

#endif //HIGRAPHICS_SHADER_PARSE_H
