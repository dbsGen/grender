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

using namespace gr;

namespace higraphics {
    class GL2FrameBuffer;
    
    CLASS_BEGIN_N(GL2RendererIMP, RendererIMP)

    private:
        CLASS_BEGIN_0_N(GL2VirtualMachine)
        private:
            struct Compare {
                Camera *camera;
                _FORCE_INLINE_ bool operator() (void *o1, void *o2) const {
                    return camera->sortCompare((Object*)o1, (Object*)o2);
                }
                _FORCE_INLINE_ Compare(Camera *camera) {
                    this->camera = camera;
                }
            };
    
            Camera *camera;
            Canvas *canvas;
            pointer_map reload;
            list<Ref<Object> > add;
            list<Ref<Object> > remove;
    
            pointer_list sub_machines;

            pointer_list items;
            GL2RendererIMP *renderer;
            GL2FrameBuffer *frame_buffer;

            void addToSubMachine(const Ref<Object> &object, Canvas *canvas);
            void _clear();
            void _changeMaterial(Object *object, Material *old_mat);
            void _add(const Ref<Object> &object, Canvas *canvas);
            void _remove(const Ref<Object> &object);
            void addObjects(const list<Ref<Object> > &objects, Canvas *canvas);
    
            pointer_map     drawables;
            pointer_list    drawables_list;
            bool drawables_updated;
            void addDrawItem(Object *object);
            void removeDrawItem(Object *object);
            void reloadDrawItem(void *old_key, Object *object);
            void removeDrawItemWithKey(void *key, Object *object);
            void *keyOfObject(Object *object);
    
            friend class GL2RendererIMP;

        public:
            void reset(const list<Ref<Object> > &objects);
            void step();
    
            GL2VirtualMachine *findSubMachine(Canvas *canvas);
    
            _FORCE_INLINE_ GL2VirtualMachine() : frame_buffer(NULL), camera(NULL), canvas(NULL), drawables_updated(false) {}
            ~GL2VirtualMachine();

        CLASS_END

        struct VMCompare {
            _FORCE_INLINE_ bool operator() (void *o1, void *o2) {
                GL2VirtualMachine *vm1 = (GL2VirtualMachine*)o1,  *vm2 = (GL2VirtualMachine*)o2;
                return (vm1->camera->getOuputTarget() && !vm2->camera->getOuputTarget()) || (vm1->camera->getDepth() < vm2->camera->getDepth());
            }
        };
    
        void pushCamera(Camera *camera);
        void removeCamera(Camera *camera);
        pointer_list machines;
    
        void pushFrameBuffer(GL2FrameBuffer *frame_buffer);
        GL2FrameBuffer *popFrameBuffer();
        pointer_list frame_buffer_stack;
    
        void deleteFrameBuffer();
        void createFrameBuffer();

        bool initialized;
        GLuint default_frame_buffer;
        void addWithList(const Ref<Object> &object, Canvas *canvas, bool top);

    protected:

        virtual void prepare();
        virtual void add(const Ref<Object> &object);
        virtual void remove(const Ref<Object> &object);
        virtual void reload(Object *object, Material *old_mat);
        virtual void maskChanged(const Ref<Object> &object, Mask from, Mask to);
        virtual void hitMaskChanged(const Ref<Object> &object, Mask from, Mask to);

        virtual void reload();
        virtual void render();
        virtual void updateSize(const HSize &size);
        friend class GL2VirtualMachine;

    public:
        _FORCE_INLINE_ GL2RendererIMP() : initialized(false) {}
        ~GL2RendererIMP();
        _FORCE_INLINE_ void setDefaultFrameBuffer(GLuint buffer) { default_frame_buffer = buffer; }

    CLASS_END
}

#endif // HIGRAPHICS_GL2_RENDERER_IMP_H
