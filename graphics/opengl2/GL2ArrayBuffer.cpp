//
//  GL2ArrayBuffer.cpp
//  hirender_iOS
//
//  Created by gen on 01/01/2017.
//  Copyright © 2017 gen. All rights reserved.
//

#include "GL2ArrayBuffer.h"
#include <object/Object3D.h>
#include "../math/Math.hpp"
#include "GL2MeshIMP.h"
#include <thread>

using namespace gr;
using namespace gc;
using namespace gg;
using namespace std;

#define SIZE(SIZE, COUNT, TYPE) size_t SIZE = 0; int COUNT = 0; GLenum t_type; \
switch (TYPE) {\
case Shader::Vecter2:\
SIZE = sizeof(float);\
COUNT = 2; \
t_type = GL_FLOAT; \
break;\
case Shader::Vecter3:\
SIZE = sizeof(float);\
COUNT = 3; \
t_type = GL_FLOAT; \
break;\
case Shader::Vecter4:\
SIZE = sizeof(float);\
COUNT = 4; \
t_type = GL_FLOAT; \
break;\
case Shader::Int:\
case Shader::Long:\
SIZE = sizeof(int);\
COUNT = 1; \
t_type = GL_INT;\
break;\
case Shader::Float:\
case Shader::Double:\
SIZE = sizeof(float);\
COUNT = 1; \
t_type = GL_FLOAT;\
break;\
\
default:\
break;\
}

struct GL2BufferData{
    size_t offset_from;
    size_t length;
    Object3D *pose_target;
    GL2AttrVector *attr_vec;
    
    GL2BufferData(size_t offset_from, size_t length, GL2AttrVector *vec, Object3D *pose_target = NULL) {
        this->offset_from = offset_from;
        this->length = length;
        this->pose_target = pose_target;
        attr_vec = vec;
    }
};

void GL2ArrayBuffer::bufferChanged(void *sender, void *send_data, void *data) {
    GL2AttrVector *avec = (GL2AttrVector*)sender;
    GL2ArrayBuffer *abuffer = (GL2ArrayBuffer*)data;
    GL2BufferData *mc = (GL2BufferData*)send_data;
    
    if (mc->pose_target) {
        abuffer->updatePose(avec, mc->pose_target,
                            mc->offset_from, mc->length);
    }else
        abuffer->update(avec, mc->offset_from, mc->length);
}

void GL2ArrayBuffer::poseChanged(void *sender, void *send_data, void *data) {
    GL2ArrayBuffer *abuffer = (GL2ArrayBuffer*)data;
    GL2BufferData *mc = (GL2BufferData*)send_data;
    
    abuffer->updatePose(mc->attr_vec, mc->pose_target,
                        mc->offset_from, mc->length);
}

void GL2ArrayBuffer::update(gg::GL2AttrVector *vec, size_t offset_start, size_t length) {
    glBindBuffer(GL_ARRAY_BUFFER, glID);
    if (vec->isRepeat() && vec->getSize() > 0) {
        size_t suboff = 0;
        while (suboff + vec->getSize() <= length) {
            glBufferSubData(GL_ARRAY_BUFFER,
                            offset_start + suboff,
                            vec->getSize(),
                            vec->buffer());
        
            suboff += vec->getSize();
        }
        if (suboff < length) {
        
            glBufferSubData(GL_ARRAY_BUFFER,
                            offset_start + suboff,
                            length - suboff,
                            vec->buffer());
        }
    }else {
        if (vec->getSize() < length) {
            void *nb = malloc(length);
            memset(nb, 0, length);
            memcpy(nb, vec->buffer(), vec->getSize());
            glBufferSubData(GL_ARRAY_BUFFER, offset_start, length, nb);
            free(nb);
        }else {
            glBufferSubData(GL_ARRAY_BUFFER, offset_start,
                            min(length, (size_t)vec->getSize()),
                            vec->buffer());
        }
    }
}

void GL2ArrayBuffer::updatePose(gg::GL2AttrVector *vec, void *pose_target,
                                size_t offset_start, size_t length) {
    if (glID) {
        Object3D *pt = (Object3D*)pose_target;
        size_t vec_count = pt->getMesh()->getVertexesCount();
        if (single) {
            glBindBuffer(GL_ARRAY_BUFFER, glID);
            glBufferSubData(GL_ARRAY_BUFFER, offset_start, length, vec->buffer());
        }else {
            char *buffer = (char*)malloc(length);
            for (int i = 0; i < vec_count; ++i) {
                Vector3f v3((float*)(vec->buffer() + i * sizeof(float) * 3));
                v3 *= pt->getGlobalPose();
                float *fb = (float*)(buffer + i * sizeof(float) * 3);
                fb[0] = v3.v[0];
                fb[1] = v3.v[1];
                fb[2] = v3.v[2];
            }
            glBindBuffer(GL_ARRAY_BUFFER, glID);
            glBufferSubData(GL_ARRAY_BUFFER, offset_start, length, buffer);
            free(buffer);
        }
    }
}

