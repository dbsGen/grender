//
//  GL2RendererIMP.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/3.
//  Copyright © 2016年 gen. All rights reserved.
//

#include <math/Math.hpp>
#include "include_opengl2.h"
#include "GL2RendererIMP.h"
#include "GL2MeshIMP.h"
#include "GL2ArrayBuffer.h"
#include "GL2MaterialIMP.h"
#include "GL2FrameBuffer.h"
#include <object/Canvas.h>
#include <texture/RenderTexture.h>

using namespace gg;
using namespace gr;
using namespace gc;
using namespace std;

namespace gg {
    struct DrawItem {
        Material *mat;
        pointer_list    objects;
        pointer_vector  array_buffers;
        uint    vertex_count, index_count;
        bool    dirty;
        
        DrawItem() : mat(NULL), vertex_count(0), index_count(0) {}
        ~DrawItem();
        
        void makeBuffers() {
            const pointer_vector &attrs = mat->getShader()->getAttributes();
            for (auto it = attrs.begin(), _e = attrs.end(); it != _e; ++it) {
                array_buffers.push_back(new GL2ArrayBuffer);
            }
            // add index
            array_buffers.push_back(new GL2ArrayBuffer);
        }
    };
}

DrawItem::~DrawItem() {
    for (auto it = array_buffers.begin(), _e = array_buffers.end(); it != _e; ++it) {
        delete (GL2ArrayBuffer*)*it;
    }
}

int vm_search(const list<Ref<Object3D> > &l, Object3D *t) {
    int count = 0;
    for (auto it = l.begin(), _e = l.end(); it != _e; ++it) {
        if (**it == t) {
            return count;
        }
        count ++;
    }
    return -1;
}
int vm_search(const pointer_list &l, void *t) {
    int count = 0;
    for (auto it = l.begin(), _e = l.end(); it != _e; ++it) {
        if (*it == t) {
            return count;
        }
        count ++;
    }
    return -1;
}

GL2RendererIMP::~GL2RendererIMP() {
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        delete (GL2RendererIMP::GL2VirtualMachine*)*it;
    }
}

void GL2RendererIMP::reload() {
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        ((GL2RendererIMP::GL2VirtualMachine*)*it)->reset(getTarget()->getObjects());
    }
}

void GL2RendererIMP::render() {
//    if (sizeChanged) {
//        const Vector2i &size = getTarget()->getSize();
//        glViewport(0, 0, size.x(), size.y());
//        sizeChanged = false;
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    const HColor &color = getTarget()->getClearColor();
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    machines.sort(VMCompare());
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        ((GL2RendererIMP::GL2VirtualMachine*)*it)->step();
    }
    drawEnd();
}

void GL2RendererIMP::updateSize(const Size &size) {
}

void GL2RendererIMP::pushFrameBuffer(GL2FrameBuffer *frame_buffer) {
    frame_buffer->prepare();
    frame_buffer_stack.push_back(frame_buffer);
}

GL2FrameBuffer *GL2RendererIMP::popFrameBuffer() {
    GL2FrameBuffer *buffer = (GL2FrameBuffer*)frame_buffer_stack.back();
    frame_buffer_stack.pop_back();
    if (frame_buffer_stack.size()) {
        GL2FrameBuffer *last = (GL2FrameBuffer*)frame_buffer_stack.back();
        last->prepare();
    }else {
        glBindFramebuffer(GL_FRAMEBUFFER, default_frame_buffer);
    }
    return buffer;
}

void GL2RendererIMP::pushCamera(Camera *camera) {
    GL2VirtualMachine *vm = new GL2VirtualMachine;
    vm->camera = camera;
    vm->renderer = this;
    vm->addObjects(getTarget()->getObjects(), NULL);
    machines.push_back(vm);
}

void GL2RendererIMP::removeCamera(Camera *camera) {
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        GL2VirtualMachine *vm = (GL2RendererIMP::GL2VirtualMachine*)*it;
        if ((vm->canvas ? (void*)vm->canvas : (void*)vm->camera) == camera) {
            machines.erase(it);
            delete vm;
            return;
        }
    }
}

