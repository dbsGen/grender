//
//  Label.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/25.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "Label.h"
#include <font/CharacterMesh.h>
#include <font/FTUnicode.h>
#include <utils/NotificationCenter.h>
#include <Renderer.h>
#include <core/math/Math.hpp>
#include "../../render_define.h"

using namespace gr;
using namespace gcore;

namespace gr {
    CLASS_BEGIN_N(CharacterObject, View)

    private:
        static Ref<CharacterMesh> getCharacterMesh(const CharacterObject *object);
    public:
        CharacterObject() : View() {}
        CharacterObject(const Ref<Character> &ch, const Ref<Material> &mat);

        void setCharacter(const Ref<Character> &ch);
        _FORCE_INLINE_ const Ref<Character> &getCharacter() {
            return getCharacterMesh(this)->getCharacter();
        }

        void setColor(const Vector4f &color);
        const Vector4f getColor() const;

        virtual bool onMessage(const StringName &key, const Array *vars);

        void render();

    CLASS_END

    struct LineInfo {
        pointer_vector *chars;
        float width;

        LineInfo(pointer_vector *chars, float width) {
            this->chars = chars;
            this->width = width;
        }
    };
    
    
    struct CharacterInfo {
        Ref<Character> character;
        Vector2f offset;
        
        CharacterInfo(const Ref<Character> &ch, const Vector2f &off) {
            character = ch;
            offset = off;
        }
    };
    
    FTUnicodeStringItr<unsigned char> ellipsis_unicode((unsigned char*)"…");
}

Ref<CharacterMesh> CharacterObject::getCharacterMesh(const CharacterObject *object) {
    return object->getMesh();
}

void CharacterObject::setCharacter(const Ref<Character> &ch) {
    const Ref<CharacterMesh> &mesh = getCharacterMesh(this);
    if (mesh) {
        mesh->setCharacter(ch);
    }
}

void CharacterObject::render() {
    const Ref<CharacterMesh> &mesh = getCharacterMesh(this);
    if (mesh) {
        mesh->render();
    }
}

const Vector4f CharacterObject::getColor() const {
    const Ref<CharacterMesh> &mesh = getCharacterMesh(this);
    if (mesh) {
        mesh->getColor();
    }
    return Vector4f(1,1,1,1);
}

void CharacterObject::setColor(const Vector4f &color) {
    const Ref<CharacterMesh> &mesh = getCharacterMesh(this);
    if (mesh) {
        mesh->setColor(HColor(color[0], color[1], color[2], color[3] * getFinalAlpha()));
    }
}

CharacterObject::CharacterObject(const Ref<Character> &ch, const Ref<Material> &mat) : View() {
    CharacterMesh *chMesh = new CharacterMesh;
    chMesh->setCharacter(ch);
    setMesh(chMesh);

    setMaterial(mat);
//    setSingle(true);
//    setStatic(true);
}

bool CharacterObject::onMessage(const StringName &key, const Array *vars) {
    if (key == MESSAGE_ALPHA_CHANGED) {
//        const Ref<Material> &material = getMaterial();
//        if (material) {
//            material->setUniform("ALPHA", getFinalAlpha());
//        }
    }
    return View::onMessage(key, vars);
}


Label::Label() : changed(false),
                 max_width(0),
                 color(1,1,1,1),
                 space(0, 0),
                 h_anchor(Begin),
                 point_size(0),
                 ellipsis(true) {
    setFont(Font::defaultFont());
    notification_key = NotificationCenter::keyFromObject(this);
    NotificationCenter::getInstance()->listen(Renderer::NOTIFICATION_PREV_FRAME, [](void *name, void *params, void* data){
        ((Label*)data)->prevFrame();
    }, notification_key, this);
                     setFont(Font::defaultFont());
                     v_anchor = Begin;
                     h_anchor = Begin;

}

Label::~Label() {
    NotificationCenter::getInstance()->remove(Renderer::NOTIFICATION_PREV_FRAME, notification_key);
}

void Label::prevFrame() {
    if (changed) {
        refresh();
    }
}

void Label::awake(Renderer *renderer) {
    if (changed) {
        refresh();
    }
}

