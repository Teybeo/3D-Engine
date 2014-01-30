#include "utils/vec2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Vec2_Mul_Scal(Vec2* vec, float scal) {
    vec->x *= scal;
    vec->y *= scal;
}

void Vec2_Add(Vec2* a, Vec2 b) {
    a->x += b.x;
    a->y += b.y;
}

Vec2 Vec2_SubOut(const Vec2 a, const Vec2 b) {

    Vec2 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    return res;
}

void Vec2_Mul(Vec2 *p, Vec2 a) {
    p->x *= a.x;
    p->y *= a.y;
}

bool Vec2_Equal(Vec2 a, Vec2 b) {

    return !memcmp(&a, &b, sizeof(Vec2));
}

void setVec2(Vec2* vec, float x, float y) {
    vec->x = x;
    vec->y = y;
}

void afficheVec2(Vec2 a) {

    printf("   x      y\n");
    printf("%.2f  %.2f\n", a.x, a.y);

}

