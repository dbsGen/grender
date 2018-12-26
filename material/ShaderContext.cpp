//
//  ShaderContext.cpp
//  hirender_iOS
//
//  Created by mac on 2017/4/25.
//  Copyright © 2017年 gen. All rights reserved.
//

#include <cstring>
#include "ShaderContext.h"
#include "ShaderCompiler.h"

using namespace gr;
using namespace gc;

namespace gr {
    class ShaderContextItem {
        StringName name;
        Ref<Data> data;
        ShaderParser *parser;
        
    public:
        _FORCE_INLINE_ ShaderContextItem(const StringName &name, const Ref<gc::Data> &data) {
            this->name = name;
            this->data = data;
            parser = nullptr;
        }
        _FORCE_INLINE_ ~ShaderContextItem() {
            if (parser) delete parser;
        }
        
        _FORCE_INLINE_ const Ref<Data> &getData() {
            return data;
        }
        _FORCE_INLINE_ ShaderParser *getParser() {
            return parser;
        }
        _FORCE_INLINE_ const gc::StringName &getName() {
            return name;
        }
        
        void prepare() {
            if (data) {
                if (!parser) {
                    parser = new ShaderParser((const char *)data->getBuffer());
                }
            }
        }
        
    };
}

ShaderContext::~ShaderContext() {
    for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
        delete (ShaderContextItem*)it->second;
    }
}

ShaderContextItem *ShaderContext::findItem(const StringName &name) {
    auto it = items.find(name);
    ShaderContextItem *item = nullptr;
    if (it != items.end()) {
        item = (ShaderContextItem*)it->second;
    }
    if (!item) {
        for (auto it = paths.begin(), _e = paths.end(); it != _e; ++it) {
            string path = *it + '/' + name.str();
            FILE *file = fopen(path.c_str(), "r");
            if (file) {
                fclose(file);
                item = new ShaderContextItem(name, new FileData(path));
                items[name] = item;
                break;
            }
        }
    }
    return item;
}

void ShaderContext::_parse(ShaderContextItem *item, ShaderContext::Result &result) {
    
    item->prepare();
    ShaderParser *parser = item->getParser();
    const pointer_vector &attrs = parser->getAttributes();
    for (auto it = attrs.begin(), _e = attrs.end(); it != _e; ++it) {
        ShaderParser::AttributeItem *attr = (ShaderParser::AttributeItem *)*it;
        switch (attr->attr_type) {
            case ShaderParser::AttributeItem::Attr:
            {
                result.attributes.push_back(attr);
                break;
            }
            case ShaderParser::AttributeItem::Uniform:
            {
                result.uniforms.push_back(attr);
                break;
            }
            case ShaderParser::AttributeItem::Compile:
            {
                result.compiles.push_back(attr);
                break;
            }
                
            default: break;
        }
    }
    
    for (auto it = parser->getActions().begin(), _e = parser->getActions().end(); it != _e; ++it) {
        ShaderParser::ActionItem *action_item = (ShaderParser::ActionItem*)*it;
        switch (action_item->action_type) {
            case ShaderParser::ActionItem::Include:
            {
                if (action_item->argvs.size() > 0) {
                    ShaderParser::ConstantItem *const_item = (ShaderParser::ConstantItem*)action_item->argvs[0];
                    ShaderContextItem *sitem = findItem(const_item->variant.c_str());
                    if (sitem) {
                        _parse(sitem, result);
                    }
                }
            }
                break;
                
            default:
                break;
        }
    }
}

void ShaderContext::_compile(ShaderCompiler &compiler,
                             ShaderContextItem *item,
                             const variant_map &compile_argvs,
                             string &shader,
                             bool is_function) {
    item->prepare();
    ShaderParser *parser = item->getParser();
    
    StringName last_name = compiler.getMainName();
    for (auto it = parser->getActions().begin(), _e = parser->getActions().end(); it != _e; ++it) {
        ShaderParser::ActionItem *action_item = (ShaderParser::ActionItem*)*it;
        switch (action_item->action_type) {
            case ShaderParser::ActionItem::Include:
            {
                if (action_item->argvs.size() > 0) {
                    ShaderParser::ConstantItem *const_item = (ShaderParser::ConstantItem*)action_item->argvs[0];
                    ShaderContextItem *sitem = findItem(StringName(const_item->variant.c_str()));
                    if (sitem) {
                        if (is_function) {
                            size_t s = const_item->variant.find('.');
                            string strkey;
                            strkey.resize(s);
                            memcpy(&strkey[0], const_item->variant.data(), s);
                            StringName key(strkey.c_str());
                            compiler.setMainName(key);
                        }
                        _compile(compiler, sitem, compile_argvs, shader, is_function);
                        if (is_function) {
                            compiler.setMainName(last_name);
                        }
                    }else {
                        LOG(w, "not found shader :%s", const_item->variant.c_str());
                    }
                }
                break;
            }
                
            default: {
                LOG(i, "Unkown action %d", action_item->action_type);
            }
                break;
        }
    }
    compiler.compile(is_function ? parser->getFunctions() : parser->getAttributes(), compile_argvs, shader);
}

void ShaderContext::addData(const StringName &name, const Ref<Data> &data, bool replase) {
    auto it = items.find(name);
    if (it != items.end()) {
        if (replase) {
            delete (ShaderContextItem*)it->second;
        }else {
            return;
        }
    }
    items[name] = new ShaderContextItem(name, data);
}

void ShaderContext::addPath(const string &path) {
    paths.push_back(path);
}

void ShaderContext::compile(const StringName &name, const variant_map &compile_argvs, string &shader) {
    ShaderContextItem *item = findItem(name);
    if (item) {
        ShaderCompiler compiler;
        item->prepare();
        ShaderParser *parser = item->getParser();
        for (auto it = parser->getActions().begin(), _e = parser->getActions().end(); it != _e; ++it) {
            ShaderParser::ActionItem *action_item = (ShaderParser::ActionItem *) *it;
            if (action_item->action_type == ShaderParser::ActionItem::Extend) {
                    if (action_item->argvs.size() > 0) {
                        ShaderParser::ConstantItem *const_item = (ShaderParser::ConstantItem*)action_item->argvs[0];
                        shader += const_item->variant;
                        shader += '\n';
                    }
                    break;
            }
        }

        shader += "precision mediump float;\n";
        _compile(compiler, item, compile_argvs, shader, false);
        _compile(compiler, item, compile_argvs, shader, true);
    }
}

void ShaderContext::parse(const StringName &name, Result &result) {
    ShaderContextItem *item = findItem(name);
    if (item) {
        _parse(item, result);
    }
}



