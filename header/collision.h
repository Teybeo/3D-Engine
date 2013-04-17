#ifndef H_COLLISION_GUARD
#define H_COLLISION_GUARD

#include <stdbool.h>

#define MUR_DROIT 400
#define MUR_GAUCHE -400
#define MUR_HAUT 400
#define MUR_BAS 0
#define MUR_AVANT 400
#define MUR_ARRIERE -400

typedef struct Particule Particule;
typedef struct Vec3 Vec3;

void resoudCollisionCercleCercle(Particule* a, Particule*b);
void resoudCollisionCercleMur(Particule* a);

void rameneEnContact(Particule* a, Particule* b, Vec3 normale, float valPenetration);

bool testCollisionCercleCercle(Particule a, Particule b);

#endif // Collision

