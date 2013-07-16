#include "utils/listeString.h"

#include <stdlib.h>
#include <string.h>

ElemString* empilerStr(ElemString *tetepile, char* chaine) {

    ElemString* nouveau = malloc(sizeof(ElemString));
    strcpy(nouveau->chaine, chaine);
    nouveau->suivant = tetepile;

    return nouveau;
}

ElemString* depilerStr(ElemString* tetepile) {

    if (tetepile == NULL)
        return NULL;

    else {

        ElemString* restant = tetepile->suivant;
        free(tetepile);
        return restant;
    }
}

void* liberePileStr(ElemString* tetepile) {

    while (tetepile != NULL) {

        tetepile = depilerStr(tetepile);
    }

    return NULL;
}

int getElemNumberStr(ElemString* tetepile) {

    int nb = 0;
    ElemString* curseur = tetepile;

    while (curseur != NULL) {

        nb++;
        curseur = curseur->suivant;
    }

    return nb;

}

char** dumpListeToArrayStr(ElemString* tetepile) {

    int nb = getElemNumberStr(tetepile);
    if (nb == 0)
        return NULL;

    char** array = malloc(sizeof(char*) * nb);
    ElemString* curseur = tetepile;
    int i = 0;

    while (curseur != NULL) {

        array[nb-1 - i] = malloc(sizeof(char) * 128);
        strcpy(array[nb-1 - i], curseur->chaine);
        i++;
        curseur = curseur->suivant;
    }

    return array;

}
