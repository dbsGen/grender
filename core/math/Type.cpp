//
// Created by gen on 16/5/30.
//

#include "Type.h"

using namespace hicore;

Vector2f Vector2f::_zero;

BASE_CLASS_IMPLEMENT(Vector2i)
BASE_CLASS_IMPLEMENT(Vector3i)
BASE_CLASS_IMPLEMENT(Vector4i)
BASE_CLASS_IMPLEMENT(Vector2f)
BASE_CLASS_IMPLEMENT(Vector3f)
BASE_CLASS_IMPLEMENT(Vector4f)
BASE_CLASS_IMPLEMENT(Matrix2)
BASE_CLASS_IMPLEMENT(Matrix3)
BASE_CLASS_IMPLEMENT(Matrix4)

// --------- Vector3 ---------

Vector3f &Vector3f::operator*=(const Matrix4 &other) {
    Vector3f v3(*this);
    v[0] = other.m[0] * v3.v[0] + other.m[4] * v3.v[1] + other.m[8] * v3.v[2] + other.m[12];
    v[1] = other.m[1] * v3.v[0] + other.m[5] * v3.v[1] + other.m[9] * v3.v[2] + other.m[13];
    v[2] = other.m[2] * v3.v[0] + other.m[6] * v3.v[1] + other.m[10] * v3.v[2] + other.m[14];
    return *this;
}
Vector3f Vector3f::operator*(const Matrix4 &other) const {
    Vector3f v3(*this);
    v3 *= other;
    return v3;
}

Vector3f Vector3f::operator*(const Vector4f &quat) const {
    float num = quat.x() * 2;
    float num2 = quat.y() * 2;
    float num3 = quat.z() * 2;
    float num4 = quat.x() * num;
    float num5 = quat.y() * num2;
    float num6 = quat.z() * num3;
    float num7 = quat.x() * num2;
    float num8 = quat.x() * num3;
    float num9 = quat.y() * num3;
    float num10 = quat.w() * num;
    float num11 = quat.w() * num2;
    float num12 = quat.w() * num3;

    Vector3f result;
    result[0] = (1 - (num5 + num6)) * x() + (num7 - num12) * y() + (num8 + num11) * z();
    result[1] = (num7 + num12) * x() + (1 - (num4 + num6)) * y() + (num9 - num10) * z();
    result[2] = (num8 - num11) * x() + (num9 + num10) * y() + (1 - (num4 + num5)) * z();
    return result;
}

Vector4f &Vector4f::operator*=(const Matrix4 &other) {
    Vector4f v4(*this);
    v[0] = other.m[0] * v4.v[0] + other.m[4] * v4.v[1] + other.m[8] * v4.v[2] + other.m[12] * v4.v[3];
    v[1] = other.m[1] * v4.v[0] + other.m[5] * v4.v[1] + other.m[9] * v4.v[2] + other.m[13] * v4.v[3];
    v[2] = other.m[2] * v4.v[0] + other.m[6] * v4.v[1] + other.m[10] * v4.v[2] + other.m[14] * v4.v[3];
    v[3] = other.m[3] * v4.v[0] + other.m[7] * v4.v[1] + other.m[11] * v4.v[2] + other.m[15] * v4.v[3];
    return *this;
}

Vector4f Vector4f::operator*(const Matrix4 &other) const {
    Vector4f v4(*this);
    v4 *= other;
    return v4;
}

Vector4f Vector4f::fromEuler(const Vector3f &euler) {
    float c1 = cosf(euler.y()/2);
    float s1 = sinf(euler.y()/2);
    float c2 = cosf(euler.z()/2);
    float s2 = sinf(euler.z()/2);
    float c3 = cosf(euler.x()/2);
    float s3 = sinf(euler.x()/2);
    float c1c2 = c1*c2;
    float s1s2 = s1*s2;
    return Vector4f(c1c2*s3 + s1s2*c3,
                    s1*c2*c3 + c1*s2*s3,
                    c1*s2*c3 - s1*c2*s3,
                    c1c2*c3 - s1s2*s3);
}

const Vector4f& Vector4f::identity() {
    static const Vector4f id(0,0,0,1);
    return id;
}

