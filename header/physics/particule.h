#ifndef H_PARTICULE_GUARD
#define H_PARTICULE_GUARD

#include <stdbool.h>

#include "instance.h"
#include "utils/vec3.h"

typedef struct Particule {

    Vec3 position;
    Vec3 vitesse;
    Vec3 forceAccum;

    float coeffRebond;
    float masse;

} Particule;

Particule Particule_Init(float elasticite, float masse);

void Particule_Integre(Particule* const balle, float facteur);

void Particule_AjouteForce(Particule* a, float angle, float magnitude);
void Particule_AjouteForceRand(Particule* const balle, bool randAngle, bool randMagnitude);

void Particule_SetVitesse(Particule*a, Vec3 vitesse);

void Particule_SetPosition(Particule*a, Vec3 position);
void Particule_SetPosition2(Particule*a, float x, float y, float z);
void Particule_SetMasse(Particule*a, float masse);

void Particule_Debug(Particule);

#endif // Particule


