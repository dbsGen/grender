//
//  HTTPClient.hpp
//  hirender_iOS
//
//  Created by mac on 2017/4/11.
//  Copyright © 2017年 gen. All rights reserved.
//

#ifndef HTTPClient_hpp
#define HTTPClient_hpp

#include <core/Object.h>
#include <core/Action.h>
#include <core/Callback.h>
#include <core/Map.h>
#include <core/Data.h>
#include <types.h>
#include "../../render_define.h"

namespace gr {
    CLASS_BEGIN_N(HTTPClient, gc::RefObject)

        std::string url;
        std::string path;
        gc::StringName method;
        std::string error;
        pointer_map properties;
        float percent;
        bool read_cache;
        float delay;
        int retry_count;
        int retry_num;
        Time timeout;
        std::map<std::string, std::string> headers;
        std::map<std::string, gc::Variant> parameters;
        gc::Map response_headers;

        gc::RCallback on_complete;
        gc::RCallback on_progress;
        gc::Ref<gc::Data> body;

        void _initialize();
        void _finalize();

    public:
        static const gc::StringName METHOD_GET;
        static const gc::StringName METHOD_POST;
        static const gc::StringName METHOD_PUT;
        static const gc::StringName METHOD_DELETE;

        void event(const gc::StringName &name, void *data);

        HTTPClient();
        ~HTTPClient();

        METHOD _FORCE_INLINE_ void initialize(const std::string &url) {
            this->url = url;
            _initialize();
        }
        METHOD _FORCE_INLINE_ const std::string &getUrl() {
            return url;
        }
        METHOD _FORCE_INLINE_ const std::string &getError() {
            return error;
        }

        METHOD _FORCE_INLINE_ float getPercent() const {
            return percent;
        }

        /**
         * @params (HTTPClient *c, const string &path)  
         * @return NULL;
         */
        METHOD _FORCE_INLINE_ void setOnComplete(const gc::RCallback &callback) {
            on_complete = callback;
        }
        METHOD _FORCE_INLINE_ const gc::RCallback &getOnComplete() const {
            return on_complete;
        }
        PROPERTY(on_complete, getOnComplete, setOnComplete);
        METHOD _FORCE_INLINE_ void setOnProgress(const gc::RCallback &callback) {
            on_progress = callback;
        }
        METHOD _FORCE_INLINE_ const gc::RCallback &getOnProgress() const {
            return on_progress;
        }
        PROPERTY(on_progress, getOnProgress, setOnProgress)
        METHOD _FORCE_INLINE_ const std::string &getPath() const {
            return path;
        }
        PROPERTY(path, getPath, NULL);
        METHOD _FORCE_INLINE_ void setReadCache(bool read_cache) {
            this->read_cache = read_cache;
        }
        METHOD _FORCE_INLINE_ bool getReadCache() const {
            return read_cache;
        }
        PROPERTY(read_cache, getReadCache, setReadCache);
        METHOD _FORCE_INLINE_ void setDelay(float delay) {
            this->delay = delay;
        }
        METHOD _FORCE_INLINE_ float getDelay() const {
            return delay;
        }
        PROPERTY(delay, getDelay, setDelay);

        METHOD _FORCE_INLINE_ void setRetryCount(int count) {
            retry_count = count;
        }
        METHOD _FORCE_INLINE_ int getRetryCount() const {
            return retry_count;
        }
        PROPERTY(retry_count, getRetryCount, setRetryCount);

        METHOD _FORCE_INLINE_ void setMethod(const gc::StringName &method) {
            this->method = method;
        }
        METHOD _FORCE_INLINE_ const gc::StringName &getMethod() const {
            return method;
        }
        PROPERTY(method, getMethod, setMethod);

        METHOD _FORCE_INLINE_ void addHeader(const std::string &name, const std::string &val) {
            headers[name] = val;
        }
        METHOD _FORCE_INLINE_ void removeHeader(const std::string &name) {
            headers.erase(name);
        }

        METHOD _FORCE_INLINE_ void addParameter(const std::string &name, const gc::Variant &val) {
            parameters[name] = val;
        }
        METHOD _FORCE_INLINE_ void removeParameter(const std::string &name) {
            parameters.erase(name);
        }

        METHOD _FORCE_INLINE_ const gc::Map &getResponseHeaders() {
            return response_headers;
        }
        METHOD _FORCE_INLINE_ void setResponseHeaders(const gc::Map &headers) {
            response_headers = headers;
        }
        PROPERTY(response_headers, getResponseHeaders, setResponseHeaders);

        METHOD _FORCE_INLINE_ Time getTimeout() {
            return timeout;
        }
        METHOD _FORCE_INLINE_ void setTimeout(Time timeout) {
            this->timeout = timeout;
        }
        PROPERTY(timeout, setTimeout, getTimeout);

        METHOD _FORCE_INLINE_ const gc::Ref<gc::Data> &getBody() const {
            return body;
        }
        METHOD _FORCE_INLINE_ void setBody(const gc::Ref<gc::Data> &body) {
            this->body = body;
        }
        PROPERTY(body, setBody, getBody);

        METHOD void start();
        METHOD void cancel();

    protected:
        ON_LOADED_BEGIN(cls, RefObject)
            ADD_PROPERTY(cls, "on_complete", ADD_METHOD(cls, HTTPClient, getOnComplete), ADD_METHOD(cls, HTTPClient, setOnComplete));
            ADD_PROPERTY(cls, "on_progress", ADD_METHOD(cls, HTTPClient, getOnProgress), ADD_METHOD(cls, HTTPClient, setOnProgress));
            ADD_PROPERTY(cls, "path", ADD_METHOD(cls, HTTPClient, getPath), NULL);
            ADD_PROPERTY(cls, "read_cache", ADD_METHOD(cls, HTTPClient, getReadCache), ADD_METHOD(cls, HTTPClient, setReadCache));
            ADD_PROPERTY(cls, "delay", ADD_METHOD(cls, HTTPClient, getDelay), ADD_METHOD(cls, HTTPClient, setDelay));
            ADD_PROPERTY(cls, "retry_count", ADD_METHOD(cls, HTTPClient, getRetryCount), ADD_METHOD(cls, HTTPClient, setRetryCount));
            ADD_PROPERTY(cls, "method", ADD_METHOD(cls, HTTPClient, getMethod), ADD_METHOD(cls, HTTPClient, setMethod));
            ADD_PROPERTY(cls, "response_headers", ADD_METHOD(cls, HTTPClient, getResponseHeaders), ADD_METHOD(cls, HTTPClient, setResponseHeaders));
            ADD_PROPERTY(cls, "timeout", ADD_METHOD(cls, HTTPClient, getTimeout), ADD_METHOD(cls, HTTPClient, setTimeout));
            ADD_PROPERTY(cls, "body", ADD_METHOD(cls, HTTPClient, getBody), ADD_METHOD(cls, HTTPClient, setBody));
            ADD_METHOD(cls, HTTPClient, initialize);
            ADD_METHOD(cls, HTTPClient, getError);
            ADD_METHOD(cls, HTTPClient, getPercent);
            ADD_METHOD(cls, HTTPClient, addHeader);
            ADD_METHOD(cls, HTTPClient, removeHeader);
            ADD_METHOD(cls, HTTPClient, addParameter);
            ADD_METHOD(cls, HTTPClient, removeParameter);
            ADD_METHOD(cls, HTTPClient, start);
            ADD_METHOD(cls, HTTPClient, cancel);
        ON_LOADED_END
    CLASS_END
}

#endif /* HTTPClient_hpp */
