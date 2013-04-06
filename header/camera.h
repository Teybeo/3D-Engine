#ifndef camera_GUARD
#define camera_GUARD

#include "vec3.h"

#include <stdbool.h>

#include "SDL_events.h"
#include "glew.h"

typedef enum CameraMode {

    CAMERAMODE_FREE,
    CAMERAMODE_THIRD_PERSON,
    CAMERAMODE_FIRST_PERSON,
    NB_MODES
} CameraMode;

typedef struct Camera {

    Vec3 pos;              // Position
    float phi;             // Orientation, angle horizontal
    float theta;           // Inclinaison, angle vertical
    Vec3 frontalVec;       // Vecteur du regard de la camera, -z, exprimé dans un repère de base (sans la rotation)
    Vec3 lateralVec;       // Vecteur latéral de la caméra, x, exprimé "               "
    bool key[5];           // Contrôles
    float mondeToCam[16];  // Matrice
    GLuint locMondeToCam;  // Emplacement dans le shader
    CameraMode camMode;

} Camera;


#include "robot.h"

void Camera_init(Camera* cam, GLuint program);
void Camera_computeMatrix(Camera* cam, Robot* robot, Vec3 playerPos);
void Camera_sendMatrix(Camera cam);

void Camera_mouseEvent(Camera* cam, float dPhi, float dTheta);
void Camera_keyEvent(Camera* cam, SDL_KeyboardEvent keyEv);

void Camera_computeVectors(Camera* cam);
void Camera_move(Camera* cam);

#endif // camera


