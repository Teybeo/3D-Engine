#include "utils/liste.h"

ElemVec3* empiler(ElemVec3 *tetepile, Vec3 vec) {

    ElemVec3 *nouveau = malloc(sizeof(ElemVec3));
    nouveau->vec = vec;
    nouveau->suivant = tetepile;

    return nouveau;

}

ElemVec3* empilerFin(ElemVec3 *tetepile, Vec3 vec) {

    ElemVec3 *nouveau = malloc(sizeof(ElemVec3));
    nouveau->vec = vec;
    nouveau->suivant = NULL;

    if (tetepile == NULL)
        tetepile = nouveau;
    else {

        ElemVec3* curseur = tetepile;
        while (curseur->suivant != NULL) {

            curseur = curseur->suivant;
        }
        curseur->suivant = nouveau;
    }

    return tetepile;

}

ElemVec3* depiler(ElemVec3* tetepile) {

    if (tetepile == NULL)
        return NULL;

    else {

        ElemVec3* restant = tetepile->suivant;
        free(tetepile);
        return restant;
    }
}

void* liberePile(ElemVec3* tetepile) {

    while (tetepile != NULL) {

        tetepile = depiler(tetepile);
    }

    return NULL;

}

int getElemNumber(ElemVec3* tetepile) {

    ElemVec3* curseur = tetepile;
    int nb = 0;

    while (curseur != NULL)
    {
        nb++;
        curseur = curseur->suivant;
    }

    return nb;

}

// On extrait chaque x des vecteurs de la liste tetepile et
// on les range dans une case du tableau
// L'ordre est inversé pour que le bas de la pile soit au début du tableau
unsigned int* dumpListeToArray(ElemVec3* tetepile) {

    ElemVec3* curseur = tetepile;
    int nb = getElemNumber(tetepile);
    unsigned int* array = malloc(sizeof(unsigned int)*nb);

    int i = 0;
    while (curseur != NULL)
    {
        array[nb-1 - i] = curseur->vec.x;
        curseur = depiler(curseur);
        i++;
    }

    return array;

}
// On extrait chaque vecteur de la liste tetepile et
// on les range dans une case du tableau
// L'ordre est inversé pour que le bas de la pile soit au début du tableau
Vec3* dumpVec3ListeToArray(ElemVec3* tetepile) {

    ElemVec3* curseur = tetepile;
    int nb = getElemNumber(tetepile);
    Vec3* array = malloc(sizeof(Vec3)*nb);

    int i = 0;
    while (curseur != NULL)
    {
        array[nb-1 - i] = curseur->vec;
        curseur = depiler(curseur);
        i++;
    }

    return array;

}
// On extrait chaque vecteur de la liste tetepile et
// on les range dans une case du tableau
// L'ordre est inversé pour que le bas de la pile soit au début du tableau
Vec2* dumpVec2ListeToArray(ElemVec3* tetepile) {

    ElemVec3* curseur = tetepile;
    int nb = getElemNumber(tetepile);
    Vec2* array = malloc(sizeof(Vec2)*nb);

    int i = 0;
    while (curseur != NULL)
    {
        array[nb-1 - i].x = curseur->vec.x;
        array[nb-1 - i].y = curseur->vec.y;
        curseur = depiler(curseur);
        i++;
    }

    return array;

}

Vec3 getElemByNumber(ElemVec3* tetepile, int nb) {

    ElemVec3* curseur = tetepile;

    int step = 0;
    while (curseur != NULL && step <= nb-1)
    {
        step++;
        curseur = curseur->suivant;
    }

    return curseur->vec;

}

