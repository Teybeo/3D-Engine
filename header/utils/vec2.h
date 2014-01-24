#ifndef _VEC2_GUARD
#define _VEC2_GUARD

#include <stdbool.h>

typedef struct Vec2 {

    float x;
    float y;

} Vec2;

void Vec2_Mul_Scal(Vec2* vec, float scal);
void Vec2_Mul(Vec2 *p, Vec2 a);
void Vec2_Add(Vec2* a, Vec2 b);
bool Vec2_Equal(Vec2 a, Vec2 b);
void setVec2(Vec2* vec, float x, float y);
void afficheVec2(Vec2 a);

#endif // VEC2


