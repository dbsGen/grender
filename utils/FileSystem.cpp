//
//  FileSystem.cpp
//  hirender_iOS
//
//  Created by gen on 29/12/2016.
//  Copyright © 2016 gen. All rights reserved.
//

#include <platforms/android/AssetData.h>
#include "FileSystem.h"

using namespace gr;
using namespace gc;
using namespace std;

void FileSystem::processDefaultConfig(const Ref<Data> &data) {
    if (configs.empty()) {
        size_t length = data->getSize();
        static const int LINE_BEGIN = 0;
        static const int PROCESSING_KEY = 1;
        static const int WAIT_VALUE = 2;
        static const int PROCESSING_VALUE = 3;
        static const int SKIP = 5;
        int status = 0; // 0 line begin, 1 processing key, 3 processing value, 5 skip
        char ch;
        string key, value;
        for (int i = 0; i < length; ++i) {
            data->gets(&ch, i, 1);
            if (ch == 0) {
                break;
            }
            switch (status) {
                case LINE_BEGIN:
                {
                    if (ch == ' ' || ch == '\t') {
                    }else if (ch == '#') {
                        status = SKIP;
                    }else {
                        key.clear();
                        key.push_back(ch);
                        status = PROCESSING_KEY;
                    }
                }
                    break;
                case PROCESSING_KEY: {
                    if (ch == '=') {
                        while (key.back() == ' ' || key.back() == '\t') {
                            key.pop_back();
                        }
                        value.clear();
                        status = WAIT_VALUE;
                    }else {
                        key.push_back(ch);
                    }
                }
                    break;
                case WAIT_VALUE: {
                    if (ch == ' ' || ch == '\t') {
                    }else {
                        value.clear();
                        value.push_back(ch);
                        status = PROCESSING_VALUE;
                    }
                }
                    break;
                case PROCESSING_VALUE: {
                    if (ch == '\n' || ch == 0) {
                        while (value.back() == ' ' || value.back() == '\t') {
                            value.pop_back();
                        }
                        configs[StringName(key.c_str())] = value;
                        status = LINE_BEGIN;
                        if (ch == 0) break;
                    }else {
                        value.push_back(ch);
                    }
                }
                    break;
                    
                    
                default:
                    break;
            }
        }
        if (status == PROCESSING_VALUE && !key.empty() && !value.empty()) {
            configs[StringName(key.c_str())] = value;
        }
    }
}


bool FileSystem::configGet(const StringName &name, string &str) const {
    auto it = configs.find(name);
    if (it != configs.end()) {
        str = it->second;
        return true;
    }
    return false;
}

void FileSystem::configSet(const StringName &name, const string &value) {
    configs[name] = value;
    save();
}

void FileSystem::updateStoragePath() {
    if (!storage_path.empty()) {
        string p = storage_path + "/config.dat";
        processDefaultConfig(new FileData(p));
    }
}

void FileSystem::save() const {
    if (!storage_path.empty()) {
        string p = storage_path + "/config.dat";
        FILE *f = fopen(p.c_str(), "w");
        if (!f) return;
        string line_str;
        for (auto it = configs.begin(), _e = configs.end(); it != _e; ++it) {
            line_str.clear();
            line_str += it->first.str();
            line_str += "=";
            line_str += it->second;
            fprintf(f, "%s\n", line_str.c_str());
        }
        fclose(f);
    }
}

Ref<Data> FileSystem::resource(const string &name) {
#if defined(__APPLE__)
    return new FileData(resourcePath(name));
#elif defined(__ANDROID__)
    return new AssetData(name.c_str());
#else
    return Ref<Data>::null();
#endif
}
