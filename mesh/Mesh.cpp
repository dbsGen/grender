//
// Created by gen on 16/5/30.
//

#include "Mesh.h"
#include <graphics/Factory.h>
#include <set>
#include <object/Object.h>
#include <Renderer.h>

#define SIZE(SIZE, COUNT, TYPE) size_t SIZE = 0; int COUNT = 0; \
switch (TYPE) {\
case Shader::Vecter2:\
SIZE = sizeof(float);\
COUNT = 2; \
break;\
case Shader::Vecter3:\
SIZE = sizeof(float);\
COUNT = 3; \
break;\
case Shader::Vecter4:\
SIZE = sizeof(float);\
COUNT = 4; \
break;\
case Shader::Int:\
case Shader::Long:\
SIZE = sizeof(int);\
COUNT = 1; \
break;\
case Shader::Float:\
case Shader::Double:\
SIZE = sizeof(float);\
COUNT = 1; \
break;\
\
default:\
break;\
}

using namespace gr;
using namespace higraphics;


void AttrVector::setVector2f(const Vector2f &v, int idx) {
    setFloat(v.x(), idx * 2);
    setFloat(v.y(), idx * 2 + 1);
}

void AttrVector::setVector3f(const Vector3f &v, int idx) {
    setFloat(v.x(), idx * 3);
    setFloat(v.y(), idx * 3 + 1);
    setFloat(v.z(), idx * 3 + 2);
}

void AttrVector::setVector4f(const Vector4f &v, int idx) {
    setFloat(v.x(), idx * 4);
    setFloat(v.y(), idx * 4 + 1);
    setFloat(v.z(), idx * 4 + 2);
    setFloat(v.w(), idx * 4 + 3);
}

Vector4f AttrVector::getVector4f(int idx) {
    size_t offset = idx * 4 * sizeof(float);
    size_t end = (idx + 1) * 4 * sizeof(float);
    if (b.size() >= end) {
        float *fs = (float*)(b.data() + offset);
        return Vector4f(*fs, *(fs+1), *(fs+2), *(fs+3));
    }
    return Vector4f();
}

Vector3f AttrVector::getVector3f(int idx) {
    size_t offset = idx * 3 * sizeof(float);
    size_t end = (idx + 1) * 3 * sizeof(float);
    if (b.size() >= end) {
        float *fs = (float*)(b.data() + offset);
        return Vector3f(*fs, *(fs+1), *(fs+2));
    }
    return Vector3f();
}

void AttrVector::setFloat(float f, int idx) {
    size_t s = sizeof(float) * idx;
    if (b.size() < s + sizeof(float)) {
        b.resize(s + sizeof(float));
    }
    float *v = (float*)(b.data() + s);
    *v = f;
    dirty = true;
}


void AttrVector::copyBuffer(const void *inbuffer, size_t insize) {
    if (b.size() < insize) {
        b.resize(insize);
    }
    memcpy(b.data(), inbuffer, insize);
    dirty = true;
}

void AttrVector::setInt(int i, int idx) {
    size_t s = sizeof(int) * idx;
    if (b.size() < s + sizeof(int)) {
        b.resize(s + sizeof(int));
    }
    int *v = (int*)(b.data() + s);
    *v = i;
    dirty = true;
}

void AttrVector::addCallback(void *target, ActionCallback callback,
                             void *data) {
    ActionItem *ai = new ActionItem(callback, data);
    callbacks[target] = ai;
}

void *AttrVector::removeCallback(void *target) {
    auto it = callbacks.find(target);
    if (it != callbacks.end()) {
        ActionItem *ai = (ActionItem*)it->second;
        void *data = ai->data;
        delete ai;
        callbacks.erase(it);
        return data;
    }
    return NULL;
}

void AttrVector::onUpdate() {
    for (auto it = callbacks.begin(), _e = callbacks.end(); it != _e; ++it) {
        ActionItem *ai = (ActionItem *)it->second;
        ai->callback(this, ai->data, it->first);
    }
}

void AttrVector::attributeUpdate(void *sender, void *send_data, void *data) {
    AttrVector *av = (AttrVector*)send_data;
    av->onUpdate();
}

void AttrVector::update() {
    if (dirty) {
        Renderer *renderer = Renderer::sharedInstance();
        if (renderer) {
            renderer->doOnMainThread(this, &AttrVector::attributeUpdate);
        }
        dirty = false;
    }
}

