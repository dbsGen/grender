//
// Created by gen on 16/9/9.
//

#ifdef __ANDROID__
#include <script/java/JScript.h>
#include <cstring>
#include "AssetData.h"

using namespace gr;
using namespace gscript;

AAssetManager *AssetData::asset_manager = NULL;

AssetData::AssetData(const char *name) : AssetData() {
    resource_path = name;
}

AssetData::~AssetData() {
    close();
}

AssetData::AssetData() : asset(NULL) {}

long AssetData::getSize() const {
    if (!asset) {
        if (asset_manager && !resource_path.empty()) {
            AssetData *data = const_cast<AssetData*>(this);
            data->asset = AAssetManager_open(asset_manager, resource_path.c_str(), AASSET_MODE_BUFFER);
        }else {
            return 0;
        }
    }
    if (!asset) return 0;
    return AAsset_getLength(asset);
}

const void *AssetData::getBuffer() {
    if (!asset) {
        if (asset_manager && !resource_path.empty()) {
            asset = AAssetManager_open(asset_manager, resource_path.c_str(), AASSET_MODE_BUFFER);
        }else {
            return 0;
        }
    }
    return AAsset_getBuffer(asset);
}

bool AssetData::gets(void *chs, long start, long len) {
    long size = getSize();
    const char *buffer = (const char *)getBuffer();
    if (start + len <= size) {
        memcpy(chs, buffer + start, len);
        return true;
    }else {
        LOG(e, "Range out of bound.");
        return false;
    }
}

bool AssetData::empty() const  {
    return resource_path.empty();
}

void AssetData::close() {
    if (asset) {
        AAsset_close(asset);
        asset = NULL;
    }
}

#endif //__ANDROID__