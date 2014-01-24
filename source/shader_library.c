#include "shader_library.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static ShaderLibrary* lib = NULL;

void ShaderLibrary_Refresh() {

    ShaderLibrary* tmp = lib;

    while (tmp != NULL) {
        Shader_Refresh(tmp->shader);
        tmp = tmp->suivant;
    }

}

void ShaderLibrary_Add(Shader* shader) {

    ShaderLibrary* nouveau = malloc(sizeof(ShaderLibrary));
    nouveau->shader = shader;
    nouveau->suivant = lib;

    lib = nouveau;

}

Shader* ShaderLibrary_Get(const char* shader) {

    ShaderLibrary* tmp = lib;

    while (tmp != NULL) {
        if (strcmp(tmp->shader->name, shader) == 0)
            return tmp->shader;
        tmp = tmp->suivant;
    }

    printf("Error: Shader [%s] not found in library\n", shader);

    return NULL;

}
