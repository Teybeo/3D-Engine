#include "app.h"

#include "shader.h"
#include "matrix.h"
#include "gl_debug.h"

#include <stdbool.h>
#include <time.h>

#include "SDL.h"
#include "glew.h"
#include "texture.h"
#include "listeContact.h"

void calcule(CollisionSphere* balle, int nb, CollisionSphere* robot, float duree, bool const pause);

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
    glUseProgram(app->instancePerFragmentProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instancePerFragmentProgram, name), 1, &app->lampe[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(app->instancePerFragmentProgram, name), 1, &app->lampe[i].color.x);
        }
    glUseProgram(0);
    glUseProgram(app->perFragmentProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(app->perFragmentProgram, name), 1, &app->lampe[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(app->perFragmentProgram, name), 1, &app->lampe[i].color.x);
        }
    glUseProgram(0);

//    InstanceGroupe_Draw(app->objectGroupe, app->player.mondeToCam, app->fenetre.camToClip);

    Robot_draw(&app->robot, app->player.mondeToCam, app->fenetre.camToClip);

    for (i = 0 ; i < 1 ; i++ )
        Instance_Draw(app->objects[i], app->player.mondeToCam, app->fenetre.camToClip);

    for (i = 0 ; i < 6 ; i++ )
        Instance_Draw(app->lampe[i].instance, app->player.mondeToCam, app->fenetre.camToClip);

    SphereGroupe_Draw(app->sphereGroupe, app->player.mondeToCam, app->fenetre.camToClip);

//    for (i = 0 ; i < app->nb ; i++ )
//        Sphere_Draw(app->balle[i], app->player.mondeToCam, app->fenetre.camToClip);

    SDL_GL_SwapWindow(app->fenetre.ecran);

//    getchar();
//    }

}


void App_Logic(App* app, float duree) {

    Player_update(&app->player);

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
        loadIdentity(app->lampe[i].instance.matrix);
        translate(app->lampe[i].instance.matrix, app->lampe[i].pos.x, app->lampe[i].pos.y, app->lampe[i].pos.z);
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

    app->robot.collisionSphere.particule.position = app->player.posRobot;

    calcule(app->sphereGroupe.collisionData, app->sphereGroupe.nbSpheres, &app->robot.collisionSphere, duree*0.01, false);

    t += 0.01;
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

    if (initProgram(&app->noTexNoLightProgram, "../source/shaders/noTexNoLight.vert", "../source/shaders/noTexNoLight.frag") == false)
        return false;

    if (initProgram(&app->texProgram, "../source/shaders/noLight.vert", "../source/shaders/noLight.frag") == false)
        return false;

//    if (initProgram(&app->perVertexProgram, "../source/shaders/perVertex.vert", "../source/shaders/perVertex.frag") == false)
//        return false;

    if (initProgram(&app->perFragmentProgram, "../source/shaders/perFragment.vert", "../source/shaders/perFragment.frag") == false)
        return false;

///////////////////// ROBOT

    if (Robot_init(&app->robot, app->perFragmentProgram) == false)
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

    app->objects[0] = Instance_Create(carre, app->perFragmentProgram, solTexture);

    loadIdentity(app->objects[0].matrix);
    translate(app->objects[0].matrix, 0, -3, 0);
    scale(app->objects[0].matrix, 500, 1, 500);
    rotate(app->objects[0].matrix, -90, 0, 0);

////////////////////  GROUPE D'INSTANCES SANS INSTACIATION GEOMETRIQUE

    Model* sphere = Model_Load(MODEL_OBJ , "../models/sphere.obj");
    if (sphere == NULL)
        return false;

    Instance object = Instance_Create(sphere, app->perFragmentProgram, solTexture);

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

//////////////////////// GROUPE D'INSTANCES

    Model* cubeTexNorm = Model_Load(MODEL_OBJ , "../models/sphere.obj");
    if (cubeTexNorm == 0)
        return false;

    if (initProgram(&app->instancePerFragmentProgram, "../source/shaders/instancePerFragment.vert", "../source/shaders/perFragment.frag") == false)
        return false;

    app->objectGroupe = InstanceGroupe_Create(cubeTexNorm, 50, app->instancePerFragmentProgram, stoneTexture);

//////////////////////// SKYBOX

    if ((skyboxTexture = chargerTexture("../images/skybox.bmp", GL_NEAREST)) == 0)
        return false;

    Model* cubeTex = Model_Load(MODEL_OBJ, "../models/skybox.obj");
    if (cubeTex == NULL)
        return false;

    app->skybox = Instance_Create(cubeTex, app->texProgram, skyboxTexture);
    loadIdentity(app->skybox.matrix);
    scale(app->skybox.matrix, 5, 5, 5);

//////////////// LUMIERES

    Model* lightBox = Model_Load(MODEL_OBJ, "../models/sphere.obj");
    if (lightBox == NULL)
        return false;

    Instance light = Instance_Create(lightBox, app->perFragmentProgram, stoneTexture);

    for (i = 0 ; i < 10 ; i++ )
        app->lampe[i].instance = light;

    Light_SetPosColor(&app->lampe[0], (Vec3){-15, 2, 0}, (Vec3){0, 1, 0});
    Light_SetPosColor(&app->lampe[1], (Vec3){15, 2, 0}, (Vec3){1, 0, 0});
    Light_SetPosColor(&app->lampe[2], (Vec3){-25, 5, 15}, (Vec3){0, 0, 1});
    Light_SetPosColor(&app->lampe[3], (Vec3){-15, 1, -15}, (Vec3){1, 1, 0});
    Light_SetPosColor(&app->lampe[4], (Vec3){-15, 2, -10}, (Vec3){1, 0, 1});
    Light_SetPosColor(&app->lampe[5], (Vec3){-15, 2, -60}, (Vec3){0, 1, 1});


//////////// BALLES

    Model* sphereModel = Model_Load(MODEL_OBJ, "../models/sphere.obj");
    if (sphereModel == NULL)
        return false;

    app->sphereGroupe = SphereGroupe_Create(NB_BALLS_MAX, sphereModel, app->perFragmentProgram, solTexture);


    app->locProjMatrix = glGetUniformLocation(app->noTexNoLightProgram, "camClip");

    return true;
}

