#include "collision.h"

#include <stdio.h>
#include <stdlib.h>

#include "particule.h"
#include "fenetre.h"
#include "vec3.h"
#include "listeContact.h"

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

Contact* CollisionGenerator_PlanInfiniSphere(CollisionPlanInfini* a, CollisionSphere* b) {

    Vec3 ab = Vec3_SubOut(a->pos, b->particule.position);

    Vec3 planToSphere = Vec3_Project(ab, a->normale);

    float interpenetration = b->rayon - Vec3_Length(planToSphere);

    if (interpenetration > 0)
    {
        //printf("Normale: %.1f %.1f %.1f\n", a->normale.x, a->normale.y, a->normale.z);

        Contact* contact = malloc(sizeof(Contact));
        contact->interpenetration = interpenetration;
        contact->normale = a->normale;
        contact->a = &b->particule;
        contact->b = NULL;

        return contact;
    }
    else
        return NULL;
}


int indexContainer = 0;

void Container_AddCollisionsToCheck(CollisionObject** container, CollisionObject* tab, int nb) {

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

void Container_Process(CollisionObject** objet, int nb, float duree, bool const pause) {

    Contact* contact = NULL;
    ElemContact* pile = NULL;

    if (pause == false) {

        int i, j;

        for (i = 0; i < nb; i++) {

            if (objet[i]->type == COLLISION_SPHERE)
                Particule_Integre(&objet[i]->sphere.particule, duree);
//            resoudCollisionCercleMur(objet[i]);

            for (j = i + 1; j < nb; j++) {

                if (objet[i]->type == COLLISION_SPHERE && objet[j]->type == COLLISION_SPHERE)
                    contact = CollisionGenerator_SphereSphere(&objet[i]->sphere, &objet[j]->sphere);

                else if (objet[i]->type == COLLISION_SPHERE && objet[j]->type == COLLISION_PLAN)
                    contact = CollisionGenerator_PlanInfiniSphere(&objet[j]->plan, &objet[i]->sphere);

                else if (objet[i]->type == COLLISION_PLAN && objet[j]->type == COLLISION_SPHERE)
                    contact = CollisionGenerator_PlanInfiniSphere(&objet[i]->plan, &objet[j]->sphere);

                if (contact != NULL)
                {
                    pile = empilerContact(pile, *contact);
                    free(contact);
                }

            }

        }

        CollisionResolver_Resolve(pile);

    }

    liberePileContact(pile);
}
