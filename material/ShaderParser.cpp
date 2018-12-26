//
//  GL2Parser.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "ShaderParser.h"

using namespace gr;


const gc::StringName main_key("main");
const gc::StringName key_words[]{
        "include",
        "extend"
};

#define _ERR(MSG) {errors.push_back(new Error(token->line, MSG));return false;}
#define _ERR_SPACE(MSG) {errors.push_back(new Error(token->line, MSG));return NULL;}
#define ERR {printErrors();return;}

void ShaderParser::printErrors() {
    for (auto it = errors.begin(), _e = errors.end(); it != _e; ++it) {
        Error *err = (Error*)*it;
        LOG(e, "Shader error: %s (%d)", err->resean.c_str(), err->line);
    }
}

void ShaderParser::process() {
    clearErrors();
    root.clear();
    block_stack.clear();
    block_stack.push_back(&root);
    tokenizer->reset();
    
    if (!parseBlock(true)) {
        ERR
    }
    
    attributes.clear();
    functions.clear();
    actions.clear();
    for (auto it = root.sub_items.begin(), _e = root.sub_items.end(); it != _e; ++it) {
        ShaderParser::Item *item = (ShaderParser::Item*)*it;
        switch (item->type) {
            case ShaderParser::Attribute:
                attributes.push_back(item);
                break;
            case ShaderParser::Function: {
                functions.push_back(item);
            }
                break;
            case ShaderParser::Action: {
                actions.push_back(item);
            }
                break;
                
            default:
                break;
        }
    }
    
    processed = true;
}

void ShaderParser::clearErrors() {
    for (auto it = errors.begin(), _e = errors.end(); it != _e; ++it) {
        delete (ShaderParser::Error*)*it;
    }
    errors.clear();
}

bool ShaderParser::parseAction() {
    const ShaderTokenizer::Token *token = tokenizer->current();
    if (token->type == ShaderTokenizer::Word) {
        if (token->value == key_words[0]) {
            token = tokenizer->step();
            if (token->type == ShaderTokenizer::String) {
                ActionItem *action = new ActionItem;
                action->argvs.push_back(new ConstantItem(true, token->str));
                currentBlock()->sub_items.push_back(action);
                return true;
            }else {
                _ERR("#include params error.");
            }
        }else if (token->value == key_words[1]) {
            token = tokenizer->step();
            if (token->type == ShaderTokenizer::String) {
                ActionItem *action = new ActionItem;
                action->action_type = ActionItem::Extend;
                action->argvs.push_back(new ConstantItem(true, token->str));
                currentBlock()->sub_items.push_back(action);
                return true;
            }else {
                _ERR("#include params error.");
            }
        }
    }else {
        _ERR("Must have a word behind #.");
    }
    _ERR("Parse action failed.");
}

