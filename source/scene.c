#include "scene.h"

#include "texture.h"

#include <stdlib.h>
#include <stdbool.h>

float identity[16];

void Scene_Update(Scene* scene, float duree) {

    static float t = 0;

    scene->lampe[0].pos.x = -300 + 20*sin(t);
    scene->lampe[0].pos.z = 20*cos(t);
    scene->lampe[0].pos.y = 30;

    scene->lampe[1].pos.x = 200+(20*cos(t));
    scene->lampe[1].pos.z = 0;
    scene->lampe[1].pos.y = 30 + 20*sin(t*0.01);

    scene->lampe[2].pos.x = 200*(cos(t+3));
    scene->lampe[2].pos.z = 20+20 * sin(t+3);
    scene->lampe[2].pos.y = 35;

    scene->lampe[3].pos.x = 60;
    scene->lampe[3].pos.z = 300 + 20*cos(t+.5);
    scene->lampe[3].pos.y = 30 + 20*sin(t*0.5);

    scene->lampe[4].pos.x = 60*cos(t+0.1);
    scene->lampe[4].pos.z = -400;
    scene->lampe[4].pos.y = 20 + 20*sin(t*0.1);

    scene->lampe[5].pos.x = 50*cos((t*0.2)+0.6);
    scene->lampe[5].pos.z = 50*sin((t*0.2)+.6);
    scene->lampe[5].pos.y = 30;

    int i;
    for (i = 0 ; i < 6 ; i++ )
    {
        loadIdentity(scene->lampe[i].object.matrix);
        translate(scene->lampe[i].object.matrix, scene->lampe[i].pos.x, scene->lampe[i].pos.y, scene->lampe[i].pos.z);
    }
/*
    for (i = 1 ; i < 100 ; i++ )
    {
        loadIdentity(scene->objects[i].matrix);
        translate(scene->objects[i].matrix, 50*cos(i - t), 5 + i/10., 50*sin(i+-t));
        scale(scene->objects[i].matrix, 3, 3, 3);
        rotate(scene->objects[i].matrix, 100*t+i, 100*t+i, 100*t+i);
//        translate(scene->objects[i].matrix, 0.1*i, 5, 5);
    }*/

//    float scaleFactor;
//    for (i = 0 ; i < scene->groupe.nbObject3Ds ; i++ )
//    {
//        scaleFactor = 3+(i/100);
//        loadIdentity(scene->groupe.matrix[i]);
//        translate(scene->groupe.matrix[i], 400*sin(0.1*i - t), 5 + i/10., 400*cos(i+-t));
//        scale(scene->groupe.matrix[i], scaleFactor, scaleFactor, scaleFactor);
//        transpose(scene->groupe.matrix[i]);
//    }


//    scene->planes[0].collisionData->plan.angleZ = 20*cos(t*1.);
//    scene->planes[0].collisionData->plan.angleX = 20*sin(t*1.);
//    scene->planes[2].collisionData->plan.angleZ -= .2;
//    Plan_RotateBase(&scene->planes[0]);
//    Plan_RotateBase(&scene->planes[1]);
//    Plan_RotateBase(&scene->planes[2]);

//    uploadMatrix(scene->groupe);

    scene->robot->collisionObject.sphere.particule.position = scene->player.posRobot;

    int nbObjects = scene->sphere.nbSpheres + scene->bullet.nbBullets + 0 + 3;// enlever +1;

    CollisionObject** container = malloc(sizeof(CollisionObject*) * nbObjects );

    Container_Clear();
    Container_AddCollisionsToCheck(container, scene->sphere.collisionData, scene->sphere.nbSpheres);
    Container_AddCollisionsToCheck(container, scene->bullet.collisionData, scene->bullet.nbBullets);
   // Container_AddCollisionsToCheck(container, scene->wall, 5);
    for (i = 0 ; i < 3 ; i++ )
        Container_AddCollisionsToCheck(container, scene->planes[i].collisionData, 1);

//    Container_AddCollisionsToCheck(container, &scene->robot.collisionObject, 1);

    Container_Process(container, nbObjects, duree*0.01, false);

    SphereGroupe_Update(scene->sphere);


    free(container);
    t += 0.01;
}