GL2RendererIMP::GL2VirtualMachine *GL2RendererIMP::GL2VirtualMachine::findSubMachine(Canvas *canvas) {
    if (this->canvas == canvas) {
        return this;
    }
    for (auto it = sub_machines.begin(), _e = sub_machines.end(); it != _e; ++it) {
        GL2RendererIMP::GL2VirtualMachine * sub = ((GL2RendererIMP::GL2VirtualMachine *)*it)->findSubMachine(canvas);
        if (sub) return sub;
    }
    return NULL;
}

void GL2RendererIMP::maskChanged(const Ref<Object3D> &object, Mask from, Mask to) {
    Object3D *parent = object->getParent();
    Canvas *canvas = NULL;
    while (parent) {
        Canvas *cv = parent->cast_to<Canvas>();
        if (cv) {
            canvas = cv;
            break;
        }
        parent = parent->getParent();
    }
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        GL2VirtualMachine *vm = (GL2RendererIMP::GL2VirtualMachine *)*it;
        if (canvas) {
            vm = vm->findSubMachine(canvas);
        }
        Mask hit = vm->canvas ? vm->canvas->getHitMask() : vm->camera->getHitMask();
        if (MaskHit(hit, from) && !MaskHit(hit, to)) {
            vm->_remove(object);
        }else if (!MaskHit(hit, from) && MaskHit(hit, to)) {
            vm->_add(object, canvas);
        }
    }
}

void GL2RendererIMP::hitMaskChanged(const Ref<Object3D> &object, Mask from, Mask to) {
    Canvas *canvas = object->cast_to<Canvas>();
    Camera *camera = object->cast_to<Camera>();
    if (camera) {
        for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
            GL2VirtualMachine *vm = (GL2RendererIMP::GL2VirtualMachine *)*it;
            if ((vm->canvas ? (void*)vm->canvas : (void*)vm->camera) == object) {
                vm->_clear();
                vm->addObjects(getTarget()->getObjects(), NULL);
            }
        }
    }else if (canvas) {
        for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
            GL2VirtualMachine *vm = (GL2RendererIMP::GL2VirtualMachine *)*it;
            vm = vm->findSubMachine(canvas);
            if (canvas) {
                vm->_clear();
                vm->addObjects(canvas->getChildren(), NULL);
            }
        }
    }
}

void GL2RendererIMP::deleteFrameBuffer() {
}

void GL2RendererIMP::createFrameBuffer() {
    
}

void GL2RendererIMP::prepare() {
    if (!initialized) {
        
    }
}

void GL2RendererIMP::GL2VirtualMachine::reset(const list<Ref<Object3D>> &objects) {
    _clear();
    //    auto depthIndex = items.begin();
    for (auto ite = objects.begin(), _e = objects.end(); ite != _e; ++ite) {
        auto o = *ite;
        renderer->add(o);
    }
}

void GL2RendererIMP::GL2VirtualMachine::_changeMaterial(Object3D *object, Material *old_mat) {
    if (object->getInstanceClass()->isTypeOf(Camera::getClass())) {
        return;
    }
    if (vm_search(items, object) != -1 && vm_search(add, object) == -1 && vm_search(remove, object) == -1) {
        auto it = reload.find(object);
        if (it == reload.end())
            reload[object] = old_mat? old_mat : keyOfObject(object);
        drawables_updated = true;
    }
}

void GL2RendererIMP::GL2VirtualMachine::addObjects(const list<Ref<Object3D> > &objects, Canvas *canvas) {
    for (auto it = objects.begin(), _e = objects.end(); it != _e; ++it) {
        const Ref<Object3D> &obj = *it;
        _add(obj, canvas);
        Canvas *cv = obj->cast_to<Canvas>();
        if (cv) {
            addObjects(obj->getChildren(), cv);
        }else
            addObjects(obj->getChildren(), canvas);
    }
}

void GL2RendererIMP::GL2VirtualMachine::addToSubMachine(const Ref<Object3D> &object, Canvas *canvas) {
    for (auto it = sub_machines.begin(), _e = sub_machines.end(); it != _e; ++it) {
        GL2RendererIMP::GL2VirtualMachine *machine = (GL2RendererIMP::GL2VirtualMachine *)*it;
        if (machine->canvas == canvas) {
            machine->_add(object, NULL);
        }else {
            machine->addToSubMachine(object, canvas);
        }
    }
}

void *GL2RendererIMP::GL2VirtualMachine::keyOfObject(Object3D *object) {
    return object->isSingle() ? (void*)object : (void*)*object->getMaterial();
}