bool ShaderParser::parseBlock(bool root) {
    const ShaderTokenizer::Token *token = tokenizer->current();
    if (!token) {
        _ERR("Error in get nil token.");
    }
    if (!root) {
        if (token->type == ShaderTokenizer::Symbol &&
            token->symbol == ShaderTokenizer::BraceLeft) {
            token = tokenizer->step();
        }else {
            _ERR("Block must start with '{'.");
        }
    }
    while (token->type != ShaderTokenizer::End) {
        switch (token->type) {
            case ShaderTokenizer::Symbol:
            {
                switch (token->symbol) {
                    case ShaderTokenizer::Attribute:
                    {
                        tokenizer->step();
                        if (!parseVariant(AttributeItem::Attr)) return false;
                    }
                        break;
                    case ShaderTokenizer::Varying:
                    {
                        tokenizer->step();
                        if (!parseVariant(AttributeItem::Varying)) return false;
                    }
                        break;
                    case ShaderTokenizer::Uniform:
                    {
                        tokenizer->step();
                        if (!parseVariant(AttributeItem::Uniform)) return false;
                    }
                        break;
                    case ShaderTokenizer::Compile:
                    {
                        tokenizer->step();
                        if (!parseVariant(AttributeItem::Compile)) return false;
                    }
                        break;
                    case ShaderTokenizer::Const:
                    {
                        if (!parseVariant(AttributeItem::Normal)) return false;
                    }
                        break;
                    case ShaderTokenizer::Pound: {
                        const ShaderTokenizer::Token* nt = tokenizer->token(1);
                        if (nt->type == ShaderTokenizer::Word) {
                            tokenizer->step();
                            if (!parseAction()) return false;
                        }
                    }
                        break;
                    case ShaderTokenizer::BraceRight: {
                        if (root) {
                            _ERR("Error char '}'");
                        }else {
                            return true;
                        }
                    }
                        break;
                    case ShaderTokenizer::BraceLeft: {
                        _ERR("Un carched block.");
                    }
                        break;
                        
                    default:
                        break;
                }
            }
                break;
                
            case ShaderTokenizer::Variant: {
                const ShaderTokenizer::Token *t1 = tokenizer->token(1);
                const ShaderTokenizer::Token *t2 = tokenizer->token(2);
                
                if (t1->type == ShaderTokenizer::Word &&
                    t2->type == ShaderTokenizer::Symbol &&
                    t2->symbol == ShaderTokenizer::ParentheseLeft) {
                    if (root) {
                        if (!parseFunction()) return false;
                    }else {
                        _ERR("Function must define in root block.");
                    }
                }else {
                    if (!parseVariant(AttributeItem::Normal)) return false;
                }
            }
                break;
                
            case ShaderTokenizer::Word: {
                Item *item = parseException(true);
                if (item) {
                    currentBlock()->sub_items.push_back(item);
                }else {
                    _ERR("Parse failed.");
                }
            }
                break;
            case ShaderTokenizer::Flow: {
                switch (token->flow) {
                    case ShaderTokenizer::If:
                    {
                        FlowControlItem *item = parseIfControl();
                        if (item) {
                            currentBlock()->sub_items.push_back(item);
                        }else {
                            _ERR("Parse failed.");
                        }
                        break;
                    }
                    case ShaderTokenizer::For:
                    {
                        FlowControlItem *item = parseForControl();
                        if (item) {
                            currentBlock()->sub_items.push_back(item);
                        }else {
                            _ERR("Parse failed.");
                        }
                        break;
                    }
                    case ShaderTokenizer::While:
                    {
                        FlowControlItem *item = parseWhileControl();
                        if (item) {
                            currentBlock()->sub_items.push_back(item);
                        }else {
                            _ERR("Parse failed.");
                        }
                        break;
                    }
                    
                    default:
                    break;
                }
            }
                break;
                
            default:
                
                break;
        }
        token = tokenizer->step();
    }
    if (!root) _ERR("Unkown error.");
    return true;
}

ShaderParser::FlowControlItem *ShaderParser::parseIfControl() {
    const ShaderTokenizer::Token *token = tokenizer->current();
    if (token->type == ShaderTokenizer::Flow &&
        token->flow == ShaderTokenizer::If) {
        FlowControlItem *item = new FlowControlItem;
        item->flow_type = FlowControlItem::If;
        const ShaderTokenizer::Token *token1 = tokenizer->token(1);
        if (token1->type == ShaderTokenizer::Symbol &&
            token1->symbol == ShaderTokenizer::ParentheseLeft) {
            tokenizer->move(2);
            ShaderParser::Item *control_item = parseException(true);
            if (!control_item) {
                delete item;
                _ERR_SPACE("Can not parse if flow control.[3]");
            }
            item->controls.push_back(control_item);
            token1 = tokenizer->current();
            const ShaderTokenizer::Token *token2 = tokenizer->token(1);
            if (token1->type == ShaderTokenizer::Symbol &&
                token1->symbol == ShaderTokenizer::ParentheseRight) {
                tokenizer->move(1);
                block_stack.push_back(item->block);
                if (token2->type == ShaderTokenizer::Symbol &&
                    token2->symbol == ShaderTokenizer::BraceLeft) {
                    parseBlock(false);
                }else {
                    Item *ex = parseException(true);
                    if (ex) {
                        currentBlock()->sub_items.push_back(ex);
                    }else {
                        delete item;
                        _ERR_SPACE("Can not parse if flow control.[7]");
                    }
                }
                block_stack.pop_back();
                token1 = tokenizer->token(1);
                if (token1->type == ShaderTokenizer::Flow &&
                    token1->flow == ShaderTokenizer::Else) {
                    token1 = tokenizer->token(2);
                    if (token1->type == ShaderTokenizer::Flow &&
                        token1->flow == ShaderTokenizer::If) {
                        tokenizer->move(2);
                        FlowControlItem *else_item = parseIfControl();
                        if (else_item) {
                            item->else_flow = else_item;
                        }else {
                            delete item;
                            _ERR_SPACE("Can not parse else flow control.[1]");
                        }
                    }else {
                        tokenizer->move(2);
                        FlowControlItem *else_item = new FlowControlItem;
                        item->flow_type = FlowControlItem::If;
                        block_stack.push_back(else_item->block);
                        if (token1->type == ShaderTokenizer::Symbol &&
                            token1->symbol == ShaderTokenizer::BraceLeft)
                            parseBlock(false);
                        else {
                            Item *ex = parseException(true);
                            if (ex) {
                                currentBlock()->sub_items.push_back(ex);
                            }else {
                                delete item;
                                _ERR_SPACE("Can not parse if flow control.[7]");
                            }
                        }
                        block_stack.pop_back();
                        item->else_flow = else_item;
                    }
                }
            }else {
                delete item;
                _ERR_SPACE("Can not parse if flow control.[4]");
            }
        }else {
            delete item;
            _ERR_SPACE("Can not parse if flow control.[5]");
        }
        return item;
    }
    return NULL;
}

