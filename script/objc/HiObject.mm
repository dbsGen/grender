//
//  HiObject.m
//  hirender_iOS
//
//  Created by gen on 16/9/17.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifdef __OBJC__

#import <objc/runtime.h>
#import <GLKit/GLKit.h>
#include <core/Ref.h>
#include <core/String.h>
#include <core/FixType.h>
#include <core/Array.h>
#include <core/math/Type.h>
#include "ObjcNativeObject.h"
#include "../Utils.h"
#import "HiObject.h"
#include "ObjcInstance.h"
#include "ObjcClass.h"
#include "ObjcScript.h"
#import "ObjcUtils.h"

using namespace hiscript;


@interface HiObject (){
@public
    ScriptInstance  *_scriptInstance;
    HiClass     *_nClass;
}

- (instancetype)_init:(HiClass*)cls;
- (void)initClass:(HiClass*)cls;

@end



@interface HiClass () {
@public
    NSString    *_name;
    Protocol    *_instanceProtocol;
    Class       _instanceClass;
    ObjcClass   *_scriptClass;
}

@end

@implementation HiEngine {
    NSMutableDictionary<NSString*, HiClass *> *_classes;
    Protocol *_classProtocol;
    ObjcScript  *_script;
}

static HiEngine *_HiEngine_instance = NULL;

+ (id)getInstance {
    @synchronized (self) {
        if (!_HiEngine_instance) {
            _HiEngine_instance = [[HiEngine alloc] init];
        }
    }
    return _HiEngine_instance;
}


- (id)init {
    self = [super init];
    if (self) {
        _classProtocol = @protocol(HiClass);
        _classes = [[NSMutableDictionary alloc] init];
        _script = new ObjcScript;
    }
    return self;
}

- (void)dealloc {
    [super dealloc];
    [_classes release];
}

- (void)reg:(Class)cls {
    unsigned int count;
    Protocol ** ptcs = class_copyProtocolList(cls, &count);
    for (int i = 0; i < count; i++) {
        if (protocol_conformsToProtocol(ptcs[i], _classProtocol)) {
            NSString *name = [cls nativeClassName];
            bool create;
            ObjcClass *ncls = (ObjcClass*)_script->find(name.UTF8String, create);
            if (create) {
                HiClass *c = [[HiClass alloc] init];
                c->_name = [name retain];
                c->_instanceClass = [cls retain];
                c->_instanceProtocol = [ptcs[i] retain];
                c->_scriptClass = (ObjcClass*)ncls;
                ((ObjcClass*)ncls)->setOCClass(c);
                [_classes setObject:c
                             forKey:name];
                
                Class _cs = object_getClass(cls);
                IMP fiimp = imp_implementationWithBlock(^(id s, NSInvocation *iv) {
                    [c forwardInvocation:iv];
                });
                BOOL ret = class_addMethod(_cs, @selector(forwardInvocation:),
                                           fiimp, "v24@0:8@16");
                IMP msimp = imp_implementationWithBlock(^(id s, SEL sel) {
                    return [c methodSignatureForSelector:sel];
                });
                ret = class_addMethod(_cs, @selector(methodSignatureForSelector:),
                                      msimp, "@24@0:8:16");
                
                return;
            }
        }
    }
}

- (HiClass *)clazz:(NSString *)name {
    HiClass *cls = [_classes objectForKey:name];
    if (!cls) {
        bool create;
        ObjcClass *ncls = (ObjcClass*)_script->find(name.UTF8String, create);
        if (create) {
            cls = [[HiClass alloc] init];
            cls->_name = [name retain];
            const HClass *scls = ncls->getClass()->getParent();
            while (scls) {
                HiClass *hcls = [_classes objectForKey:[NSString stringWithUTF8String:scls->getFullname().str()]];
                if (hcls) {
                    cls->_instanceClass = hcls->_instanceClass;
                    break;
                }
                scls = scls->getParent();
            }
            if (!cls->_instanceClass) cls->_instanceClass = [[HiObject class] retain];
            cls->_scriptClass = ncls;
            ncls->setOCClass(cls);
            [_classes setObject:cls
                         forKey:name];
            
            
            IMP fiimp = imp_implementationWithBlock(^(id s, NSInvocation *iv) {
                [cls forwardInvocation:iv];
            });
            Class _cs = object_getClass(cls);
            BOOL ret = class_addMethod(_cs, @selector(forwardInvocation:),
                                       fiimp, "v24@0:8@16");
            IMP msimp = imp_implementationWithBlock(^(id s, SEL sel) {
                return [cls methodSignatureForSelector:sel];
            });
            ret = class_addMethod(_cs, @selector(methodSignatureForSelector:),
                                  msimp, "@24@0:8:16");
        }
    }
    return cls;
}

