#ifndef _PLAYER_GUARD
#define _PLAYER_GUARD

#include "robot.h"
#include "vec3.h"
#include "matrix.h"
#include <GL/gl.h>
#include <math.h>

#include "SDL_events.h"

typedef enum CameraMode {

    CAMERAMODE_THIRD_PERSON,
    CAMERAMODE_FIRST_PERSON,
    CAMERAMODE_FREE,
    NB_CAMERAMODES

} CameraMode;

typedef enum KeyState {

    AVANCER,
    RECULER,
    GAUCHE,
    DROITE,
    RALENTI,
    NB_KEYSTATE

} KeyState;

typedef struct Player {

    float mondeToCam[16];
	float* robotMat;
	Robot* robot;
	KeyState key[NB_KEYSTATE];
	CameraMode camMode;
	float angleY; // Angle autour de l'axe Y, horizontal
	float angleX; // Angle autor de l'axe X, vertical
	float angleYRobot; // Orientation du robot, elle est appliquée seulement si déplacement
	Vec3 frontalVec;
	Vec3 lateralVec;
	Vec3 posRobot;
	Vec3 posCam;
	Vec3 position;

} Player;

Player Player_init(Robot* robotMatrix);
void Player_update(Player* player);
void Player_mouseMotionEvent(Player* player, float dPhi, float dTheta);
void Player_mouseButtonEvent(Player* player, SDL_MouseButtonEvent ev);
void Player_keyEvent(Player* player, SDL_KeyboardEvent ev);


#endif // PLAYER
