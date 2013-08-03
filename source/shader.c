#include "shader.h"

#include "shaderLoader.h"
#include "utils/vec3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Shader Shader_Create(const char* vertexFile, const char* fragmentFile) {

    Shader shader = {};

    initProgram(&shader.id, vertexFile, fragmentFile);

    int uniformMaxLength = 0;
    glGetProgramiv(shader.id, GL_ACTIVE_UNIFORMS, &shader.nbUniform);
    glGetProgramiv(shader.id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);
    printf("%d uniforms\n", shader.nbUniform);
    shader.uniformName = malloc(sizeof(char*) * shader.nbUniform);
    shader.uniformLoc = malloc(sizeof(int) * shader.nbUniform);

//    int i;
//    for (i = 0 ; i < shader.nbUniform ; i++ )
//    {
//        shader.uniformName[i] = malloc(sizeof(char) * uniformMaxLength);
//        glGetActiveUniformName(shader.id, i, uniformMaxLength, NULL, shader.uniformName[i]);
//        shader.uniformLoc[i] = glGetUniformLocation(shader.id, shader.uniformName[i]);
//        printf("%d: %s\n", shader.uniformLoc[i], shader.uniformName[i]);
//    }
    char buffer[128] = "";
    int nameLength;
    int uniformSize;
    GLenum uniformType;

    int i;
    for (i = 0 ; i < shader.nbUniform ; i++ )
    {
        glGetActiveUniform(shader.id, i, 128, &nameLength, &uniformSize, &uniformType, buffer);

        shader.uniformName[i] = malloc(sizeof(char) * nameLength + 1);
        strcpy(shader.uniformName[i], buffer);
        shader.uniformLoc[i] = glGetUniformLocation(shader.id, shader.uniformName[i]);

        printf("%d: %s (%d)\n", shader.uniformLoc[i], shader.uniformName[i], uniformSize);
    }

    return shader;

}

int Shader_FindUniform(Shader shader, const char* name);

void Shader_SendUniform(Shader shader, const char* name, int type, void* data) {

    Shader_SendUniformArray(shader, name, type, 1, data);

}

void Shader_SendUniformArray(Shader shader, const char* name, int type, int nb, void* data) {

    int location = Shader_FindUniform(shader, name);
    if (location == -1)
        return;

    switch (type) {

    case GL_FLOAT_VEC3:
        glUniform3fv(location, nb, data);
        break;

    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(location, nb, GL_TRUE, data);
        break;

    case GL_INT:
        glUniform1iv(location, nb, data);
        break;

    }

}

int Shader_FindUniform(Shader shader, const char* name) {

    int i;
    for (i = 0 ; i < shader.nbUniform ; i++ )
    {
        if (strcmp(shader.uniformName[i], name) == 0)
            return shader.uniformLoc[i];
    }

    return -1;
}

/*
void Shader_SendUniformVec3(Shader shader, const char* name, Vec3* value) {

    int location = Shader_FindUniform(shader, name);
    if (location != -1)
        glUniform3fv(location, 1, &value->x);
}
void Shader_SendUniformMat4(Shader shader, const char* name, float* value) {

    int location = Shader_FindUniform(shader, name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_TRUE, value);
}
void Shader_SendUniformMat4(Shader shader, const char* name, int value) {

    int location = Shader_FindUniform(shader, name);
    if (location != -1)
        glUniform1i(location, 1, GL_TRUE, value);
}
*/