+ (HiClass*)clazz:(NSString *)name {
    return [[self getInstance] clazz:name];
}

@end

@implementation HiClass

@synthesize name = _name, instanceClass = _instanceClass, scriptClass = _scriptClass;

- (void)setName:(NSString*)name {
    [_name release];
    _name = name;
    [_name retain];
}

- (void)dealloc {
    [super dealloc];
    [_name release];
    [_instanceClass release];
    [_instanceProtocol release];
}

- (void)call:(NSString *)name args:(NSArray *)params returnBlock:(void (^)(void *))returnBlock {
    if (_scriptClass) {
        NSInteger c = params.count;
        Variant *vs = new Variant[c];
        Variant **ps = (Variant **)malloc(sizeof(Variant*) * c);
        [ObjcUtils makeArgs:vs params:params];
        for (int i = 0; i < c; ++i) {
            ps[i] = vs + i;
        }
        Variant var = _scriptClass->call(name.UTF8String, (const Variant **)ps, c);
        if (returnBlock) returnBlock(&var);
        free(ps);
        delete[] vs;
    }
}

- (struct objc_method_description)getMethodDes:(Protocol*)ptc sel:(SEL)sel {
    struct objc_method_description ret = protocol_getMethodDescription(ptc, sel, YES, NO);
    if (!ret.types) {
        ret = protocol_getMethodDescription(ptc, sel, NO, NO);
    }
    return ret;
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    if (_instanceProtocol) {
        struct objc_method_description des = [self getMethodDes:_instanceProtocol sel:aSelector];
        if (des.types) {
            char *chs = (char *)malloc((strlen(des.types) + 1) * sizeof(char));
            const char *och = des.types;
            char *dch = chs;
            bool skip = false;
            bool inbra = false;
            while(*och) {
                if (*och == '(') {
                    skip = true;
                    inbra = true;
                }
                if (!skip && *och != ')') {
                    *dch = *och;
                    ++dch;
                }
                if (skip && inbra && (*och == '=' || *och == ')')) {
                    if (*och == ')')inbra = false;
                    skip = false;
                }
                else if (!skip && inbra && *och == '}')
                    skip = true;
                och++;
            }
            *dch = 0;
            NSMethodSignature *signature = [NSMethodSignature signatureWithObjCTypes:chs];
            free(chs);
            return signature;
        }
    }
    return [class_getSuperclass(_instanceClass) methodSignatureForSelector:aSelector];
}

typedef Variant (^objc_call_block)(const StringName &, const Variant **, int);

void call_method(NSString *name,
                 variant_vector params,
                 void (^block)(void *),
                 objc_call_block call_block) {
    size_t c = params.size();
    Variant **ps = (Variant **)malloc(sizeof(Variant*) * c);
    for (int i = 0; i < c; ++i) {
        ps[i] = &params[i];
    }
    Variant var = call_block(name.UTF8String, (const Variant**)ps, (int)c);
    if (block) block(&var);
    free(ps);
}

