//
//  GL2Tokenizer.h
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_SHADER_TOKENIZER_H
#define HIGRAPHICS_SHADER_TOKENIZER_H

#include <vector>
#include "../render_define.h"

using namespace std;

namespace gr {
    CLASS_BEGIN_0_NV(ShaderTokenizer)

    public:
        enum TokenType {
            Unknown,
            Symbol,
            Variant,
            Word,
            String,
            Other,
            Operator,
            Number,
            Flow,

            End
        };

        enum SymbolType {
            Attribute,
            Varying,
            Uniform,
            Const,
            In,
            Out,
            Inout,
            Compile,

            Semicolon,
            Comma,
            ParentheseLeft,
            ParentheseRight,
            BracketLeft,
            BracketRight,
            BraceLeft,
            BraceRight,
            Pound,
        };

        enum VariantType {
            Vecter2,
            Vecter3,
            Vecter4,

            Void,
            Int,
            Long,
            Float,
            Double,

            Matrix2,
            Matrix3,
            Matrix4,
            Sampler
        };
    
        enum FlowType {
            If,
            For,
            While,
            Else
        };

        enum OperatorType {
            Equal,
            Plus,
            Minus,
            Multi,
            Division,
            Dot,
            And,
            Or,
            Greater,
            Lesser,
            Not,
        };
    
        struct Token {
            TokenType type;
            gcore::StringName value;
            std::string str;
            int line;
            union {
                SymbolType symbol;
                VariantType variant;
                OperatorType op;
                FlowType flow;
            };
        };

    private:
        pointer_vector tokens;
        int offset;
        void clear();

    public:
        ShaderTokenizer(){offset = 0;}
        ~ShaderTokenizer();

        void process(const char *shader);

        const Token *current();
        const Token *step();
        const Token *token(int off);
        void move(int off);
        void reset() {offset = 0;}
        bool hasNext();

//    GL2Tokenizer &operator++();
//    GL2Tokenizer operator++(int);
//    GL2Tokenizer &operator--();
//    GL2Tokenizer operator--(int);
//    GL2Tokenizer &operator+=(int off);
//    GL2Tokenizer &operator-=(int off);
//    GL2Tokenizer operator+(int off) const;
//    GL2Tokenizer operator-(int off) const;

    CLASS_END
}

#endif //HIGRAPHICS_GL2_TOKENIZER_H
