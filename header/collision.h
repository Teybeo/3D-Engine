#ifndef H_COLLISION_GUARD
#define H_COLLISION_GUARD

#include <stdbool.h>

#define MUR_BAS 0
#define MUR_HAUT 10000

#define MUR_DROIT 80
#define MUR_GAUCHE -80
#define MUR_AVANT 80
#define MUR_ARRIERE -80

#include "particule.h"
#include "contact.h"

typedef enum CollisionObjectType {

    COLLISION_SPHERE,
    COLLISION_PLAN_INFINI,
    COLLISION_PLAN

} CollisionObjectType;

typedef struct CollisionSphere {

    Particule particule;
    float rayon;

} CollisionSphere;

typedef struct Vec3 Vec3;

typedef struct CollisionPlanInfini {

    //Particule Particule;
    Vec3 pos;     // Un point servant à positionner le plan
    Vec3 normale; // Normale servant à orienter le plan

} CollisionPlanInfini;

typedef struct CollisionPlan {

    //Particule Particule;
    Vec3 pos;     // Un point servant à positionner le plan
    Vec3 normale; // Vecteur y, perpendiculaire au plan
    Vec3 x;       // Vecteur x
    Vec3 z;       // Vecteur z
    float xLength;
    float zLength;
    float angleX;
    float angleY;
    float angleZ;

} CollisionPlan;

typedef struct CollisionObject {

    CollisionObjectType type;
    union {
        CollisionSphere sphere;
        CollisionPlanInfini planInfini;
        CollisionPlan plan;
    };

} CollisionObject;

Contact* CollisionGenerator_SphereSphere(CollisionSphere* a, CollisionSphere* b);
Contact* CollisionGenerator_PlanInfiniSphere(CollisionPlanInfini* a, CollisionSphere* b);
Contact* CollisionGenerator_PlanSphere(CollisionPlan* a, CollisionSphere* b);

void rameneEnContact(Particule* a, Particule* b, Vec3 normale, float valPenetration);

bool CollisionGenerator_AreCollidingSphere(CollisionSphere a, CollisionSphere b);

void Container_Process(CollisionObject** objet, int nb, float duree, bool const pause);
void Container_AddCollisionsToCheck(CollisionObject** container, CollisionObject* tab, int nb);
void Container_Clear();

#endif // Collision

