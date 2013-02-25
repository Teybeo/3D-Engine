#include "vec3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Vec3_Mul_float(Vec3* vec, float scal) {
    vec->x *= scal;
    vec->y *= scal;
    vec->z *= scal;
}

void Vec3_Add(Vec3* a, Vec3 b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

void Vec3_Mul(Vec3 *p, Vec3 a) {
    p->x *= a.x;
    p->y *= a.y;
    p->z *= a.z;
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