void GL2RendererIMP::GL2VirtualMachine::_add(const Ref<Object3D> &object, Canvas *canvas) {
    const Class *cls = object->getInstanceClass();
    if (cls->isTypeOf(Camera::getClass()) || !object->isFinalEnable()) {
        return;
    }
    if (canvas) {
        addToSubMachine(object, canvas);
    }else {
        if (cls->isTypeOf(Canvas::getClass()) &&
            MaskHit(canvas ? canvas->getHitMask() : camera->getHitMask(), object->getMask())) {
            bool exist = false;
            Canvas *can = object->cast_to<Canvas>();
            for (auto it = sub_machines.begin(), _e = sub_machines.end(); it != _e; ++it) {
                if (((GL2RendererIMP::GL2VirtualMachine*)*it)->canvas == can) {
                    exist = true;
                    break;
                }
            }
            if (!exist) {
                GL2VirtualMachine *vm = new GL2VirtualMachine;
                vm->camera = camera;
                vm->canvas = can;
                vm->renderer = renderer;
                sub_machines.push_back(vm);
            }
        }
        if (vm_search(remove, *object) >= 0) {
            remove.remove(object);
            if (reload.find(*object) == reload.end())
                reload[*object] = keyOfObject(*object);
        }else if (vm_search(items, *object) == -1 && vm_search(add, *object) == -1 &&
                  MaskHit(canvas ? canvas->getHitMask() : camera->getHitMask(), object->getMask())) {
            add.push_back(object);
        }
    }
}

void GL2RendererIMP::GL2VirtualMachine::_remove(const Ref<Object3D> &object) {
    if (object->getInstanceClass()->isTypeOf(Camera::getClass())) {
        return;
    }
    if (vm_search(add, *object) >= 0) {
        add.remove(object);
    }else if (vm_search(items, *object) != -1 && vm_search(remove, *object) == -1) {
        remove.push_back(object);
    }
}

void GL2RendererIMP::GL2VirtualMachine::_clear() {
    for (auto it = items.begin(), _e = items.end(); it != _e; ++it) {
        Object3D *obj = (Object3D*)*it;
        renderer->removeObject(obj);
    }
    items.clear();
    for (auto it = drawables.begin(), _e = drawables.end(); it != _e; ++it) {
        delete (DrawItem *)it->second;
    }
    drawables.clear();
    for (auto it = sub_machines.begin(), _e = sub_machines.end(); it != _e; ++it) {
        delete (GL2RendererIMP::GL2VirtualMachine*)*it;
    }
    sub_machines.clear();
}

void GL2RendererIMP::add(const Ref<Object3D> &object) {
    addWithList(object, NULL, true);
}

void GL2RendererIMP::addWithList(const Ref<Object3D> &object, Canvas *canvas, bool top) {
    if (top) {
        Object3D *parent = object->getParent();
        while (parent) {
            if (!canvas) {
                Canvas *cv = parent->cast_to<Canvas>();
                if (cv) {
                    canvas = cv;
                    break;
                }
            }
            parent = parent->getParent();
        }
    }
    const Class *cls = object->getInstanceClass();
    bool is_camera = cls->isTypeOf(Camera::getClass());
    bool is_canvas = cls->isTypeOf(Canvas::getClass());
    Canvas *ori_canvas = canvas;
    if (is_camera) {
        pushCamera(object->cast_to<Camera>());
    }
    if (!is_camera) {
        for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
            ((GL2RendererIMP::GL2VirtualMachine*)*it)->_add(object, canvas);
        }
    }
    
    if (is_canvas) {
        ori_canvas = canvas;
        canvas = object->cast_to<Canvas>();
    }
    const list<Ref<Object3D> > &children = object->getChildren();
    for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
        addWithList(*it, canvas, false);
    }
    if (is_canvas) {
        canvas = ori_canvas;
    }
}

void GL2RendererIMP::remove(const Ref<Object3D> &object) {
    const list<Ref<Object3D> > &children = object->getChildren();
    for (auto it = children.begin(), _e = children.end(); it != _e ; ++it) {
        remove(*it);
    }
    if (object->instanceOf(Camera::getClass())) {
        removeCamera(object->cast_to<Camera>());
    }else {
        for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
            ((GL2RendererIMP::GL2VirtualMachine*)*it)->_remove(object);
        }
    }
}
void GL2RendererIMP::reload(Object3D *object, Material *old_mat) {
    for (auto it = machines.begin(), _e = machines.end(); it != _e; ++it) {
        ((GL2RendererIMP::GL2VirtualMachine*)*it)->_changeMaterial(object, old_mat);
    }
}