Vector3f Vector4f::euler() const {
    float test = x()*y() + z()*w();
    if (test > 0.499) { // singularity at north pole
        return Vector3f(0, 2 * atan2f(x(),w()), M_PI_2);
    }
    if (test < -0.499) { // singularity at south pole
        return Vector3f(0, -2 * atan2f(x(),w()), -M_PI_2);
    }
    float sqx = x()*x();
    float sqy = y()*y();
    float sqz = z()*z();
    return Vector3f(atan2f(2*x()*w()-2*y()*z() , 1 - 2*sqx - 2*sqz),
                    atan2f(2*y()*w()-2*x()*z() , 1 - 2*sqy - 2*sqz),
                    asinf(2*test));
}

Matrix3 Matrix3::fromEuler(const Vector3f &e) {
    Matrix3 m3;
    float ch = cosf(e.y());
    float sh = sinf(e.y());
    float ca = cosf(e.z());
    float sa = sinf(e.z());
    float cb = cosf(e.x());
    float sb = sinf(e.x());

    m3.m00 = ch * ca;
    m3.m01 = sh*sb - ch*sa*cb;
    m3.m02 = ch*sa*sb + sh*cb;
    m3.m10 = sa;
    m3.m11 = ca*cb;
    m3.m12 = -ca*sb;
    m3.m20 = -sh*ca;
    m3.m21 = sh*sa*cb + ch*sb;
    m3.m22 = -sh*sa*sb + ch*cb;
    return m3;
}

Vector4f Matrix3::rotation() const {
    float tr = m00 + m11 + m22;
    Vector4f q;
    if (tr > 0) {
        float S = sqrtf(tr+1.0f) * 2; // S=4*qw
        q.w(0.25f * S);
        q.x((m21 - m12) / S);
        q.y((m02 - m20) / S);
        q.z((m10 - m01) / S);
    } else if ((m00 > m11)&(m00 > m22)) {
        float S = sqrtf(1.0f + m00 - m11 - m22) * 2; // S=4*qx
        q.w((m21 - m12) / S);
        q.x(0.25f * S);
        q.y((m01 + m10) / S);
        q.z((m02 + m20) / S);
    } else if (m11 > m22) {
        float S = sqrtf(1.0f + m11 - m00 - m22) * 2; // S=4*qy
        q.w((m02 - m20) / S);
        q.x((m01 + m10) / S);
        q.y(0.25f * S);
        q.z((m12 + m21) / S);
    } else {
        float S = sqrtf(1.0f + m22 - m00 - m11) * 2; // S=4*qz
        q.w((m10 - m01) / S);
        q.x((m02 + m20) / S);
        q.y((m12 + m21) / S);
        q.z(0.25f * S);
    }
    return q;
}

// --------- Matrix4 ---------
Matrix4 Matrix4::operator*(const Matrix4 &other) const {
    Matrix4 mat;

    mat.m[0]  = m[0] * other.m[0]  + m[4] * other.m[1]  + m[8] * other.m[2]   + m[12] * other.m[3];
    mat.m[4]  = m[0] * other.m[4]  + m[4] * other.m[5]  + m[8] * other.m[6]   + m[12] * other.m[7];
    mat.m[8]  = m[0] * other.m[8]  + m[4] * other.m[9]  + m[8] * other.m[10]  + m[12] * other.m[11];
    mat.m[12] = m[0] * other.m[12] + m[4] * other.m[13] + m[8] * other.m[14]  + m[12] * other.m[15];

    mat.m[1]  = m[1] * other.m[0]  + m[5] * other.m[1]  + m[9] * other.m[2]   + m[13] * other.m[3];
    mat.m[5]  = m[1] * other.m[4]  + m[5] * other.m[5]  + m[9] * other.m[6]   + m[13] * other.m[7];
    mat.m[9]  = m[1] * other.m[8]  + m[5] * other.m[9]  + m[9] * other.m[10]  + m[13] * other.m[11];
    mat.m[13] = m[1] * other.m[12] + m[5] * other.m[13] + m[9] * other.m[14]  + m[13] * other.m[15];

    mat.m[2]  = m[2] * other.m[0]  + m[6] * other.m[1]  + m[10] * other.m[2]  + m[14] * other.m[3];
    mat.m[6]  = m[2] * other.m[4]  + m[6] * other.m[5]  + m[10] * other.m[6]  + m[14] * other.m[7];
    mat.m[10] = m[2] * other.m[8]  + m[6] * other.m[9]  + m[10] * other.m[10] + m[14] * other.m[11];
    mat.m[14] = m[2] * other.m[12] + m[6] * other.m[13] + m[10] * other.m[14] + m[14] * other.m[15];

    mat.m[3]  = m[3] * other.m[0]  + m[7] * other.m[1]  + m[11] * other.m[2]  + m[15] * other.m[3];
    mat.m[7]  = m[3] * other.m[4]  + m[7] * other.m[5]  + m[11] * other.m[6]  + m[15] * other.m[7];
    mat.m[11] = m[3] * other.m[8]  + m[7] * other.m[9]  + m[11] * other.m[10] + m[15] * other.m[11];
    mat.m[15] = m[3] * other.m[12] + m[7] * other.m[13] + m[11] * other.m[14] + m[15] * other.m[15];

    return mat;
}