bool Scene_Init(Scene* scene) {

///////////////////// ROBOT

    scene->robot = malloc(sizeof(Robot));
    if (Robot_init(scene->robot) == false)
        return false;

    scene->player = Player_init(scene->robot);

//////////////////// OBJECTS

    GLuint stoneTexture, solTexture, skyboxTexture;

    if ((stoneTexture = chargerTexture("../images/stone.png", GL_NEAREST)) == 0)
        return false;

    if ((solTexture = chargerTexture("../images/cyclopean.jpg", GL_LINEAR)) == 0)
        return false;

    int i;
/*    Mesh* carre20 = Mesh_Load(MESH_CARRE_TEX_NORM2, NULL);
    if (carre20 == NULL)
        return false;*/

    loadIdentity(identity);

    scene->objects[1] = Object3D_Create(Mesh_LoadBuiltin(MESH_CARRE_TEX), "noLight", -1);
    scale(scene->objects[1].matrix, 5, 5, 5);
    translate(scene->objects[1].matrix, 2.1, -1.1, -3);

    scene->objects[0] = Object3D_Load("../models/Tests/sponza_muret.obj");
    loadIdentity(scene->objects[0].matrix);
    scale(scene->objects[0].matrix, 10, 10, 10);
    translate(scene->objects[0].matrix, 0, 0.01, 0);

////////////////////  GROUPE D'INSTANCES SANS INSTANCIATION GEOMETRIQUE

    /*Mesh* sphere = Mesh_Load(MESH_OBJ , "../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Object3D object = Object3D_Load("../models/sphere.obj", scene->perFragmentProgram);

    srand(time(NULL));

    int i;
    for (i = 1 ; i < 10000 ; i++ )
    {
        scene->objects[i] = object;
        loadIdentity(scene->objects[i].matrix);
        translate(scene->objects[i].matrix, -40 + rand() % 80, rand() % 30, -40+rand() % 80);
//        scale(scene->objects[i].matrix, 1+ rand() % 4, 1 + rand() % 4, 1+ rand() % 4);
        rotate(scene->objects[i].matrix, rand() % 90, rand() % 90, rand() % 90);
    }
    loadIdentity(scene->objects[1].matrix);*/


//////////////////////// SKYBOX

    if ((skyboxTexture = chargerTexture("../images/skybox.png", GL_NEAREST)) == 0)
        return false;

    Mesh* skyboxMesh = Mesh_LoadBuiltin(MESH_CUBE_TEX_FLIP);
    if (skyboxMesh == NULL)
        return false;

    scene->skybox = Object3D_Create(skyboxMesh, "noLight", skyboxTexture);
    loadIdentity(scene->skybox.matrix);

//////////////// LUMIERES

    Mesh* sphere = Mesh_Load("../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Object3D light = Object3D_Create(sphere, "fullset", stoneTexture);

    for (i = 0 ; i < 6 ; i++ )
        scene->lampe[i].object = light;

    Light_SetPosColor(&scene->lampe[0], (Vec3){-15, 2, 0}, (Vec3){0, 1., 0});
    Light_SetPosColor(&scene->lampe[1], (Vec3){15, 2, 0}, (Vec3){1., 0, 0});
    Light_SetPosColor(&scene->lampe[2], (Vec3){-25, 5, 15}, (Vec3){0, 0, 1.});
    Light_SetPosColor(&scene->lampe[3], (Vec3){-15, 1, -15}, (Vec3){1., 1., 0});
    Light_SetPosColor(&scene->lampe[4], (Vec3){-15, 2, -10}, (Vec3){1., 0, 1});
    Light_SetPosColor(&scene->lampe[5], (Vec3){-15, 2, -60}, (Vec3){0, 1., 1.});

//////////////////////// INSTANCIATION GEOMETRIQUE

    scene->groupe = Object3DGroupe_Create(sphere, 200, "instance", stoneTexture);

//////////// BALLES

    scene->sphere = SphereGroupe_Create(NB_BALLS_MAX, sphere, "instance", solTexture);
    SphereGroupe_Randomize(&scene->sphere);
    SphereGroupe_Update(scene->sphere);

//////////// BULLETS

    /*Mesh* sphereMesh = Mesh_Load(MESH_OBJ, "../models/sphere.obj");
    if (sphereMesh == NULL)
        return false;*/

    int bulletTex = chargerTexture("../images/bulletTex.png", GL_NEAREST);
    if (bulletTex == 0)
        return false;

    scene->bullet = BulletGroupe_Create(NB_BULLETS_MAX, sphere, "fullset", bulletTex);

////////////////////////

    CollisionPlanInfini planInfini[5] = {};

    planInfini[0].normale = (Vec3){1 , 0,  0};
    planInfini[1].normale = (Vec3){-1, 0,  0};
    planInfini[2].normale = (Vec3){0 , 0,  1};
    planInfini[3].normale = (Vec3){0 , 0, -1};
    planInfini[4].normale = (Vec3){0, 1,  0};

    planInfini[0].pos = (Vec3){-500, 0,  0};
    planInfini[1].pos = (Vec3){ 500, 0,  0};
    planInfini[2].pos = (Vec3){0 , 0,  -500};
    planInfini[3].pos = (Vec3){0 , 0, 500};
    planInfini[4].pos = (Vec3){  0, -200,  0};

    for (i = 0 ; i < 5 ; i++ )
    {
        scene->wall[i].planInfini = planInfini[i];
        scene->wall[i].type = COLLISION_PLAN_INFINI;
    }

//////////////////////////////////

    CollisionPlan plan[3] = {};

    plan[0].normale = (Vec3){0, 1,  0};
    plan[1].normale = (Vec3){0, 1,  0};
    plan[2].normale = (Vec3){0, 1,  0};

    plan[0].pos = (Vec3){ 0, 0, 0};
    plan[1].pos = (Vec3){-20, 0, -20};
    plan[2].pos = (Vec3){-70, -5,  50};

    plan[0].x = (Vec3){1, 0,  0};
    plan[1].x = (Vec3){1, 0,  0};
    plan[2].x = (Vec3){1, 0,  0};

    plan[0].z = (Vec3){0, 0, 1};
    plan[1].z = (Vec3){0, 0, 1};
    plan[2].z = (Vec3){0, 0, 1};

    plan[2].angleX = -20;
    plan[1].angleX = 100;

    plan[0].xLength = 500;
    plan[1].xLength = 20;
    plan[2].xLength = 10;
    plan[0].zLength = 500;
    plan[1].zLength = 20;
    plan[2].zLength = 20;

    Mesh* carre = Mesh_LoadBuiltin(MESH_CARRE_TEX_NORM2);
    if (carre == NULL)
        return false;

    GLuint planTex = chargerTexture("../images/blanc.bmp", GL_LINEAR);
    if (planTex == 0)
        return false;

    for (i = 0 ; i < 3 ; i++ )
    {
        scene->planes[i] = Plan_Create(carre, plan[i], "fullset", planTex);
    }
    scene->planes[0] = Plan_Create(carre, plan[0], "fullset", solTexture);

    return true;
}
