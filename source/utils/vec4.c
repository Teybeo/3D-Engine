#include "utils/vec4.h"

Vec4 setVec4(float x, float y, float z, float w) {
    Vec4 res;
    res.x = x;
    res.y = y;
    res.z = z;
    res.w = w;
    return res;
}

void Vec4_div(Vec4* vec, float a) {

    vec->x /= a;
    vec->y /= a;
    vec->z /= a;
    vec->w /= a;

}
