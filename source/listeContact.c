#include "listeContact.h"

#include "stdlib.h"

ElemContact* empilerContact(ElemContact *tetepile, Contact contact) {

    ElemContact *nouveau = malloc(sizeof(ElemContact));
    nouveau->contact = contact;
    nouveau->suivant = tetepile;

    return nouveau;

}

ElemContact* empilerContactFin(ElemContact *tetepile, Contact contact) {

    ElemContact *nouveau = malloc(sizeof(ElemContact));
    nouveau->contact = contact;
    nouveau->suivant = NULL;

    if (tetepile == NULL)
        tetepile = nouveau;
    else {

        ElemContact* curseur = tetepile;
        while (curseur->suivant != NULL) {

            curseur = curseur->suivant;
        }
        curseur->suivant = nouveau;
    }

    return tetepile;

}

ElemContact* depilerContact(ElemContact* tetepile) {

    if (tetepile == NULL)
        return NULL;

    else {

        ElemContact* restant = tetepile->suivant;
        free(tetepile);
        return restant;
    }
}

void* liberePileContact(ElemContact* tetepile) {

    while (tetepile != NULL) {

        tetepile = depilerContact(tetepile);
    }

    return NULL;

}
