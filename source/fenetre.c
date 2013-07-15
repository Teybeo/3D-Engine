#include "fenetre.h"

#include <stdlib.h>
#include "glew.h"

#include "gl_debug.h"

#include "SDL.h"
#include "SDL_events.h"
#include "utils/matrix.h"

#define FEN_L 1100
#define FEN_H 750

#define GL_MAJOR 3
#define GL_MINOR 3

void setPleinEcran(Fenetre* fen, bool pleinEcran);
void grabMouse(Fenetre* fen, bool grabState);

bool initFenetre(Fenetre* fen) {

    fen->largeur = FEN_L;
    fen->hauteur = FEN_H;

    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 3);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 3);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

//    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    fen->ecran = SDL_CreateWindow("Vertex Sending", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, fen->largeur, fen->hauteur, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);
    if (fen->ecran == NULL)
    {
        printf("La fenetre n'a pas pu etre creee\n");
        return false;
    }

    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    SDL_SetWindowDisplayMode(fen->ecran, &mode); // Le plein écran utilisera désormais le meme mode que le bureau

    SDL_GLContext contexte = SDL_GL_CreateContext(fen->ecran);
    if (contexte == NULL)
    {
        printf("Le contexte OpenGL n'a pas pu etre cree\n");
        printf("Version du contexte OpenGL demandee: %d.%d\n", GL_MAJOR, GL_MINOR);
        return false;
    }

    SDL_GL_MakeCurrent(fen->ecran, contexte);

    printf("\n %s\n", glGetString(GL_RENDERER));
    printf("\n %s\n", glGetString(GL_VERSION));

    setPerspective(fen);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    grabMouse(fen, true);

    return true;
}

void eventFenetre(Fenetre* fen, SDL_WindowEvent ev) {

    switch (ev.event) {

        case SDL_WINDOWEVENT_RESIZED:

            printf("[Resized] : %d %d\n", ev.data1, ev.data2);
            fen->largeur = ev.data1;
            fen->hauteur = ev.data2;
            setPerspective(fen);

            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:

            grabMouse(fen, false);

            break;

    }

}

void majFenetreKeys(Fenetre* fen, SDL_KeyboardEvent keyEv) {

    static bool alt = false;
    static bool f4 = false;

    if (keyEv.keysym.sym == SDLK_LALT)
        alt = keyEv.state;

    else if (keyEv.keysym.sym == SDLK_F4)
        f4 = keyEv.state;

    if (alt && f4)
        fen->arret = true;

    if (keyEv.type == SDL_KEYDOWN)
    {
        if (keyEv.keysym.sym == SDLK_ESCAPE)
            grabMouse(fen, !fen->grab);

        else if (keyEv.keysym.sym == SDLK_F11) {
            setPleinEcran(fen, !fen->pleinEcran);
            // Il faut peut-etre recharger les données du contexte OpenGL
        }

    }

}

void grabMouse(Fenetre* fen, bool grabState) {

    fen->grab = grabState;

    SDL_SetWindowGrab(fen->ecran, fen->grab); // On libère la souris
    SDL_ShowCursor(!fen->grab);               // On la réaffiche
    SDL_SetRelativeMouseMode(fen->grab);      // On passe en mode relatif

}

void setPleinEcran(Fenetre* fen, bool pleinEcran) {

    fen->pleinEcran = pleinEcran;

    SDL_SetWindowFullscreen(fen->ecran, fen->pleinEcran);
    SDL_GetWindowSize(fen->ecran, &fen->largeur, &fen->hauteur);

    setPerspective(fen);

    SDL_UpdateWindowSurface(fen->ecran);
//    printf("Plein Ecran -> %d\n", fen->pleinEcran);
//    printf("Window size: %d x %d\n", fen->largeur, fen->hauteur);

}

void setPerspective(Fenetre* fen) {

    glViewport(0, 0, fen->largeur, fen->hauteur);

    loadIdentity(fen->camToClip);

    projection(fen->camToClip, 70, (float)fen->largeur/fen->hauteur, 1, 1000);

}


