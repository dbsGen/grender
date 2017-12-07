//
// Created by gen on 13/04/2017.
//

#include "ModelLoader.h"

using namespace hirender;
using namespace hicore;

typedef uint8_t ModelType;

enum {
    TYPE_OBJECT = 1,
    TYPE_ATTRIBUTE,
    TYPE_CONTENT,
    TYPE_END_OBJECT
};

bool ModelLoader::read(Data *data, void *buffer, size_t len) {
    if (data->gets(buffer, cursor, len)) {
        cursor += len;
        return true;
    }
    return false;
}

uint64_t ModelLoader::readLength(Data *data) {
    uint64_t len = 0;
    if (read(data, &len, sizeof(uint64_t))) {
        return len;
    }
    return 0;
}

void ModelLoader::clear() {
    for (auto it = meshes.begin(), _e = meshes.end(); it != _e; ++it) {
        delete (Ref<Mesh>*)it->second;
    }
    meshes.clear();
}

void ModelLoader::load(Data *data) {
    cursor = 0;
    char key[3];
    if (read(data, key, 3) && key[0] == 'H' && key[1] == 'M' && key[2] == 'D') {
        if (read(data, &version, sizeof(float))) {
            ModelType type;
            string tmp;
            variant_vector ns;
            vector<int> indexes;

            if (read(data, &type, sizeof(ModelType)) && type == TYPE_OBJECT) {
                while (true) {
                    if (type == TYPE_OBJECT) {
                        uint64_t s = readLength(data);
                        tmp.resize(s);
                        read(data, &tmp[0], s);
                        StringName name(tmp.c_str());
                        Ref<Mesh> mesh = new Mesh;
                        mesh->setColor(HColor(1,1,1,1), Mesh::DEFAULT_COLOR_NAME);
                        while (true) {
                            if (read(data, &type, sizeof(ModelType))) {
                                switch (type) {
                                    case TYPE_OBJECT: {
                                        goto end_loop;
                                    }
                                    case TYPE_ATTRIBUTE: {
                                        s = readLength(data);
                                        tmp.resize(s);
                                        read(data, &tmp[0], s);
                                        StringName att_name(tmp.c_str());
                                        if (att_name == Mesh::INDEX_NAME) {
                                            if (read(data, &type, sizeof(ModelType)) && type == TYPE_CONTENT) {
                                                s = readLength(data);
                                                indexes.resize(s / sizeof(int));
                                                if (!read(data, indexes.data(), s)) {
                                                    LOG(w, "Load model failed in %s(%s)", name.str(), att_name.str());
                                                    return;
                                                }
                                            }
                                        }else {
                                            if (read(data, &type, sizeof(ModelType)) && type == TYPE_CONTENT) {
                                                s = readLength(data);
                                                AttrVector *attr = mesh->getAttribute(att_name);
                                                attr->resize(s);
                                                if (!read(data, attr->buffer(), s)) {
                                                    LOG(w, "Load model failed in %s(%s)", name.str(), att_name.str());
                                                    return;
                                                }
                                            }
                                        }
                                    }
                                        break;
                                    case TYPE_END_OBJECT: {
                                        // TODO: 用来做层级关系的
                                        LOG(i, "Object end");
                                    }
                                        break;
                                    default:
                                        LOG(w, "Unkown type!");
                                        break;
                                }
                            }else {
                                goto end_loop;
                            }
                            continue;
                            end_loop:
                            if (indexes.size() > 0) {
                                AttrVector *attr = mesh->getAttribute(Mesh::DEFAULT_UV1_NAME);
                                if (attr->getSize() == 0) {
                                    attr->setVector2f(Vector2f(0,0), 0);
                                    attr->setRepeat(true);
                                    attr->update();
                                }

                                mesh->commit(indexes);
                                meshes[(void*)name] = new Ref<Mesh>(mesh);
                                ns.push_back(name);
                            }
                            break;
                        }

                    }else {
                        break;
                    }
                }
            }
            names = Array(ns);
            return;
        }
    }

    LOG(w, "Model load failed, it is not a model.");
    return;
}

const Ref<Mesh>& ModelLoader::getMesh(const StringName &name) {
    auto it = meshes.find((void*)name);
    if (it != meshes.end()) {
        return *(Ref<Mesh>*)it->second;
    }
    return Ref<Mesh>::null();
}

ModelLoader::~ModelLoader() {
    clear();
}