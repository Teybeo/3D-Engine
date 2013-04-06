#include "camera.h"

#include "matrix.h"

#include <GL/gl.h>
#include <math.h>

#define TAU 6.283185307179586476925286766559

#define AVANT 0
#define ARRIERE 1
#define GAUCHE 2
#define DROITE 3
#define RALENTI 4

void Camera_init(Camera* cam, GLuint program) {

    setVec3(&cam->pos, 0, -10, 10);
    cam->phi = 180;
    cam->theta = 0;
    cam->locMondeToCam = glGetUniformLocation(program, "worldCam");
    memset(cam->key, 0, 5*sizeof(bool));
}

void Camera_computeMatrix(Camera* cam, Robot* robot, Vec3 playerPos) {

//    glRotatef(cam.theta, 1, 0, 0); // Inclinaison
//    glRotatef(cam.phi, 0, 1, 0);   // Orientation
//    glTranslatef(-cam.pos.    x, -cam.pos.y, -cam.pos.z);

    loadIdentity(cam->mondeToCam);

    if (cam->camMode != CAMERAMODE_FREE)
    {
        memcpy(&cam->pos, &playerPos, sizeof(Vec3));
    }

    rotate(cam->mondeToCam, cam->theta, cam->phi, 0);
    translate(cam->mondeToCam, cam->pos.x, cam->pos.y, cam->pos.z);

    if (cam->camMode == CAMERAMODE_FIRST_PERSON)
    {
        translate(cam->mondeToCam, 0, -8, 0);
        translate(cam->mondeToCam, 2*robot->frontalVec.x, 2*robot->frontalVec.y, 2*robot->frontalVec.z);
    }
    else if (cam->camMode == CAMERAMODE_THIRD_PERSON )
    {
        translate(cam->mondeToCam, 0, -15, 0);
        translate(cam->mondeToCam, -15*robot->frontalVec.x, -15*robot->frontalVec.y, -15*robot->frontalVec.z);
    }

}

void Camera_sendMatrix(Camera cam) {

    glUniformMatrix4fv(cam.locMondeToCam, 1, GL_TRUE, cam.mondeToCam);

}

void Camera_mouseEvent(Camera* cam, float dPhi, float dTheta) {

    cam->phi += dPhi * 0.1; // OpenGL a un repère direct; angle++ -> regard vers la droite
    cam->theta += dTheta * 0.1;

//    printf("%f\n %f\n", cam->phi, cam->theta);

}

void Camera_keyEvent(Camera* cam, SDL_KeyboardEvent keyEv) {

    switch (keyEv.keysym.sym) {

        case SDLK_z:
            cam->key[AVANT] = keyEv.state;
            break;
        case SDLK_s:
            cam->key[ARRIERE] = keyEv.state;
            break;
        case SDLK_q:
            cam->key[GAUCHE] = keyEv.state;
            break;
        case SDLK_d:
            cam->key[DROITE] = keyEv.state;
            break;
        case SDLK_LSHIFT:
            cam->key[RALENTI] = keyEv.state;
            break;
        case SDLK_c:
            if (keyEv.type == SDL_KEYDOWN)
            {
                cam->camMode++;
                cam->camMode %= NB_MODES;
                printf("camMode: %d\n", cam->camMode);
            }
            break;
        default:
            break;
    }


}

// Recalcule les vecteurs frontaux et latéraux définissant l'orientation de la caméra
void Camera_computeVectors(Camera* cam) {

    float phi = cam->phi * TAU/360;
    float theta = cam->theta * TAU/360;

    // Calcul du vecteur -z de la caméra
    cam->frontalVec.z = -cos(phi) * cos(theta);
    cam->frontalVec.x = sin(phi) * cos(theta);
    cam->frontalVec.y = -sin(theta);

    // Calcul du vecteur x de la caméra
    cam->lateralVec.z = sin(phi);
    cam->lateralVec.x = cos(phi);
    cam->lateralVec.y = 0;
    // L'angle theta n'a pas d'influence sur la direction du vecteur x

}
void Camera_move(Camera* cam) {

    Vec3 deplacement = {};

    // On avance la caméra
    if (cam->key[AVANT])
    {
        Vec3_Add(&deplacement, cam->frontalVec);
    }

    // On recule la caméra
    if (cam->key[ARRIERE])
    {
        Vec3 backward = cam->frontalVec;
        Vec3_Mul_Scal(&backward, -1);
        Vec3_Add(&deplacement, backward);
    }

    // On décale vers la droite la caméra
    if (cam->key[DROITE])
    {
        Vec3_Add(&deplacement, cam->lateralVec);
    }

    // On décale vers la gauche la caméra
    if (cam->key[GAUCHE])
    {
        Vec3 left = cam->lateralVec;
        Vec3_Mul_Scal(&left, -1);
        Vec3_Add(&deplacement, left);
    }

    if (cam->key[RALENTI])
    {
        Vec3_Mul_Scal(&deplacement, 0.1);
    }

    // Negatif ca la caméra ne se déplace pas dans le monde,
    // C'est elle qui déplace le monde
    Vec3_Mul_Scal(&deplacement, -0.1);
    Vec3_Add(&cam->pos, deplacement);

}


