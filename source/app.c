#include "app.h"

#include "shaderLoader.h"
#include "utils/matrix.h"
#include "gl_debug.h"

#include <stdbool.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "glew.h"
#include "texture.h"

float identity[16];
Vec3 sunDir;
void App_Draw(App* app) {

//    if (app->fenetre.grab == false) {
//        SDL_Delay(50);
//    }
//    else {

    glClear(GL_DEPTH_BUFFER_BIT);

        int i;
    /*glUseProgram(app->instancePerFragmentProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instancePerFragment, name), 1, &app->lampe[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instancePerFragment, name), 1, &app->lampe[i].color.x);
        }
    glUseProgram(0);
    */
//    glUseProgram(app->perFragment.id);
//        for (i = 0 ; i < 6 ; i++ )
//        {
//            char name[50] = "";
//            sprintf(name, "lightPos[%d]", i);
//            glUniform3fv(glGetUniformLocation(app->perFragment.id, name), 1, &app->lampe[i].pos.x);
//            sprintf(name, "lightColor[%d]", i);
//            glUniform3fv(glGetUniformLocation(app->perFragment.id, name), 1, &app->lampe[i].color.x);
//        }
    Vec3* light = Light_Serialize(app->lampe, 6);
    Shader_SendUniformArray(&app->perFragment, "lightPos", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(&app->perFragment, "lightColor", GL_FLOAT_VEC3, 6, &light[6].x);
    Shader_SendUniformArray(&app->instancePerFragment, "lightPos", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(&app->instancePerFragment, "lightColor", GL_FLOAT_VEC3, 6, &light[6].x);
    Shader_SendUniformArray(&app->shadow, "lightPos", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(&app->shadow, "lightColor", GL_FLOAT_VEC3, 6, &light[6].x);
    free(light);
//        for (i = 0 ; i < 6 ; i++ )
//        {
//            char name[50] = "";
//            sprintf(name, "lightInput[%d].pos", i);
//            Shader_SendUniform(app->perFragment, name, GL_FLOAT_VEC3, &app->lampe[i].pos.x);
//            sprintf(name, "lightInput[%d].color", i);
//            Shader_SendUniform(app->perFragment, name, GL_FLOAT_VEC3, &app->lampe[i].color.x);
//        }
//    glUseProgram(0);

//    for (i = 1 ; i <2 ; i++ )
//        Plan_Draw(app->planes[i], app->player.mondeToCam, app->fenetre.camToClip);

//    Object3DGroupe_Draw(app->objectGroupe, app->player.mondeToCam, app->fenetre.camToClip);

    glBindFramebuffer(GL_FRAMEBUFFER, app->framebuffer);
    glViewport(0, 0, SHADOWMAP_W, SHADOWMAP_H);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
        // Render shadowmap for object 0
        Object3D_Draw(app->objects[0], true, app->depth_mondeToCam, app->depth_camToProj, &app->depth);
        SphereGroupe_Draw(app->sphereGroupe, true, app->depth_mondeToCam, app->depth_camToProj, &app->depth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    setPerspective(&app->fenetre);

    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, app->shadowMap);
    glActiveTexture(GL_TEXTURE0);

    Shader_SendUniform(&app->shadow, "depth_mvp", GL_FLOAT_MAT4, MatxMat_GaucheVersDroite(app->depth_mondeToCam, app->depth_camToProj));
    Shader_SendUniform(&app->shadow, "depth_worldCam", GL_FLOAT_MAT4, app->depth_mondeToCam);
    Shader_SendUniform(&app->shadow, "depth_camClip", GL_FLOAT_MAT4, app->depth_camToProj);

    // Render object 0 with shadows
    Object3D_Draw(app->objects[0], false, app->player.mondeToCam, app->fenetre.camToClip, NULL);
    Plan_Draw(app->planes[0], app->player.mondeToCam, app->fenetre.camToClip);

    // shadowmap texture to quad
    glDisable(GL_DEPTH_TEST);
    Object3D_Draw(app->objects[1], false, identity, app->fenetre.camToClip, NULL);
    glEnable(GL_DEPTH_TEST);

    for (i = 0 ; i < 6 ; i++ )
        Object3D_Draw(app->lampe[i].object, false, app->player.mondeToCam, app->fenetre.camToClip, NULL);

    Robot_draw(&app->robot, app->player.mondeToCam, app->fenetre.camToClip);

    SphereGroupe_Draw(app->sphereGroupe, false, app->player.mondeToCam, app->fenetre.camToClip, NULL);
    BulletGroupe_Draw(app->bulletGroupe, app->player.mondeToCam, app->fenetre.camToClip);
//    for (i = 0 ; i < app->nb ; i++ )
//        Sphere_Draw(app->balle[i], app->player.mondeToCam, app->fenetre.camToClip);

    loadIdentity(app->skybox.matrix);
    if (app->player.camMode == CAMERAMODE_FREE)
        translateByVec(app->skybox.matrix, app->player.posCam);
    else
        translateByVec(app->skybox.matrix, app->player.posRobot);
    scale(app->skybox.matrix, 2000, 2000, 2000);
    Object3D_Draw(app->skybox, false, app->player.mondeToCam, app->fenetre.camToClip, NULL);

//    glFinish();
    SDL_GL_SwapWindow(app->fenetre.ecran);

//    getchar();
//    }

}


void App_Logic(App* app, float duree) {

//    memcpy(&app->player.posRobot, &app->lampe[0].pos, sizeof(Vec3));
    Player_update(&app->player);
    updateShadowMatrix(app);

    static float t = 0;

    app->lampe[0].pos.x = -300 + 20*sin(t);
    app->lampe[0].pos.z = 20*cos(t);
    app->lampe[0].pos.y = 30;

    app->lampe[1].pos.x = 200+(20*cos(t));
    app->lampe[1].pos.z = 0;
    app->lampe[1].pos.y = 30 + 20*sin(t*0.01);

    app->lampe[2].pos.x = 200*(cos(t+3));
    app->lampe[2].pos.z = 20+20 * sin(t+3);
    app->lampe[2].pos.y = 35;

    app->lampe[3].pos.x = 60;
    app->lampe[3].pos.z = 300 + 20*cos(t+.5);
    app->lampe[3].pos.y = 30 + 20*sin(t*0.5);

    app->lampe[4].pos.x = 60*cos(t+0.1);
    app->lampe[4].pos.z = -400;
    app->lampe[4].pos.y = 20 + 20*sin(t*0.1);

    app->lampe[5].pos.x = 50*cos(t+0.4);
    app->lampe[5].pos.z = 50*sin(t+.6);
    app->lampe[5].pos.y = 30 + 20*sin(t*0.8);

    int i;
    for (i = 0 ; i < 6 ; i++ )
    {
        loadIdentity(app->lampe[i].object.matrix);
        translate(app->lampe[i].object.matrix, app->lampe[i].pos.x, app->lampe[i].pos.y, app->lampe[i].pos.z);
    }
/*
    for (i = 1 ; i < 100 ; i++ )
    {
        loadIdentity(app->objects[i].matrix);
        translate(app->objects[i].matrix, 50*cos(i - t), 5 + i/10., 50*sin(i+-t));
        scale(app->objects[i].matrix, 3, 3, 3);
        rotate(app->objects[i].matrix, 100*t+i, 100*t+i, 100*t+i);
//        translate(app->objects[i].matrix, 0.1*i, 5, 5);
    }*/

//    float scaleFactor;
//    for (i = 0 ; i < app->objectGroupe.nbObject3Ds ; i++ )
//    {
//        scaleFactor = 3+(i/100);
//        loadIdentity(app->objectGroupe.matrix[i]);
//        translate(app->objectGroupe.matrix[i], 400*sin(0.1*i - t), 5 + i/10., 400*cos(i+-t));
//        scale(app->objectGroupe.matrix[i], scaleFactor, scaleFactor, scaleFactor);
//        transpose(app->objectGroupe.matrix[i]);
//    }

//    app->planes[0].collisionData->plan.angleZ = 5*cos(t/2.);
//    app->planes[0].collisionData->plan.angleX = 5*sin(t/2.);
    app->planes[2].collisionData->plan.angleZ -= .2;
//    Plan_RotateBase(&app->planes[0]);
    Plan_RotateBase(&app->planes[2]);
//    Plan_RotateBase(&app->planes[3]);

    uploadMatrix(app->objectGroupe);

    app->robot.collisionObject.sphere.particule.position = app->player.posRobot;

    int nbObjects = app->sphereGroupe.nbSpheres + app->bulletGroupe.nbBullets + 0 + 3;// enlever +1;

    CollisionObject** container = malloc(sizeof(CollisionObject*) * nbObjects );

    Container_Clear();
//    Container_AddCollisionsToCheck(container, app->objectGroupe.collisionData, app->sphereGroupe.nbSpheres);
    Container_AddCollisionsToCheck(container, app->sphereGroupe.collisionData, app->sphereGroupe.nbSpheres);
    Container_AddCollisionsToCheck(container, app->bulletGroupe.collisionData, app->bulletGroupe.nbBullets);
   // Container_AddCollisionsToCheck(container, app->wall, 5);
    for (i = 0 ; i < 3 ; i++ )
        Container_AddCollisionsToCheck(container, app->planes[i].collisionData, 1);

//    Container_AddCollisionsToCheck(container, &app->robot.collisionObject, 1);

    Container_Process(container, nbObjects, duree*0.01, false);
    free(container);
    t += 0.01;

    Shader_Refresh(&app->perFragment);
    Shader_Refresh(&app->depth);
    Shader_Refresh(&app->shadow);
    Shader_Refresh(&app->onlyTex);
}

bool App_Init(App* app) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur au chargement de SDL2 '%s'\n", SDL_GetError());
        return false;
    }

    if (initFenetre(&app->fenetre) == false)
        return false;

    GLenum err = glewInit();
    glGetError(); // Fix l'erreur de glewInit
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return false;
    }

    if (Init_DebugOutput() == true)
    {
        enableLowSeverityMessages(true);
        enableCallback(true);
    }

    app->onlyTex = Shader_Create("../source/vert_shaders/noLight.vert", "../source/frag_shaders/noLight.frag");
    app->noTexNoLight = Shader_Create("../source/vert_shaders/noTexNoLight.vert", "../source/frag_shaders/noTexNoLight.frag");
    app->perFragment = Shader_Create("../source/vert_shaders/perFragment.vert", "../source/frag_shaders/perFragment.frag");
    app->perVertex = Shader_Create("../source/vert_shaders/perVertex.vert", "../source/frag_shaders/perVertex.frag");
    app->depth = Shader_Create("../source/vert_shaders/depth.vert", "../source/frag_shaders/depth.frag");
    app->shadow = Shader_Create("../source/vert_shaders/shadow.vert", "../source/frag_shaders/shadow.frag");

///////////////////// ROBOT

    if (Robot_init(&app->robot, &app->perFragment) == false)
        return false;

    app->player = Player_init(&app->robot);

//////////////////// OBJECTS

    GLuint stoneTexture, solTexture, skyboxTexture;

    if ((stoneTexture = chargerTexture("../images/stone.png", GL_NEAREST)) == 0)
        return false;

    if ((solTexture = chargerTexture("../images/cyclopean.jpg", GL_LINEAR)) == 0)
        return false;

    int i;

    glGenTextures(1, &app->shadowMap);
/*    Mesh* carre20 = Mesh_Load(MESH_CARRE_TEX_NORM2, NULL);
    if (carre20 == NULL)
        return false;*/

    loadIdentity(identity);

    app->objects[1] = Object3D_Create(Mesh_LoadBuiltin(MESH_CARRE_TEX), &app->onlyTex, app->shadowMap);
    scale(app->objects[1].matrix, 5, 5, 5);
    translate(app->objects[1].matrix, 2.1, -1.1, -3);

    app->objects[0] = Object3D_Load("../models/totem.obj", &app->shadow);
    loadIdentity(app->objects[0].matrix);
    scale(app->objects[0].matrix, 10, 10, 10);
    translate(app->objects[0].matrix, 0, 0.1, 0);

////////////////////  GROUPE D'INSTANCES SANS INSTANCIATION GEOMETRIQUE

    /*Mesh* sphere = Mesh_Load(MESH_OBJ , "../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Object3D object = Object3D_Load("../models/sphere.obj", app->perFragmentProgram);

    srand(time(NULL));

    int i;
    for (i = 1 ; i < 10000 ; i++ )
    {
        app->objects[i] = object;
        loadIdentity(app->objects[i].matrix);
        translate(app->objects[i].matrix, -40 + rand() % 80, rand() % 30, -40+rand() % 80);
//        scale(app->objects[i].matrix, 1+ rand() % 4, 1 + rand() % 4, 1+ rand() % 4);
        rotate(app->objects[i].matrix, rand() % 90, rand() % 90, rand() % 90);
    }
    loadIdentity(app->objects[1].matrix);*/


//////////////////////// SKYBOX

    if ((skyboxTexture = chargerTexture("../images/miramar_large2.jpg", GL_NEAREST)) == 0)
        return false;

    Mesh* skyboxMesh = Mesh_LoadBuiltin(MESH_CUBE_TEX_FLIP);
    if (skyboxMesh == NULL)
        return false;

    app->skybox = Object3D_Create(skyboxMesh, &app->onlyTex, skyboxTexture);
    loadIdentity(app->skybox.matrix);

//////////////// LUMIERES

    Mesh* sphere = Mesh_Load("../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Object3D light = Object3D_Create(sphere, &app->perFragment, stoneTexture);

    for (i = 0 ; i < 6 ; i++ )
        app->lampe[i].object = light;

    Light_SetPosColor(&app->lampe[0], (Vec3){-15, 2, 0}, (Vec3){0, 1., 0});
    Light_SetPosColor(&app->lampe[1], (Vec3){15, 2, 0}, (Vec3){1., 0, 0});
    Light_SetPosColor(&app->lampe[2], (Vec3){-25, 5, 15}, (Vec3){0, 0, 1.});
    Light_SetPosColor(&app->lampe[3], (Vec3){-15, 1, -15}, (Vec3){1., 1., 0});
    Light_SetPosColor(&app->lampe[4], (Vec3){-15, 2, -10}, (Vec3){1., 0, 1});
    Light_SetPosColor(&app->lampe[5], (Vec3){-15, 2, -60}, (Vec3){0, 1., 1.});

//////////////////////// INSTANCIATION GEOMETRIQUE

    Mesh* geomMesh = Mesh_Load("../models/sphere.obj");
    if (geomMesh == 0)
        return false;

    app->instancePerFragment = Shader_Create("../source/vert_shaders/instancePerFragment.vert", "../source/frag_shaders/perFragment.frag");

    app->objectGroupe = Object3DGroupe_Create(geomMesh, 200, &app->instancePerFragment, stoneTexture);


//////////// BALLES

    app->sphereGroupe = SphereGroupe_Create(NB_BALLS_MAX, sphere, &app->perFragment, solTexture);
    SphereGroupe_Randomize(&app->sphereGroupe);

//////////// BULLETS

    /*Mesh* sphereMesh = Mesh_Load(MESH_OBJ, "../models/sphere.obj");
    if (sphereMesh == NULL)
        return false;*/

    int bulletTex = chargerTexture("../images/bulletTex.png", GL_NEAREST);
    if (bulletTex == 0)
        return false;

    app->bulletGroupe = BulletGroupe_Create(NB_BULLETS_MAX, sphere, &app->perFragment, bulletTex);

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
        app->wall[i].planInfini = planInfini[i];
        app->wall[i].type = COLLISION_PLAN_INFINI;
    }

//////////////////////////////////

    CollisionPlan plan[3] = {};

    plan[0].normale = (Vec3){0, 1,  0};
    plan[1].normale = (Vec3){0, 1,  0};
    plan[2].normale = (Vec3){0, 1,  0};

    plan[0].pos = (Vec3){ 0, 0, 0};
    plan[1].pos = (Vec3){-20, 0, -20};
    plan[2].pos = (Vec3){-50, 100,  20};

    plan[0].x = (Vec3){1, 0,  0};
    plan[1].x = (Vec3){1, 0,  0};
    plan[2].x = (Vec3){1, 0,  0};

    plan[0].z = (Vec3){0, 0, 1};
    plan[1].z = (Vec3){0, 0, 1};
    plan[2].z = (Vec3){0, 0, 1};

    plan[2].angleX = 90;

    plan[0].xLength = 500;
    plan[1].xLength = 20;
    plan[2].xLength = 20;
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
        app->planes[i] = Plan_Create(carre, plan[i], &app->shadow, planTex);
    }
  //  app->planes[0] = Plan_Create(carre20, plan[0], app->perFragment, solTexture);

    glGenFramebuffers(1, &app->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, app->framebuffer);


    glBindTexture(GL_TEXTURE_2D, app->shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOWMAP_W, SHADOWMAP_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, app->shadowMap, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        puts("Framebuffer incomplete");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}
