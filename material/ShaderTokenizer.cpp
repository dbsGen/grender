//
//  GL2Tokenizer.c
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "ShaderTokenizer.h"
#include <core/StringName.h>
#include <cstring>

using namespace gc;

#define isLatin(ch) ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')

typedef enum {
    TOKEN,
    SYMBOL,
    WORD,
    STRING,
    NUMBER,
    END,
    WRONG,
    NEWLINE,
} GLSCheckType;

const StringName gls_tokens[] {
        "attribute",
        "varying",
        "uniform",
        "const",
        "in",
        "out",
        "inout",
        "compile",

#define BORDER_TYPE 8
        "void",
        "int",
        "long",
        "float",
        "double",

#define BORDER_VEC2 (BORDER_TYPE+5)
        "vec2", // 8
        "bvec2",
        "ivec2",
        "uvec2",

#define BORDER_VEC3 (BORDER_VEC2+4)
        "vec3", //12
        "bvec3",
        "ivec3",
        "uvec3",

#define BORDER_VEC4 (BORDER_VEC3+4)
        "vec4", // 16
        "bvec4",
        "ivec4",
        "uvec4",

#define BORDER_MAT2 (BORDER_VEC4+4)
        "mat2", //20
        "mat2x2",
        "dmat2x2",

#define BORDER_MAT3 (BORDER_MAT2+3)
        "mat2x3", //23
        "mat3",
        "mat3x2",
        "mat3x3",
        "dmat2x3",
        "dmat3x2",
        "dmat3x3",

#define BORDER_MAT4 (BORDER_MAT3+7)
        "mat2x4", //30
        "mat4",
        "mat3x4",
        "mat4x2",
        "mat4x3",
        "mat4x4",
        "dmat2x4",
        "dmat3x4",
        "dmat4x2",
        "dmat4x3",
        "dmat4x4",

#define BORDER_SAMPLER (BORDER_MAT4+11)
        "sampler1D", // 41
        "sampler2D",
        "sampler3D",
        "samplerExternalOES",
    
#define BORDER_FLOW (BORDER_SAMPLER+4)
        "if",
        "for",
        "while",
        "else",

#define BORDER_SKIP (BORDER_FLOW+4)
        "lowp", // 46
        "highp"
};

const char symbol[] = {
    ';',
    ',',
    '(',
    ')',
    '[',
    ']',
    '{',
    '}',
    '#',
    
    '=',//9
    '+',
    '-',
    '*',
    '/',
    
    '.',//14
    '&',
    '|',
    '>',
    '<',
    '!',
};

int shader_token_total = 0;
StringName current_shader_name;
string current_string;

#define isNumber(CH) (CH >= '0' && CH <= '9' || CH =='.')

