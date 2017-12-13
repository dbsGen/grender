//
// Created by gen on 16/5/30.
//

#ifndef HIRENDER_MESH_H
#define HIRENDER_MESH_H

#include <material/Material.h>
#include <graphics/Factory.h>
#include <IMP.h>
#include <vector>
#include <core/math/Type.h>
#include "../render_define.h"

using namespace std;

namespace gr {
    class Renderer;
    class MeshIMP;
    class Mesh;
    class Object;

    CLASS_BEGIN_0_NV(AttrVector)

        b8_vector   b;
        bool    dirty;
        bool    repeat;

        pointer_map callbacks;

        static void attributeUpdate(void *sender, void *send_data, void *data);

        void onUpdate();

        friend class Mesh;

    public:
        _FORCE_INLINE_ AttrVector() : dirty(false), repeat(false) { }
        virtual ~AttrVector();

        _FORCE_INLINE_ long long getSize() { return (long long)b.size(); }

        _FORCE_INLINE_ bool isDirty() { return dirty; }

        _FORCE_INLINE_ void setRepeat(bool repeat) { this->repeat = repeat; }
        _FORCE_INLINE_ bool isRepeat() { return repeat; }

        _FORCE_INLINE_ const b8_vector &getVector() { return b; }

        // memery setting
        void setFloat(float f, int idx);
        void setInt(int i, int idx);
        void copyBuffer(const void *inbuffer, size_t insize);

        void setVector2f(const Vector2f &v, int idx);
        void setVector3f(const Vector3f &v, int idx);
        void setVector4f(const Vector4f &v, int idx);

        void update();

        Vector4f getVector4f(int idx);
        Vector3f getVector3f(int idx);

        // get buffer
        _FORCE_INLINE_ uint8_t *buffer() {
            return b.data();
        }
        _FORCE_INLINE_ void resize(size_t s) {
            b.resize(s);
        }

        // callbacks
        void addCallback(void *target, ActionCallback callback,
                         void *data);
        void *removeCallback(void *target);

    CLASS_END

    CLASS_BEGIN_N(Mesh, RefObject)

    private:

        MeshIMP             *imp;
        pointer_map         attrs_map;
        size_t              vertex_length;

        StringName          vertex_name;

        bool    setuped;

        friend class Renderer;
        friend class MeshGroup;

    protected:
        virtual void _copy(const HObject *other);

    public:
        static const StringName INDEX_NAME;
        static const StringName DEFAULT_VERTEX_NAME;
        static const StringName DEFAULT_COLOR_NAME;
        static const StringName DEFAULT_UV1_NAME;
        static const StringName DEFAULT_NORMAL_NAME;

        Mesh();
        ~Mesh();

        /**
         * First of all, you must push all vertexes and commit;
         */
        void push(const Vector3f &v3);
        void commit(const vector<int> &indexes);

        void setVertex(const Vector3f &v3, int i);

        AttrVector *getAttribute(const StringName &name);
        AttrVector *getIndexesAttr();

        void setVertexName(const StringName &name);
        _FORCE_INLINE_ const StringName &getVertexName() { return vertex_name; }

        const b8_vector &getVertexes();
        const b8_vector &getIndexes();

        void setColor(const HColor &color, const StringName &name = DEFAULT_COLOR_NAME);
        HColor getColor(const StringName &name = DEFAULT_COLOR_NAME);

        size_t getVertexesCount();
        size_t getIndexesCount();

        _FORCE_INLINE_ MeshIMP *getIMP() {
            return imp;
        }

    CLASS_END

    CLASS_BEGIN_TNV(MeshIMP, Imp, 1, Mesh)

    protected:

        virtual AttrVector *newAttribute() = 0;

        friend class Mesh;
    public:

    CLASS_END
}


#endif //HIRENDER_MESH_H