void Label::updateSize(const Vector2f &originalSize, const Vector2f &size) {
    setMaxWidth(size.x());
}

void Label::setText(const string &text) {
    this->text = text;
    changed = true;
    change();
}

const string &Label::getText() const {
    return text;
}

void Label::setSpace(const HSize &space) {
    this->space = space;
    changed = true;
}
const HSize &Label::getSpace() {
    return space;
}

void Label::setMaxWidth(float maxWidth) {
    this->max_width = maxWidth;
    changed = true;
}
float Label::getMaxWidth() {
    return max_width;
}

void Label::setColor(const HColor &color) {
    this->color = color;
    for (auto it = characters.begin(), _e = characters.end(); it != _e; ++it) {
        CharacterObject *co = (*it)->cast_to<CharacterObject>();
        co->setColor(color);
    }
}
const HColor &Label::getColor() {
    return color;
}

void Label::setFont(const Ref<Font> &font) {
    this->font = font;
    if (point_size == 0) {
        point_size = font->getSize();
    }
    changed = true;
    clearCachedChars();
}

const Ref<Font> &Label::getFont() const {
    return font;
}

bool Label::onMessage(const StringName &key, const Array *vars) {
    if (key == MESSAGE_MASK_CHANGE) {
        const list<Ref<Object> > &children = getChildren();
        for (auto it = children.begin(), _e = children.end(); it != _e; ++it) {
            Object *obj = **it;
            if (obj->getClass() == CharacterObject::getClass()) {
                obj->setMask((Mask) (int)vars->vec().at(1));
            }
        }
    }
    return View::onMessage(key, vars);
}

void Label::setEllipsis(bool ellipsis) {
    if (this->ellipsis != ellipsis) {
        this->ellipsis = ellipsis;
        changed = true;
        change();
    }
}

void Label::refresh() {
    changed = false;
    pushAllToCache();
    _offset = Vector2f(0, 0);
    if (text.size() && font) {
        FTUnicodeStringItr<unsigned char> utf8Str((unsigned char*)text.c_str());
        float maxLineWidth = 0,
        lineWidth = 0,
        totalHeight = 0;
        bool hasChar = false;
        text_size.y(0);

        pointer_vector lines;
        pointer_vector *lineChars = new pointer_vector;

        while (*utf8Str) {
            unsigned int thisChar = *utf8Str;

#define NEW_LINE if (ellipsis && totalHeight + point_size + space.y() > getSize().height()) { \
            const Ref<Character> &ch = font->character(*ellipsis_unicode); \
            if (lineChars->size() > 0) { \
                CharacterInfo *info = (CharacterInfo *)lineChars->back(); \
                info->character = ch; \
            }else \
                lineChars->push_back(new CharacterInfo(ch, _offset)); \
            break; \
        }\
        lines.push_back(new LineInfo(lineChars, lineWidth));\
            lineChars = new pointer_vector;\
            \
            _offset.x(0);\
            _offset.y(totalHeight + point_size + space.y());\
            totalHeight = _offset.y();\
            if (lineWidth > maxLineWidth) maxLineWidth = lineWidth;\
            lineWidth = 0;\
            hasChar = false;

            if (thisChar == 10) {
                NEW_LINE
                ++utf8Str;
                continue;
            }
            const Ref<Character> &ch = font->character(thisChar);

            float cWidth = max(0.3f * point_size, (float)(ch->getWidth() + ch->getCorner().x()) * (point_size / font->getSize()));
            if ((_offset.x() + cWidth > max_width && max_width > 0) && hasChar) {
                NEW_LINE
            }

            lineChars->push_back(new CharacterInfo(ch, _offset));
            _offset[0] += cWidth + space.x();
            lineWidth = _offset.x() - space.x();

            hasChar = true;

            unsigned int nextChar = *(++utf8Str);
            if (nextChar) {
                Vector2f v2 = font->offset(thisChar, nextChar);
                _offset += v2;
            }
        }
        if (lineChars->size()) {
            lines.push_back(new LineInfo(lineChars, lineWidth));
            if (lineWidth > maxLineWidth) maxLineWidth = lineWidth;
            _offset.y(_offset.y() + point_size + space.y());
        }else {
            delete lineChars;
        }
        text_size.x(maxLineWidth);
        float height = _offset.y();
        text_size.y(height);

        for (auto it = lines.begin(), _e = lines.end(); it != _e; ++it) {
            LineInfo* chars = (LineInfo*)*it;
            makeLine(chars->chars, chars->width, height);
            delete chars->chars;
            delete chars;
        }
    }
}

