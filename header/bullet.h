#ifndef _BULLET_GUARD
#define _BULLET_GUARD

#include "physics/collision.h"
#include "shader.h"

typedef struct Bullet {

    CollisionSphere collisionData;
    Instance instance;
    Vec3 pos;
    Vec3 color;

} Bullet;

typedef struct BulletGroupe {

    CollisionObject* collisionData;
    int nbBullets;
    int nbMax;
    Mesh* mesh;
    Shader shader;
    GLuint texture;

} BulletGroupe;

BulletGroupe BulletGroupe_Create(int nbMax, Mesh* mesh, Shader shader, GLuint texture);
void BulletGroupe_Draw(BulletGroupe bulletGroupe, float* mondeToCam, float* camToClip);
void Bullet_Add(BulletGroupe* bulletGroupe, Vec3 position, Vec3 direction);

void Bullet_Draw(Bullet bullet, float* mondeToCam, float* camToClip);

#endif // BULLET

