#include "collision.h"

#include <stdio.h>
#include <stdlib.h>

#include "particule.h"
#include "fenetre.h"
#include "vec3.h"
#include "contact.h"

Contact* CollisionGenerator_SphereSphere(CollisionSphere* a, CollisionSphere* b) {

    float interpenetration = (a->rayon + b->rayon) - Vec3_Distance2Points(a->particule.position, b->particule.position);

    if (interpenetration > 0)
    {
        Contact* contact = malloc(sizeof(Contact));
        contact->interpenetration = interpenetration;
        contact->normale = Vec3_Normalise_Out(Vec3_SubOut(a->particule.position, b->particule.position));
        contact->a = &a->particule;
        contact->b = &b->particule;

        return contact;
    }
    else
        return NULL;

}

bool CollisionGenerator_AreCollidingSphere(CollisionSphere a, CollisionSphere b) {

    return Vec3_Distance2PointsCarree(a.particule.position, b.particule.position) < (a.rayon + b.rayon) * (a.rayon + b.rayon);

}

int indexContainer = 0;

void Container_AddCollisionsToCheck(CollisionSphere** container, CollisionSphere* tab, int nb) {

    if (nb == 0)
        return;

    int i;
    for (i = 0 ; i < nb ; i++ )
        container[indexContainer + i] = &tab[i];

    indexContainer += i;
}

void Container_Clear() {

    indexContainer = 0;

}