ShaderParser::FlowControlItem *ShaderParser::parseForControl() {
    const ShaderTokenizer::Token *token = tokenizer->current();
    const ShaderTokenizer::Token *tmp = tokenizer->token(1);
    if (token->type == ShaderTokenizer::Flow &&
        token->flow == ShaderTokenizer::For &&
        tmp->type == ShaderTokenizer::Symbol &&
        tmp->symbol == ShaderTokenizer::ParentheseLeft) {
        FlowControlItem *item = new FlowControlItem;
        item->flow_type = FlowControlItem::For;
        tokenizer->move(2);
        for (int i = 0; i < 3; ++i) {
            Item *citem = parseException(true);
            if (!citem) {
                delete item;
                _ERR_SPACE("Failed to parse exp in for control.");
            }
            item->controls.push_back(citem);
            token = tokenizer->current();
            if (i < 2) {
                if (token->type != ShaderTokenizer::Symbol ||
                    token->symbol != ShaderTokenizer::Semicolon) {
                    delete item;
                    _ERR_SPACE("Failed to parse exp in for control.");
                }
                tokenizer->step();
            }else {
                if (token->type != ShaderTokenizer::Symbol ||
                    token->symbol != ShaderTokenizer::ParentheseRight) {
                    delete item;
                    _ERR_SPACE("Failed to parse exp in for control.");
                }
                tokenizer->step();
            }
        }
        token = tokenizer->current();
        if (token->type == ShaderTokenizer::Symbol &&
            token->symbol == ShaderTokenizer::BraceLeft) {
            block_stack.push_back(item->block);
            parseBlock(false);
            block_stack.pop_back();
        }
        return item;
    }else {
        _ERR_SPACE("This is not a for control.");
    }
}

ShaderParser::FlowControlItem *ShaderParser::parseWhileControl() {
    const ShaderTokenizer::Token *token = tokenizer->current();
    const ShaderTokenizer::Token *tmp = tokenizer->token(1);
    if (token->type == ShaderTokenizer::Flow &&
        token->flow == ShaderTokenizer::While &&
        tmp->type == ShaderTokenizer::Symbol &&
        tmp->symbol == ShaderTokenizer::ParentheseLeft) {
        FlowControlItem *item = new FlowControlItem;
        item->flow_type = FlowControlItem::While;
        tokenizer->move(2);
        
        Item *citem = parseException(true);
        if (!citem) {
            delete item;
            _ERR_SPACE("Failed to parse exp in for control.");
        }
        item->controls.push_back(citem);
        token = tokenizer->current();
        if (token->type != ShaderTokenizer::Symbol ||
            token->symbol != ShaderTokenizer::ParentheseRight) {
            delete item;
            _ERR_SPACE("Failed to parse exp in for control.");
        }
        
        token = tokenizer->step();
        if (token->type == ShaderTokenizer::Symbol &&
            token->symbol == ShaderTokenizer::BraceLeft) {
            block_stack.push_back(item->block);
            parseBlock(false);
            block_stack.pop_back();
        }
        return item;
    }else {
        _ERR_SPACE("This is not a for control.");
    }
}

