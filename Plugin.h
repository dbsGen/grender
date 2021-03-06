//
// Created by gen on 16/9/3.
//

#ifndef VOIPPROJECT_PLUGIN_H
#define VOIPPROJECT_PLUGIN_H


#include <core/Ref.h>
#include <core/StringName.h>
#include "types.h"

#include "render_define.h"

namespace gr {
    class Renderer;

    CLASS_BEGIN_NV(Plugin, gc::RefObject)

        gc::StringName  name;
        Renderer *renderer;
        friend class Renderer;

    protected:
        _FORCE_INLINE_ Plugin() {}
        _FORCE_INLINE_ Plugin(const gc::StringName &name) : name(name) {}

        _FORCE_INLINE_ virtual void attach(Renderer *renderer) {}
        _FORCE_INLINE_ virtual void disattach(Renderer *renderer) {}
        // Override this method for frame process
        _FORCE_INLINE_ virtual void fixedStep(Renderer *renderer, Time delta) {}
        _FORCE_INLINE_ virtual void step(Renderer *renderer, Time delta) {}

        _FORCE_INLINE_ Renderer *getRenderer() {
            return renderer;
        }
        friend class Renderer;

    public:
        const gc::StringName &getName();
        _FORCE_INLINE_ void setName(const gc::StringName &name) {
            this->name = name;
        }

    CLASS_END
}


#endif //VOIPPROJECT_PLUGIN_H
