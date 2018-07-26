//
//  HTTPClient.m
//  hirender_iOS
//
//  Created by mac on 2017/4/12.
//  Copyright © 2017年 gen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "HTTPClient.h"
#import "DIManager.h"

const StringName event_complete("COMPLETE");
const StringName event_failed("FAILED");
const StringName event_process("PROCESS");

@interface HTTPClientObject : NSObject <DIItemDelegate> {
@public
    gr::HTTPClient *c_client;
}

@property (nonatomic, strong) NSURLSession *session;
@property (nonatomic, strong) DIItem *item;

- (id)init;
- (void)start;

@end

@implementation HTTPClientObject

- (id)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

- (void)start {
    if (_item) {
        _item.delegate = self;
        [_item start];
    }
}


- (void)itemStarted:(DIItem*)item {
    
}

- (void)item:(DIItem*)item process:(float)percent {
    c_client->event(event_process, &percent);
}

- (void)itemCanceled:(DIItem*)item {
    
}

- (void)itemRemoved:(DIItem*)item {
    
}

- (void)itemFailed:(DIItem *)item error:(NSError *)error {
    c_client->event(event_failed, (void*)error.domain.UTF8String);
}

- (void)itemComplete:(DIItem *)item {
    c_client->event(event_complete, (void*)item.path.UTF8String);
}

- (void)itemPaused:(DIItem *)item {
    
}

@end

using namespace gr;

const StringName ObjcTarget("OBJECT_C_OBJECT");

void HTTPClient::event(const gcore::StringName &name, void *data) {
    if (name == event_complete) {
        path = (const char *)data;
        error.clear();
        if (on_complete)
            (**on_complete)(this, path);
    }else if (name == event_failed) {
        if (retry_num >= retry_count) {
            error = (const char *)data;
            if (on_complete)
                (**on_complete)(this, error);
        }else {
            retry_count ++;
            HTTPClientObject *obj = (HTTPClientObject *)properties[ObjcTarget];
            if (delay) {
                [obj performSelector:@selector(start)
                          withObject:nil
                          afterDelay:delay];
            }else {
                [obj start];
            }
        }
    }else if (name == event_process) {
        percent = *(float*)data;
        if (on_progress)
            (**on_progress)(this, percent);
    }
}

void HTTPClient::_initialize() {
    HTTPClientObject *obj = [[HTTPClientObject alloc] init];
    obj->c_client = this;
    properties[ObjcTarget] = obj;
}
void HTTPClient::_finalize() {
    for (auto it = properties.begin(), _e = properties.end(); it != _e; ++it) {
        id obj = (id)it->second;
        [obj release];
    }
}

void HTTPClient::start() {
    retry_num = 0;
    HTTPClientObject *obj = (HTTPClientObject *)properties[ObjcTarget];
    DIItem *item = [[DIManager defaultManager] itemWithURLString:[NSString stringWithUTF8String:url.c_str()]];
    item.method = [NSString stringWithUTF8String:method.str()];
    item.readCache = read_cache;
    if (headers.size()) {
        NSMutableDictionary *dic = [NSMutableDictionary dictionary];
        for (auto it = headers.begin(), _e = headers.end(); it != _e; ++it) {
            [dic setObject:[NSString stringWithUTF8String:it->second.c_str()]
                    forKey:[NSString stringWithUTF8String:it->first.c_str()]];
        }
        item.headers = dic;
    }
    if (parameters.size()) {
        NSMutableDictionary *dic = [NSMutableDictionary dictionary];
        for (auto it = parameters.begin(), _e = parameters.end(); it != _e; ++it) {
            [dic setObject:[NSString stringWithUTF8String:it->second]
                    forKey:[NSString stringWithUTF8String:it->first.c_str()]];
        }
        item.datas = dic;
    }
    obj.item = item;
    if (delay) {
        [obj performSelector:@selector(start)
                  withObject:nil
                  afterDelay:delay];
    }else {
        [obj start];
    }
}

void HTTPClient::cancel() {
    HTTPClientObject *obj = (HTTPClientObject *)properties[ObjcTarget];
    [obj.item cancel];
}
