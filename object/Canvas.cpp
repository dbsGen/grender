//
//  Canvas.cpp
//  hirender_iOS
//
//  Created by mac on 2017/1/22.
//  Copyright © 2017年 gen. All rights reserved.
//

#include "Canvas.h"
#include "Camera.h"

using namespace gr;
using namespace gc;

Canvas::Canvas() {
    texture = new RenderTexture;
    setMaterial(new Material(Shader::defaultShader()));
    getMaterial()->setTexture(texture, 0);
    setSingle(true);
}

void Canvas::willRender(gr::Camera *camera) {
    auto it = change_flags.find(camera);
    bool changed = it == change_flags.end() || it->second;
    if (changed) {
        const Ref<Mesh> &mesh = getMesh();
        AttrVector *varr = mesh->getAttribute(mesh->getVertexName());
        AttrVector *uvarr = mesh->getAttribute(Mesh::DEFAULT_UV1_NAME);
        const Matrix4 &m4 = getGlobalPose();
        for (int i = 0, t = mesh->getVertexesCount(); i < t; ++i) {
            Vector3f v3 = varr->getVector3f(i) * m4;
            Vector3f out = camera->worldToScreen(v3);
            uvarr->setVector2f(Vector2f(out.x(), 1-out.y()), i);
        }
        uvarr->update();
        
        change_flags[camera] = NULL;
    }
}

void Canvas::onPoseChanged() {
    vertexChanged();
}

void Canvas::setMesh(const Ref<gr::Mesh> &mesh) {
    processMesh(mesh);
    Object3D::setMesh(mesh);
    vertexChanged();
}

void Canvas::processMesh(const Ref<gr::Mesh> &mesh) {
    const Ref<Mesh> &old_mesh = getMesh();
    if (old_mesh) {
        AttrVector *varr = old_mesh->getAttribute(old_mesh->getVertexName());
        varr->removeCallback(this);
    }
    
    if (mesh) {
        AttrVector *varr = mesh->getAttribute(mesh->getVertexName());
        varr->addCallback(this, &Canvas::vertexUpdate, NULL);
    }
}

void Canvas::vertexChanged() {
    for (auto it = change_flags.begin(), _e = change_flags.end(); it != _e; ++it) {
        change_flags[it->first] = (void*)0x1;
    }
}

void Canvas::vertexUpdate(void *sender, void *send_data, void *data) {
    Canvas *canvas = (Canvas*)data;
    canvas->vertexChanged();
}