GLSCheckType get_shader_token(const char *sch, size_t *skip, int *index, const char** start) {
    if (!shader_token_total) shader_token_total = sizeof(gls_tokens)/ sizeof(StringName);
    const char *ch = sch;
    if (*ch == 0) {
        return END;
    }
    // skip whitespcae
    while (*ch == ' ' || *ch == '\t') {
        *start = ++ch;
    }
    if (*ch == '\n') {
        *start = ++ch;
        *index = 1;
        return NEWLINE;
    }
    // skip //
    if (*ch == '/' && *(ch+1) == '/') {
        ch += 2;
        while (*ch != '\n') {
            if (*ch == 0) {
                return END;
            }
            ch ++;
        }
        *start = ++ch;
        *index = 1;
        return NEWLINE;
    }
    // skip /**/
    if (*ch == '/' && *(ch+1) == '*') {
        int lcount = 0;
        ch += 2;
        while (true) {
            if (*ch == 0) {
                return END;
            }else if (*ch == '\n') {
                lcount ++;
            } else if (*ch == '*' && *(ch+1) == '/') {
                break;
            }
            ++ch;
        }
        *start = ++ch;
        *index = lcount;
        return NEWLINE;
    }

    if (*ch == '"') {
        int lcount = 1;
        ++ch;
        bool is_trans = false;
        while (true) {
            if (is_trans) {
                is_trans = false;
            }else {
                switch (*ch) {
                    case '"': {
                        *skip = lcount + 1;
                        return STRING;
                    }
                    case '\\': {
                        is_trans = true;
                        break;
                    }
                    case '\n':
                    case 0: {
                        return WRONG;
                    }
                    default:
                        break;
                }
            }
            ++ch;++lcount;
        }
    }
    if (isLatin(*ch)) {
        int count = 0;
        string word;
        if (*ch >= '0' && *ch <= '9') {
            while (isNumber(*ch)) {
                word.push_back(*ch);
                ch ++;
                count++;
            }
            *skip = (size_t) count;
            current_string = word;
            return NUMBER;
        }
        else {
            while (isLatin(*ch)) {
                word.push_back(*ch);
                ch ++;
                count++;
            }
            StringName string_name(word.c_str());
            current_shader_name = string_name;
            *skip = (size_t) count;
            for (int i = 0; i < shader_token_total; i ++) {
                const StringName &gls = gls_tokens[i];
                if (gls == string_name) {
                    if (i >= BORDER_SKIP) {
                        return get_shader_token(ch, skip, index, start);
                    }else {
                        *index = i;
                        return TOKEN;
                    }
                }
            }
            return WORD;
        }
    }else if (*ch == 0) {
        return END;
    }else {
        const int len = sizeof(symbol)/sizeof(char);
        *skip = 1;
        for (int i = 0; i < len; i++) {
            if (symbol[i] == *ch) {
                *index = i;
                return SYMBOL;
            }
        }
        *index = -1;
        current_string = *ch;
        return SYMBOL;
    }
    return WRONG;
}

using namespace gr;

