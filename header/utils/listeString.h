#ifndef _LISTESTRING_GUARD
#define _LISTESTRING_GUARD

typedef struct ElemString {

    char chaine[128];
    struct ElemString* suivant;

} ElemString;

ElemString* empilerStr(ElemString *tetepile, char* vec);
ElemString* depilerStr(ElemString* tetepile);
void* liberePileStr(ElemString* tetepile);
int getElemNumberStr(ElemString* tetepile);
char** dumpListeToArrayStr(ElemString* tetepile);

#endif // LISTESTRING

