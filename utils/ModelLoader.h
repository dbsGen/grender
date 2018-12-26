//
// Created by gen on 13/04/2017.
//

#ifndef NEXTLAUNCHER_GIT_MODELLOADER_H
#define NEXTLAUNCHER_GIT_MODELLOADER_H

#include <core/Object.h>
#include <core/Data.h>
#include <core/Array.h>
#include <mesh/Mesh.h>
#include "../render_define.h"

namespace gr {
    CLASS_BEGIN_0_N(ModelLoader)

        pointer_map meshes;
        gc::RArray names;
        float version;
        size_t cursor;

        bool read(gc::Data *data, void *buffer, size_t len);
        uint64_t readLength(gc::Data *data);

    public:
        ~ModelLoader();

        void clear();
        void load(gc::Data *data);

        /**
         *
         * @return array of StringName
         */
        const gc::RArray &getNames() {
            return names;
        }
        const gc::Ref<Mesh> &getMesh(const gc::StringName &name);
        _FORCE_INLINE_ float getVersion() {
            return version;
        }

    CLASS_END
}


#endif //NEXTLAUNCHER_GIT_MODELLOADER_H
