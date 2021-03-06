#include "shader_library.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static ShaderLibrary* lib = NULL;

const char shader_data[][3][128] =
{
    {"noLight",       "../source/vert_shaders/noLight.vert",             "../source/frag_shaders/noLight.frag"},
    {"noTexNoLight",  "../source/vert_shaders/noTexNoLight.vert",        "../source/frag_shaders/noTexNoLight.frag"},
    {"depth",         "../source/vert_shaders/depth.vert",               "../source/frag_shaders/depth.frag"},
    {"depthInstance", "../source/vert_shaders/depthInstance.vert",       "../source/frag_shaders/depth.frag"},
    {"instance",      "../source/vert_shaders/instancePerFragment.vert", "../source/frag_shaders/fullset.frag"},
    {"fullset",       "../source/vert_shaders/fullset.vert",             "../source/frag_shaders/fullset.frag"}
};

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

bool ShaderLibrary_Init() {

    Shader* shader = NULL;

    unsigned int i;
    for (i = 0 ; i < sizeof(shader_data)/sizeof(*shader_data) ; i++ )
    {
        shader = Shader_Create(shader_data[i][0], shader_data[i][1], shader_data[i][2]);

        if (shader == NULL)
            return false;
        ShaderLibrary_Add(shader);
    }

    return true;
}
