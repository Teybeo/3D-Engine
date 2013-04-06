#ifndef H_FENETRE_GUARD
#define H_FENETRE_GUARD

#include <stdbool.h>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_WindowEvent SDL_WindowEvent;
typedef struct SDL_KeyboardEvent SDL_KeyboardEvent;

typedef struct Fenetre {

    SDL_Window* ecran;
    int largeur, hauteur;
    bool arret;
    bool pleinEcran;
    bool grab;
    float camToClip[16];

} Fenetre;

bool initFenetre(Fenetre* fen);

void setPerspective(Fenetre* fen);
void eventFenetre(Fenetre* fen, SDL_WindowEvent ev);
void majFenetreKeys(Fenetre* fen, SDL_KeyboardEvent keyEv);

#endif // Fenetre