Matrix4 Matrix4::operator+(const Matrix4 &other) const {
    Matrix4 mat;

    mat.m[0] = m[0] + other.m[0];
    mat.m[1] = m[1] + other.m[1];
    mat.m[2] = m[2] + other.m[2];
    mat.m[3] = m[3] + other.m[3];

    mat.m[4] = m[4] + other.m[4];
    mat.m[5] = m[5] + other.m[5];
    mat.m[6] = m[6] + other.m[6];
    mat.m[7] = m[7] + other.m[7];

    mat.m[8] = m[8] + other.m[8];
    mat.m[9] = m[9] + other.m[9];
    mat.m[10] = m[10] + other.m[10];
    mat.m[11] = m[11] + other.m[11];

    mat.m[12] = m[12] + other.m[12];
    mat.m[13] = m[13] + other.m[13];
    mat.m[14] = m[14] + other.m[14];
    mat.m[15] = m[15] + other.m[15];

    return mat;
}
Matrix4 Matrix4::operator-(const Matrix4 &other) const {
    Matrix4 mat;

    mat.m[0] = m[0] - other.m[0];
    mat.m[1] = m[1] - other.m[1];
    mat.m[2] = m[2] - other.m[2];
    mat.m[3] = m[3] - other.m[3];

    mat.m[4] = m[4] - other.m[4];
    mat.m[5] = m[5] - other.m[5];
    mat.m[6] = m[6] - other.m[6];
    mat.m[7] = m[7] - other.m[7];

    mat.m[8] = m[8] - other.m[8];
    mat.m[9] = m[9] - other.m[9];
    mat.m[10] = m[10] - other.m[10];
    mat.m[11] = m[11] - other.m[11];

    mat.m[12] = m[12] - other.m[12];
    mat.m[13] = m[13] - other.m[13];
    mat.m[14] = m[14] - other.m[14];
    mat.m[15] = m[15] - other.m[15];

    return mat;
}
Matrix4 Matrix4::operator+(const Vector3f &trans) const {
    Matrix4 mat(m);
    mat += trans;
    return mat;
}

Matrix4 &Matrix4::operator+=(const Vector3f &trans) {
    m[12] = m[0]*trans.v[0] + m[4]*trans.v[1] + m[8]*trans.v[2] + m[12];
    m[13] = m[1]*trans.v[0] + m[5]*trans.v[1] + m[9]*trans.v[2] + m[13];
    m[14] = m[2]*trans.v[0] + m[6]*trans.v[1] + m[10]*trans.v[2] + m[14];
    m[15] = m[3]*trans.v[0] + m[7]*trans.v[1] + m[11]*trans.v[2] + m[15];
    return *this;
}

Matrix4 Matrix4::operator*(const Vector3f &scale) const {
    Matrix4 mat(m);
    mat *= scale;
    return mat;
}

