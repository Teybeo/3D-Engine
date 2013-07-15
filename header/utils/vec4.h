#ifndef _VEC4_GUARD
#define _VEC4_GUARD

typedef struct Vec4 {

    float x;
    float y;
    float z;
    float w;

} Vec4;

Vec4 setVec4(float x, float y, float z, float w);
void Vec4_div(Vec4* vec, float a);

#endif // VEC4

