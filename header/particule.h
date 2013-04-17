#ifndef H_PARTICULE_GUARD
#define H_PARTICULE_GUARD

#include <stdbool.h>

#include "instance.h"
#include "vec3.h"

typedef struct SDL_Surface SDL_Surface;

typedef struct Particule {

    Vec3 position;
    Vec3 vitesse;
    Vec3 forceAccum;

    unsigned int rayon;
    float coeffRebond;
    float masse;

    Instance instance;

} Particule;

void afficheParticule(Particule);

Particule initParticule(int rayon, float elasticite, float masse);
void ajouteForce(Particule* a, float angle, float magnitude);
void ajouteForceRand(Particule* const balle, bool randAngle, bool randMagnitude);
void setPosition(Particule*a, float x, float y, float z);
void setMasse(Particule*a, float masse);
void setRayon(Particule*a, float rayon);

void integreParticule(Particule* const balle, float facteur);

void dessineParticule(Particule balle);


#endif // Particule