Matrix4 &Matrix4::operator*=(const Vector3f &scale) {
    m[0] *= scale.v[0];
    m[1] *= scale.v[0];
    m[2] *= scale.v[0];
    m[3] *= scale.v[0];
    m[4] *= scale.v[1];
    m[5] *= scale.v[1];
    m[6] *= scale.v[1];
    m[7] *= scale.v[1];
    m[8] *= scale.v[2];
    m[9] *= scale.v[2];
    m[10] *= scale.v[2];
    m[11] *= scale.v[2];
//    m[12] = m[12];
//    m[13] = m[13];
//    m[14] = m[14];
//    m[15] = m[15];
    return *this;
}

Matrix4 Matrix4::rotate(float radians, const Vector3f &axis) const {
    return (*this) * Matrix4(radians, axis);
}

Matrix4 Matrix4::rotate(float radians, float x, float y, float z) const {
    return (*this) * Matrix4(radians, Vector3f(x, y, z));
}

Matrix4 Matrix4:: inverse() const {
    float det;
    Matrix4 inv;
    int i;

    inv.m00 = m11 * m22 * m33
                  + m12 * m23 * m31
                  + m13 * m21 * m32
                  - m11 * m23 * m32
                  - m12 * m21 * m33
                  - m13 * m22 * m31;

    inv.m01 = m01 * m23 * m32
                  + m02 * m21 * m33
                  + m03 * m22 * m31
                  - m01 * m22 * m33
                  - m02 * m23 * m31
                  - m03 * m21 * m32;

    inv.m02 = m01 * m12 * m33
                  + m02 * m23 * m31
                  + m03 * m11 * m32
                  - m01 * m13 * m32
                  - m02 * m11 * m33
                  - m03 * m12 * m31;

    inv.m03 = m01 * m13 * m22
                  + m02 * m11 * m23
                  + m03 * m12 * m21
                  - m01 * m12 * m23
                  - m02 * m13 * m21
                  - m03 * m11 * m22;

    inv.m10 = m10 * m23 * m32
                  + m12 * m20 * m33
                  + m13 * m22 * m30
                  - m10 * m22 * m33
                  - m12 * m23 * m30
                  - m13 * m20 * m32;

    inv.m11 = m00 * m22 * m33
                  + m02 * m23 * m30
                  + m03 * m20 * m32
                  - m00 * m23 * m32
                  - m02 * m20 * m33
                  - m03 * m22 * m30;

    inv.m12 = m00 * m13 * m32
                  + m02 * m10 * m33
                  + m03 * m12 * m30
                  - m00 * m12 * m33
                  - m02 * m13 * m30
                  - m03 * m10 * m32;

    inv.m13 = m00 * m12 * m23
                  + m02 * m13 * m20
                  + m03 * m10 * m22
                  - m00 * m13 * m22
                  - m02 * m10 * m23
                  - m03 * m12 * m20;

    inv.m20 = m10 * m21 * m33
                  + m11 * m23 * m30
                  + m13 * m20 * m31
                  - m10 * m23 * m31
                  - m11 * m20 * m33
                  - m13 * m21 * m30;

    inv.m21 = m00 * m23 * m31
                  + m01 * m20 * m33
                  + m03 * m21 * m30
                  - m00 * m21 * m33
                  - m01 * m23 * m30
                  - m03 * m20 * m31;

    inv.m22 = m00 * m11 * m33
                  + m01 * m13 * m30
                  + m03 * m10 * m31
                  - m00 * m13 * m31
                  - m01 * m10 * m33
                  - m03 * m11 * m30;

    inv.m23 = m00 * m13 * m21
                  + m01 * m10 * m23
                  + m03 * m11 * m20
                  - m00 * m11 * m23
                  - m01 * m13 * m20
                  - m03 * m10 * m21;

    inv.m30 = m10 * m22 * m31
                  + m11 * m20 * m32
                  + m12 * m21 * m30
                  - m10 * m21 * m32
                  - m11 * m22 * m30
                  - m12 * m20 * m31;

    inv.m31 = m00 * m21 * m32
                  + m01 * m22 * m30
                  + m02 * m20 * m31
                  - m00 * m22 * m31
                  - m01 * m20 * m32
                  - m02 * m21 * m30;

    inv.m32 = m00 * m12 * m31
                  + m01 * m10 * m32
                  + m02 * m11 * m30
                  - m00 * m11 * m32
                  - m01 * m12 * m30
                  - m02 * m10 * m31;

    inv.m33 = m00 * m11 * m22
                  + m01 * m12 * m20
                  + m02 * m10 * m21
                  - m00 * m12 * m21
                  - m01 * m10 * m22
                  - m02 * m11 * m20;

    det = m00 * inv.m00 + m01 * inv.m10 + m02 * inv.m20 + m03 * inv.m30;

    if (det == 0)
        return Matrix4::identity();

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    return inv;
}

