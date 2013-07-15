#ifndef _CONTACT_GUARD
#define _CONTACT_GUARD

#include "utils/vec3.h"

typedef struct Particule Particule;
typedef struct ElemContact ElemContact;
typedef struct CollisionSphere CollisionSphere;

typedef struct Contact {

    Particule* a;
    Particule* b;
    float interpenetration;
    Vec3 normale;

} Contact;

void CollisionResolver_Resolve(ElemContact* tetepile);
void resoudCollisionCercleMur(CollisionSphere* a);

#endif // CONTACT

