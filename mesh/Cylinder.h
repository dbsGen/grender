//
// Created by hu jun on 17/5/15.
//

#ifndef NEXTLAUNCHER_CYLINDER_H
#define NEXTLAUNCHER_CYLINDER_H

#include "Mesh.h"
#include <render_define.h>

namespace gr {
    CLASS_BEGIN_N(Cylinder, Mesh)

    public:

    private:
        float r;
        float h;
        Size3D size;
        void updateVertexes();
        void createVertexes();


    public:


        Cylinder();
        INITIALIZE(Cylinder, PARAMS(float r),
                   this->r = r;
                           updateVertexes();
        )
        _FORCE_INLINE_ const Size3D &getSize(){ return size;};
        void setSize(Size3D size);
//        _FORCE_INLINE_ float getHeight(){ return h;}
//        _FORCE_INLINE_ void setHeight(float h);



    CLASS_END
}

#endif //NEXTLAUNCHER_CYLINDER_H