Vector3f Matrix4::euler() const {
    Vector3f eu;
    if (m10 > 0.999999999) {
        eu.y(atan2f(m02, m22));
        eu.z((float)M_PI_2);
    }else if (m10 < -0.999999999) {
        eu.y(atan2f(m02, m22));
        eu.z(-(float)M_PI_2);
    }else {
        eu.y(atan2f(-m20, m00));
        eu.x(atan2f(-m12,m11));
        eu.z(asinf(m10));
    }
    return eu;
}

Vector3f Matrix4::position() const {
    return Vector3f(m[12], m[13], m[14]);
}

Vector4f Matrix4::rotation() const {
    float tr = m00 + m11 + m22;
    Vector4f q;
    if (tr > 0) {
        float S = sqrtf(tr+1.0f) * 2; // S=4*qw
        q.w(0.25f * S);
        q.x((m21 - m12) / S);
        q.y((m02 - m20) / S);
        q.z((m10 - m01) / S);
    } else if ((m00 > m11)&(m00 > m22)) {
        float S = sqrtf(1.0f + m00 - m11 - m22) * 2; // S=4*qx
        q.w((m21 - m12) / S);
        q.x(0.25f * S);
        q.y((m01 + m10) / S);
        q.z((m02 + m20) / S);
    } else if (m11 > m22) {
        float S = sqrtf(1.0f + m11 - m00 - m22) * 2; // S=4*qy
        q.w((m02 - m20) / S);
        q.x((m01 + m10) / S);
        q.y(0.25f * S);
        q.z((m12 + m21) / S);
    } else {
        float S = sqrtf(1.0f + m22 - m00 - m11) * 2; // S=4*qz
        q.w((m10 - m01) / S);
        q.x((m02 + m20) / S);
        q.y((m12 + m21) / S);
        q.z(0.25f * S);
    }
    return q;
}

Vector3f Matrix4::scale() const {
    Vector3f s;
    s.v[0] = Vector3f(m[0], m[1], m[2]).length();
    s.v[1] = Vector3f(m[4], m[5], m[6]).length();
    s.v[2] = Vector3f(m[8], m[9], m[10]).length();
    return s;
}

Matrix4 Matrix4::transpose() const {
    float fls[] {
            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15],
    };
    return fls;
}

const Matrix4 &Matrix4::identity() {
    static const Matrix4 mat((float[]){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    });
    return mat;
}

Matrix4 Matrix4::fromQuaternion(const Vector4f &q) {
    Matrix4 m4;
    float x = q.x(), y = q.y(), z = q.z(), w = q.w();
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m4.m[ 0 ] = 1 - ( yy + zz );
    m4.m[ 4 ] = xy - wz;
    m4.m[ 8 ] = xz + wy;

    m4.m[ 1 ] = xy + wz;
    m4.m[ 5 ] = 1 - ( xx + zz );
    m4.m[ 9 ] = yz - wx;

    m4.m[ 2 ] = xz - wy;
    m4.m[ 6 ] = yz + wx;
    m4.m[ 10 ] = 1 - ( xx + yy );

    // last column
    m4.m[ 3 ] = 0;
    m4.m[ 7 ] = 0;
    m4.m[ 11 ] = 0;

    // bottom row
    m4.m[ 12 ] = 0;
    m4.m[ 13 ] = 0;
    m4.m[ 14 ] = 0;
    m4.m[ 15 ] = 1;

    return m4;
}

