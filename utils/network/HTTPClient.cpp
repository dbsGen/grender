//
//  HTTPClient.cpp
//  hirender_iOS
//
//  Created by mac on 2017/4/11.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "HTTPClient.h"

using namespace hirender;

const StringName HTTPClient::METHOD_GET("GET");
const StringName HTTPClient::METHOD_POST("POST");
const StringName HTTPClient::METHOD_PUT("PUT");
const StringName HTTPClient::METHOD_DELETE("DELETE");

HTTPClient::HTTPClient() : method(METHOD_GET), read_cache(false), delay(0), retry_count(0), retry_num(0) {
    
}

HTTPClient::~HTTPClient() {
    _finalize();
}