void GL2ArrayBuffer::bind(const StringName &prop_name,
                          Shader::Type prop_type,
                          const pointer_list &objects,
                          size_t count, GLuint index) {
    SIZE(unit_size, unit_count, prop_type)
    if (!glID) {
        size = totle_size = count * unit_size * unit_count;
        uint8_t *buffer = (uint8_t*)malloc(totle_size);
        memset(buffer, 0, totle_size);
        size_t offset = 0;
        size_t list_count = objects.size();
        single = list_count <= 1;
        for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
            Object3D * obj = (Object3D *)*it;
            GL2AttrVector *attr_vector = (GL2AttrVector *)obj->getMesh()->getAttribute(prop_name);
            size_t vertex_count = obj->getMesh()->getVertexesCount();
            size_t attr_length = vertex_count * unit_size * unit_count;
            bool is_pose = obj->getMesh()->getVertexName() == prop_name;
            if (!obj->isStatic()) {
                attr_vector->addCallback(this, GL2ArrayBuffer::bufferChanged,
                                         new GL2BufferData(offset, attr_length, attr_vector, is_pose ? obj : NULL));
                attrs.push_back(attr_vector);
            }
            if (is_pose) {
                if (single) {
                    memcpy(buffer + offset, attr_vector->buffer(), min(attr_length, (size_t)attr_vector->getSize()));
                }else {
                    for (int i = 0; i < vertex_count; ++i) {
                        Vector3f v3((float*)(attr_vector->buffer() + i * sizeof(float) * 3));
                        v3 *= obj->getGlobalPose();
                        float *fb = (float*)(buffer + offset + i * sizeof(float) * 3);
                        fb[0] = v3.v[0];
                        fb[1] = v3.v[1];
                        fb[2] = v3.v[2];
                    }
                }
                if (!obj->isStatic() && !single) {
                    obj->addPoseCallback(this, GL2ArrayBuffer::poseChanged, new GL2BufferData(offset, attr_length, attr_vector, obj));
                    pose_objects.push_back(obj);
                }
            }else {
                if (attr_vector->isRepeat() && attr_vector->getSize()) {
                    size_t suboff = 0;
                    while (suboff + attr_vector->getSize() <= attr_length) {
                        memcpy(buffer + offset + suboff,
                               attr_vector->buffer(),
                               attr_vector->getSize());
                        suboff += attr_vector->getSize();
                    }
                    if (suboff < attr_length) {
                        memcpy(buffer + offset + suboff,
                               attr_vector->buffer(),
                               attr_length - suboff);
                    }
                }else {
                    memcpy(buffer + offset, attr_vector->buffer(), min(attr_length, (size_t)attr_vector->getSize()));
                }
            }
            offset += attr_length;
        }
        glGenBuffers(1, &glID);
        glBindBuffer(GL_ARRAY_BUFFER, glID);
        glBufferData(GL_ARRAY_BUFFER, totle_size, buffer, GL_DYNAMIC_DRAW);
        free(buffer);
        
    }
    if (glID) {
        glBindBuffer(GL_ARRAY_BUFFER, glID);
        glVertexAttribPointer(index, unit_count, t_type, GL_FALSE, unit_size * unit_count, (GLvoid*)NULL);
    }
}

void GL2ArrayBuffer::clear() {
    if (glID) {
        glDeleteBuffers(1, &glID);
        glID = 0;
        size = 0;
        totle_size = 0;
        single = true;
        for (auto it = attrs.begin(), _e = attrs.end(); it != _e; ++it) {
            GL2BufferData*data = (GL2BufferData*)((GL2AttrVector*)*it)->removeCallback(this);
            if (data) delete data;
        }
        attrs.clear();
        for (auto it = pose_objects.begin(), _e = pose_objects.end(); it != _e; ++it) {
            GL2BufferData*data = (GL2BufferData*)((Object3D*)*it)->removePoseCallback(this);
            if (data) delete data;
        }
        pose_objects.clear();
    }
}

GL2ArrayBuffer::~GL2ArrayBuffer() {
    clear();
}

