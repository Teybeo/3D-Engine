#include "shader.h"

#include "shaderLoader.h"
#include "utils/vec3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL_timer.h"

#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1

#include <windows.h>

uint32_t GetLastWriteTime(const char* filename);
void cache_uniforms(Shader* shader);

Shader* Shader_Create(const char* name, const char* vertexFile, const char* fragmentFile) {

    Shader* shader = malloc(sizeof(Shader));

    if (initProgram(&shader->id, vertexFile, fragmentFile) == false)
        return NULL;

    shader->name = malloc(sizeof(char) * strlen(name) + 1);
    shader->vertexFile = malloc(sizeof(char) * strlen(vertexFile) + 1);
    shader->fragmentFile = malloc(sizeof(char) * strlen(fragmentFile) + 1);
    strcpy(shader->name, name);
    strcpy(shader->vertexFile, vertexFile);
    strcpy(shader->fragmentFile, fragmentFile);
    shader->lastWrite[VERTEX_SHADER] = GetLastWriteTime(vertexFile);
    shader->lastWrite[FRAGMENT_SHADER] = GetLastWriteTime(fragmentFile);

    cache_uniforms(shader);

    return shader;
}

void cache_uniforms(Shader* shader) {

    char buffer[128] = "";
    int nameLength;
    int uniformSize;
    GLenum uniformType;

    glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &shader->nbUniform);
    printf("%d uniforms\n", shader->nbUniform);

    shader->uniformName = malloc(sizeof(char*) * shader->nbUniform);
    shader->uniformLoc = malloc(sizeof(int) * shader->nbUniform);

    int i;
    for (i = 0 ; i < shader->nbUniform ; i++ )
    {
        glGetActiveUniform(shader->id, i, 128, &nameLength, &uniformSize, &uniformType, buffer);

        shader->uniformName[i] = malloc(sizeof(char) * nameLength + 1);
        strcpy(shader->uniformName[i], buffer);
        shader->uniformLoc[i] = glGetUniformLocation(shader->id, shader->uniformName[i]);

        printf("%d: %s (%d)\n", shader->uniformLoc[i], shader->uniformName[i], uniformSize);
    }

}

void Shader_Refresh(Shader* shader) {

    static uint32_t lastCheck = 0;

    if (SDL_GetTicks() < lastCheck + 0)
        return;
    lastCheck = SDL_GetTicks();

    uint32_t lastWrite[2];
    lastWrite[VERTEX_SHADER] = GetLastWriteTime(shader->vertexFile);
    lastWrite[FRAGMENT_SHADER] = GetLastWriteTime(shader->fragmentFile);

    // Si la date de dernière modif à changé, on recompile le programme
    if ((lastWrite[0] != shader->lastWrite[0]) || (lastWrite[1] != shader->lastWrite[1]))
    {
        uint32_t programTemp = 0;
        if (initProgram(&programTemp, shader->vertexFile, shader->fragmentFile) == true)
        {
            glDeleteProgram(shader->id);
            shader->id = programTemp;
            cache_uniforms(shader);
        }
        shader->lastWrite[VERTEX_SHADER]   = lastWrite[VERTEX_SHADER];
        shader->lastWrite[FRAGMENT_SHADER] = lastWrite[FRAGMENT_SHADER];
    }

}

int Shader_FindUniform(Shader* shader, const char* name);

void Shader_SendUniform(Shader* shader, const char* name, int type, void* data) {

    Shader_SendUniformArray(shader, name, type, 1, data);

}

void Shader_SendUniformArray(Shader* shader, const char* name, int type, int nb, void* data) {

    int location = Shader_FindUniform(shader, name);
    if (location == -1)
    {
//        printf("Uniform [%s] non trouve dans [%s] !\n", name, shader->name);
        return;
    }

    int real_bool;

    switch (type) {

    case GL_FLOAT_VEC3:
        glProgramUniform3fv(shader->id, location, nb, data);
        break;

    case GL_FLOAT_MAT4:
        glProgramUniformMatrix4fv(shader->id, location, nb, GL_TRUE, data);
        break;

    case GL_BOOL:
        real_bool = *(bool*)data;
        data = &real_bool;
    case GL_INT:
        glProgramUniform1iv(shader->id, location, nb, data);
        break;

    }

}

int Shader_FindUniform(Shader* shader, const char* name) {

    int i;
    for (i = 0 ; i < shader->nbUniform ; i++ )
    {
        if (strcmp(shader->uniformName[i], name) == 0)
            return shader->uniformLoc[i];
    }

    return -1;
}

/// Retrieves the last-write time in millisecondes of a file since 00:00
uint32_t GetLastWriteTime(const char* filename)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;
    unsigned int writeTime = 0;

    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        puts("Error GetFiletime");

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Convert and add the separate units in milliseconds
    // 23:59:59 = 84 399 000, donc ca tient sur un uint
    writeTime = (stLocal.wHour * 3600 + stLocal.wMinute * 60 + stLocal.wSecond) * 1000;

    CloseHandle(hFile);

    return writeTime;
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