ShaderParser::AttributeItem *ShaderParser::parseAttribute(int t) {
    const ShaderTokenizer::Token *token = tokenizer->current();
    AttributeItem::Prefix p = AttributeItem::None;
    if (t < 0) {
        if (token->type == ShaderTokenizer::Symbol) {
            switch (token->symbol) {
                case ShaderTokenizer::In: {
                    p = AttributeItem::In;
                    break;
                }
                case ShaderTokenizer::Out: {
                    p = AttributeItem::Out;
                    break;
                }
                case ShaderTokenizer::Inout: {
                    p = AttributeItem::Inout;
                    break;
                }
                case ShaderTokenizer::Const: {
                    p = AttributeItem::Const;
                    break;
                }
                    
                default:
                {
                    errors.push_back(new Error(token->line, "Unknown token type."));
                    return NULL;
                }
                    break;
            }
            token = tokenizer->step();
        }
        if (token->type != ShaderTokenizer::Variant) {
            errors.push_back(new Error(token->line, "Can not parse a attribute."));
            return NULL;
        }
    }
    AttributeItem *attr = new AttributeItem;
    attr->prefix = p;
    if (t >= 0) {
        attr->var_type = t;
    }else {
        attr->var_type = token->variant;
        attr->var_string = token->value;
        token = tokenizer->step();
    }
    if (token->type == ShaderTokenizer::Word) {
        attr->name = token->value;
        const ShaderTokenizer::Token *t1 = tokenizer->token(1);
        if (t1->type == ShaderTokenizer::Symbol &&
            t1->symbol == ShaderTokenizer::BracketLeft) {
            const ShaderTokenizer::Token *t2 = tokenizer->token(2);
            const ShaderTokenizer::Token *t3 = tokenizer->token(3);
            
            if (t2->type == ShaderTokenizer::Number) {
                int num = atoi(t2->str.c_str());
                if (num && t3->type == ShaderTokenizer::Symbol &&
                    t3->symbol == ShaderTokenizer::BraceRight) {
                    setVariantCount(attr->var_type, num);
                    tokenizer->move(3);
                }
            }
        }
        token = tokenizer->token(1);
        if (token->type == ShaderTokenizer::Operator) {
            if (token->op == ShaderTokenizer::Equal) {
                tokenizer->move(2);
                Item *dv = parseException(true);
                if (dv) {
                    attr->def_value = dv;
                }else {
                    _ERR_SPACE("Must set default value after equal.");
                }
            }else {
                errors.push_back(new Error(token->line, "Wrong operator."));
                return NULL;
            }
        }
    }else {
        tokenizer->move(-1);
    }
    return attr;
}

void replacePlaceholder(ShaderParser::OperatorItem *item, const pointer_map &phs) {
    for (auto it = item->argvs.begin(), _e = item->argvs.end(); it != _e; ++it) {
        ShaderParser::Item *subitem = (ShaderParser::Item*)*it;
        if (subitem->type == ShaderParser::Operator) {
            replacePlaceholder((ShaderParser::OperatorItem *)subitem, phs);
        }else if (subitem->type == ShaderParser::Other) {
            auto fit = phs.find(subitem);
            if (fit != phs.end()) {
                *it = fit->second;
            }
        }
    }
}

