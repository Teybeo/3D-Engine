#include "shaderLoader.h"

#include "glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHADER 0
#define PROGRAM 1

char* chargerSource(const char *filename);
bool linkShaders(unsigned int* program, int vert, int frag);
bool validateProgram(unsigned int program);
void printLog(int object, int type);

bool createProgram(unsigned int* program, int vert, int frag) {

    return linkShaders(program, vert, frag) == true && validateProgram(*program) == true;

}

bool linkShaders(unsigned int* program, int vert, int frag) {

    printf("Linkage des shaders %d et %d...", vert, frag);

    *program = glCreateProgram();
    glAttachShader(*program, vert);
    glAttachShader(*program, frag);
    glLinkProgram(*program);

    int link_status = true;

    glGetProgramiv(*program, GL_LINK_STATUS, &link_status);

    link_status ? puts("Ok") : puts("Error");

    printLog(*program, PROGRAM);

    return link_status;
}

bool validateProgram(unsigned int program) {

    printf("Validation du programme %d...", program);

    glValidateProgram(program);

    int validate_status = false;

    glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_status);

    validate_status ? puts("Ok") : puts("Error");

    printLog(program, PROGRAM);

    return validate_status;
}

bool initShader(int* shaderID, GLenum type, const char* chemin) {

    printf("Compilation de '%s'...", chemin);

    *shaderID = glCreateShader(type);
    if (shaderID == NULL) {
        printf("Erreur création du shader '%s'\n", chemin);
        return false;
    }

    char* source = chargerSource(chemin);
    if (source == NULL)
        return false;

    glShaderSource(*shaderID, 1, (const char**)&source, NULL);

    glCompileShader(*shaderID);

    free(source);

    GLint compile_status = true;
    glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &compile_status);

    compile_status ? puts("Ok") : puts("Error");

    printLog(*shaderID, SHADER);

    return compile_status;
}

void printLog(int object, int type) {

    GLsizei logsize = 0;

    if (type == SHADER)
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logsize);
    else if (type == PROGRAM)
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logsize);

    // On affiche les messages
    if (logsize != 0)
    {
        char *log = malloc(logsize + 1);
        if (log == NULL)
        {
            fprintf(stderr, "impossible d'allouer de la memoire !\n");
            return;
        }

        memset(log, '\0', logsize + 1);

        // On récupère le message
        if (type == SHADER)
            glGetShaderInfoLog(object, logsize, &logsize, log);
        else if (type == PROGRAM)
            glGetProgramInfoLog(object,  logsize, &logsize, log);

        printf("%s", log);

        free(log);
    }
}

bool initProgram(GLuint* program, const char* vertexShaderFile, const char* fragmentShaderFile) {

    puts("\n----------- Shader ---------------------");

    GLint vertexShader, fragShader;
    if (initShader(&vertexShader, GL_VERTEX_SHADER, vertexShaderFile) == false)
        return false;

    if (initShader(&fragShader, GL_FRAGMENT_SHADER, fragmentShaderFile) == false)
        return false;

    if (createProgram(program, vertexShader, fragShader) == false)
        return false;

    return true;

}


char* chargerSource(const char *filename)
{
    char *src = NULL;   /* code source de notre shader */
    FILE *fp = NULL;    /* fichier */
    long size;          /* taille du fichier */
    long i;             /* compteur */

    fp = fopen(filename, "r");

    if(fp == NULL)
    {
        fprintf(stderr, "Erreur ouverture du fichier '%s'\n", filename);
        return NULL;
    }

    /* on recupere la longueur du fichier */
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    /* on se replace au debut du fichier */
    rewind(fp);

    /* on alloue de la memoire pour y placer notre code source */
    src = malloc(size+1); /* +1 pour le caractere de fin de chaine '\0' */
    if(src == NULL)
    {
        fclose(fp);
        fprintf(stderr, "Erreur d'allocation memoire pour le shader '%s'\n", filename);
        return NULL;
    }

    /* lecture du fichier */
    for(i=0; i<size; i++)
        src[i] = fgetc(fp);

    /* on place le dernier caractere a '\0' */
    src[size] = '\0';

    fclose(fp);

    return src;
}

