#ifndef _SCENE_GUARD
#define _SCENE_GUARD

#include "object3d.h"
#include "sphere.h"
#include "bullet.h"
#include "robot.h"
#include "player.h"
#include "light.h"
#include "plan.h"
#include "physics/collision.h"

#define NB_BULLETS_MAX 4
#define NB_BALLS_MAX 50

typedef struct _Scene {

    Object3D objects[100];
    Object3DGroupe groupe;
    SphereGroupe sphere;
    BulletGroupe bullet;
    Robot* robot;
    Player player;
    Object3D skybox;
    Light lampe[10];
    CollisionObject wall[6];
    Plan planes[3];

} Scene;

void Scene_Update(Scene* scene, float duree);
bool Scene_Init(Scene* scene);

#endif // SCENE

