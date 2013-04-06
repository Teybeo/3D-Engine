#include "shader.h"

#include "glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* chargerSource(const char *filename);

bool createProgram(unsigned int* program, int vert, int frag) {

    *program = glCreateProgram();
    glAttachShader(*program, vert);
    glAttachShader(*program, frag);
    glLinkProgram(*program);

    int link_status = true;

    glGetProgramiv(*program, GL_LINK_STATUS, &link_status);

    if (link_status == GL_TRUE)
        fprintf(stderr, "Linkage reussi\n");

    // On récupère la taille du log du linkage */
    GLsizei logsize = 0;
    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logsize);

    // Aucun message, on considère que le link est OK (sur ?)
    if (logsize == 0)
    {
        fprintf(stderr, "Aucun log de linkage\n");
    }
    else {

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

    glValidateProgram(*program);
    int validate_status;
    glGetProgramiv(*program, GL_VALIDATE_STATUS, &validate_status);

    if (validate_status == GL_FALSE)
        fprintf(stderr, "Programme invalide\n");

    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &logsize);
    if (logsize == 0)
    {
        fprintf(stderr, "Aucun log de validation\n");
    }
    else {

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
//    int nb_uniforms;

//    glGetProgramiv(*program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nb_uniforms);
//    fprintf(stderr, "Uniforms actifs: %d\n", nb_uniforms);

//    glUseProgram(*program);

    return true;

}
bool initShader(int* shaderID, GLenum type, const char* chemin) {

    *shaderID = glCreateShader(type);
    if (shaderID == NULL) {
        printf("Erreur création du shader '%s'\n", chemin);
        return false;
    }

    char* source1 = chargerSource(chemin);
    if (source1 == NULL)
        return false;
    char source2[50] = "";
//    sprintf(source2, "#define NB_CUBES %d\n", NB_CUBES);
    const char *sources[2] = {source2, source1};

//    glShaderSource(*shaderID, 1, (const char**)&source, NULL);
    glShaderSource(*shaderID, 2, sources, NULL);

    glCompileShader(*shaderID);

    free(source1);

    GLint compile_status = GL_TRUE;

    // Vérification du succès de la compilation
    glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &compile_status);

    if(compile_status == GL_TRUE)
        fprintf(stderr, "Compilation reussie\n");

    // On récupère la taille du log de la compilation */
    GLsizei logsize = 0;
    glGetShaderiv(*shaderID, GL_INFO_LOG_LENGTH, &logsize);

    // Aucun message, on considère que la compil est OK (sur ?)
    if (logsize == 0)
    {
        fprintf(stderr, "Aucun log de compilation\n");
        return true;
    }
    else {

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

    return true;

}


bool initProgram(GLuint* program, const char* vertexShaderFile, const char* fragmentShaderFile) {

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