void ShaderTokenizer::process(const char *shader) {
    const char *chs = shader;
    const char *org = chs;
    int line = 0;
    
    while (*chs) {
        size_t skip = 0;
        int index = 0;
        GLSCheckType type = get_shader_token(chs, &skip, &index, &chs);
        if (type == WRONG) {
            continue;
        }else if (type == NEWLINE) {
            line += index;
            continue;
        }
        Token *token = new Token;
        token->line = line;
        
#define SetValue token->value = current_shader_name;
        
        token->value = current_shader_name;
        switch (type) {
            case END:
                token->type = End;
                break;
            case TOKEN:
                token->value = current_shader_name;
                if (index < BORDER_TYPE) {
                    token->type = Symbol;
                    switch (index) {
                        case 0:
                            token->symbol = Attribute;
                            break;
                        case 1:
                            token->symbol = Varying;
                            break;
                        case 2:
                            token->symbol = Uniform;
                            break;
                        case 3:
                            token->symbol = Const;
                            break;
                        case 4:
                            token->symbol = In;
                            break;
                        case 5:
                            token->symbol = Out;
                            break;
                        case 6:
                            token->symbol = Inout;
                            break;
                        case 7:
                            token->symbol = Compile;
                            break;
                        
                        default:
                            break;
                    }
                }else if (index < BORDER_VEC2) {
                    token->type = Variant;
                    switch (index) {
                        case (BORDER_TYPE):
                            token->variant = Void;
                            break;
                        case (BORDER_TYPE+1):
                            token->variant = Int;
                            break;
                        case (BORDER_TYPE+2):
                            token->variant = Long;
                            break;
                        case (BORDER_TYPE+3):
                            token->variant = Float;
                            break;
                        case (BORDER_TYPE+4):
                            token->variant = Double;
                            break;
                            
                        default:
                            break;
                    }
                }else if (index < BORDER_VEC3) {
                    token->type = Variant;
                    token->variant = Vecter2;
                }else if (index < BORDER_VEC4) {
                    token->type = Variant;
                    token->variant = Vecter3;
                }else if (index < BORDER_MAT2) {
                    token->type = Variant;
                    token->variant = Vecter4;
                }else if (index < BORDER_MAT3) {
                    token->type = Variant;
                    token->variant = Matrix2;
                }else if (index < BORDER_MAT4) {
                    token->type = Variant;
                    token->variant = Matrix3;
                }else if (index < BORDER_SAMPLER) {
                    token->type = Variant;
                    token->variant = Matrix4;
                }else if (index < BORDER_FLOW) {
                    token->type = Variant;
                    token->variant = Sampler;
                }else if (index < BORDER_SKIP) {
                    token->type = Flow;
                    switch (index) {
                        case BORDER_FLOW: {
                            token->flow = If;
                            break;
                        }
                        case (BORDER_FLOW + 1): {
                            token->flow = For;
                            break;
                        }
                        case (BORDER_FLOW + 2): {
                            token->flow = While;
                            break;
                        }
                        case (BORDER_FLOW + 3): {
                            token->flow = Else;
                            break;
                        }
                        
                        default:
                            break;
                    }
                }else {
                    token->type = Other;
                    SetValue;
                }
                break;
            case SYMBOL:
                token->type = Symbol;
                switch (index) {
                    case 0:
                        token->symbol = Semicolon;
                        break;
                    case 1:
                        token->symbol = Comma;
                        break;
                    case 2:
                        token->symbol = ParentheseLeft;
                        break;
                    case 3:
                        token->symbol = ParentheseRight;
                        break;
                    case 4:
                        token->symbol = BracketLeft;
                        break;
                    case 5:
                        token->symbol = BracketRight;
                        break;
                    case 6:
                        token->symbol = BraceLeft;
                        break;
                    case 7:
                        token->symbol = BraceRight;
                        break;
                    case 8:
                        token->symbol = Pound;
                        break;
                    case 9:
                        token->type = Operator;
                        token->op = Equal;
                        break;
                    case 10:
                        token->type = Operator;
                        token->op = Plus;
                        break;
                    case 11:
                        token->type = Operator;
                        token->op = Minus;
                        break;
                    case 12:
                        token->type = Operator;
                        token->op = Multi;
                        break;
                    case 13:
                        token->type = Operator;
                        token->op = Division;
                        break;
                    case 14:
                        token->type = Operator;
                        token->op = Dot;
                        break;
                    case 15:
                        token->type = Operator;
                        token->op = And;
                        break;
                    case 16:
                        token->type = Operator;
                        token->op = Or;
                        break;
                    case 17:
                        token->type = Operator;
                        token->op = Greater;
                        break;
                    case 18:
                        token->type = Operator;
                        token->op = Lesser;
                        break;
                    case 19:
                        token->type = Operator;
                        token->op = Not;
                        break;
                    
                    default:
                        token->type = Unknown;
                        token->str = current_string;
                        break;
                }
                break;
            case WORD:
                token->type = Word;
                SetValue;
                break;
            case NUMBER:
                token->type = Number;
                token->str = current_string;
                break;
            case STRING:
            {
                token->type = String;
                size_t len = skip - 2;
                token->str.resize(len);
                memcpy(&token->str[0], chs + 1, len);
                break;
            }
                
            default:
                break;
        }
        tokens.push_back(token);
        if (token->type == End) return;
        chs += skip;
    }
    Token *token = new Token;
    token->line = line;
    token->type = End;
    tokens.push_back(token);
}

ShaderTokenizer::~ShaderTokenizer() {
    clear();
}

void ShaderTokenizer::clear() {
    for (auto ite = tokens.begin(), _e = tokens.end(); ite != _e; ++ite) {
        delete (ShaderTokenizer::Token*)*ite;
    }
    tokens.clear();
}


const ShaderTokenizer::Token *ShaderTokenizer::current() {
    if (offset >= 0 && offset < tokens.size()) {
        return (ShaderTokenizer::Token *)tokens[offset];
    }
    return NULL;
}
const ShaderTokenizer::Token *ShaderTokenizer::step() {
    if (offset < tokens.size() - 1) 
        offset++;
    return current();
}
const ShaderTokenizer::Token* ShaderTokenizer::token(int off) {
    int o = offset + off;
    if (o >= 0 && o < tokens.size()) {
        return (ShaderTokenizer::Token *)tokens[o];
    }
    return NULL;
}

void ShaderTokenizer::move(int off) {
    size_t t = tokens.size();
    if (t > 0) {
        offset = offset + off;
        if (offset >= t) {
            offset = (int)t-1;
        }
    }
}

bool ShaderTokenizer::hasNext() {
    return offset < tokens.size() - 1;
}
