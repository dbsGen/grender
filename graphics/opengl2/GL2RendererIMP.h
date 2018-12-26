//
//  GL2RendererIMP.hpp
//  hirender_iOS
//
//  Created by gen on 16/6/3.
//  Copyright © 2016年 gen. All rights reserved.
//

#ifndef HIGRAPHICS_GL2_RENDERER_IMP_H
#define HIGRAPHICS_GL2_RENDERER_IMP_H

#include <Renderer.h>
#include <object/Canvas.h>
#include <set>

#include <graphics/graphics_define.h>

namespace gg {
    class GL2FrameBuffer;
    
    CLASS_BEGIN_N(GL2RendererIMP, gr::RendererIMP)

    private:
        CLASS_BEGIN_0_N(GL2VirtualMachine)
        private:
            struct Compare {
                gr::Camera *camera;
                _FORCE_INLINE_ bool operator() (void *o1, void *o2) const {
                    return camera->sortCompare((gr::Object3D*)o1, (gr::Object3D*)o2);
                }
                _FORCE_INLINE_ Compare(gr::Camera *camera) {
                    this->camera = camera;
                }
            };
    
            gr::Camera *camera;
            gr::Canvas *canvas;
            pointer_map reload;
            std::list<gc::Ref<gr::Object3D> > add;
            std::list<gc::Ref<gr::Object3D> > remove;
    
            pointer_list sub_machines;

            pointer_list items;
            GL2RendererIMP *renderer;
            GL2FrameBuffer *frame_buffer;

            void addToSubMachine(const gc::Ref<gr::Object3D> &object, gr::Canvas *canvas);
            void _clear();
            void _changeMaterial(gr::Object3D *object, gr::Material *old_mat);
            void _add(const gc::Ref<gr::Object3D> &object, gr::Canvas *canvas);
            void _remove(const gc::Ref<gr::Object3D> &object);
            void addObjects(const std::list<gc::Ref<gr::Object3D> > &objects, gr::Canvas *canvas);
    
            pointer_map     drawables;
            pointer_list    drawables_list;
            bool drawables_updated;
            void addDrawItem(gr::Object3D *object);
            void removeDrawItem(gr::Object3D *object);
            void reloadDrawItem(void *old_key, gr::Object3D *object);
            void removeDrawItemWithKey(void *key, gr::Object3D *object);
            void *keyOfObject(gr::Object3D *object);
    
            friend class GL2RendererIMP;

        public:
            void reset(const std::list<gc::Ref<gr::Object3D> > &objects);
            void step();
    
            GL2VirtualMachine *findSubMachine(gr::Canvas *canvas);
    
            _FORCE_INLINE_ GL2VirtualMachine() : frame_buffer(NULL), camera(NULL), canvas(NULL), drawables_updated(false) {}
            ~GL2VirtualMachine();

        CLASS_END

        struct VMCompare {
            _FORCE_INLINE_ bool operator() (void *o1, void *o2) {
                GL2VirtualMachine *vm1 = (GL2VirtualMachine*)o1,  *vm2 = (GL2VirtualMachine*)o2;
                return (vm1->camera->getOuputTarget() && !vm2->camera->getOuputTarget()) || (vm1->camera->getDepth() < vm2->camera->getDepth());
            }
        };
    
        void pushCamera(gr::Camera *camera);
        void removeCamera(gr::Camera *camera);
        pointer_list machines;
    
        void pushFrameBuffer(GL2FrameBuffer *frame_buffer);
        GL2FrameBuffer *popFrameBuffer();
        pointer_list frame_buffer_stack;
    
        void deleteFrameBuffer();
        void createFrameBuffer();

        bool initialized;
        GLuint default_frame_buffer;
        void addWithList(const gc::Ref<gr::Object3D> &object, gr::Canvas *canvas, bool top);

    protected:

        virtual void prepare();
        virtual void add(const gc::Ref<gr::Object3D> &object);
        virtual void remove(const gc::Ref<gr::Object3D> &object);
        virtual void reload(gr::Object3D *object, gr::Material *old_mat);
        virtual void maskChanged(const gc::Ref<gr::Object3D> &object, gr::Mask from, gr::Mask to);
        virtual void hitMaskChanged(const gc::Ref<gr::Object3D> &object, gr::Mask from, gr::Mask to);

        virtual void reload();
        virtual void render();
    virtual void updateSize(const gr::Size &size);
        friend class GL2VirtualMachine;

    public:
        _FORCE_INLINE_ GL2RendererIMP() : initialized(false) {}
        ~GL2RendererIMP();
        _FORCE_INLINE_ void setDefaultFrameBuffer(GLuint buffer) { default_frame_buffer = buffer; }

    CLASS_END
}

#endif // HIGRAPHICS_GL2_RENDERER_IMP_H