void calcule(CollisionSphere* balle, int nb, CollisionSphere* robot, float duree, bool const pause) {

    Contact* contact = NULL;
    ElemContact* pile = NULL;
    int nbObjects = nb;// + 1;

    CollisionSphere** objet = malloc(sizeof(CollisionSphere*) * nbObjects );

    int i;
    for (i = 0 ; i < nbObjects ; i++ )
        objet[i] = &balle[i];
//    objet[i] = robot;

    if (pause == false) {

        int i, j;

        for (i = 0; i < nbObjects; i++) {

            Particule_Integre(&objet[i]->particule, duree);
            resoudCollisionCercleMur(objet[i]);

            for (j = i + 1; j < nbObjects; j++) {

                contact = CollisionGenerator_SphereSphere(objet[i], objet[j]);
                if (contact != NULL)
                {
                    pile = empilerContact(pile, *contact);
                    free(contact);
                }

            }

        }

        CollisionResolver_Resolve(pile);

    }

    liberePileContact(pile);
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

            Vec3 direction = {};

            direction.x =  sin(angleY) * cos(angleX);
            direction.y = -sin(angleX);
            direction.z = -cos(angleY) * cos(angleX);
            Vec3_Mul_Scal(&direction, 10);

            Vec3 pos = app->player.posRobot;
            printf("Pos %f %f %f\n", pos.x, pos.y, pos.z);
            Vec3_Add(&pos, direction);
            pos.y += 11;
            Sphere_Add(&app->sphereGroupe, pos, direction);

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
    float duree = 0;

    while (app->fenetre.arret == false) {

        App_Event(app);
        App_Logic(app, duree);
        App_Draw(app);

        char title[20] = "";
        duree = SDL_GetTicks() - debut;
        sprintf(title, "%.0f ms", duree);
        SDL_SetWindowTitle(app->fenetre.ecran, title);

        debut = SDL_GetTicks();

    }

}
