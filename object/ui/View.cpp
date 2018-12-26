//
// Created by gen on 16/6/22.
//

#include "View.h"
#include <Renderer.h>

using namespace gr;
using namespace gc;
using namespace std;

const StringName View::MESSAGE_ALPHA_CHANGED("MESSAGE_VIEW_ALPHA_CHANGED");
const StringName View::MESSAGE_DEPTH_CHANGED("MESSAGE_VIEW_DEPTH_CHANGED");

const StringName kSIZE_WIDTH("SIZE_WIDTH");
const StringName kSIZE_HEIGHT("SIZE_HEIGHT");
const StringName kCORNER("CORNER");
const StringName kBORDER_COLOR("BORDER_COLOR");
const StringName kBORDER_WIDTH("BORDER_WIDTH");
const StringName kALPHA("ALPHA");

ViewMaterial::ViewMaterial() : Material(Shader::uiShader()) {
    setCullFace(Material::Front);
}

View::View() : Object3D(), depth(0), alpha(1), alpha_changed(true) {
    border_width = 0;
    corner = 0;
    depth_dirty = true;
    Panel *panel = new Panel;
    panel->setAnchor(Vector2f(0, 0));
    setMesh(panel);
    setMask(UIMask);
}

void View::setSize(const Size &size) {
    Size original = this->size;
    this->size = size;
    const Ref<Panel> &panel = getPanel();
//    panel->setSize(size);
    panel->setSize(Vector2f(size.x(), -size.y()));
//    panel->setPoseObject(this, true);
    const Ref<Material> &material = getMaterial();
    if (material && material->instanceOf(ViewMaterial::getClass())) {
        material->setUniform(kSIZE_WIDTH,   size.x());
        material->setUniform(kSIZE_HEIGHT,  size.y());
    }
    updateSize(original, size);
}

void View::setDepth(int depth) {
    if (this->depth != depth) {
        this->depth = depth;
        sendMessage(MESSAGE_DEPTH_CHANGED, DOWN);
    }
}

void View::setBorderWidth(float border_width) {
    this->border_width = border_width;
    const Ref<Material> &material = getMaterial();
    if (material) {
        material->setUniform(kBORDER_WIDTH, border_width);
    }
}

void View::setCorner(float corner) {
    this->corner = corner;
    const Ref<Material> &material = getMaterial();
    if (material && material->instanceOf(ViewMaterial::getClass())) {
        material->setUniform(kCORNER, corner);
    }
}

void View::setBorderColor(const Color &color) {
    this->border_color = color;
    const Ref<Material> &material = getMaterial();
    if (material && material->instanceOf(ViewMaterial::getClass())) {
        material->setUniform(kBORDER_COLOR, color);
    }
}

void View::setMaterial(const Ref<gr::Material> &material) {
    if (material && material->instanceOf(ViewMaterial::getClass())) {
        material->setUniform(kSIZE_WIDTH,   size.width());
        material->setUniform(kSIZE_HEIGHT,  size.height());
        material->setUniform(kBORDER_COLOR, border_color);
        material->setUniform(kBORDER_WIDTH, border_width);

        material->setUniform(kCORNER,  corner);
        material->setBlend(true);
        material->setUniform(kALPHA, getFinalAlpha());
    }
    Object3D::setMaterial(material);
}

double View::getFinalDepth() {
    if (depth_dirty) {
        Object *parent = getParent();
        double pd = 0;
        if (parent && parent->getInstanceClass()->isTypeOf(View::getClass())) {
            pd = (parent->cast_to<View>()->getFinalDepth()) + 0.0001;
        }
        final_depth = depth + pd;
        depth_dirty = false;
    }
    return final_depth;
}

float View::getFinalAlpha() {
    if (alpha_changed) {
        if (getParent() && getParent()->getInstanceClass()->isTypeOf(View::getClass())) {
            final_alpha = alpha * ((View*)getParent())->getFinalAlpha();
        }else {
            final_alpha = alpha;
        }
    }
    return final_alpha;
}

void View::setAlpha(float alpha) {
    if (this->alpha != alpha) {
        this->alpha = alpha;
        sendMessage(MESSAGE_ALPHA_CHANGED, DOWN);
        change();
    }
}

bool View::onMessage(const StringName &key, const RArray *vars) {
    if (key == MESSAGE_ALPHA_CHANGED) {
        alpha_changed = true;
        const Ref<Mesh> &mesh = getMesh();
        if (mesh) {
            Color color = mesh->getColor();
            color.w(getFinalAlpha());
            mesh->setColor(color);
        }
    }else if (key == MESSAGE_DEPTH_CHANGED) {
        depth_dirty = true;
        if (getMaterial()) {
            if (getRenderer())
                getRenderer()->reload(this);
        }
    }
    return Object3D::onMessage(key, vars);
}