struct GL2RendererIMPCompare {
    Camera *camera;
    
    GL2RendererIMPCompare(Camera *camera) {
        this->camera = camera;
    }

    virtual bool operator()(void *o1, void *o2) {
        return camera->sortCompare((Object3D*)o1, (Object3D*)o2);
    }
};

struct GL2RendererIMPDrawItemCompare : GL2RendererIMPCompare {

    GL2RendererIMPDrawItemCompare(Camera *camera) : GL2RendererIMPCompare(camera) {
    }
    virtual bool operator()(void *o1, void *o2) {
        DrawItem *item1 = (DrawItem*)o1, *item2 = (DrawItem*)o2;
        if (item1->mat->getRenderIndex() == item2->mat->getRenderIndex() &&
                item1->objects.size() > 0 && item2->objects.size() > 0) {
            return GL2RendererIMPCompare::operator()(item1->objects.front(), item2->objects.front());
        }else {
            return item1->mat->getRenderIndex() < item2->mat->getRenderIndex();
        }
    }
};

void GL2RendererIMP::GL2VirtualMachine::step() {
    if (!camera || !camera->isFinalEnable() || (canvas && !canvas->isFinalEnable())) return;
    
    for (auto it = sub_machines.begin(), _e = sub_machines.end();
         it != _e; ++it) {
        GL2VirtualMachine *svm = (GL2RendererIMP::GL2VirtualMachine*)*it;
        if (svm->canvas) {
            svm->step();
        }
    }
    bool has_output = false;
    if (camera) {
        has_output = !!camera->getOuputTarget();
        if (has_output) {
            if (!frame_buffer) {
                frame_buffer = new GL2FrameBuffer;
                RenderTexture *tex = camera->getOuputTarget()->cast_to<RenderTexture>();
                if (tex->isResized()) {
                    frame_buffer->clear();
                    tex->resetResized();
                }
                frame_buffer->bind(tex->getIMP());
            }
            renderer->pushFrameBuffer(frame_buffer);
        }else if (canvas) {
            RenderTexture *tex = *canvas->getTexture();
            // size.v[0] = nextPowerOf2((int)size.v[0]);
            // size.v[1] = nextPowerOf2((int)size.v[1]);
            
            canvas->willRender(camera);
            
            if (!frame_buffer) {
                Size size = renderer->getTarget()->getSize();
                tex->setSize(size);
                frame_buffer = new GL2FrameBuffer;
                if (tex->isResized()) {
                    frame_buffer->clear();
                    tex->resetResized();
                }
                frame_buffer->bind(tex->getIMP());
            }
            renderer->pushFrameBuffer(frame_buffer);
        }
        if (camera->isClean()) {
            const Color &color = camera->getCleanColor();
            glClearColor(color.v[0], color.v[1], color.v[2], color.v[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }else return;
    const Size &size = renderer->getTarget()->getSize();
    const Rect &vp = camera->getViewport();
    glViewport((GLint) (size.v[0] * vp.v[0]),
               (GLint) (size.v[1] * vp.v[1]),
               (GLint) (size.v[0] * vp.v[2]),
               (GLint) (size.v[1] * vp.v[3]));

    if (reload.size()) {
        for (auto it = reload.begin(), _e = reload.end(); it != _e; ++it) {
            Object3D *obj = (Object3D*)it->first;
            reloadDrawItem(it->second, obj);
        }
        reload.clear();
    }
    if (add.size()) {
        for (auto it = add.begin(), _e = add.end(); it != _e; ++it) {
            Object3D *obj = **it;
            items.push_back(obj);
            addDrawItem(obj);
            renderer->addObject(obj);
        }
        add.clear();
    }
    if (remove.size()) {
        for (auto it = remove.begin(), _e = remove.end(); it != _e; ++it) {
            Object3D *obj = **it;
            items.remove(**it);
            removeDrawItem(obj);
            renderer->removeObject(obj);
        }
    }

    if (drawables_updated) {
        drawables_list.sort(GL2RendererIMPDrawItemCompare(camera));
        drawables_updated = false;
    }
    for (auto it = drawables_list.begin(), _e = drawables_list.end(); it != _e; ++it) {
        DrawItem *item = (DrawItem *)*it;
        
        item->mat->setProjection(camera->getProjection());
        item->mat->setView(camera->getGlobalPose().inverse());
        item->mat->process();
        if (item->array_buffers.size() == 0)
            item->makeBuffers();
        const pointer_vector &attrs = item->mat->getShader()->getAttributes();
        int count = 0;
       
        if (item->dirty) 
            item->objects.sort(GL2RendererIMPCompare(camera));
        for (auto ait = attrs.begin(), ae = attrs.end(); ait != ae; ++ait) {
            const Shader::Property *attr_prop = (const Shader::Property *)*ait;
            GL2ArrayBuffer *attr_buffer = (GL2ArrayBuffer *)item->array_buffers[count];
            if (item->dirty) attr_buffer->clear();
            attr_buffer->bind(attr_prop->name, attr_prop->type,
                              item->objects, item->vertex_count, count++);
        }
        
        GL2MaterialIMP *mat = item->mat->getIMP()->cast_to<GL2MaterialIMP>();
        size_t obj_count = item->objects.size();
        if (mat) {
            if (obj_count > 1) {
                glUniformMatrix4fv(mat->getTranslateID(), 1, GL_FALSE, Matrix4::identity().m);
            }else if (obj_count == 1){
                Object3D *obj = (Object3D *)item->objects.front();
                glUniformMatrix4fv(mat->getTranslateID(), 1, GL_FALSE, obj->getGlobalPose().m);
            }
        }
        GL2ArrayBuffer *index_buffer = (GL2ArrayBuffer *)item->array_buffers[attrs.size()];
        index_buffer->drawIndexes(item->objects, item->index_count);
        renderer->drawCalled();
        item->dirty = false;
    }
    if (remove.size())
        remove.clear();
    if (camera && (has_output || canvas))
        renderer->popFrameBuffer();
}

GL2RendererIMP::GL2VirtualMachine::~GL2VirtualMachine() {
    _clear();
    if (frame_buffer) delete frame_buffer;
}


void GL2RendererIMP::GL2VirtualMachine::addDrawItem(Object3D *object) {
    if (!object->getMesh() || !object->getMaterial()) return;
    void *key = keyOfObject(object);
    auto it = drawables.find(key);
    DrawItem *item;
    if (it == drawables.end()) {
        item = new DrawItem;
        item->mat = *object->getMaterial();
        drawables[key] = item;
        drawables_list.push_back(item);
        drawables_updated = true;
    }else {
        item = (DrawItem*)it->second;
    }
//    auto oit = item->objects.begin(), oe = item->objects.end();
//    while (oit != oe) {
//        Object *obj = (Object*)*oit;
//        if (camera->sortCompare(object, obj)) {
//            break;
//        }
//        ++oit;
//    }
//    item->objects.insert(oit, object);
    item->objects.push_back(object);
    item->vertex_count += object->getMesh()->getVertexesCount();
    item->index_count += object->getMesh()->getIndexesCount();
    item->dirty = true;
}
void GL2RendererIMP::GL2VirtualMachine::removeDrawItemWithKey(void *key, Object3D *object) {
    auto it = drawables.find(key);
    if (it != drawables.end()) {
        DrawItem *item = (DrawItem*)it->second;
        for (auto oit = item->objects.begin(), oe = item->objects.end(); oit != oe; ++oit) {
            if (object == *oit) {
                item->objects.erase(oit);
                if (item->objects.size() == 0) {
                    delete item;
                    drawables.erase(it);
                    drawables_list.remove(item);
                    drawables_updated = true;
                }
                break;
            }
        }
        item->dirty = true;
    }
}
void GL2RendererIMP::GL2VirtualMachine::removeDrawItem(Object3D *object) {
    if (!object->getMesh() || !object->getMaterial()) return;
    void *key = keyOfObject(object);
    removeDrawItemWithKey(key, object);
}
void GL2RendererIMP::GL2VirtualMachine::reloadDrawItem(void *old_key, Object3D *object) {
    if (!object->getMesh() || !object->getMaterial()) return;
    void *key = keyOfObject(object);
    if (key == old_key) {
        auto it = drawables.find(key);
        if (it != drawables.end()) {
            DrawItem *item = (DrawItem*)it->second;
            item->dirty = true;
        }
    }else {
        removeDrawItemWithKey(old_key, object);
        addDrawItem(object);
    }
}
