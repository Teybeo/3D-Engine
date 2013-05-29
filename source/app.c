#include "app.h"

#include "shader.h"
#include "matrix.h"
#include "gl_debug.h"

#include <stdbool.h>
#include <time.h>

#include "SDL.h"
#include "glew.h"
#include "texture.h"

void App_Draw(App* app) {

//    if (app->fenetre.grab == false) {
//        SDL_Delay(50);
//    }
//    else {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
        loadIdentity(app->skybox.matrix);
        if (app->player.camMode == CAMERAMODE_FREE)
            translateByVec(app->skybox.matrix, app->player.posCam);
        else
            translateByVec(app->skybox.matrix, app->player.posRobot);
        scale(app->skybox.matrix, 50, 50, 50);
        Instance_Draw(app->skybox, app->player.mondeToCam, app->fenetre.camToClip);
    glEnable(GL_DEPTH_TEST);

        int i;
    glUseProgram(app->instanceTexPerFragmentDiffuseProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instanceTexPerFragmentDiffuseProgram, name), 1, &app->halogene[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instanceTexPerFragmentDiffuseProgram, name), 1, &app->halogene[i].color.x);
        }
    glUseProgram(0);
    glUseProgram(app->texPerFragmentDiffuseProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(app->texPerFragmentDiffuseProgram, name), 1, &app->halogene[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(app->texPerFragmentDiffuseProgram, name), 1, &app->halogene[i].color.x);
        }
    glUseProgram(0);

    InstanceGroupe_Draw(app->objectGroupe, app->player.mondeToCam, app->fenetre.camToClip);

//        loadIdentity(app->robot.matrix);
//        translate(app->robot.matrix, 20, 0, 0);
//    memcpy(app->robot.matrix, app->player.robot, 16*4);
//    for (i = 0 ; i < 16 ; i++ )
//        app->robot.matrix[i] = app->player.robotMat[i];
    Robot_draw(&app->robot, app->player.mondeToCam, app->fenetre.camToClip);

//    for (i = 0 ; i < 1 ; i++ )
//    {
//        loadIdentity(app->robot.matrix);
//        translate(app->robot.matrix, 0.1*i*cos(i), 0, 0.1*i*sin(i));
//        drawRobot(&app->robot, app->cam.mondeToCam, app->fenetre.camToClip);
//    }
//
    for (i = 0 ; i < 1 ; i++ )
        Instance_Draw(app->objects[i], app->player.mondeToCam, app->fenetre.camToClip);

    for (i = 0 ; i < 6 ; i++ )
        Instance_Draw(app->halogene[i].instance, app->player.mondeToCam, app->fenetre.camToClip);

    SDL_GL_SwapWindow(app->fenetre.ecran);
//    getchar();
//    }

}