//void GL2ArrayBuffer::draw(const StringName &prop_name,
//                          hirender::Shader::Type prop_type,
//                          const pointer_list &objects,
//                          size_t count, GLuint index) {
//    SIZE(unit_size, unit_count, prop_type)
//    size_t nsize = count * unit_size * unit_count;
//    if (glID && totle_size < nsize) {
//        glDeleteBuffers(1, &glID);
//        glID = 0;
//    }
//    if (!glID) {
//        size = totle_size = nsize;
//        uint8_t *buffer = (uint8_t*)malloc(totle_size);
//        memset(buffer, 0, totle_size);
//        size_t offset = 0;
//        size_t list_count = objects.size();
//        for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
//            Object * obj = (Object *)*it;
//            GL2AttrVector *attr_vector = (GL2AttrVector *)obj->getMesh()->getAttribute(prop_name);
//            size_t vertex_count = obj->getMesh()->getVertexesCount();
//            size_t attr_length = vertex_count * unit_size * unit_count;
//            if (obj->getMesh()->getVertexName() == prop_name && list_count > 1) {
//                for (int i = 0; i < vertex_count; ++i) {
//                    Vector3f v3((float*)(attr_vector->buffer() + i * sizeof(float) * 4));
//                    v3 *= obj->getGlobalPose();
//                    float *fb = (float*)(buffer + offset + i * sizeof(float) * 4);
//                    fb[0] = v3.v[0];
//                    fb[1] = v3.v[1];
//                    fb[2] = v3.v[2];
//                    fb[3] = 1;
//                }
//            }else {
//                if (attr_vector->isRepeat() && attr_vector->getSize()) {
//                    size_t suboff = 0;
//                    while (suboff + attr_vector->getSize() <= attr_length) {
//                        memcpy(buffer + offset + suboff,
//                               attr_vector->buffer(),
//                               attr_vector->getSize());
//                        suboff += attr_vector->getSize();
//                    }
//                    if (suboff < attr_length) {
//                        memcpy(buffer + offset + suboff,
//                               attr_vector->buffer(),
//                               attr_length - suboff);
//                    }
//                }else {
//                    memcpy(buffer + offset, attr_vector->buffer(), min(attr_length, (size_t)attr_vector->getSize()));
//                }
//            }
//            offset += attr_length;
//        }
//        glGenBuffers(1, &glID);
//        glBindBuffer(GL_ARRAY_BUFFER, glID);
//        glBufferData(GL_ARRAY_BUFFER, totle_size, buffer, GL_DYNAMIC_DRAW);
//        free(buffer);
////        this->objects = objects;
//        
//        if (glID) {
//            glVertexAttribPointer(index, unit_count, t_type, GL_FALSE, unit_size * unit_count, (GLvoid*)NULL);
//        }
//    }else {
////        size_t offset = 0;
////        bool diferent = false;
////        auto nit = objects.begin(), ne = objects.end();
////        auto cit = this->objects.begin(), ce = this->objects.end();
////        size_t nidx = objects.size();
////        glBindBuffer(GL_ARRAY_BUFFER, glID);
////        while (nit != ne) {
////            if (!diferent && *nit != *cit) {
////                diferent = true;
////            }
////            
////            Object * obj = (Object *)*nit;
////            GL2AttrVector *attr_vector = (GL2AttrVector *)obj->getMesh()->getAttribute(prop_name);
////            size_t attr_length = obj->getMesh()->getVertexesCount() * unit_size * unit_count;
////            
////            if (diferent) {
////                if (attr_vector->isRepeat() && attr_vector->getSize()) {
////                    size_t suboff = 0;
////                    while (suboff + attr_vector->getSize() <= attr_length) {
////                        
////                        glBufferSubData(GL_ARRAY_BUFFER,
////                                        offset + suboff,
////                                        attr_vector->getSize(),
////                                        attr_vector->buffer());
////                        
////                        suboff += attr_vector->getSize();
////                    }
////                    if (suboff < attr_length) {
////                        
////                        glBufferSubData(GL_ARRAY_BUFFER,
////                                        offset + suboff,
////                                        attr_length - suboff,
////                                        attr_vector->buffer());
////                    }
////                }else {
////                    if (attr_vector->getSize() < attr_length) {
////                        void *nb = malloc(attr_length);
////                        memset(nb, 0, attr_length);
////                        memcpy(nb, attr_vector->buffer(), attr_vector->getSize());
////                        glBufferSubData(GL_ARRAY_BUFFER, offset, attr_length, nb);
////                        free(nb);
////                    }else {
////                        glBufferSubData(GL_ARRAY_BUFFER, offset,
////                                        min(attr_length, (size_t)attr_vector->getSize()),
////                                        attr_vector->buffer());
////                    }
////                }
////                obj->getMesh()->getIMP()->resetPoseUpdated(obj);
////            }else {
////                if (obj->getMesh()->getVertexName() == prop_name && obj->getMesh()->getIMP()->isPoseUpdated(obj)) {
////                    if (nidx > 1) {
////                        size_t vertex_count = obj->getMesh()->getVertexesCount();
////                        size_t buffer_length = sizeof(float) * 4 * vertex_count;
////                        float *buffer = (float*)malloc(buffer_length);
////                        for (int i = 0; i < vertex_count; ++i) {
////                            Vector3f v3((float*)(attr_vector->buffer() + i * sizeof(float) * 4));
////                            v3 *= obj->getGlobalPose();
////                            float *fb = buffer + i * 4;
////                            fb[0] = v3.v[0];
////                            fb[1] = v3.v[1];
////                            fb[2] = v3.v[2];
////                            fb[3] = 1;
////                        }
////                        
////                        glBufferSubData(GL_ARRAY_BUFFER,
////                                        offset,
////                                        buffer_length,
////                                        buffer);
////                        free(buffer);
////                    }
////                    obj->getMesh()->getIMP()->resetPoseUpdated(obj);
////                }
////            }
////            
////            offset += attr_length;
////            ++nit;
////            if (cit != ce) ++cit;
////        }
////        size = offset;
////        if (!diferent && objects.size() != this->objects.size()) {
////            diferent = true;
////        }
////        if (diferent) this->objects = objects;
//    }
//}

