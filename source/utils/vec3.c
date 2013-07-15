#include "utils/vec3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

inline void Vec3_Mul_Scal(Vec3* vec, float scal) {
    vec->x *= scal;
    vec->y *= scal;
    vec->z *= scal;
}

inline void Vec3_Div_Scal(Vec3* vec, float scal) {
    vec->x /= scal;
    vec->y /= scal;
    vec->z /= scal;
}
inline Vec3 Vec3_Div_Scal_Out(Vec3 const vec, float scal) {
    Vec3 res = vec;

    res.x /= scal;
    res.y /= scal;
    res.z /= scal;

    return res;
}

// a * b
inline float Vec3_Mul_Out(Vec3 const a, Vec3 const b) {

    return a.x*b.x + a.y*b.y + a.z*b.z;

}

inline void Vec3_Add(Vec3* a, Vec3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

inline void Vec3_Sub(Vec3* a, Vec3 b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

inline Vec3 Vec3_SubOut(Vec3 const a, Vec3 const b) {

    Vec3 res = a;

    res.x -= b.x;
    res.y -= b.y;
    res.z -= b.z;

    return res;
}

inline void Vec3_Mul(Vec3 *p, Vec3 a) {
    p->x *= a.x;
    p->y *= a.y;
    p->z *= a.z;
}

inline float Vec3_Length(Vec3 vec) {

//    return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
    return sqrtf(Vec3_Mul_Out(vec, vec));

}

//  [u projeté sur v] = (u.v / v.v) * v
Vec3 Vec3_Project(Vec3 u, Vec3 v) {

    float coef = Vec3_LenghtFromProjection(u, v);

    return Vec3_Mul_Scal_out(v, coef);
}

//  [u projeté sur v normal] = (u.v) * v
// Si v normal
Vec3 Vec3_ProjectOnNormal(Vec3 u, Vec3 v) {

    float coef = Vec3_LenghtFromProjectionOnNormal(u, v);

    return Vec3_Mul_Scal_out(v, coef);
}

//  longueur de [u projeté sur v] = (u.v / v.v)
float Vec3_LenghtFromProjection(Vec3 u, Vec3 v) {

    return Vec3_Mul_Out(u, v) / Vec3_Mul_Out(v, v);
}

//  longueur de [u projeté sur v normale] = (u.v)
// Si v est normal, v.v = |v|² = |1|² = 1
float Vec3_LenghtFromProjectionOnNormal(Vec3 u, Vec3 v) {

    return Vec3_Mul_Out(u, v);
}

inline float Vec3_Distance2Points(Vec3 const a, Vec3 const b) {

    return Vec3_Length(Vec3_SubOut(a, b));

}

inline float Vec3_Distance2PointsCarree(Vec3 const a, Vec3 const b) {

    return ((a.x - b.x) * (a.x - b.x) +
            (a.y - b.y) * (a.y - b.y) +
            (a.z - b.z) * (a.z - b.z));
}

void Vec3_Normalise(Vec3* vec) {

    Vec3_Div_Scal(vec, Vec3_Length(*vec));
}

Vec3 Vec3_Normalise_Out(Vec3 const vec) {

    return Vec3_Div_Scal_Out(vec, Vec3_Length(vec));
}

Vec3 Vec3_Mul_Scal_out(Vec3 vec, float scal) {

    Vec3 res = vec;

    res.x *= scal;
    res.y *= scal;
    res.z *= scal;

    return res;
}

Vec3 Vec3_Random() {

    float angleX = (rand() % 360) * (TAU/360);
    float angleY = (rand() % 360) * (TAU/360);
    float magnitude = 1 + (rand() % 2000)*0.01;

    Vec3 vec = {};
    vec.x =  sin(angleY) * cos(angleX) * magnitude;
    vec.y = -sin(angleX) * magnitude;
    vec.z = -cos(angleY) * cos(angleX) * magnitude;

    return vec;

}

Vec3 Vec3_RandomPos(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax) {

    Vec3 vec = {};

    vec.x = xMin + rand() % (abs(xMin) + xMax);
    vec.y = yMin + rand() % (abs(yMin) + yMax);
    vec.z = zMin + rand() % (abs(zMin) + zMax);

    return vec;

}

void setVec3(Vec3* vec, float x, float y, float z) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

void afficheVec3(Vec3 a) {

    printf("   x      y      z\n");
    printf("%.2f  %.2f  %.2f\n", a.x, a.y, a.z);

}