ShaderParser::Item *ShaderParser::parseException(bool root) {
    const ShaderTokenizer::Token *token = tokenizer->current();
    if (!root) {
        if (token->type == ShaderTokenizer::Symbol &&
            token->symbol == ShaderTokenizer::ParentheseLeft) {
            token = tokenizer->step();
        }else {
            _ERR_SPACE("Can not parse exception (1)");
        }
    }
    Item *r = NULL;
    OperatorItem *last_op = NULL;
    enum ReceiveType {
        Value,
        OperatorOrEnd,
        End
    };
    ReceiveType r_op = Value;
    struct PlaceholderItem : public Item {
        _FORCE_INLINE_ PlaceholderItem() {
            type = Other;
        }
    };
    
    pointer_map placeholdder_map;
    
    while (true) {
        if (!tokenizer->hasNext()) {
            if (r)  delete r;
            _ERR_SPACE("Bad end.");
        }
        switch (r_op) {
            case Value:
            {
                Item *value = NULL;
                switch (token->type) {
                    case ShaderTokenizer::Variant: {
                        const ShaderTokenizer::Token *nt = tokenizer->token(1);
                        if (nt->type != ShaderTokenizer::Symbol ||
                            nt->symbol != ShaderTokenizer::ParentheseLeft) {
                            ShaderParser::AttributeItem *attr = parseAttribute();
                            tokenizer->move(-1);
                            if (!attr) {
                                if (r) delete r;
                                _ERR_SPACE("failed to parse exception.");
                            }
                            value = attr;
                            break;
                        }
                    }
                    case ShaderTokenizer::Word: {
                        const ShaderTokenizer::Token *nt = tokenizer->token(1);
                        if (nt->type == ShaderTokenizer::Symbol &&
                            nt->symbol == ShaderTokenizer::ParentheseLeft) {
                            tokenizer->move(2);
                            CallItem *call_item = new CallItem;
                            call_item->name = token->value;
                            token = tokenizer->current();
                            value = call_item;
                            while (true) {
                                if (token->type == ShaderTokenizer::Symbol &&
                                    token->symbol == ShaderTokenizer::ParentheseRight) {
                                    break;
                                }
                                Item *subitem = parseException(true);
                                if (subitem) {
                                    if (subitem->type == Operator) {
                                        if (((OperatorItem*)subitem)->operator_type <= OperatorItem::DivisionEqual) {
                                            _ERR_SPACE("Can not insert equal exception.");
                                        }
                                    }
                                    token = tokenizer->current();
                                    if (token->type == ShaderTokenizer::Symbol &&
                                        (token->symbol == ShaderTokenizer::Comma ||
                                         token->symbol == ShaderTokenizer::ParentheseRight)) {
                                            if (token->symbol == ShaderTokenizer::Comma) {
                                                token = tokenizer->step();
                                                call_item->argvs.push_back(subitem);
                                            }else {
                                                call_item->argvs.push_back(subitem);
                                                break;
                                            }
                                    }else {
                                        if (value) delete value;
                                        if (r) delete r;
                                        _ERR_SPACE("Parse exception error (1).");
                                    }
                                }else {
                                    if (value) delete value;
                                    if (r) delete r;
                                    _ERR_SPACE("Parse exception error (2).");
                                }
                            }
                        }else {
                            value = new WordItem(token->value);
                        }
                        break;
                    }
                    case ShaderTokenizer::String: {
                        value = new ConstantItem(true, token->str);
                        break;
                    }
                    case ShaderTokenizer::Number: {
                        value = new ConstantItem(false, token->str);
                        break;
                    }
                    case ShaderTokenizer::Symbol: {
                        if (token->symbol == ShaderTokenizer::ParentheseLeft) {
                            Item *it = parseException(false);
                            if (it) {
                                value = new PlaceholderItem;
                                placeholdder_map[value] = it;
                            }else {
                                if (r)  delete r;
                                return NULL;
                            }
                        }else {
                            if (r)  delete r;
                            _ERR_SPACE("Can not parse exception (2)");
                        }
                        break;
                    }
                    case ShaderTokenizer::Operator: {
                        switch (token->op) {
                            case ShaderTokenizer::Minus: {
                                const ShaderTokenizer::Token *nt = tokenizer->token(1);
                                if (nt->type == ShaderTokenizer::Number) {
                                    string m("-");
                                    value = new ConstantItem(false, m + nt->str);
                                    tokenizer->step();
                                    break;
                                }else if (nt->type == ShaderTokenizer::Operator &&
                                          nt->op == ShaderTokenizer::Minus) {
                                    OperatorValueItem *item = new OperatorValueItem(OperatorValueItem::MinusMinus);
                                    tokenizer->move(2);
                                    Item *ex_item = parseException(true);
                                    tokenizer->move(-1);
                                    if (!ex_item) {
                                        if (r)  delete r;
                                        _ERR_SPACE("Can not parse exception (12)");
                                    }
                                    item->value = ex_item;
                                    value = item;
                                }else {
                                    OperatorValueItem *item = new OperatorValueItem(OperatorValueItem::Minus);
                                    tokenizer->move(1);
                                    Item *ex_item = parseException(true);
                                    tokenizer->move(-1);
                                    if (!ex_item) {
                                        if (r)  delete r;
                                        _ERR_SPACE("Can not parse exception (13)");
                                    }
                                    item->value = ex_item;
                                    value = item;
                                }
                            }
                                break;
                            case ShaderTokenizer::Plus: {
                                const ShaderTokenizer::Token *nt = tokenizer->token(1);
                                if (nt->type == ShaderTokenizer::Operator &&
                                    nt->op == ShaderTokenizer::Plus) {
                                    OperatorValueItem *item = new OperatorValueItem(OperatorValueItem::PlusPlus);
                                    tokenizer->move(2);
                                    Item *ex_item = parseException(true);
                                    tokenizer->move(-1);
                                    if (!ex_item) {
                                        if (r)  delete r;
                                        _ERR_SPACE("Can not parse exception (14)");
                                    }
                                    item->value = ex_item;
                                    value = item;
                                }else {
                                    _ERR_SPACE("Can not parse exception (15)");
                                }
                            }
                                break;
                            case ShaderTokenizer::Not: {
                                OperatorValueItem *item = new OperatorValueItem(OperatorValueItem::Not);
                                tokenizer->move(1);
                                Item *ex_item = parseException(true);
                                tokenizer->move(-1);
                                if (!ex_item) {
                                    if (r)  delete r;
                                    _ERR_SPACE("Can not parse exception (16)");
                                }
                                item->value = ex_item;
                                value = item;
                            }
                                break;
                            default: {
                                if (r)  delete r;
                                _ERR_SPACE("Can not parse exception (10)");
                                break;
                            }
                        }
                    }
                        break;
                        
                    default:
                        if (r)  delete r;
                        _ERR_SPACE("Can not parse exception (3)");
                        break;
                }
                if (value) {
                    token = tokenizer->token(1);
                    if (token->type == ShaderTokenizer::Symbol &&
                        token->symbol == ShaderTokenizer::BracketLeft) {
                        tokenizer->move(2);
                        IndexItem *index_item = new IndexItem;
                        index_item->item = value;
                        index_item->index_item = parseException(true);
                        //tokenizer->move(-1);
                        token = tokenizer->current();
                        if (token->type != ShaderTokenizer::Symbol ||
                            token->symbol != ShaderTokenizer::BracketRight) {
                            _ERR_SPACE("Not found ']'.");
                        }else {
                            value = index_item;
                        }
                    }
                }
                if (r) {
                    if (last_op) {
                        last_op->argvs.push_back(value);
                    }else {
                        if (value) delete value;
                        if (r)  delete r;
                        _ERR_SPACE("Can not parse exception (4)");
                    }
                }else {
                    r = value;
                }
                token = tokenizer->step();
                r_op = OperatorOrEnd;
            }
                break;
            case OperatorOrEnd:
            {
                if (token->type == ShaderTokenizer::Operator) {
                    OperatorItem *op_item = new OperatorItem;
                    switch (token->op) {
                        case ShaderTokenizer::Equal: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::EqualEqual;
                            }else {
                                op_item->operator_type = OperatorItem::Equal;
                            }
                        }
                            break;
                        case ShaderTokenizer::Plus: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::PlusEqual;
                            }else {
                                op_item->operator_type = OperatorItem::Plus;
                            }
                        }
                            break;
                        case ShaderTokenizer::Minus: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::MinusEqual;
                            }else {
                                op_item->operator_type = OperatorItem::Minus;
                            }
                        }
                            break;
                        case ShaderTokenizer::Multi: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::MultiEqual;
                            }else {
                                op_item->operator_type = OperatorItem::Multi;
                            }
                        }
                            break;
                        case ShaderTokenizer::Division: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::DivisionEqual;
                            }else {
                                op_item->operator_type = OperatorItem::Division;
                            }
                        }
                            break;
                        case ShaderTokenizer::Dot: {
                            op_item->operator_type = OperatorItem::Dot;
                        }
                            break;
                        case ShaderTokenizer::And: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::AndEqual;
                            }else if (nt->type == ShaderTokenizer::Operator &&
                                      nt->op == ShaderTokenizer::And) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::AndAnd;
                            }else {
                                op_item->operator_type = OperatorItem::And;
                            }
                        }
                            break;
                        case ShaderTokenizer::Or: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::OrEqual;
                            }else if (nt->type == ShaderTokenizer::Operator &&
                                      nt->op == ShaderTokenizer::Or) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::OrOr;
                            }else {
                                op_item->operator_type = OperatorItem::Or;
                            }
                        }
                            break;
                        case ShaderTokenizer::Greater: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::GreaterEqual;
                            }else if (nt->type == ShaderTokenizer::Operator &&
                                      nt->op == ShaderTokenizer::Greater){
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::RightShift;
                            }else {
                                op_item->operator_type = OperatorItem::Greater;
                            }
                        }
                            break;
                        case ShaderTokenizer::Lesser: {
                            auto nt = tokenizer->token(1);
                            if (nt->type == ShaderTokenizer::Operator &&
                                nt->op == ShaderTokenizer::Equal) {
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::LesserEqual;
                            }else if (nt->type == ShaderTokenizer::Operator &&
                                      nt->op == ShaderTokenizer::Lesser){
                                token = tokenizer->step();
                                op_item->operator_type = OperatorItem::LeftShift;
                            }else {
                                op_item->operator_type = OperatorItem::Lesser;
                            }
                        }
                            break;
                        
                        default: {
                            if (op_item) delete op_item;
                            if (r)  delete r;
                            _ERR_SPACE("Can not parse exception (5)");
                        }
                            break;
                    }
                    if (last_op) {
                        // compare the priority level
                        last_op = op_item;
                        Item *will_insert_to = r;
                        OperatorItem *parent_item = NULL;
                        int current_level = OperatorItem::priorityLevel(op_item->operator_type);
                        while (will_insert_to->type == Operator) {
                            OperatorItem* op = (OperatorItem*)will_insert_to;
                            int last_level = OperatorItem::priorityLevel(op->operator_type);
                            if (last_level > current_level) {
                                break;
                            }
                            parent_item = op;
                            will_insert_to = (Item*)op->argvs.back();
                        }
                        if (parent_item) {
                            parent_item->argvs.back() = op_item;
                        }else {
                            r = op_item;
                        }
                        op_item->argvs.push_back(will_insert_to);
                    }else if (r->type == Word ||
                              r->type == Constant ||
                              r->type == Other ||
                              r->type == Call) {
                        if (op_item->operator_type <= OperatorItem::DivisionEqual) {
                            if (r->type == Word) {
                                op_item->argvs.push_back(r);
                                r = op_item;
                                last_op = op_item;
                            }else {
                                if (op_item) delete op_item;
                                if (r)  delete r;
                                _ERR_SPACE("Can not parse exception (6)");
                            }
                        }else {
                            op_item->argvs.push_back(r);
                            r = op_item;
                            last_op = op_item;
                        }
                    }else {
                        if (r)  delete r;
                        _ERR_SPACE("Can not parse exception (7)");
                    }
                    token = tokenizer->step();
                    r_op = Value;
                }else if (token->type == ShaderTokenizer::Symbol) {
                    if (root) {
                        goto loop_end;
                    }else if (token->symbol == ShaderTokenizer::ParentheseRight) {
                        goto loop_end;
                    }else {
                        if (r)  delete r;
                        _ERR_SPACE("Can not parse exception (8)");
                    }
                }else {
                    if (r)  delete r;
                    _ERR_SPACE("Can not parse exception (9)");
                }
            }
                break;
                
            default:
                break;
        }
    }
