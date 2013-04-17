#include "vec3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void Vec3_Mul_Scal(Vec3* vec, float scal) {
    vec->x *= scal;
    vec->y *= scal;
    vec->z *= scal;
}

void Vec3_Div_Scal(Vec3* vec, float scal) {
    vec->x /= scal;
    vec->y /= scal;
    vec->z /= scal;
}

// a * b
float Vec3_Mul_Out(Vec3 const a, Vec3 const b) {

    return a.x*b.x + a.y*b.y + a.z*b.z;

}

void Vec3_Add(Vec3* a, Vec3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

void Vec3_Sub(Vec3* a, Vec3 b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

Vec3 Vec3_SubOut(Vec3 const a, Vec3 const b) {

    Vec3 res = a;

    res.x -= b.x;
    res.y -= b.y;
    res.z -= b.z;

    return res;
}

void Vec3_Mul(Vec3 *p, Vec3 a) {
    p->x *= a.x;
    p->y *= a.y;
    p->z *= a.z;
}

float Vec3_Length(Vec3 vec) {

    return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);

}
//  [v projeté sur u] = (v.u / u.u) * u
Vec3 Vec3_Project(Vec3 a, Vec3 b) {

    float coef = (Vec3_Mul_Out(a, b) / Vec3_Mul_Out(b, b) );

    return Vec3_Mul_Scal_out(b, coef);
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

    Vec3_Mul_Scal(vec, 1 / Vec3_Length(*vec));
}

Vec3 Vec3_Normalise_Out(Vec3 const vec) {

    Vec3 res = vec;

    Vec3_Mul_Scal(&res, 1 / Vec3_Length(res));

    return res;
}

Vec3 Vec3_Mul_Scal_out(Vec3 vec, float scal) {

    Vec3 res = vec;

    res.x *= scal;
    res.y *= scal;
    res.z *= scal;

    return res;
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