Matrix4 Matrix4::fromEuler(const Vector3f &e) {
    Matrix4 m4 = Matrix4::identity();
    float ch = cosf(e.y());
    float sh = sinf(e.y());
    float ca = cosf(e.z());
    float sa = sinf(e.z());
    float cb = cosf(e.x());
    float sb = sinf(e.x());

    m4.m00 = ch * ca;
    m4.m01 = sh*sb - ch*sa*cb;
    m4.m02 = ch*sa*sb + sh*cb;
    m4.m10 = sa;
    m4.m11 = ca*cb;
    m4.m12 = -ca*sb;
    m4.m20 = -sh*ca;
    m4.m21 = sh*sa*cb + ch*sb;
    m4.m22 = -sh*sa*sb + ch*cb;
    return m4;
}

Matrix4 Matrix4::TRS(const Vector3f &position, const Vector4f &quaternion,
                     const Vector3f &scale) {
    Matrix4 m4 = fromQuaternion(quaternion);
    m4 *= scale;
    m4.setPosition(position);
    return m4;
}

void Matrix4::setPosition(const Vector3f &pos) {
    m[ 12 ] = pos.v[0];
    m[ 13 ] = pos.v[1];
    m[ 14 ] = pos.v[2];
}

Matrix4 &Matrix4::operator=(const Matrix4 &other) {
    memcpy(m, other.m, sizeof(m));
    return *this;
}

Matrix4 &Matrix4::operator=(const float *other) {
    memcpy(m, other, sizeof(m));
    return *this;
}

Matrix4 Matrix4::ortho(float left, float right,
                       float bottom, float top,
                       float near, float far) {
    Matrix4 mat;
    mat.m00 = 2/(right - left);
    mat.m11 = 2/(top - bottom);
    mat.m22 = 1/(far - near);

    mat.m03 = (right + left) / (left - right);
    mat.m13 = (top + bottom) / (bottom - top);
    mat.m23 = near / (near - far);

    mat.m33 = 1;

    return mat;
}

Matrix4 Matrix4::perspective(float left, float right,
                             float bottom, float top,
                             float near, float far) {
    Matrix4 mat;
    mat.m00 = 2 * near / (right - left);
    mat.m11 = 2 * near / (top - bottom);
    mat.m22 = (far + near) / (near - far);

    mat.m02 = (right + left) / (right - left);
    mat.m12 = (top + bottom) / (top - bottom);
    mat.m23 = 2 * far * near / (near - far);

    mat.m32 = -1;
    mat.m33 = 0;

    return mat;
}

Matrix4 Matrix4::lookAt(const Vector3f &target, const Vector3f &up) {
    Vector3f z = target.normalize();
    Vector3f x = up.cross(z);
    if (x.length() == 0) {
        z.v[2] += 0.0001;
        x = up.cross(z);
    }
    x = x.normalize();
    Vector3f y = z.cross(x).normalize();

    return Matrix4((float[]){
            x.x(), x.y(), x.z(), 0,
            y.x(), y.y(), y.z(), 0,
            z.x(), z.y(), z.z(), 0,
            0,     0,     0,     1
    });
}

Matrix4 Matrix4::lerp(const Matrix4 &from, const Matrix4 &to, float p) {
    static const int s = 16;
    float res[s];
    for (int i = 0; i < s; ++i) {
        res[i] = from.m[i] * (1-p) + to.m[i] * p;
    }
    return Matrix4(res);
}

Matrix3 Matrix3::operator*(const Matrix3 &o) const {
    return Matrix3((float[]){
            m[0] * o.m[0] + m[1] * o.m[3] + m[2] * o.m[6],
            m[0] * o.m[1] + m[1] * o.m[4] + m[2] * o.m[7],
            m[0] * o.m[2] + m[1] * o.m[5] + m[2] * o.m[8],
            m[3] * o.m[0] + m[4] * o.m[3] + m[5] * o.m[6],
            m[3] * o.m[1] + m[4] * o.m[4] + m[5] * o.m[7],
            m[3] * o.m[2] + m[4] * o.m[5] + m[5] * o.m[8],
            m[6] * o.m[0] + m[7] * o.m[3] + m[8] * o.m[6],
            m[6] * o.m[1] + m[7] * o.m[4] + m[8] * o.m[7],
            m[6] * o.m[2] + m[7] * o.m[5] + m[8] * o.m[8]
    });
}

