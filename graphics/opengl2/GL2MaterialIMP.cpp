//
//  GL2MaterialIMP.cpp
//  hirender_iOS
//
//  Created by gen on 16/6/1.
//  Copyright © 2016年 gen. All rights reserved.
//

#include "GL2MaterialIMP.h"
#include "GL2ShaderIMP.h"
#include <core/math/Type.h>
#include <texture/Texture.h>
#include "GL2TextureIMP.h"

using namespace higraphics;
using namespace hirender;

GLuint GL2MaterialIMP::getTranslateID() {
    if (!translate_id && getTarget()->getTranslateUniform()) {
        int idx = indexOf(getTarget()->getShader()->getUniforms(), getTarget()->getTranslateUniform()->name);
        translate_id = idx >= 0 ? uniform_indexes[idx] : 0;
    }
    return translate_id;
}

int GL2MaterialIMP::indexOf(const pointer_vector &arr, const StringName &name) {
    int count = 0;
    for (auto ite = arr.begin(), _e = arr.end(); ite != _e; ite ++) {
        Shader::Property *p = (Shader::Property *)*ite;
        if (p->name == name) return count;
        count ++;
    }
    return -1;
}

void GL2MaterialIMP::updateShader(const Shader *shader) {
    if (shader) {
        if (uniform_indexes) {
            uniform_indexes = (GLuint*)realloc(uniform_indexes, shader->getUniforms().size() * sizeof(GLuint));
        }else {
            uniform_indexes = (GLuint*)malloc(shader->getUniforms().size() * sizeof(GLuint));
        }
    }else {
        if (uniform_indexes) {
            free(uniform_indexes);
            uniform_indexes = NULL;
        }
    }
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}

void GL2MaterialIMP::process() {
    if (!program)
        loadShader();
    glUseProgram(program);
    Material *mat = getTarget();
    if (mat->getBlend()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }else {
        glDisable(GL_BLEND);
    }
    if (mat->getDepth()) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }else {
        glDisable(GL_DEPTH_TEST);
    }
    switch (mat->getCullFace()) {
        case Material::Front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
        case Material::Back:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        case Material::Both:
            glDisable(GL_CULL_FACE);
//            glEnable(GL_CULL_FACE);
//            glCullFace(GL_FRONT_AND_BACK);
            break;
        case Material::None:
            glDisable(GL_CULL_FACE);
            glCullFace(GL_FRONT_AND_BACK);
            break;
            
        default:
            break;
    }
    updateUniformValues();
//    glUseProgram(0);
}

void GL2MaterialIMP::loadShader() {
    program = glCreateProgram();
    GL2ShaderIMP *shaderIMP = (GL2ShaderIMP *)getTarget()->getShader()->getIMP();
    
    if (shaderIMP) {
        GLuint vert_shader, frag_shader;
        vert_shader = shaderIMP->compileVertex();
        frag_shader = shaderIMP->compileFragment();
        if (vert_shader != 0 && frag_shader != 0) {
            glAttachShader(program, vert_shader);
            glAttachShader(program, frag_shader);
        }
        
        bindAttributes();
        
        glLinkProgram(program);
        
#if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(program, logLength, &logLength, log);
            LOG(w, "Program link log:\n%s", log);
            free(log);
        }
#endif
        
        int ret = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &ret);
        if (ret == 0) {
            
            if (vert_shader) glDeleteShader(vert_shader);
            if (frag_shader) glDeleteShader(frag_shader);
            
            if (program)  {
                glDeleteProgram(program);
                program = 0;
            }
            LOG(e, "Faild to link program.");
            return;
        }
        
        configureUniforms();
        if (vert_shader) {
            glDetachShader(program, vert_shader);
            glDeleteShader(vert_shader);
        }
        if (frag_shader) {
            glDetachShader(program, frag_shader);
            glDeleteShader(frag_shader);
        }
    }
}

