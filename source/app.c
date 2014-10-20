#include "app.h"

#include "shaderLoader.h"
#include "shader_library.h"
#include "utils/matrix.h"
#include "gl_debug.h"

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "glew.h"
#include "texture.h"


void App_Draw(App* app) {

//    if (app->fenetre.grab == false) {
//        SDL_Delay(50);
//    }
//    else {

    Renderer_Render(&app->renderer);

//    glFinish();
    SDL_GL_SwapWindow(app->fenetre.ecran);

//    getchar();
//    }

}


void App_Logic(App* app, float duree) {

    Scene_Update(app->scene, duree);
    Renderer_Update(&app->renderer);
}

bool App_Init(App* app) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur au chargement de SDL2 '%s'\n", SDL_GetError());
        return false;
    }

    if (initFenetre(&app->fenetre) == false)
        return false;

    GLenum err = glewInit();
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

    if (ShaderLibrary_Init() == false)
        return false;

    app->scene = malloc(sizeof(Scene));
    if (Scene_Init(app->scene) == false)
        return false;

    if (Renderer_Init(&app->renderer, app->scene, app->fenetre.largeur, app->fenetre.hauteur) == false)
        return false;


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
            Player_keyEvent(&app->scene->player, ev.key);

        majFenetreKeys(&app->fenetre, ev.key);
        Renderer_keyEvent(&app->renderer, ev.key);

        break;

    case SDL_KEYUP:

        if (app->fenetre.grab)
            Player_keyEvent(&app->scene->player, ev.key);

        majFenetreKeys(&app->fenetre, ev.key);
        Renderer_keyEvent(&app->renderer, ev.key);

        break;

    case SDL_MOUSEMOTION:

        if (app->fenetre.grab)
            Player_mouseMotionEvent(&app->scene->player, ev.motion.xrel, ev.motion.yrel);

        break;

    case SDL_MOUSEBUTTONDOWN:

        Player_mouseButtonEvent(&app->scene->player, ev.button);

        if (ev.button.button == SDL_BUTTON_LEFT) {

            float angleY = (app->scene->player.angleY) * M_PI/180;
            float angleX = (app->scene->player.angleX) * M_PI/180;

            // On compense le regard plus bas du mode 3ème personne
            if (app->scene->player.camMode == CAMERAMODE_THIRD_PERSON)
                angleX -= 0.1;

            Vec3 direction = {};

            direction.x =  sin(angleY) * cos(angleX);
            direction.y = -sin(angleX);
            direction.z = -cos(angleY) * cos(angleX);

            Vec3 pos = app->scene->player.posRobot;
            pos.y += 11; // On tire à hauteur de la tete
            Vec3_Add(&pos, Vec3_Mul_Scal_out(direction, 15)); // On avance pour sortir de la hitsphere

            if (app->scene->player.weapon == WEAPONMODE_BALL)
            {
                Vec3_Mul_Scal(&direction, 1); // On donne une force au tir
                Sphere_Add(&app->scene->sphere, pos, direction);
            }
            else if (app->scene->player.weapon == WEAPONMODE_GUN)
            {
                Vec3_Mul_Scal(&direction, 10); // On donne une force au tir
                Bullet_Add(&app->scene->bullet, pos, direction);
            }
        }

        break;

    case SDL_MOUSEWHEEL:

        if (ev.wheel.y > 0)
            app->scene->player.weapon++;
        else if (ev.wheel.y < 0)
            app->scene->player.weapon--;

        app->scene->player.weapon %= NB_WEAPONMODES;
        printf("%s\n", app->scene->player.weapon == 0 ? "BALL" : "GUN");
        break;

    case SDL_WINDOWEVENT:

        eventFenetre(&app->fenetre, ev.window);
        if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
            Renderer_Resize(&app->renderer, ev.window.data1, ev.window.data2);

        break;

    }

    }

}
#define NB_TIMES (50)
float avg_time(float* times);

void App_Run(App* app) {

    Uint32 debut = SDL_GetTicks();
    float cpu_times[NB_TIMES] = {};
    float gpu_times[NB_TIMES] = {};
    int index_cpu = 0;
    int index_gpu = 0;
    float lastUpdate = 0;
    Uint64 gpuDuration = 0;
    unsigned int query;
    glGenQueries(1, &query);
    char title[60] = "";

    while (app->fenetre.arret == false) {

        App_Event(app);
        App_Logic(app, cpu_times[index_cpu]);

        glBeginQuery(GL_TIME_ELAPSED, query);
            App_Draw(app);
        glEndQuery(GL_TIME_ELAPSED);

        // Get GPU time
        glGetQueryObjectui64v(query, GL_QUERY_RESULT_NO_WAIT, &gpuDuration);
        gpu_times[index_gpu++] = gpuDuration;
        index_gpu %= NB_TIMES;

        // Get CPU time
        cpu_times[index_cpu++] = (SDL_GetTicks() - debut);
        index_cpu %= NB_TIMES;

        // Limit the title bar update rate to 0.1/s to keep it readable and not spam dwm.exe
        if (fabsf(SDL_GetTicks() - lastUpdate) >= 100)
        {
            sprintf(title, "CPU: %.2f ms / GPU: %.2f ms", avg_time(cpu_times), avg_time(gpu_times)/1000000);
            SDL_SetWindowTitle(app->fenetre.ecran, title);
            lastUpdate = SDL_GetTicks();
        }

        debut = SDL_GetTicks();
    }

}

float avg_time(float* times) {
    float avg = 0;
    int i;
    for (i = 0 ; i < NB_TIMES ; i++ )
        avg += times[i];
    return (avg / NB_TIMES);
}
