#ifndef _LISTE
#define _LISTE

#include <stdlib.h>

#include "vec3.h"
#include "vec2.h"

typedef struct ElemVec3 {

    Vec3 vec;
    struct ElemVec3 *suivant;

} ElemVec3;

ElemVec3* empiler(ElemVec3 *tetepile, Vec3 vec);
ElemVec3* empilerFin(ElemVec3 *tetepile, Vec3 vec);
ElemVec3* depiler(ElemVec3* tetepile);
void* liberePile(ElemVec3* tetepile);
int getElemNumber(ElemVec3* tetepile);
unsigned int* dumpListeToArray(ElemVec3* tetepile);
Vec3* dumpVec3ListeToArray(ElemVec3* tetepile);
Vec2* dumpVec2ListeToArray(ElemVec3* tetepile);
Vec3 getElemByNumber(ElemVec3* tetepile, int nb);

#endif
