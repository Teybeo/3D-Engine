#ifndef _BULLET_GUARD
#define _BULLET_GUARD

#include "physics/collision.h"
#include "shader.h"
#include "object3d.h"

typedef struct Bullet {

    CollisionSphere collisionData;
    Object3D object;
    Vec3 pos;
    Vec3 color;

} Bullet;

typedef struct BulletGroupe {

    CollisionObject* collisionData;
    int nbBullets;
    int nbMax;
    Mesh* mesh;
    Shader* shader;
    GLuint texture;

} BulletGroupe;

typedef struct _Renderer Renderer;

BulletGroupe BulletGroupe_Create(int nbMax, Mesh* mesh, const char* shader, GLuint texture);
void BulletGroupe_Draw(BulletGroupe bulletGroupe, Renderer* renderer);
void Bullet_Add(BulletGroupe* bulletGroupe, Vec3 position, Vec3 direction);

void Bullet_Draw(Bullet bullet, Renderer* renderer);

#endif // BULLET

