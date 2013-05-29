#include "shader.h"

#include "glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* chargerSource(const char *filename);

bool createProgram(unsigned int* program, int vert, int frag) {

    printf("Linkage des shaders %d et %d...", vert, frag);

    *program = glCreateProgram();
    glAttachShader(*program, vert);
    glAttachShader(*program, frag);
    glLinkProgram(*program);

    // On récupère la taille du log du linkage */
    GLsizei logsize = 0;
    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logsize);

    // Si messages de link, le linkage est traité comme raté
    if (logsize != 0)
    {
        char *log = malloc(logsize + 1);
        if (log == NULL)
        {
            fprintf(stderr, "impossible d'allouer de la memoire !\n");
            return false;
        }

        memset(log, '\0', logsize + 1);

        // On récupère le message
        glGetProgramInfoLog(*program, logsize, &logsize, log);
        fprintf(stderr, "impossible de lier les shader :\n%s", log);

        free(log);
        glDeleteProgram(*program);

        // On met en pause

        return false;
    }

    puts("Ok");

    glValidateProgram(*program);

    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize != 0)
    {
        char *log = malloc(logsize + 1);
        if (log == NULL)
        {
            fprintf(stderr, "impossible d'allouer de la memoire !\n");
            return false;
        }

        memset(log, '\0', logsize + 1);

        // On récupère et on affiche le message
        glGetProgramInfoLog(*program, logsize, &logsize, log);

        fprintf(stderr, "%s", log);

        free(log);
    }

    return true;
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

    // On récupère la taille du log de la compilation */
    GLsizei logsize = 0;
    glGetShaderiv(*shaderID, GL_INFO_LOG_LENGTH, &logsize);

    // Si messages de compil, la compil est traitée comme ratée
    if (logsize != 0)
    {
        char *log = malloc(logsize + 1);
        if (log == NULL)
        {
            fprintf(stderr, "impossible d'allouer de la memoire !\n");
            return false;
        }

        memset(log, '\0', logsize + 1);

        // On récupère le message
        glGetShaderInfoLog(*shaderID, logsize, &logsize, log);
        fprintf(stderr, "impossible de compiler le shader '%s' :\n%s",  chemin, log);

        free(log);
        glDeleteShader(*shaderID);

        // On met en pause

        return false;
    }

    puts("Ok");

    return true;

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

