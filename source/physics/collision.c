#include "physics/collision.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "physics/particule.h"
#include "fenetre.h"
#include "utils/vec3.h"
#include "utils/listeContact.h"

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

// On calcule le vecteur reliant le centre de la sphere et un point (prédéfini) du plan.
// Ce vecteur est ensuite projeté sur la normale du plan.
// Le vecteur résultant relie le centre de la sphère avec le point le plus proche sur le plan du centre de la sphère.
// Puis on n'a plus qu'a comparer la longueur de ce vecteur avec le rayon de la sphère pour savoir s'il y a contact.
Contact* CollisionGenerator_PlanInfiniSphere(CollisionPlanInfini* a, CollisionSphere* b) {

    Vec3 planToSphere = Vec3_SubOut(a->pos, b->particule.position);

    Vec3 closestPointOnPlanToSphere = Vec3_Project(planToSphere, a->normale);

    float interpenetration = b->rayon - Vec3_Length(closestPointOnPlanToSphere);

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

// En plus de calculer la distance au plan comme dans le cas d'un plan infini, il faut
// s'assurer que le point de contact se situe bien dans les limites du plan
// On crée donc un vecteur partant d'un point sur le plan jusqu'au centre de la sphère
// qu'on projette sur chacun des 2 vecteurs orthonormaux du plan
// Il ne reste plus qu'à tester les longueurs des projections par rapport aux limites du plan
Contact* CollisionGenerator_PlanSphere(CollisionPlan* a, CollisionSphere* b) {

    CollisionPlanInfini testPlan = {};
    testPlan.normale = a->normale;
    testPlan.pos = a->pos;

    Contact* contact = CollisionGenerator_PlanInfiniSphere(&testPlan, b);

    if (contact == NULL)
        return NULL;

    Vec3 planToSphere = Vec3_SubOut(a->pos, b->particule.position);

    Vec3 planToSphereOnX = Vec3_Project(planToSphere, a->x);
    Vec3 planToSphereOnZ = Vec3_Project(planToSphere, a->z);

    float insideU = a->xLength - fabsf(Vec3_Length(planToSphereOnX));
    float insideV = a->zLength - fabsf(Vec3_Length(planToSphereOnZ));

    if (insideU > 0 && insideV > 0)
    {
        //printf("Normale: %.1f %.1f %.1f\n", a->normale.x, a->normale.y, a->normale.z);
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
            {
                Particule_Integre(&objet[i]->sphere.particule, duree);
                resoudCollisionCercleMur(&objet[i]->sphere);
            }

            for (j = i + 1; j < nb; j++) {

                if (objet[i]->type == COLLISION_SPHERE && objet[j]->type == COLLISION_SPHERE)
                    contact = CollisionGenerator_SphereSphere(&objet[i]->sphere, &objet[j]->sphere);

                if (objet[i]->type == COLLISION_SPHERE && objet[j]->type == COLLISION_PLAN_INFINI)
                    contact = CollisionGenerator_PlanInfiniSphere(&objet[j]->planInfini, &objet[i]->sphere);

                else if (objet[i]->type == COLLISION_PLAN_INFINI && objet[j]->type == COLLISION_SPHERE)
                    contact = CollisionGenerator_PlanInfiniSphere(&objet[i]->planInfini, &objet[j]->sphere);

                else if (objet[i]->type == COLLISION_SPHERE && objet[j]->type == COLLISION_PLAN)
                    contact = CollisionGenerator_PlanSphere(&objet[j]->plan, &objet[i]->sphere);

                else if (objet[i]->type == COLLISION_PLAN && objet[j]->type == COLLISION_SPHERE)
                    contact = CollisionGenerator_PlanSphere(&objet[i]->plan, &objet[j]->sphere);

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
