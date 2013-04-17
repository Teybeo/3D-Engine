#include "collision.h"

#include <stdio.h>
#include <stdlib.h>

#include "particule.h"
#include "fenetre.h"
#include "vec3.h"

void resoudCollisionCercleMur(Particule* a) {

    if (a->position.x + a->rayon > MUR_DROIT) { // Droite
        a->position.x = MUR_DROIT - a->rayon;
        a->vitesse.x *= -a->coeffRebond;
    }
    if (a->position.x - a->rayon < MUR_GAUCHE) { // Gauche
        a->position.x = MUR_GAUCHE + (int)a->rayon;
        a->vitesse.x *= -a->coeffRebond;
    }
    if (a->position.y + a->rayon > MUR_HAUT) { // Haut
        a->position.y = MUR_HAUT - a->rayon;
        a->vitesse.y *= -a->coeffRebond;
    }
    if (a->position.y - a->rayon < MUR_BAS) { // Bas
        a->position.y = MUR_BAS + (int)a->rayon;
        a->vitesse.y *= -a->coeffRebond;
    }
    if (a->position.z - a->rayon < MUR_ARRIERE) { // Arrière
        a->position.z = MUR_ARRIERE + (int)a->rayon;
        a->vitesse.z *= -a->coeffRebond;
    }
    if (a->position.z + a->rayon > MUR_AVANT) { // Avant
        a->position.z = MUR_AVANT - (int)a->rayon;
        a->vitesse.z *= -a->coeffRebond;
    }

}

void resoudCollisionCercleCercle(Particule* a, Particule*b) {

    float interpenetration = (a->rayon + b->rayon) - Vec3_Distance2Points(a->position, b->position);

    if (interpenetration > 0)
    {
        // Vecteur de la vitesse de rapprochement de b vers a
        Vec3 vitRelative = { a->vitesse.x - b->vitesse.x,
                            a->vitesse.y - b->vitesse.y,
                            a->vitesse.z - b->vitesse.z };

        // Normale au plan de collision
        Vec3 normale = Vec3_Normalise_Out(Vec3_SubOut(a->position, b->position));

        // Projection de la vitesse relative sur la normale de la collision
        Vec3 impulsionA = Vec3_Project(vitRelative, normale);

        Vec3_Div_Scal(&impulsionA, (1/a->masse) + (1/b->masse));

        Vec3 impulsionB = impulsionA;

        // En dessous d'un certain seuil de vitesse verticale, on annule l'élasticité pour réduire les instabilités
        float coeffA = a->coeffRebond, coeffB = b->coeffRebond;
        if (abs(a->vitesse.y)+abs(a->vitesse.x)+abs(a->vitesse.z) < 0.1)
            coeffA = 0;

        if (abs(b->vitesse.y)+abs(b->vitesse.x)+abs(b->vitesse.z) < 0.1)
            coeffB = 0;

        Vec3_Mul_Scal(&impulsionA, (1+coeffA)/a->masse);
        Vec3_Mul_Scal(&impulsionB, (1+coeffB)/b->masse);

        Vec3_Sub(&a->vitesse, impulsionA);
        Vec3_Add(&b->vitesse, impulsionB);

        rameneEnContact(a, b, normale, interpenetration);

    }


}

void rameneEnContact(Particule* a, Particule* b, Vec3 normale, float valPenetration) {

    // On applique sur cet axe la moitié de la distance de pénetration;
    Vec3 separateur = Vec3_Mul_Scal_out(normale, -0.5*valPenetration);

    Vec3_Sub(&a->position, separateur);
    Vec3_Add(&b->position, separateur);

}

bool testCollisionCercleCercle(Particule a, Particule b) {

  return Vec3_Distance2PointsCarree(a.position, b.position) < (a.rayon + b.rayon)*(a.rayon + b.rayon);

}

