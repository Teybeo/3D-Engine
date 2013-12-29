#include "app.h"

#include "shaderLoader.h"
#include "shader_library.h"
#include "utils/matrix.h"
#include "gl_debug.h"

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

    ShaderLibrary_Add(Shader_Create("noLight",       "../source/vert_shaders/noLight.vert",             "../source/frag_shaders/noLight.frag"));
    ShaderLibrary_Add(Shader_Create("noTexNoLight",  "../source/vert_shaders/noTexNoLight.vert",        "../source/frag_shaders/noTexNoLight.frag"));
    ShaderLibrary_Add(Shader_Create("perFragment",   "../source/vert_shaders/perFragment.vert",         "../source/frag_shaders/perFragment.frag"));
    ShaderLibrary_Add(Shader_Create("perVertex",     "../source/vert_shaders/perVertex.vert",           "../source/frag_shaders/perVertex.frag"));
    ShaderLibrary_Add(Shader_Create("depth",         "../source/vert_shaders/depth.vert",               "../source/frag_shaders/depth.frag"));
    ShaderLibrary_Add(Shader_Create("shadow",        "../source/vert_shaders/shadow.vert",              "../source/frag_shaders/shadow.frag"));
    ShaderLibrary_Add(Shader_Create("instance",      "../source/vert_shaders/instancePerFragment.vert", "../source/frag_shaders/perFragment.frag"));
    ShaderLibrary_Add(Shader_Create("normalMap",     "../source/vert_shaders/normalMap.vert",           "../source/frag_shaders/normalMap.frag"));

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

        break;

    case SDL_KEYUP:

        if (app->fenetre.grab)
            Player_keyEvent(&app->scene->player, ev.key);

        majFenetreKeys(&app->fenetre, ev.key);

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