Matrix3 &Matrix3::operator*=(float d) {
    m[0] *= d;
    m[1] *= d;
    m[2] *= d;
    m[3] *= d;
    m[4] *= d;
    m[5] *= d;
    m[6] *= d;
    m[7] *= d;
    m[8] *= d;
    return *this;
}

Matrix3 &Matrix3::operator+=(const Matrix3 &other) {
    m[0] += other.m[0];
    m[1] += other.m[1];
    m[2] += other.m[2];
    m[3] += other.m[3];
    m[4] += other.m[4];
    m[5] += other.m[5];
    m[6] += other.m[6];
    m[7] += other.m[7];
    m[8] += other.m[8];
    return *this;
}

void Matrix3::setColumn(int col, const Vector3f &v) {
    m[col] = v.x();
    m[col + 3] = v.y();
    m[col + 6] = v.z();
}

Matrix3 Matrix3::transpose() {
    Matrix3 ret;
    float m1 = m[1];
    float m2 = m[2];
    float m5 = m[5];
    ret.m[0] = m[0];
    ret.m[1] = m[3];
    ret.m[2] = m[6];
    ret.m[3] = m1;
    ret.m[4] = m[4];
    ret.m[5] = m[7];
    ret.m[6] = m2;
    ret.m[7] = m5;
    ret.m[8] = m[8];
    return ret;
}

Matrix3 Matrix3::operator+(const Matrix3 &other) {
    Matrix3 ret;
    ret = *this;
    ret += other;
    return ret;
}

Vector3f Matrix3::operator*(const Vector3f &other) const {
    return Vector3f(
            m[0] * other.v[0] + m[1] * other.v[1] + m[2] * other.v[2],
            m[3] * other.v[0] + m[4] * other.v[1] + m[5] * other.v[2],
            m[6] * other.v[0] + m[7] * other.v[1] + m[8] * other.v[2]
    );
}

Matrix3 &Matrix3::operator-=(const Matrix3 &other) {
    m[0] -= other.m[0];
    m[1] -= other.m[1];
    m[2] -= other.m[2];
    m[3] -= other.m[3];
    m[4] -= other.m[4];
    m[5] -= other.m[5];
    m[6] -= other.m[6];
    m[7] -= other.m[7];
    m[8] -= other.m[8];
    return *this;
}

void Matrix3::getColumn(int col, Vector3f &v) {
    v.x(m[col]);
    v.y(m[col + 3]);
    v.z(m[col + 6]);
}

float Matrix3::determinant() {
    return m00 * (m11 * m22 - m21 * m12) -
            m01 * (m10 * m22 - m12 * m20) +
            m02 * (m10 * m21 - m11 * m20);
}

Matrix3 Matrix3::identity() {
    Matrix3 mat((float[]){
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    });
    return mat;
}

Matrix3 Matrix3::invert() {
    float d = determinant();
    if (d == 0.0f) {
        return Matrix3();
    }
    float invdet = 1.0f / d;

    return Matrix3((float[]){
            (m[4] * m[8] - m[7] * m[5]) * invdet,
            -(m[1] * m[8] - m[2] * m[7]) * invdet,
            (m[1] * m[5] - m[2] * m[4]) * invdet,
            -(m[3] * m[8] - m[5] * m[6]) * invdet,
            (m[0] * m[8] - m[2] * m[6]) * invdet,
            -(m[0] * m[5] - m[3] * m[2]) * invdet,
            (m[3] * m[7] - m[6] * m[4]) * invdet,
            -(m[0] * m[7] - m[6] * m[1]) * invdet,
            (m[0] * m[4] - m[3] * m[1]) * invdet
    });
}

Vector3f Vector3f::ortho() const {
    int k = largestAbsComponent() - 1;
    if (k < 0) {
        k = 2;
    }
    Vector3f result;
    result.v[k] = 1.0f;

    result = cross(result);
    result.normalize();
    return result;
}

int Vector3f::largestAbsComponent() const {
    float xAbs = fabsf(v[0]);
    float yAbs = fabsf(v[1]);
    float zAbs = fabsf(v[2]);
    if (xAbs > yAbs)
    {
        if (xAbs > zAbs) {
            return 0;
        }
        return 2;
    }
    if (yAbs > zAbs) {
        return 1;
    }
    return 2;
}
