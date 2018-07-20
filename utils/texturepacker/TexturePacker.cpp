//
// Created by gen on 28/02/2017.
//

#include <texture/ImageTexture.h>
#include "../json/libjson.h"
#include <object/ui/View.h>
#include "TexturePacker.h"

using namespace gr;

namespace gr {
    struct SpriteInfo {
        StringName  filename;
        HRect   frame;
        bool    rotated;
        bool    trimmed;
        HRect   sprite_source_size;
        HSize   source_size;
        HPoint  pivot;
        Ref<Panel>   panel;

        SpriteInfo(JSONNODE *node) {
            JSONNODE *f = json_get(node, "filename");
            if (f) {
                json_char *chs = json_as_string(f);
                filename = chs;
                json_free(chs);
            }

            f = json_get(node, "frame");
            if (f) {
                frame.x((float) json_as_float(json_get(f, "x")));
                frame.y((float) json_as_float(json_get(f, "y")));
                frame.z((float) json_as_float(json_get(f, "w")));
                frame.w((float) json_as_float(json_get(f, "h")));
            }
            rotated = (bool) json_as_bool(json_get(node, "rotated"));
            trimmed = (bool) json_as_bool(json_get(node, "trimmed"));
            f = json_get(node, "spriteSourceSize");
            if (f) {
                sprite_source_size.x((float) json_as_float(json_get(f, "x")));
                sprite_source_size.y((float) json_as_float(json_get(f, "y")));
                sprite_source_size.z((float) json_as_float(json_get(f, "w")));
                sprite_source_size.w((float) json_as_float(json_get(f, "h")));
            }
            f = json_get(node, "sourceSize");
            if (f) {
                source_size.x((float) json_as_float(json_get(f, "w")));
                source_size.y((float) json_as_float(json_get(f, "h")));
            }
            f = json_get(node, "pivot");
            if (f) {
                pivot.x((float) json_as_float(json_get(f, "x")));
                pivot.y((float) json_as_float(json_get(f, "y")));
            }
        }
    };
}

TexturePacker::TexturePacker() {
    Shader *shader = new Shader("simple.vert", "image.frag");
    material = new ViewMaterial();
    material->setShader(shader);
    material->setBlend(true);
}

TexturePacker::TexturePacker(const Ref<Data> &image, const Ref<Data> &json) : TexturePacker() {
    material->setTexture(new ImageTexture(image), 0);
    long size =json->getSize();
    char *chs = (char*)malloc(size + 1);
    json->gets(chs, 0, size);
    chs[size] = 0;
    JSONNODE *root = json_parse(chs);
    if (root) {
        JSONNODE *meta = json_get(root, "meta");
        if (json_ctype(meta) == JSON_NODE) {
            JSONNODE *sn = json_get(meta, "size");
            this->size.x((float) json_as_float(json_get(sn, "w")));
            this->size.y((float) json_as_float(json_get(sn, "h")));
            this->scale = (float) json_as_float(json_get(meta, "scale"));
        }
        material->setUniform("SIZE_WIDTH", this->size.width());
        material->setUniform("SIZE_HEIGHT", this->size.height());

        JSONNODE *frames = json_get(root, "frames");
        if (json_ctype(frames) == JSON_ARRAY) {
            for (int i = 0, t = json_size(frames); i < t; ++i) {
                JSONNODE *frame = json_at(frames, i);
                SpriteInfo *sprite_info = new SpriteInfo(frame);
                tex_frams[(void*)sprite_info->filename] = sprite_info;
                LOG(i, "parse %s", sprite_info->filename.str());
            }
        }

        json_delete(root);
    }
    free(chs);
}

TexturePacker::~TexturePacker() {
    for (auto it = tex_frams.begin(), _e = tex_frams.end(); it != _e; ++it) {
        delete (SpriteInfo*)it->second;
    }
}

const Ref<Panel> &TexturePacker::getPanel(const StringName &name) {
    auto it = tex_frams.find((void*)name);
    if (it != tex_frams.end()) {
        SpriteInfo *si = (SpriteInfo*)it->second;
        if (!si->panel) {
            si->panel = new Panel();
            AttrVector *uv_attr = si->panel->getAttribute(Mesh::DEFAULT_UV1_NAME);
            float l = si->frame.x() / size.width(),
                    t = si->frame.y() / size.height(),
                    r = l + si->frame.width() / size.width(),
                    b = t + si->frame.height() / size.height();
            uv_attr->setVector2f(Vector2f(r, b), 0);
            uv_attr->setVector2f(Vector2f(r, t), 1);
            uv_attr->setVector2f(Vector2f(l, t), 2);
            uv_attr->setVector2f(Vector2f(l, b), 3);
        }
        return si->panel;
    }
    return Ref<Mesh>::null();
}

Ref<Panel> TexturePacker::makePanel(const StringName &name) {
    auto it = tex_frams.find((void*)name);
    if (it != tex_frams.end()) {
        SpriteInfo *si = (SpriteInfo*)it->second;
        Panel *panel = new Panel();
        AttrVector *uv_attr = panel->getAttribute(Mesh::DEFAULT_UV1_NAME);
        float l = si->frame.x() / size.width(),
                t = si->frame.y() / size.height(),
                r = l + si->frame.width() / size.width(),
                b = t + si->frame.height() / size.height();
        uv_attr->setVector2f(Vector2f(r, b), 0);
        uv_attr->setVector2f(Vector2f(r, t), 1);
        uv_attr->setVector2f(Vector2f(l, t), 2);
        uv_attr->setVector2f(Vector2f(l, b), 3);
        return panel;
    }
    return Ref<Mesh>::null();
}

HRect TexturePacker::getUV(const StringName &name) const {
    auto it = tex_frams.find((void*)name);
    if (it != tex_frams.end()) {
        SpriteInfo *si = (SpriteInfo*)it->second;
        float l = si->frame.x() / size.width(),
                t = si->frame.y() / size.height(),
                w = si->frame.width() / size.width(),
                h = si->frame.height() / size.height();
        return HRect(l, t, w, h);
    }
    return HRect(0,0,0,0);
}

const HSize &TexturePacker::getSize(const StringName &name) const {
    auto it = tex_frams.find((void*)name);
    if (it != tex_frams.end()) {
        SpriteInfo *si = (SpriteInfo*)it->second;
        return si->source_size;
    }
    return HSize::zero();
}
