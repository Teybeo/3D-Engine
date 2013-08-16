#ifndef _SPHERE_GUARD
#define _SPHERE_GUARD

#include "physics/collision.h"
#define NB_BALLS_MAX 20

typedef struct Sphere {

    CollisionSphere collisionData;
    Object3D object;

} Sphere;

typedef struct SphereGroupe {

    CollisionObject* collisionData;
    int nbSpheres;
    int nbMax;
    Mesh* mesh;
    Shader shader;
    GLuint texture;

} SphereGroupe;

SphereGroupe SphereGroupe_Create(int nbMax, Mesh* mesh, Shader shader, GLuint texture);
void SphereGroupe_Draw(SphereGroupe sphereGroupe, float* mondeToCam, float* camToClip);
void Sphere_Add(SphereGroupe* sphereGroupe, Vec3 position, Vec3 direction);
void SphereGroupe_Randomize(SphereGroupe* groupe);

void Sphere_Draw(Sphere sphere, float* mondeToCam, float* camToClip);
Sphere* initGroupeSphere(Object3D object, int nombre);

#endif // SPHERE