loop_end:
    token = tokenizer->current();
    if (!placeholdder_map.empty()) {
        if (r->type == Operator) {
            replacePlaceholder((OperatorItem*)r, placeholdder_map);
        }else if (r->type == Other){
            delete r;
            r = (Item*)placeholdder_map.begin()->second;
        }
    }
    return r;
}

bool ShaderParser::parseVariant(AttributeItem::AttributeType type) {
    const ShaderTokenizer::Token *token = tokenizer->current();
variant_start:
    switch (token->type) {
        case ShaderTokenizer::Other:
            token = tokenizer->step();
            goto variant_start;
        case ShaderTokenizer::Symbol:
        case ShaderTokenizer::Variant:
        {
            ShaderParser::AttributeItem *ai = parseAttribute();
            if (!ai) {
                printErrors();return false;
            }
            VariantType t = ai->var_type;
            ai->attr_type = type;
            if (ai) {
                if (ai->name.empty()) {
                    delete ai;
                    _ERR("Type with a word.");
                }else {
                    currentBlock()->sub_items.push_back(ai);
                    
                    token = tokenizer->token(1);
                    if (token->type == ShaderTokenizer::Symbol &&
                        token->symbol == ShaderTokenizer::Comma) {
                        tokenizer->move(2);
                        while (true) {
                            ai = parseAttribute(t);
                            if (ai) {
                                currentBlock()->sub_items.push_back(ai);
                                token = tokenizer->step();
                                if (token->type != ShaderTokenizer::Symbol ||
                                    token->symbol != ShaderTokenizer::Comma)
                                    return true;
                                else
                                    tokenizer->step();
                            }else {
                                _ERR("Can not parse attr.");
                            }
                        }
                    }
                    return true;
                }
            }else {
                _ERR("Symbol error.")
            }
        }
            
        default:
            _ERR("It is not a variant type.");
    }
}