void Label::makeLine(const pointer_vector *chs, float lineWidth, float totalHeight) {
    float offX, offY;
    const HSize &size = getSize();
    switch (h_anchor) {
        case Center:
            offX = (size.x() - lineWidth) / 2;
            break;
        case End:
            offX = size.x() - lineWidth;
            break;
        case Begin:
        default:
            offX = 0;
            break;
    }
    switch (v_anchor) {
        case Center:
            offY = (size.y() - totalHeight) / 2;
            break;
        case End:
            offY = size.y() - totalHeight;
            break;
        case Begin:
        default:
            offY = 0;
            break;
    }

    Vector3f scale = Vector3f::one() * (point_size / font->getSize());
    for (auto it = chs->begin(); it != chs->end(); ++it) {
        const CharacterInfo *ci = (const CharacterInfo *)*it;
        Ref<CharacterObject> object(popChar(ci->character));
        if (object) {
            object->setPosition(Vector3f(offX + ci->offset.x(),
                                         //TODO: Android 上这里的表现貌似不一样
                                         offY + ci->offset.y()/* + line_height*/,
                                         VIEW_LAYER_GAP));
            object->setScale(scale);
            object->setColor(color);
            object->render();
            object->setMask(getMask());
            characters.push_back(object);
            add(object);
        }
        delete ci;
    }
}

Label::Anchor Label::getHAnchor() {
    return h_anchor;
}

void Label::setHAnchor(Label::Anchor anchor) {
    if (h_anchor != anchor) {
        h_anchor = anchor;
        changed = true;
    }
}

Label::Anchor Label::getVAnchor() {
    return v_anchor;
}

void Label::setVAnchor(Anchor anchor) {
    if (v_anchor != anchor) {
        v_anchor = anchor;
        changed = true;
    }
}

void Label::pushChar(const Reference &ch) {
    auto chr = ch->cast_to<CharacterObject>()->getCharacter()->getChar();
    auto it = cache_characters.find((void*)chr);
    if (it == cache_characters.end()) {
        ref_vector *cos = new ref_vector;
        cos->push_back(ch);
        cache_characters[(void*)chr] = cos;
    }else {
        ((ref_vector*)it->second)->push_back(ch);
    }
}

Reference Label::popChar(unsigned long ch) {
    return popChar(font->character((unsigned int)ch));
}

Reference Label::popChar(const Ref<Character> &ch) {
    unsigned long chr = ch->getChar();
    auto it = cache_characters.find((void*)chr);
    if (it != cache_characters.end()) {
        ref_vector *vc = (ref_vector *)cache_characters[(void*)chr];
        if (vc->size()) {
            Reference obj = vc->back();
            vc->pop_back();
            return obj;
        }
    }
    Texture *tex = *ch->getTexture();
    auto mit = materials.find((void*)tex);
    if (mit == materials.end()) {
        Ref<Material> mat = isSingle() ? ch->getFont()->createMaterial(ch->getTexture()) : ch->getFont()->getMaterial(ch->getTexture());
        materials[(void*)tex] = mat;
        return new CharacterObject(ch, mat);
    }else {
        return new CharacterObject(ch, materials[(void*)tex]);
    }
}

void Label::clearCachedChars() {
    for (auto it = cache_characters.begin(), _e = cache_characters.end(); it != _e; ++it) {
        ref_vector *rv = (ref_vector *)it->second;
        delete rv;
    }
    cache_characters.clear();
}

void Label::pushAllToCache() {
    for (auto it = characters.begin(), _e = characters.end(); it != _e; ++it) {
        pushChar(*it);
        remove(*it);
    }
    characters.clear();
}
