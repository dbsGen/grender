//
//  ShadowView.cpp
//  hirender_iOS
//
//  Created by mac on 2017/5/1.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "ShadowView.h"
#include <texture/ColorTexture.h>

using namespace gr;

const StringName ShadowView::ATTR_BLUR("A_BORDER_WIDTH");
const StringName ShadowView::ATTR_CORNER("A_CORNER");
const StringName ShadowView::ATTR_SIZE_WIDTH("A_SIZE_WIDTH");
const StringName ShadowView::ATTR_SIZE_HEIGHT("A_SIZE_HEIGHT");
Ref<Material> ShadowView::shadow_material;

ShadowView::ShadowView() {
    if (!shadow_material) {
        shadow_material = new ViewMaterial;
        shadow_material->setShader(new Shader(Shader::SHADER_SHADOW_VERT, Shader::SHADER_SHADOW_FRAG));
        shadow_material->setTexture(new ColorTexture(HColor(1,1,1,1)), 0);
    }
    setMaterial(shadow_material);
    
    setCorner(20);
    setSize(HSize(60, 60));
    setBlur(20);
    setSingle(true);
}

void ShadowView::setColor(const HColor &color) {
    getMesh()->setColor(color);
}

void ShadowView::setBlur(float blur) {
    const Ref<Mesh> &mesh = getMesh();
    AttrVector *vec = mesh->getAttribute(ATTR_BLUR);
    vec->setRepeat(true);
    vec->setFloat(blur, 0);
    change();
}
void ShadowView::setCorner(float corner) {
    const Ref<Mesh> &mesh = getMesh();
    AttrVector *vec = mesh->getAttribute(ATTR_CORNER);
    vec->setRepeat(true);
    vec->setFloat(corner, 0);
    change();
}


void ShadowView::updateSize(const Vector2f &originalSize, const Vector2f &size) {
    const Ref<Mesh> &mesh = getMesh();
    AttrVector *vec = mesh->getAttribute(ATTR_SIZE_WIDTH);
    vec->setRepeat(true);
    vec->setFloat(size.width(), 0);
    
    vec = mesh->getAttribute(ATTR_SIZE_HEIGHT);
    vec->setRepeat(true);
    vec->setFloat(size.height(), 0);
    change();
}