bool ShaderParser::parseFunction() {
    const ShaderTokenizer::Token *token1 = tokenizer->current();
    const ShaderTokenizer::Token *token2 = tokenizer->token(1);
    const ShaderTokenizer::Token *token3 = tokenizer->token(2);
    if (token1->type == ShaderTokenizer::Variant &&
        token2->type == ShaderTokenizer::Word &&
        token3->type == ShaderTokenizer::Symbol &&
        token3->symbol == ShaderTokenizer::ParentheseLeft) {
        tokenizer->move(2);
        const ShaderTokenizer::Token *token = tokenizer->step();
        FunctionItem *item = new FunctionItem;
        item->ret_type = token1->variant;
        item->name = token2->value;
        
        while (true) {
            if (token->type == ShaderTokenizer::Symbol && token->symbol == ShaderTokenizer::ParentheseRight) {
                break;
            }
            AttributeItem *ai = parseAttribute();
            if (ai) {
                item->argvs.push_back(ai);
            }else {
                delete item;
                _ERR("Parse function argv failed.");
            }
            token = tokenizer->step();
            if (token->type == ShaderTokenizer::Symbol) {
                switch (token->symbol) {
                    case ShaderTokenizer::Comma:
                        token = tokenizer->step();
                        break;
                    case ShaderTokenizer::ParentheseRight:
                        break;
                        
                    default: {
                        delete item;
                        _ERR("Unknown symbol.");
                    }
                        break;
                }
            }else {
                delete item;
                _ERR("Unknown symbol.");
            }
        }
        token = tokenizer->step();
        if (token->type == ShaderTokenizer::Symbol && token->symbol == ShaderTokenizer::BraceLeft) {
            item->block = new BlockItem;
            if (item->name == main_key) {
                item->block->block_type = BlockItem::Main;
            }else {
                item->block->block_type = BlockItem::Other;
            }
            block_stack.push_back(item->block);
            bool ret = parseBlock(false);
            block_stack.pop_back();
            if (ret) {
                currentBlock()->sub_items.push_back(item);
                return true;
            }else {
                delete item;
                _ERR("Function block error.");
            }
        }else {
            delete item;
            _ERR("Must have a function behind function name.");
        }
    }
    return false;
}

