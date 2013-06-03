#include "particule.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Particule Particule_Init(float elasticite, float masse) {

    Particule balle = {};

    balle.coeffRebond = elasticite;
    balle.masse = masse;

    return balle;
}

void Particule_Integre(Particule* const balle, float facteur) {

    Vec3 accelerationFinale = balle->forceAccum; // On récupère les forces reçues pendant ce cycle
    accelerationFinale.y += (-.5 * facteur); // On y ajoute la gravité

    Vec3_Add(&balle->vitesse, accelerationFinale);

    Vec3_Add(&balle->position, Vec3_Mul_Scal_out(balle->vitesse, facteur));
//    printf("Pos %f %f %f\n", balle->position.x, balle->position.y, balle->position.z);

    setVec3(&balle->forceAccum, 0, 0, 0); // On supprime les forces après application

}

void Particule_AjouteForceRand(Particule* const balle, bool randAngle, bool randMagnitude) {

    float angle = 0, magnitude = 0;

    if (randAngle == true)
        angle = (rand() % 360) * (TAU/360);

    if (randMagnitude == true)
        magnitude = 1 + (rand() % 2000)*0.01;

    Particule_AjouteForce(balle, angle, magnitude);

}

void Particule_AjouteForce(Particule* a, float angle, float magnitude) {

    a->forceAccum.x += cos(angle)*magnitude;
    a->forceAccum.y += sin(angle)*magnitude;
}

void Particule_SetVitesse(Particule*a, Vec3 vitesse) {

    setVec3(&a->vitesse, vitesse.x, vitesse.y, vitesse.z);

}

void Particule_SetPosition(Particule*a, Vec3 position) {

    setVec3(&a->position, position.x, position.y, position.z);
}

void Particule_SetPosition2(Particule*a, float x, float y, float z) {

    setVec3(&a->position, x, y, z);
}

void Particule_SetMasse(Particule*a, float masse) {

    a->masse = masse;
}

void Particule_Debug(Particule balle) {

    puts("   Position         Vitesse        Acceleration");
    printf("%.2f %.2f   %.4f %.4f   %.4f %.4f\n", balle.position.x, balle.position.y,
                                balle.vitesse.x, balle.vitesse.y,
                                balle.forceAccum.x, balle.forceAccum.y);

}
