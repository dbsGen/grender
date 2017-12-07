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


namespace hirender {
    CLASS_BEGIN_N(TexturePacker, RefObject)

        Ref<Material>   material;
        pointer_map     tex_frams;
        HSize           size;
        float           scale;

    public:
        TexturePacker();
        TexturePacker(const Ref<Data> &image, const Ref<Data> &json);
        ~TexturePacker();

        _FORCE_INLINE_ const Ref<Material> &getMaterial() { return material; }
        const Ref<Panel> &getPanel(const StringName &name);

        Ref<Panel> makePanel(const StringName &name);

        HRect getUV(const StringName &name) const;

        const HSize &getSize(const StringName &name) const;

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_TEXTUREPACKER_H