void App_Logic(App* app) {

//    Camera_computeVectors(&app->cam);
//    Camera_move(&app->cam);

//    Robot_move(&app->robot);

//    Camera_computeMatrix(&app->cam, &app->robot, app->robot.pos);

    Player_update(&app->player);

    static float t = 0;

    app->halogene[0].pos.x = -300 + 20*sin(t);
    app->halogene[0].pos.z = 20*cos(t);
    app->halogene[0].pos.y = 30;

    app->halogene[1].pos.x = 200+(20*cos(t));
    app->halogene[1].pos.z = 0;
    app->halogene[1].pos.y = 30 + 20*sin(t*0.01);

    app->halogene[2].pos.x = 200*(cos(t+3));
    app->halogene[2].pos.z = 20+20 * sin(t+3);
    app->halogene[2].pos.y = 35;

    app->halogene[3].pos.x = 60;
    app->halogene[3].pos.z = 300 + 20*cos(t+.5);
    app->halogene[3].pos.y = 30 + 20*sin(t*0.5);

    app->halogene[4].pos.x = 60*cos(t+0.1);
    app->halogene[4].pos.z = -400;
    app->halogene[4].pos.y = 20 + 20*sin(t*0.1);

    app->halogene[5].pos.x = 50*cos(t+0.4);
    app->halogene[5].pos.z = 50*sin(t+.6);
    app->halogene[5].pos.y = 30 + 20*sin(t*0.8);

    int i;
    for (i = 0 ; i < 6 ; i++ )
    {
        loadIdentity(app->halogene[i].instance.matrix);
        translate(app->halogene[i].instance.matrix, app->halogene[i].pos.x, app->halogene[i].pos.y, app->halogene[i].pos.z);
    }

    for (i = 1 ; i < 100 ; i++ )
    {
        loadIdentity(app->objects[i].matrix);
        translate(app->objects[i].matrix, 50*cos(i - t), 5 + i/10., 50*sin(i+-t));
        scale(app->objects[i].matrix, 3, 3, 3);
        rotate(app->objects[i].matrix, 100*t+i, 100*t+i, 100*t+i);
//        translate(app->objects[i].matrix, 0.1*i, 5, 5);
    }

    float scaleFactor;
    for (i = 0 ; i < app->objectGroupe.nbInstances ; i++ )
    {
        scaleFactor = 3+(i/100);
        loadIdentity(app->objectGroupe.matrix[i]);
        translate(app->objectGroupe.matrix[i], 50*sin(0.3*i - t), 5 + i/10., 50*cos(i+-t));
        scale(app->objectGroupe.matrix[i], scaleFactor, scaleFactor, scaleFactor);
        transpose(app->objectGroupe.matrix[i]);
    }

    uploadMatrix(app->objectGroupe);

    t += 0.01;
}

bool App_Init(App* app) {
    int var;
    var = SDL_Init(SDL_INIT_VIDEO);
            printf("init: %d\n", var);
    if (var != 0) {
        printf("Erreur au chargement de SDL2 '%s'\n", SDL_GetError());
        return false;
    }

//    printf("%s\n", SDL_GetError());

    if (initFenetre(&app->fenetre) == false)
        return false;

    GLenum err = glewInit();
    glGetError(); // Fix l'erreur de glewInit
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return false;
    }

    init();
    enableLowSeverityMessages(true);
    enableCallback(true);

    if (initProgram(&app->mainProgram, "../source/shaders/main.vert", "../source/shaders/main.frag") == false)
        return false;

    if (initProgram(&app->texProgram, "../source/shaders/texture.vert", "../source/shaders/texture.frag") == false)
        return false;

//    if (initProgram(&app->texPerVertexDiffuseProgram, "../source/shaders/texPerVertexDiffuse.vert", "../source/shaders/texPerVertexDiffuse.frag") == false)
//        return false;

    if (initProgram(&app->texPerFragmentDiffuseProgram, "../source/shaders/texPerFragmentDiffuse.vert", "../source/shaders/texPerFragmentDiffuse.frag") == false)
        return false;

/////////////////////

//    Camera_init(&app->cam, app->mainProgram);
    if (Robot_init(&app->robot, app->texPerFragmentDiffuseProgram) == false)
        return false;

    app->player = Player_init(&app->robot);

