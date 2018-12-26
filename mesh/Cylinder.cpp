//
// Created by hu jun on 17/5/15.
//

#include "Cylinder.h"

#define STEP_N 80

using namespace gr;
using namespace std;

Cylinder::Cylinder() : Mesh(),r(1),h(0.1) {
    createVertexes();
}

void Cylinder::createVertexes() {
    float step_angel = 2*M_PI/STEP_N;



    vector<int> idx;
    AttrVector *uv_attr = getAttribute(Mesh::DEFAULT_UV1_NAME);

    float UV_R = 100;
    float UV_X_front = 100;
    float UV_Y_front = 100;
    float UV_X_back  = 300;
    float UV_Y_back = 100;
    float width = 400;
    float height = 212;


    //绘制上表面
    Vector3f center = Vector3f(0,0,0);
    push(center);

    uv_attr->setVector2f(Vector2f(UV_X_front/width,UV_Y_front/height),0);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        push(Vector3f(r*cosf(angel),r*sinf(angel),0)+center);
        idx.push_back(0);
        idx.push_back(i+1);
        if(i!=STEP_N-1)
            idx.push_back(i+2);
        else
            idx.push_back(1);

        float uv_x = (UV_R*cosf(angel)+UV_X_front)/width;
        float uv_y = (UV_R*sinf(angel)+UV_Y_front)/height;


        uv_attr->setVector2f(Vector2f(uv_x,uv_y),i+1);
    }


    //绘制下表面
    Vector3f center_bottom = Vector3f(0,0,-h);
    push(center_bottom);

    uv_attr->setVector2f(Vector2f(UV_X_back/width,UV_Y_back/height),STEP_N+1);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        push(Vector3f(r*cosf(angel),r*sinf(angel),0)+center_bottom);

        idx.push_back(0+STEP_N+1);
        idx.push_back(i+1+STEP_N+1);
        if(i!=STEP_N-1)
            idx.push_back(i+2+STEP_N+1);
        else
            idx.push_back(1+STEP_N+1);


        float uv_x = (UV_R*cosf(angel)+UV_X_back)/width;
        float uv_y = (UV_R*sinf(angel)+UV_Y_back)/height;



        uv_attr->setVector2f(Vector2f(uv_x,uv_y),i+1+STEP_N+1);
    }


    int N_NOW = getVertexesCount();


    //绘制侧面
    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;

        Vector3f p0 = Vector3f(r*cosf(angel),r*sinf(angel),0)+center;
        Vector3f p1 = Vector3f(r*cosf(angel),r*sinf(angel),0)+center_bottom;

        push(p0);
        push(p1);


        if(i==STEP_N-1){
            idx.push_back(N_NOW+i*2);
            idx.push_back(N_NOW);
            idx.push_back(N_NOW+1);

            idx.push_back(N_NOW+i*2);
            idx.push_back(N_NOW+1);
            idx.push_back(N_NOW+i*2+1);
        } else{
            idx.push_back(N_NOW+i*2);
            idx.push_back(N_NOW+i*2+2);
            idx.push_back(N_NOW+i*2+3);

            idx.push_back(N_NOW+i*2);
            idx.push_back(N_NOW+i*2+3);
            idx.push_back(N_NOW+i*2+1);
        }


//        float uv_x_former =401.0f/413.0f;
//        float uv_x_next = 1.0f;
//        float uv_y_former = (float)i/(float)STEP_N;
//        float uv_y_next = (float)(i+1)/(float)STEP_N;

        float uv_x_former =401.0f/413.0f;
        float uv_x_next = 1.0f;

        float uv_y_former =1- (float)i/(float)STEP_N;
        float uv_y_next =1- (float)(i+1)/(float)STEP_N;

        uv_x_former = (float)i/(float)STEP_N;
        uv_x_next = (float)(i+1)/(float)STEP_N;

        uv_y_former = 200.0f/212.0f;
        uv_y_next = 1.0f;

        uv_attr->setVector2f(Vector2f(uv_x_former,uv_y_next),N_NOW+i*2);
        uv_attr->setVector2f(Vector2f(uv_x_next,uv_y_former),N_NOW+i*2+1);
        uv_attr->setVector2f(Vector2f(uv_x_former,uv_y_next),N_NOW+i*2+2);
        uv_attr->setVector2f(Vector2f(uv_x_next,uv_y_next),N_NOW+i*2+3);
    }



    commit(idx);
}

void Cylinder::updateVertexes() {
    float step_angel = 2*M_PI/STEP_N;
    //绘制上表面
    Vector3f center = Vector3f(0,0,0);
    setVertex(center,0);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        setVertex(Vector3f(r*cosf(angel),r*sinf(angel),0)+center,i+1);
    }

    //绘制下表面
    Vector3f center_bottom = Vector3f(0,0,-h);
    setVertex(center_bottom,STEP_N+1);

    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel*i;
        setVertex(Vector3f(r*cosf(angel),r*sinf(angel),0)+center_bottom,i+STEP_N+2);
    }

    //绘制侧面
    for (int i = 0; i < STEP_N; ++i) {
        float angel = step_angel * i;

        Vector3f p0 = Vector3f(r * cosf(angel), r * sinf(angel), 0) + center;
        Vector3f p1 = Vector3f(r * cosf(angel), r * sinf(angel), 0) + center_bottom;

        setVertex(p0,2*STEP_N+2+i*2);
        setVertex(p1,2*STEP_N+2+i*2+1);
    }
    }

void Cylinder::setSize(Size3D size) {
    if (size.x() != this->size.x() || size.y() != this->size.y()|| size.z() != this->size.z()) {
        this->size = size;
        this->h = size.z();
        this->r = size.x()/2;
        updateVertexes();
    }
}
