#ifndef _LISTE_CONTACT_GUARD
#define _LISTE_CONTACT_GUARD

#include "contact.h"

typedef struct ElemContact {

    Contact contact;
    struct ElemContact* suivant;

} ElemContact;

ElemContact* depilerContact(ElemContact* tetepile);
void* liberePileContact(ElemContact* tetepile);
ElemContact* empilerContactFin(ElemContact *tetepile, Contact contact);
ElemContact* empilerContact(ElemContact *tetepile, Contact contact);

#endif // LISTE_CONTACT


