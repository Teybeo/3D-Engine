#ifndef _ROBOT_GUARD
#define _ROBOT_GUARD

#include "instance.h"
#include "vec3.h"
#include "collision.h"

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

    Instance partie[PARTIE_NB];
    float matrix[16];
    float angleJambeDroite;
    float angleJambeGauche;
    int locModelWorld;
    CollisionObject collisionObject;

} Robot;

bool Robot_init(Robot* robot, GLuint program);
void Robot_draw(Robot* robot, float* worldCam, float* camClip);

#endif // ROBOT