void GL2ArrayBuffer::drawIndexes(const std::list<void *> &objects, size_t count) {
    size_t nsize = count * sizeof(int);
    
    if (glID && totle_size < nsize) {
        glDeleteBuffers(1, &glID);
        glID = 0;
    }
    if (!glID) {
        size = totle_size = nsize;
        uint8_t *buffer = (uint8_t*)malloc(totle_size);
        memset(buffer, 0, totle_size);
        size_t offset = 0;
        size_t vertex_count = 0;
        for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
            Object3D * obj = (Object3D *)*it;
            GL2AttrVector *attr_vector = (GL2AttrVector *)obj->getMesh()->getAttribute(Mesh::INDEX_NAME);
            size_t total = obj->getMesh()->getIndexesCount();
            size_t attr_length = total * sizeof(int);
            for (int i = 0, t = min(total,  (size_t)attr_vector->getSize() / sizeof(int)); i < t; ++i) {
                int *ob = (int*)(buffer + offset);
                int *ib = (int*)attr_vector->buffer();
                ob[i] = ib[i] + vertex_count;
            }
            
            vertex_count += obj->getMesh()->getVertexesCount();
            offset += attr_length;
        }
        glGenBuffers(1, &glID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totle_size,
                     buffer,
                     GL_DYNAMIC_DRAW);
        free(buffer);
//        this->objects = objects;
    }else {
//        size_t offset = 0;
//        bool diferent = false;
//        auto nit = objects.begin(), ne = objects.end();
//        auto cit = this->objects.begin(), ce = this->objects.end();
//        size_t vertex_count = 0;
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
//        uint8_t *nb = NULL;
//        size_t m_length = 0;
//        while (nit != ne) {
//            if (!diferent && *nit != *cit) {
//                diferent = true;
//            }
//            
//            Object * obj = (Object *)*nit;
//            GL2AttrVector *attr_vector = (GL2AttrVector *)obj->getMesh()->getAttribute(Mesh::INDEX_NAME);
//            size_t idx_total = obj->getMesh()->getIndexesCount();
//            size_t attr_length = idx_total * sizeof(int);
//            
//            if (diferent) {
//                if (m_length) {
//                    if (m_length > attr_length) {
//                        nb = (uint8_t*)realloc(nb, attr_length);
//                        m_length = attr_length;
//                    }
//                }else {
//                    nb = (uint8_t*)malloc(attr_length);
//                    m_length = attr_length;
//                }
//                memset(nb, 0, attr_length);
//                for (int i = 0, t = min(idx_total,  (size_t)attr_vector->getSize() / sizeof(int)); i < t; ++i) {
//                    int *ob = (int*)nb;
//                    int *ib = (int*)attr_vector->buffer();
//                    ob[i] = ib[i] + vertex_count;
//                }
//                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, attr_length, nb);
//            }
//            
//            offset += attr_length;
//            vertex_count += obj->getMesh()->getVertexesCount();
//            ++nit;
//            if (cit != ce) ++cit;
//        }
//        if (nb) free(nb);
//        size = offset;
//        if (!diferent && objects.size() != this->objects.size()) {
//            diferent = true;
//        }
//        if (diferent) this->objects = objects;
    }
    
    if (glID) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
        glDrawElements(GL_TRIANGLES, size / sizeof(int), GL_UNSIGNED_INT, 0);
    }
}
