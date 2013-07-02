#include "contact.h"

#include "listeContact.h"
#include "particule.h"
#include "collision.h"

#include <stdlib.h>

void CollisionResolver_Resolve(ElemContact* tetepile) {

    ElemContact* curseur = tetepile;
    Particule* a = NULL;
    Particule* b = NULL;

    while (curseur != NULL) {

        a = curseur->contact.a;
        b = curseur->contact.b;

        // Vecteur de la vitesse de rapprochement de b vers a
        Vec3 vitRelative = a->vitesse;
        if (b != NULL)
            Vec3_Sub(&vitRelative, b->vitesse);

        // Projection de la vitesse relative sur la normale de la collision
        Vec3 impulsionA = Vec3_Project(vitRelative, curseur->contact.normale);

        float totalInverseMass = 1 / a->masse;
        if (b != NULL)
            totalInverseMass += (1 / b->masse);

        Vec3_Div_Scal(&impulsionA, totalInverseMass);

        Vec3 impulsionB = impulsionA;

        // En dessous d'un certain seuil de vitesse, on annule l'élasticité pour réduire les instabilités
        float coeffA = a->coeffRebond, coeffB;
        if (b != NULL)
            coeffB = b->coeffRebond;
//        if (abs(a->vitesse.y) + abs(a->vitesse.x) + abs(a->vitesse.z) < 0.001)
//            coeffA = 0;
//
//        if (abs(b->vitesse.y) + abs(b->vitesse.x) + abs(b->vitesse.z) < 0.001)
//            coeffB = 0;

        Vec3_Mul_Scal(&impulsionA, (1 + coeffA) / a->masse);
        if (b != NULL)
            Vec3_Mul_Scal(&impulsionB, (1 + coeffB) / b->masse);

        Vec3_Sub(&a->vitesse, impulsionA);
        if (b != NULL)
            Vec3_Add(&b->vitesse, impulsionB);

        rameneEnContact(a, b, curseur->contact.normale, curseur->contact.interpenetration);
        curseur = curseur->suivant;
    }

}

void resoudCollisionCercleMur(CollisionSphere* a) {

    if (a->particule.position.x + a->rayon > MUR_DROIT) { // Droite
        a->particule.position.x = MUR_DROIT - a->rayon;
        a->particule.vitesse.x *= -a->particule.coeffRebond;
    }
    if (a->particule.position.x - a->rayon < MUR_GAUCHE) { // Gauche
        a->particule.position.x = MUR_GAUCHE + (int)a->rayon;
        a->particule.vitesse.x *= -a->particule.coeffRebond;
    }
    if (a->particule.position.y + a->rayon > MUR_HAUT) { // Haut
        a->particule.position.y = MUR_HAUT - a->rayon;
        a->particule.vitesse.y *= -a->particule.coeffRebond;
    }
    if (a->particule.position.y - a->rayon < MUR_BAS) { // Bas
        a->particule.position.y = MUR_BAS + (int)a->rayon;
        a->particule.vitesse.y *= -a->particule.coeffRebond;
    }
    if (a->particule.position.z - a->rayon < MUR_ARRIERE) { // Arrière
        a->particule.position.z = MUR_ARRIERE + (int)a->rayon;
        a->particule.vitesse.z *= -a->particule.coeffRebond;
    }
    if (a->particule.position.z + a->rayon > MUR_AVANT) { // Avant
        a->particule.position.z = MUR_AVANT - (int)a->rayon;
        a->particule.vitesse.z *= -a->particule.coeffRebond;
    }

}

void rameneEnContact(Particule* a, Particule* b, Vec3 normale, float valPenetration) {

    // On applique sur cet axe la moitié de la distance de pénetration;
    Vec3 separateur = Vec3_Mul_Scal_out(normale, -0.5*valPenetration);

    Vec3_Sub(&a->position, separateur);
    if (b != NULL)
        Vec3_Add(&b->position, separateur);
    else
        Vec3_Sub(&a->position, separateur);
}