bool process_invocation(NSInvocation *anInvocation, struct objc_method_description des,
                        objc_call_block call_block) {
    if (des.types) {
        NSString *name = NSStringFromSelector(anInvocation.selector);
        NSMethodSignature *ms = anInvocation.methodSignature;
        NSRange range = [name rangeOfString:@":"];
        if (range.length)
            name = [name substringToIndex:range.location];
        NSInteger count = ms.numberOfArguments;
        variant_vector array;
        for (int i = 2; i < count; ++i) {
            const char *type = [ms getArgumentTypeAtIndex:i];
            switch (*type) {
                case _C_INT:
                {
                    int v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back(v);
                }
                    break;
                case _C_LNG:
                {
                    long v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back(v);
                }
                    break;
                case _C_FLT:
                {
                    float v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back(v);
                }
                    break;
                case _C_DBL:
                {
                    double v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back(v);
                }
                    break;
                case _C_BOOL:
                {
                    BOOL v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back(v);
                }
                    break;
                case _C_ID:
                {
                    id v;
                    [anInvocation getArgument:&v atIndex:i];
                    array.push_back([ObjcUtils makeVariant:v]);
                }
                    break;
                default:
                    if (strcmp(type, "{CGSize=ff}") == 0 || strcmp(type, "{CGSize=dd}") == 0 ) {
                        CGSize size;
                        [anInvocation getArgument:&size atIndex:i];
                        array.push_back(HSize(size.width, size.height));
                    }else if (strcmp(type, "{?=ffff}") == 0 || strcmp(type, "{?=dddd}") == 0 ) {
                        GLKVector4 v4;
                        [anInvocation getArgument:&v4 atIndex:i];
                        array.push_back(HColor(v4.r, v4.g, v4.b, v4.a));
                    }else {
//                        LOG(i, @"Unknow type of %s", type);
                        array.push_back(Variant::null());
                    }
                    break;
            }
        }
        call_method(name, array, ^(void *v) {
            Variant *var = (Variant*)v;
            const char *type = [ms methodReturnType];
            switch (*type) {
                case _C_INT:
                {
                    int v = *var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_LNG:
                {
                    long v = *var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_LNG_LNG: {
                    long long v = *var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_FLT:
                {
                    float v = *var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_DBL:
                {
                    double v = *var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_BOOL:
                {
                    BOOL v = (int)*var;
                    [anInvocation setReturnValue:&v];
                }
                    break;
                case _C_ID:
                case _C_PTR:
                {
                    const HClass *nt = var->getType();
                    if (nt) {
                        if (nt->isTypeOf(NativeObject::getClass())) {
                            NativeObject *no = var->get<NativeObject>();
                            void *v = no->getNative();
                            [anInvocation setReturnValue:&v];
                        }else if (nt->isTypeOf(String::getClass())) {
                            NSString *str = [NSString stringWithUTF8String:var->str().c_str()];
                            [anInvocation setReturnValue:&str];
                        }else if (nt->isTypeOf(Integer::getClass())) {
                            NSNumber *n = [NSNumber numberWithInt:*var];
                            [anInvocation setReturnValue:&n];
                        }else if (nt->isTypeOf(Long::getClass())) {
                            NSNumber *n = [NSNumber numberWithLong:*var];
                            [anInvocation setReturnValue:&n];
                        }else if (nt->isTypeOf(Float::getClass())) {
                            NSNumber *n = [NSNumber numberWithFloat:*var];
                            [anInvocation setReturnValue:&n];
                        }else if (nt->isTypeOf(Double::getClass())) {
                            NSNumber *n = [NSNumber numberWithDouble:*var];
                            [anInvocation setReturnValue:&n];
                        }else {
                            Variant &v = *var;
                            if (v) {
                                HiClass *cls = [HiEngine clazz:[NSString stringWithUTF8String:nt->getFullname().str()]];
                                ObjcInstance *mins = (ObjcInstance*)cls->_scriptClass->get(v);
                                if (mins) {
                                    id obj = mins->getOCObject();
                                    [anInvocation setReturnValue:&obj];
                                }else {
                                    HiObject *obj = [[[cls->_instanceClass alloc] _init:cls] autorelease];
                                    mins = (ObjcInstance*)cls->_scriptClass->create(v);
                                    obj->_scriptInstance = mins;
                                    mins->setOCObject(obj);
                                    [anInvocation setReturnValue:&obj];
                                }
                            }else {
                                id n = NULL;
                                [anInvocation setReturnValue:&n];
                            }
                        }
                    }else {
                        id n = NULL;
                        [anInvocation setReturnValue:&n];
                    }
                }
                    break;
                case 'v': {
                    
                }
                    break;
                default: {
                    const HClass *nt = var->getType();
                    if (nt == Matrix4::getClass()) {
                        GLKMatrix4 m4;
                        Matrix4 om4 = *var;
                        memcpy(m4.m, om4.m, 16*sizeof(float));
                        [anInvocation setReturnValue:&m4];
                    }else if (nt == HSize::getClass()) {
                        CGSize size;
                        HSize hs = *var;
                        size.width = hs.x();
                        size.height = hs.y();
                        [anInvocation setReturnValue:&size];
                    }else {
//                        LOG(i, @"Unknow type of %s", type);
                        id n = nil;
                        [anInvocation setReturnValue:&n];
                    }
                }
                    break;
            }
        }, call_block);
        return true;
    }
    return false;
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    if (_instanceProtocol) {
        struct objc_method_description des = [self getMethodDes:_instanceProtocol
                                                            sel:anInvocation.selector];
        if (process_invocation(anInvocation, des,
                               ^(const StringName &name,
                                 const Variant **vs,
                                 int count){
                                   return _scriptClass->call(name, vs, count);
                               })
            ) {
            return;
        }
    }
    if ([self respondsToSelector:anInvocation.selector]) {
        [anInvocation invokeWithTarget:self];
    }else {
        [class_getSuperclass(_instanceClass) forwardInvocation:anInvocation];
    }
}

@end

@implementation HiObject

@synthesize nClass = _nClass, scriptInstance = _scriptInstance;

- (instancetype)init {
    self = [super init];
    if (self) {
        HiClass *c = [HiEngine clazz:[self.class nativeClassName]];
        [self initClass:c];
    }
    return self;
}

- (instancetype)_init:(HiClass*)cls {
    self = [super init];
    if (self) {
        _nClass = cls;
    }
    return self;
}

- (void)initClass:(HiClass*)cls {
    _nClass = cls;
    if (_nClass) {
        _scriptInstance = _nClass->_scriptClass->newInstance(NULL, 0);
        ((ObjcInstance*)_scriptInstance)->setOCObject(self);
    }
}

- (void)call:(NSString *)name args:(NSArray *)params returnBlock:(void (^)(void *))returnBlock {
    if (_scriptInstance) {
        NSInteger c = params.count;
        Variant *vs = new Variant[c];
        Variant **ps = (Variant **)malloc(sizeof(Variant*) * c);
        [ObjcUtils makeArgs:vs params:params];
        for (int i = 0; i < c; ++i) {
            ps[i] = vs + i;
        }
        Variant var = _scriptInstance->call(name.UTF8String, (const Variant**)ps, (int)c);
        if (returnBlock) returnBlock(&var);
        free(ps);
        delete[] vs;
    }
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
    Protocol *ptc = self.nClass->_instanceProtocol;
    if (ptc) {
        struct objc_method_description des = [self getMethodDes:ptc sel:anInvocation.selector];
        
        if (process_invocation(anInvocation, des,
                               ^hicore::Variant(const hicore::StringName &name,
                                                const hicore::Variant **vs,
                                                int count) {
                                   return _scriptInstance->call(name, vs, count);
                               })
            ) {
            return;
        }
    }
    [super forwardInvocation:anInvocation];
}

- (struct objc_method_description)getMethodDes:(Protocol*)ptc sel:(SEL)sel {
    struct objc_method_description ret = protocol_getMethodDescription(ptc, sel, YES, YES);
    if (!ret.types) {
        ret = protocol_getMethodDescription(ptc, sel, NO, YES);
    }
    return ret;
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    Protocol *ptc = self.nClass->_instanceProtocol;
    if (ptc) {
        struct objc_method_description des = [self getMethodDes:ptc sel:aSelector];
        if (des.types) {
            char *chs = (char *)malloc((strlen(des.types) + 1) * sizeof(char));
            const char *och = des.types;
            char *dch = chs;
            uint8_t state = 0;
            while(*och) {
                if (state == 0 && *och == '(') {
                    state = 1;
                }else if (state == 1 && *och == '{') {
                    state = 2;
                }
                if (!(state & 1) && *och != ')') {
                    *dch = *och;
                    ++dch;
                }
                if (state > 0 && *och == ')') {
                    state = 0;
                }
                else if (state == 2 && *och == '}')
                    state = 3;
                och++;
            }
            *dch = 0;
            NSMethodSignature *signature = [NSMethodSignature signatureWithObjCTypes:chs];
            free(chs);
            return signature;
        }
    }
    return [super methodSignatureForSelector:aSelector];
}

- (void)dealloc {
    if (_scriptInstance) delete(_scriptInstance);
    [super dealloc];
}

@end

@implementation ObjcUtils

+ (void)objectFromVariant:(const hicore::Variant*)var blcok:(ObjcObjectCallback)callback {
    const HClass *nt = var->getType();
    if (nt) {
        if (nt->isTypeOf(NativeObject::getClass())) {
            NativeObject *no = var->get<NativeObject>();
            void *v = no->getNative();
            callback(v);
        }else if (nt->isTypeOf(_String::getClass())) {
            NSString *str = [NSString stringWithUTF8String:var->str().c_str()];
            callback(str);
        }else if (nt->isTypeOf(Integer::getClass())) {
            NSNumber *n = [NSNumber numberWithInt:*var];
            callback(n);
        }else if (nt->isTypeOf(Long::getClass())) {
            NSNumber *n = [NSNumber numberWithLong:*var];
            callback(n);
        }else if (nt->isTypeOf(Float::getClass())) {
            NSNumber *n = [NSNumber numberWithFloat:*var];
            callback(n);
        }else if (nt->isTypeOf(Double::getClass())) {
            NSNumber *n = [NSNumber numberWithDouble:*var];
            callback(n);
        }else if (nt->isTypeOf(_Array::getClass())) {
            Array arr = *var;
            int t = arr.size();
            NSMutableArray *a = [[[NSMutableArray alloc] initWithCapacity:t] autorelease];
            for (int i = 0; i < t; ++i) {
                [self objectFromVariant:&arr.at(i)
                                  blcok:^(void* obj) {
                                      [a addObject:(id)obj];
                                  }];
            }
            callback(a);
        }else {
            Variant v = *var;
            if (v) {
                HiClass *cls = [HiEngine clazz:[NSString stringWithUTF8String:nt->getFullname().str()]];
                ObjcInstance *mins = (ObjcInstance*)cls->_scriptClass->get(v);
                if (mins) {
                    callback(mins->getOCObject());
                }else {
                    HiObject *obj = [[[cls->_instanceClass alloc] _init:cls] autorelease];
                    ObjcClass *ocls = (ObjcClass *)cls.scriptClass;
                    mins = (ObjcInstance*)ocls->create(v);
                    obj->_scriptInstance = mins;
                    mins->setOCObject(obj);
                    callback(obj);
                }
            }else {
                callback(NULL);
            }
        }
    }else {
        callback(NULL);
    }
}


+ (Variant)makeVariant:(id)object {
    if ([object isKindOfClass:[NSString class]])
    {
        return Variant([object UTF8String]);
    }else if ([object isKindOfClass:[NSNumber class]]) {
        const char *c_type = [object objCType];
        switch (c_type[0]) {
            case _C_INT:
            case _C_UINT:
            case _C_BOOL:
            {
                return Variant([object intValue]);
            }
            case _C_LNG:
            case _C_ULNG:
            {
                return Variant([object longValue]);
            }
            case _C_LNG_LNG:
            case _C_ULNG_LNG:
            {
                return Variant([object longValue]);
            }
            case _C_FLT:
            {
                return Variant([object floatValue]);
            }
            case _C_DBL:
            {
                return Variant([object doubleValue]);
            }
            default:
                return Variant::null();
                break;
        }
    }else if ([object isKindOfClass:[HiObject class]]){
        HiObject *ho = object;
        if (ho->_scriptInstance)
            return ho->_scriptInstance->getTarget();
        return Variant::null();
    }else if ([object isKindOfClass:[NSNull null]]) {
        return Variant();
    }else if ([object isKindOfClass:[NSArray class]]) {
        NSArray *arr = object;
        vector<Variant> vs(arr.count);
        NSEnumerator *enu = [arr objectEnumerator];
        while (id obj = [enu nextObject]) {
            vs.push_back([self makeVariant:obj]);
        }
        return Array(vs);
    } else {
        Ref<NativeObject> res_obj(new ObjcNativeObject);
        res_obj->setNative(object);
        return Variant(res_obj);
    }
    return Variant();
}


+ (void)makeArgs:(Variant*)res params:(NSArray*)params {
    NSInteger count = params.count;
    for (NSInteger i = 0; i < count; ++i) {
        id obj = [params objectAtIndex:i];
        res[i] = [self makeVariant:obj];
    }
}

+ (Variant)apply:(id)object :(SEL)sel :(const Variant**)params :(int)count {
    
    NSMethodSignature *sign = [object methodSignatureForSelector:sel];
    if (sign) {
        NSInvocation *inv = [NSInvocation invocationWithMethodSignature:sign];
        [inv setArgument:&object atIndex:0];
        [inv setArgument:&sel atIndex:1];
        for (int i = 0; i < count; ++i) {
            const char *type = [sign getArgumentTypeAtIndex:i];
            switch (*type) {
                case _C_CHR:
                case _C_UCHR:
                {
                    char c = *params[i];
                    [inv setArgument:&c atIndex:i+2];
                }
                    break;
                case _C_SHT:
                case _C_USHT:
                {
                    short s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_INT:
                case _C_UINT:
                {
                    int s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_LNG:
                case _C_ULNG:
                {
                    long s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_LNG_LNG:
                case _C_ULNG_LNG:
                {
                    long long s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_FLT:
                {
                    float s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_DBL:
                {
                    double s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_BFLD:
                case _C_BOOL:
                {
                    bool s = *params[i];
                    [inv setArgument:&s atIndex:i+2];
                }
                    break;
                case _C_ID:
                case _C_CHARPTR:
                {
                    [ObjcUtils objectFromVariant:params[i]
                                           blcok:^(void *ret) {
                                               [inv setArgument:&ret atIndex:i+2];
                                           }];
                }
                    break;
                    
                default:
                    break;
            }
        }
        [inv invokeWithTarget:object];
        const char *type = [sign methodReturnType];
        Variant ret;
        switch (*type) {
            case _C_CHR:
            case _C_UCHR:
            {
                char c;
                [inv getReturnValue:&c];
                ret = c;
            }
                break;
            case _C_SHT:
            case _C_USHT:
            {
                short s;
                [inv getReturnValue:&s];
                ret = s;
            }
                break;
            case _C_INT:
            case _C_UINT:
            {
                int i;
                [inv getReturnValue:&i];
                ret = i;
            }
                break;
            case _C_LNG:
            case _C_ULNG:
            {
                long l;
                [inv getReturnValue:&l];
                ret = l;
            }
                break;
            case _C_LNG_LNG:
            case _C_ULNG_LNG:
            {
                long long ll;
                [inv getReturnValue:&ll];
                ret = ll;
            }
                break;
            case _C_FLT:
            {
                float f;
                [inv getReturnValue:&f];
                ret = f;
            }
                break;
            case _C_DBL:
            {
                double d;
                [inv getReturnValue:&d];
                ret = d;
            }
                break;
            case _C_BFLD:
            case _C_BOOL:
            {
                bool b;
                [inv getReturnValue:&b];
                ret = b;
            }
                break;
            case _C_ID:
            {
                id obj;
                [inv getReturnValue:&obj];
                ret = [ObjcUtils makeVariant:obj];
            }
                break;
            case _C_CHARPTR:
            {
                void *obj;
                [inv getReturnValue:&obj];
                ret = (long)obj;
            }
                break;
            case _C_VOID:
            {
                
            }
                break;
                
            default:
                LOG(e, "unknown type %s", type);
                break;
        }
        return ret;
    }
    return Variant::null();
}

@end


#endif
