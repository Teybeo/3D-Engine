#ifndef _SPHERE_GUARD
#define _SPHERE_GUARD

#include "physics/collision.h"
#include "object3d.h"

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
    Shader* shader;
    GLuint texture;

} SphereGroupe;

typedef struct _Renderer Renderer;

SphereGroupe SphereGroupe_Create(int nbMax, Mesh* mesh, const char* shader, GLuint texture);
void SphereGroupe_Draw(SphereGroupe sphereGroupe, Renderer* renderer);
void Sphere_Add(SphereGroupe* sphereGroupe, Vec3 position, Vec3 direction);
void SphereGroupe_Randomize(SphereGroupe* groupe);

void Sphere_Draw(Sphere sphere, Renderer* renderer);
Sphere* initGroupeSphere(Object3D object, int nombre);

#endif // SPHERE