void GL2MaterialIMP::bindAttributes() {
    auto attrs = getTarget()->getShader()->getAttributes();
    int count = 0;
    for (auto ite = attrs.begin(), _e = attrs.end(); ite != _e; ++ite) {
        glBindAttribLocation(program, (GLint)count, ((Shader::Property*)*ite)->name.str());
        glEnableVertexAttribArray((GLint)(count++));
    }
}

void GL2MaterialIMP::configureUniforms() {
    auto uniforms = getTarget()->getShader()->getUniforms();
    int count = 0;
    for (auto ite = uniforms.begin(), _e = uniforms.end(); ite != _e; ++ite) {
        uniform_indexes[count++] = glGetUniformLocation(program, ((Shader::Property*)*ite)->name.str());
    }
}

void GL2MaterialIMP::updateUniformValues() {
    int samplerCount = 0;
    auto uniforms = getTarget()->getShader()->getUniforms();
    const variant_map &uniformValues = getTarget()->getUniformValues();
    int count = 0;
    for (auto ite = uniforms.begin(), _e = uniforms.end(); ite != _e; ++ite) {
        Shader::Property *p = (Shader::Property*)*ite;
        Shader::Type type = p->type;
        auto ut = uniformValues.find((void*)p->name);
        if (ut != uniformValues.end()) {
            GLuint loc = uniform_indexes[count];
            
            const Variant &uv = (*ut).second;
            if (uv.empty()) {
            }else if (type == Shader::Int) {
                if (p->size == 1) {
                    glUniform1i(loc, uv);
                }
            }else if (type == Shader::Float) {
                if (p->size == 1) {
                    glUniform1f(loc, uv);
                }
            }else if (type == Shader::Vecter2) {
                const Vector2f *v = uv.get<Vector2f>();
                if (v) glUniform2fv(loc, p->size, v->v);
            }else if (type == Shader::Vecter3) {
                const Vector3f *v = uv.get<Vector3f>();
                if (v) glUniform3fv(loc, p->size, v->v);
            }else if (type == Shader::Vecter4) {
                const Vector4f *v = uv.get<Vector4f>();
                if (v) glUniform4fv(loc, p->size, v->v);
            }else if (type == Shader::Matrix2) {
                const Matrix2 *v = uv.get<Matrix2>();
                if (v) glUniformMatrix2fv(loc, p->size, GL_FALSE, v->m);
            }else if (type == Shader::Matrix3) {
                const Matrix3 *v = uv.get<Matrix3>();
                if (v) glUniformMatrix3fv(loc, p->size, GL_FALSE, v->m);
            }else if (type == Shader::Matrix4) {
                const Matrix4 *v = uv.get<Matrix4>();
                if (v) glUniformMatrix4fv(loc, p->size, GL_FALSE, v->m);
            }else if (type == Shader::Sampler) {
                Texture *v = uv.get<Texture>();
                if (v) {
                    if (p->size == 1) {
                        GL2TextureIMP *imp = (GL2TextureIMP*)v->getIMP();
                        if (v->isFrameUpdate()) v->update();

                        glActiveTexture(GL_TEXTURE0 + (GLuint)samplerCount);
                        glBindTexture(imp->getGLTarget(), imp->getID());
                        glUniform1i(loc, (GLuint)samplerCount);
                    }
                }
            }
        }
        if (type == Shader::Sampler) {
            ++samplerCount;
        }
        count ++;
    }
}

void GL2MaterialIMP::cleanPosition() {
    position_idx = -1;
}
void GL2MaterialIMP::cleanTranslate() {
    translate_id = 0;
}

GL2MaterialIMP::GL2MaterialIMP() {
    position_idx = -1;
    translate_id = 0;
    program = 0;
    uniform_indexes = NULL;
}

GL2MaterialIMP::~GL2MaterialIMP() {
    if (uniform_indexes)
        free(uniform_indexes);
    if (program)
        glDeleteProgram(program);
}
