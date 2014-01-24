#ifndef _ROBOT_GUARD
#define _ROBOT_GUARD

#include "object3d.h"
#include "utils/vec3.h"
#include "physics/collision.h"

#include "SDL_events.h"

#include <stdbool.h>

typedef enum Partie {

    TETE,
    CORPS,
    BRAS_GAUCHE,
    BRAS_DROIT,
    AVANT_BRAS_GAUCHE,
    AVANT_BRAS_DROIT,
    JAMBE_GAUCHE,
    JAMBE_DROITE,
    PARTIE_NB

} Partie;

typedef struct Robot {

    Object3D partie[PARTIE_NB];
    float matrix[16];
    float angleJambeDroite;
    float angleJambeGauche;
    int locModelWorld;
    CollisionObject collisionObject;

} Robot;

typedef struct _Renderer Renderer;

bool Robot_init(Robot* robot, const char* shader);
void Robot_draw(Robot* robot, Renderer* renderer);

#endif // ROBOT