//////////////////// SOL

    GLuint stoneTexture, solTexture, skyboxTexture;

    if ((stoneTexture = chargerTexture("../images/stone.bmp", GL_NEAREST)) == 0)
        return false;

    if ((solTexture = chargerTexture("../images/cyclopean.jpg", GL_LINEAR)) == 0)
        return false;

    Model* carre = Model_Load(MODEL_CARRE_TEX_NORM, NULL);
    if (carre == NULL)
        return false;

    app->objects[0] = Instance_Create(carre, app->texPerFragmentDiffuseProgram, solTexture);

    loadIdentity(app->objects[0].matrix);
    translate(app->objects[0].matrix, 0, -3, 0);
    scale(app->objects[0].matrix, 500, 1, 500);
    rotate(app->objects[0].matrix, -90, 0, 0);

    Model* sphere = Model_Load(MODEL_OBJ , "../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Instance object = Instance_Create(sphere, app->texPerFragmentDiffuseProgram, solTexture);

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
    loadIdentity(app->objects[1].matrix);

////////////////////////

    Model* cubeTexNorm = Model_Load(MODEL_OBJ , "../models/sphere.obj");
    if (cubeTexNorm == 0)
        return false;

    if (initProgram(&app->instanceTexPerFragmentDiffuseProgram, "../source/shaders/instanceTexPerFragmentDiffuse.vert", "../source/shaders/texPerFragmentDiffuse.frag") == false)
        return false;

    app->objectGroupe = InstanceGroupe_Create(cubeTexNorm, 50, app->instanceTexPerFragmentDiffuseProgram, stoneTexture);

////////////////////////

    if ((skyboxTexture = chargerTexture("../images/skybox.bmp", GL_NEAREST)) == 0)
        return false;

    Model* cubeTex = Model_Load(MODEL_OBJ, "../models/skybox.obj");
    if (cubeTex == NULL)
        return false;

    app->skybox = Instance_Create(cubeTex, app->texProgram, skyboxTexture);
    loadIdentity(app->skybox.matrix);
    scale(app->skybox.matrix, 5, 5, 5);

////////////////

    Model* lightBox = Model_Load(MODEL_OBJ, "../models/sphere.obj");
    if (lightBox == NULL)
        return false;

    Instance light = Instance_Create(lightBox, app->texPerFragmentDiffuseProgram, stoneTexture);

    for (i = 0 ; i < 10 ; i++ )
        app->halogene[i].instance = light;

    Light_SetPosColor(&app->halogene[0], (Vec3){-15, 2, 0}, (Vec3){0, 1, 0});
    Light_SetPosColor(&app->halogene[1], (Vec3){15, 2, 0}, (Vec3){1, 0, 0});
    Light_SetPosColor(&app->halogene[2], (Vec3){-25, 5, 15}, (Vec3){0, 0, 1});
    Light_SetPosColor(&app->halogene[3], (Vec3){-15, 1, -15}, (Vec3){1, 1, 0});
    Light_SetPosColor(&app->halogene[4], (Vec3){-15, 2, -10}, (Vec3){1, 0, 1});
    Light_SetPosColor(&app->halogene[5], (Vec3){-15, 2, -60}, (Vec3){0, 1, 1});
//    Light_SetPosColor(&app->halogene[7], (Vec3){-5, 2, 4}, (Vec3){0, 1, 1});
//    Light_SetPosColor(&app->halogene[8], (Vec3){-15, 2, 80}, (Vec3){0, 0, 1});
//    Light_SetPosColor(&app->halogene[9], (Vec3){-15, 2, -80}, (Vec3){1, 0, 1});

    app->locProjMatrix = glGetUniformLocation(app->mainProgram, "camClip");

////////////

    return true;

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
//            Camera_keyEvent(&app->cam, ev.key);
        majFenetreKeys(&app->fenetre, ev.key);
//        Robot_keyEvent(&app->robot, ev.key);

        break;

    case SDL_KEYUP:

        if (app->fenetre.grab)
            Player_keyEvent(&app->player, ev.key);
//            Camera_keyEvent(&app->cam, ev.key);
        majFenetreKeys(&app->fenetre, ev.key);
//        Robot_keyEvent(&app->robot, ev.key);

        break;

    case SDL_MOUSEMOTION:

        if (app->fenetre.grab)
        {
            Player_mouseEvent(&app->player, ev.motion.xrel, ev.motion.yrel);
//            Camera_mouseEvent(&app->cam, ev.motion.xrel, ev.motion.yrel);
//            Robot_mouseEvent(&app->robot, ev.motion.xrel, ev.motion.yrel);
        }
        break;

    case SDL_WINDOWEVENT:

        eventFenetre(&app->fenetre, ev.window);

        break;

    }

    }

}

void App_Run(App* app) {

    Uint32 debut = SDL_GetTicks();

    while (app->fenetre.arret == false) {

        App_Event(app);
        App_Logic(app);
        App_Draw(app);

        char title[20] = "";
        sprintf(title, "%u ms", SDL_GetTicks() - debut);
        SDL_SetWindowTitle(app->fenetre.ecran, title);

        debut = SDL_GetTicks();

    }

}