AttrVector::~AttrVector() {
    Renderer *renderer = Renderer::sharedInstance();
    if (renderer) {
        renderer->cancelDoOnMainThread(this);
    }
    for (auto it = callbacks.begin(), _e = callbacks.end(); it != _e; ++it) {
        delete (ActionItem *)it->second;
    }
}

const StringName Mesh::INDEX_NAME("INDEXES");
const StringName Mesh::DEFAULT_VERTEX_NAME("A_POSITION");
const StringName Mesh::DEFAULT_COLOR_NAME("A_COLOR");
const StringName Mesh::DEFAULT_UV1_NAME("A_TEX_COORD");
const StringName Mesh::DEFAULT_NORMAL_NAME("A_NORMAL");

Mesh::Mesh() : setuped(false), vertex_name(DEFAULT_VERTEX_NAME), vertex_length(0) {
    imp = (MeshIMP*)Factory::getInstance()->create(MeshIMP::getClass());
    imp->_setTarget(this);
    
    setColor(Vector4f(1, 1, 1, 1));
}

Mesh::~Mesh()  {
    Renderer *renderer = Renderer::sharedInstance();
    if (renderer) {
        renderer->cancelDoOnMainThread(this);
    }
    for (auto it = attrs_map.begin(); it != attrs_map.end(); ++it) {
        delete (AttrVector *)it->second;
    }
    delete imp;
}

void Mesh::setColor(const HColor &color, const StringName &name) {
    AttrVector *ca = getAttribute(name);
    if (ca) {
        ca->setRepeat(true);
        ca->setVector4f(color, 0);
        ca->update();
    }
}

HColor Mesh::getColor(const StringName &name) {
    AttrVector *ca = getAttribute(name);
    if (ca) {
        return ca->getVector4f(0);
    }
    return HColor();
}

AttrVector *Mesh::getIndexesAttr() {
    return getAttribute(INDEX_NAME);
}

AttrVector *Mesh::getAttribute(const StringName &name) {
    auto it = attrs_map.find(name);
    if (it == attrs_map.end()) {
        AttrVector *attr = imp->newAttribute();
        attrs_map[name] = attr;
        return attr;
    }else {
        return (AttrVector *)it->second;
    }
}

void Mesh::push(const Vector3f &v3) {
    if (setuped) return;
    AttrVector *attr = getAttribute(vertex_name);
    attr->setFloat(v3.x(), vertex_length * 3);
    attr->setFloat(v3.y(), vertex_length * 3 + 1);
    attr->setFloat(v3.z(), vertex_length * 3 + 2);
    ++vertex_length;
}

void Mesh::setVertex(const Vector3f &v3, int i) {
    AttrVector *attr = getAttribute(vertex_name);
    if (attr->getSize() >= (i+1)*3) {
        attr->setVector3f(Vector3f(v3.x(), v3.y(), v3.z()), i);
    }else {
        LOG(e, "set vertex failed , %d is out of index.", i);
    }
}

const b8_vector &Mesh::getVertexes() {
    AttrVector *ca = getAttribute(vertex_name);
    return ca->getVector();
}

const b8_vector &Mesh::getIndexes() {
    AttrVector *ca = getAttribute(INDEX_NAME);
    return ca->getVector();
}

size_t Mesh::getVertexesCount() {
    return vertex_length;
}

size_t Mesh::getIndexesCount() {
    AttrVector *ca = getAttribute(INDEX_NAME);
    return ca->getSize() / sizeof(int);
}

void Mesh::commit(const vector<int> &indexes) {
    if (setuped) return;
    if (vertex_length == 0) {
        AttrVector *attr = getAttribute(getVertexName());
        vertex_length = attr->getSize() / sizeof(float) / 3;
    }
    setuped = true;
    AttrVector *attr = getIndexesAttr();
    attr->copyBuffer(indexes.data(), indexes.size() * sizeof(int));
}

void Mesh::_copy(const HObject *other) {
    const Mesh *om = (const Mesh*)other;
    vertex_name = om->vertex_name;
    vertex_length = om->vertex_length;
    setuped = om->setuped;
    for (auto it = om->attrs_map.begin(), _e = om->attrs_map.end(); it != _e; ++it) {
        AttrVector *vec = getAttribute(it->first);
        AttrVector *ovec = (AttrVector *)it->second;
        vec->repeat = ovec->repeat;
        vec->b = ovec->b;
        vec->dirty = true;
        vec->update();
    }
}

void Mesh::setVertexName(const StringName &name) {
    if (setuped) {
        LOG(e, "Can not set vertex name after commit.");
        return;
    }
    vertex_name = name;
}