void ShaderParser::BlockItem::clear() {
    for (auto it = sub_items.begin(), _e = sub_items.end(); it != _e; ++it) {
        delete (ShaderParser::Item*)*it;
    }
    sub_items.clear();
}

void ShaderParser::FunctionItem::clear() {
    for (auto it = argvs.begin(), _e = argvs.end(); it != _e; ++it) {
        delete (AttributeItem *)*it;
    }
    argvs.clear();
    delete block;
    block = NULL;
}

void ShaderParser::OperatorItem::clear() {
    for (auto it = argvs.begin(), _e = argvs.end(); it != _e; ++it) {
        delete (Item*)*it;
    }
    argvs.clear();
}

void ShaderParser::ActionItem::clear() {
    for (auto it = argvs.begin(), _e = argvs.end(); it != _e; ++it) {
        delete (Item*)*it;
    }
    argvs.clear();
}

int ShaderParser::OperatorItem::priorityLevel(ShaderParser::OperatorItem::OperatorType op_t) {
    switch (op_t) {
        case ShaderParser::OperatorItem::Equal:
        case ShaderParser::OperatorItem::PlusEqual:
        case ShaderParser::OperatorItem::MinusEqual:
        case ShaderParser::OperatorItem::MultiEqual:
        case ShaderParser::OperatorItem::DivisionEqual:
        case ShaderParser::OperatorItem::AndEqual:
        case ShaderParser::OperatorItem::OrEqual:
            return 0;
        case ShaderParser::OperatorItem::OrOr:
            return 2;
        case ShaderParser::OperatorItem::AndAnd:
            return 3;
        case ShaderParser::OperatorItem::Or:
            return 4;
        case ShaderParser::OperatorItem::And:
            return 6;
        case ShaderParser::OperatorItem::Exclusive:
            return 5;
        case ShaderParser::OperatorItem::EqualEqual:
        case ShaderParser::OperatorItem::NotEqual:
            return 7;
        case ShaderParser::OperatorItem::Greater:
        case ShaderParser::OperatorItem::GreaterEqual:
        case ShaderParser::OperatorItem::Lesser:
        case ShaderParser::OperatorItem::LesserEqual:
            return 8;
        case ShaderParser::OperatorItem::LeftShift:
        case ShaderParser::OperatorItem::RightShift:
            return 9;
        case ShaderParser::OperatorItem::Plus:
        case ShaderParser::OperatorItem::Minus:
            return 10;
        
        case ShaderParser::OperatorItem::Multi:
        case ShaderParser::OperatorItem::Division:
        case ShaderParser::OperatorItem::Remainder:
            return 11;
        case ShaderParser::OperatorItem::Dot:
            return 15;
        default:
            break;
    }
    return 0;
}

void ShaderParser::CallItem::clear() {
    for (auto it = argvs.begin(), _e = argvs.end(); it != _e; ++it) {
        delete (ShaderParser::Item*)*it;
    }
    argvs.clear();
}

void ShaderParser::FlowControlItem::clear() {
    for (auto it = controls.begin(), _e = controls.end(); it != _e; ++it) {
        delete (ShaderParser::Item*)*it;
    }
    controls.clear();
    if (block) delete block;
    if (else_flow) delete else_flow;
}
