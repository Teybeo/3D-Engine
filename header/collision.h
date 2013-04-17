#ifndef H_COLLISION_GUARD
#define H_COLLISION_GUARD

#include <stdbool.h>

#define MUR_DROIT 100
#define MUR_GAUCHE -100
#define MUR_HAUT 100
#define MUR_BAS 0
#define MUR_AVANT 100
#define MUR_ARRIERE -100

#include "particule.h"
#include "contact.h"

typedef struct CollisionSphere {

    Particule particule;
    float rayon;

} CollisionSphere;

typedef struct Vec3 Vec3;

Contact* CollisionGenerator_SphereSphere(CollisionSphere* a, CollisionSphere* b);

void rameneEnContact(Particule* a, Particule* b, Vec3 normale, float valPenetration);

bool CollisionGenerator_AreCollidingSphere(CollisionSphere a, CollisionSphere b);

#endif // Collision

