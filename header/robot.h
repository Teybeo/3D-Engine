#ifndef _ROBOT_GUARD
#define _ROBOT_GUARD

#include "instance.h"
#include "vec3.h"

#include "SDL_events.h"

#include <stdbool.h>

typedef enum Keys {

    AVANCER,
    RECULER,
    GAUCHE,
    DROITE,
    KEYS_NB

} Keys;

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

    Keys state[KEYS_NB];
    Instance partie[PARTIE_NB];
    float matrix[16];
    Vec3 pos;              // Position
    float phi;             // Orientation, angle horizontal
    float theta;           // Inclinaison, angle vertical
    Vec3 frontalVec;       // Vecteur du regard de la camera, -z, exprimé dans un repère de base (sans la rotation)
    Vec3 lateralVec;       // Vecteur latéral de la caméra, x, exprimé "
    float angleJambeDroite;
    float angleJambeGauche;
    int locModelWorld;

} Robot;

bool Robot_init(Robot* robot, GLuint program);
void Robot_draw(Robot* robot, float* worldCam, float* camClip);
void Robot_move(Robot* robot);
void Robot_keyEvent(Robot* robot, SDL_KeyboardEvent ev);
void Robot_mouseEvent(Robot* robot, float dPhi, float dTheta);

#endif // ROBOT

