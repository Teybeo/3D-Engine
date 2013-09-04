#include "shader.h"

#include "shaderLoader.h"
#include "utils/vec3.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL_timer.h"
#include <windows.h>

uint32_t GetLastWriteTime(const char* filename);

void cache_uniforms(Shader* shader);

Shader Shader_Create(const char* vertexFile, const char* fragmentFile) {

    Shader shader = {};

    initProgram(&shader.id, vertexFile, fragmentFile);

    strcpy(shader.vertexFile, vertexFile);
    strcpy(shader.fragmentFile, fragmentFile);
    shader.lastWrite = GetLastWriteTime(fragmentFile);

    cache_uniforms(&shader);

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

    if (SDL_GetTicks() < lastCheck + 200)
        return;
    lastCheck = SDL_GetTicks();

    uint32_t lastWrite = max(GetLastWriteTime(shader->fragmentFile), GetLastWriteTime(shader->vertexFile));

    if (lastWrite > shader->lastWrite)
    {
        uint32_t programTemp = 0;
        if (initProgram(&programTemp, shader->vertexFile, shader->fragmentFile) == true)
        {
            glDeleteProgram(shader->id);
            shader->id = programTemp;
            cache_uniforms(shader);
        }
        shader->lastWrite = lastWrite;
    }

}

int Shader_FindUniform(Shader* shader, const char* name);

void Shader_SendUniform(Shader* shader, const char* name, int type, void* data) {

    Shader_SendUniformArray(shader, name, type, 1, data);

}

void Shader_SendUniformArray(Shader* shader, const char* name, int type, int nb, void* data) {

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

