//
// Created by hu jun on 17/5/16.
//

#ifndef NEXTLAUNCHER_CIRCLE_H
#define NEXTLAUNCHER_CIRCLE_H

#include "Mesh.h"
#include <render_define.h>

namespace hirender {
    CLASS_BEGIN_N(Circle, Mesh)

    public:
        enum Anchor{
            CENTER,
            TOP_LEFT
        };

    private:
        float r;
        Anchor anchor;
        Vector2f size;
        void updateVertexes();
        void createVertexes();


    public:


        Circle();
        INITIALIZE(Circle, PARAMS(float r),
                           this->r = r;this->size = HSize(r,r).scale(2);
                           updateVertexes();
        )
        _FORCE_INLINE_ const Vector2f &getSize(){ return Vector2f(r,r);};
        void setSize(const Vector2f &size);
        void setRadius(float radius);
        _FORCE_INLINE_ float getRadius(){ return r;};



        void setAnchor(Anchor anchor);
        _FORCE_INLINE_ Anchor getAnchor() const {
            return anchor;
        }

    CLASS_END
}
#endif //NEXTLAUNCHER_CIRCLE_H
