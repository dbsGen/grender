//
//  FileSystem.hpp
//  hirender_iOS
//
//  Created by gen on 29/12/2016.
//  Copyright © 2016 gen. All rights reserved.
//

#ifndef FileSystem_hpp
#define FileSystem_hpp

#include <core/Data.h>
#include <core/Singleton.h>

namespace gr {
    CLASS_BEGIN_TNV(FileSystem, gc::Singleton, 1, FileSystem)
    
    std::string resource_path;
    std::string storage_path;
    std::map<gc::StringName, std::string> configs;
    
    void save() const;
    void updateStoragePath();
    
public:
    
    _FORCE_INLINE_ void setResourcePath(std::string &&rpath) {
        resource_path = rpath;
    }
    _FORCE_INLINE_ void setResourcePath(const std::string &rpath) {
        resource_path = rpath;
    }
    _FORCE_INLINE_ const std::string &getResourcePath() {
        return resource_path;
    }
    
    _FORCE_INLINE_ std::string resourcePath(const std::string &path) {
        return resource_path + "/" + path;
    }
    _FORCE_INLINE_ void setStoragePath(const std::string &path) {
        storage_path = path;
        updateStoragePath();
    }
    _FORCE_INLINE_ void setStoragePath(std::string &&path) {
        storage_path = path;
        updateStoragePath();
    }
    _FORCE_INLINE_ const std::string &getStoragePath() {
        return storage_path;
    }
    
    _FORCE_INLINE_ void processDefaultConfig(const gc::Ref<gc::Data> &data);
    bool configGet(const gc::StringName &name, std::string &str) const;
    void configSet(const gc::StringName &name, const std::string &value);
    gc::Ref<gc::Data> resource(const std::string &name);
    
    CLASS_END
}

#endif /* FileSystem_hpp */