void updateShadowMatrix(App* app) {

    static float angleY = 0;
    static float angleX = 40;
    loadIdentity(app->depth_mondeToCam);
    rotate(app->depth_mondeToCam, angleX, angleY, 0);
    sunDir.x =  sin(angleY*TAU/360) * cos(angleX*TAU/360);
    sunDir.y = -sin(angleX*TAU/360);
    sunDir.z = -cos(angleY*TAU/360) * cos(angleX*TAU/360);
    Shader_SendUniform(&app->shadow, "sunDirection", GL_FLOAT_VEC3, &sunDir);
    angleY += 0.1;
//    angleX += 0.1;
//    memcpy(app->depth_mondeToCam, app->player.mondeToCam, sizeof(float)*16);
            loadIdentity(app->depth_camToProj);
//            memcpy(app->depth_camToProj, app->fenetre.camToClip, sizeof(float)*16);
//            ortho(app->depth_camToProj, -15, 15,-15, 15, -5, 10);
            ortho(app->depth_camToProj, -150, 150,-150, 150, -120, 150);
}
void App_Event(App* app) {

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {

    switch (ev.type) {

    case SDL_QUIT:

        app->fenetre.arret = true;

        break;

    case SDL_KEYDOWN:

        if (app->fenetre.grab)
            Player_keyEvent(&app->player, ev.key);

        majFenetreKeys(&app->fenetre, ev.key);

        break;

    case SDL_KEYUP:

        if (app->fenetre.grab)
            Player_keyEvent(&app->player, ev.key);

        majFenetreKeys(&app->fenetre, ev.key);

        break;

    case SDL_MOUSEMOTION:

        if (app->fenetre.grab)
            Player_mouseMotionEvent(&app->player, ev.motion.xrel, ev.motion.yrel);

        break;

    case SDL_MOUSEBUTTONDOWN:

        Player_mouseButtonEvent(&app->player, ev.button);

        if (ev.button.button == SDL_BUTTON_LEFT) {

            float angleY = (app->player.angleY) * M_PI/180;
            float angleX = (app->player.angleX) * M_PI/180;

            // On compense le regard plus bas du mode 3ème personne
            if (app->player.camMode == CAMERAMODE_THIRD_PERSON)
                angleX -= 0.1;

            Vec3 direction = {};

            direction.x =  sin(angleY) * cos(angleX);
            direction.y = -sin(angleX);
            direction.z = -cos(angleY) * cos(angleX);

            Vec3 pos = app->player.posRobot;
            pos.y += 11; // On tire à hauteur de la tete
            Vec3_Add(&pos, Vec3_Mul_Scal_out(direction, 15)); // On avance pour sortir de la hitsphere

            if (app->player.weapon == WEAPONMODE_BALL)
            {
                Vec3_Mul_Scal(&direction, 3); // On donne une force au tir
                Sphere_Add(&app->sphereGroupe, pos, direction);
            }
            else if (app->player.weapon == WEAPONMODE_GUN)
            {
                Vec3_Mul_Scal(&direction, 3); // On donne une force au tir
                Bullet_Add(&app->bulletGroupe, pos, direction);
            }
        }

        break;

    case SDL_MOUSEWHEEL:

        if (ev.wheel.y > 0)
            app->player.weapon++;
        else if (ev.wheel.y < 0)
            app->player.weapon--;

        app->player.weapon %= NB_WEAPONMODES;
        printf("%s\n", app->player.weapon == 0 ? "BALL" : "GUN");
        break;

    case SDL_WINDOWEVENT:

        eventFenetre(&app->fenetre, ev.window);

        break;

    }

    }

}

void App_Run(App* app) {

    Uint32 debut = SDL_GetTicks();
    float duree = 0, precedenteDuree = 0;

    while (app->fenetre.arret == false) {

        App_Event(app);
        App_Logic(app, duree);
        App_Draw(app);

        char title[20] = "";
        duree = SDL_GetTicks() - debut;
        if (fabsf(duree - precedenteDuree) >= 1)
        {
            sprintf(title, "%.0f ms", duree);
            SDL_SetWindowTitle(app->fenetre.ecran, title);
            precedenteDuree = duree;
        }

        debut = SDL_GetTicks();

    }

}
