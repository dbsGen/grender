//
// Created by gen on 28/02/2017.
//

#ifndef NEXTLAUNCHER_GIT_TEXTUREPACKER_H
#define NEXTLAUNCHER_GIT_TEXTUREPACKER_H

#include <core/Ref.h>
#include <core/Data.h>
#include <material/Material.h>
#include <mesh/Panel.h>
#include "../../render_define.h"


namespace gr {
    CLASS_BEGIN_N(TexturePacker, gc::RefObject)

        gc::Ref<Material>   material;
        pointer_map     tex_frams;
        Size           size;
        float           scale;

    public:
        TexturePacker();
        TexturePacker(const gc::Ref<gc::Data> &image, const gc::Ref<gc::Data> &json);
        ~TexturePacker();

        _FORCE_INLINE_ const gc::Ref<Material> &getMaterial() { return material; }
        const gc::Ref<Panel> &getPanel(const gc::StringName &name);

        gc::Ref<Panel> makePanel(const gc::StringName &name);

        Rect getUV(const gc::StringName &name) const;

        const Size &getSize(const gc::StringName &name) const;

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_TEXTUREPACKER_H
