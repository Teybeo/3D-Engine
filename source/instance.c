#include "instance.h"

#include "objLoader.h"
#include "texture.h"
#include "matrix.h"
#include "vec3.h"
#include "vec2.h"

#include <stdlib.h>

static GLuint activeProgram = -1;
static GLuint activeVAO = -1;
static GLuint activeTexture = -1;

static GLuint modelWorldLoc = -1;
static GLuint worldCamLoc = -1;
static GLuint camClipLoc = -1;

void Instance_Draw(Instance object, float* mondeToCam, float* camToClip) {

    if (object.program != activeProgram)
    {
        glUseProgram(object.program);
        activeProgram = object.program;
        modelWorldLoc = glGetUniformLocation(object.program, "modelWorld");
        worldCamLoc = glGetUniformLocation(object.program, "worldCam");
        camClipLoc = glGetUniformLocation(object.program, "camClip");
        glUniformMatrix4fv(worldCamLoc, 1, GL_TRUE, mondeToCam);
        glUniformMatrix4fv(camClipLoc, 1, GL_TRUE, camToClip);
    }
    if (object.mesh->vao != activeVAO)
    {
        glBindVertexArray(object.mesh->vao);
        activeVAO = object.mesh->vao;
    }
    if (object.texture != activeTexture)
    {
        glBindTexture(GL_TEXTURE_2D, object.texture);
        activeTexture = object.texture;
    }

    glUniformMatrix4fv(modelWorldLoc, 1, GL_TRUE, object.matrix);

    int i;
    for (i = 0 ; i < object.mesh->nb ; i++ )
    {
        glDrawArrays(object.mesh->primitiveType, object.mesh->drawStart[i], object.mesh->drawCount[i]);
    }

//    glBindTexture(GL_TEXTURE_2D, 0);
//    glBindVertexArray(0);
//    glUseProgram(0);

}

// Charge un fichier obj complet (mesh + texture) et en fait une instance
Instance Instance_Load(const char* objFile, GLuint program) {

    Instance instance = {};
    instance.program = -1;
    instance.texture = -1;

    char texFile[256] = "";

    instance.mesh = Mesh_FullLoad(objFile, texFile);
    // TODO: meilleur gestion d'erreur
    if (instance.mesh == NULL)
        return instance;

    if (texFile[0] != '\0')
        instance.texture = chargerTexture(texFile, GL_LINEAR);

    instance.program = program;
    loadIdentity(instance.matrix);

    return instance;
}

// Crée une instance à partir d'un mesh et d'une texture déjà chargé en mémoire
Instance Instance_Create(Mesh* mesh, GLuint program, GLuint texture) {

    Instance instance = {};

    instance.mesh = mesh;
    instance.program = program;
    instance.texture = texture;
    loadIdentity(instance.matrix);

    return instance;

}
//
void InstanceGroupe_Draw(InstanceGroupe groupe, float* mondeToCam, float* camToClip) {

//    static GLuint activeProgram = 0;
//    static GLuint activeVAO = 0;
//    static GLuint activeTexture = 0;
//
//    static GLuint worldCamLoc = 0;
//    static GLuint camClipLoc = 0;

    if (groupe.program != activeProgram)
    {
        glUseProgram(groupe.program);
        activeProgram = groupe.program;
        worldCamLoc = glGetUniformLocation(groupe.program, "worldCam");
        camClipLoc = glGetUniformLocation(groupe.program, "camClip");
        glUniformMatrix4fv(worldCamLoc, 1, GL_TRUE, mondeToCam);
        glUniformMatrix4fv(camClipLoc, 1, GL_TRUE, camToClip);
    }
    if (groupe.mesh->vao != activeVAO)
    {
        glBindVertexArray(groupe.mesh->vao);
        activeVAO = groupe.mesh->vao;
    }
    if (groupe.texture != activeTexture)
    {
        glBindTexture(GL_TEXTURE_2D, groupe.texture);
        activeTexture = groupe.texture;
    }

    glDrawArraysInstanced(groupe.mesh->primitiveType, groupe.mesh->drawStart, groupe.mesh->drawCount, groupe.nbInstances);

}

InstanceGroupe InstanceGroupe_Create(Mesh* mesh, int nbInstances, GLuint program, GLuint texture) {

    InstanceGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.nbInstances = nbInstances;
    groupe.program = program;
    groupe.texture = texture;

    groupe.matrix = malloc(sizeof(float*) * nbInstances);
    int i;
    for (i = 0 ; i < nbInstances ; i++ )
    {
        groupe.matrix[i] = malloc(sizeof(float)*16);
        loadIdentity(groupe.matrix[i]);
        translate(groupe.matrix[i], -40 + rand() % 80, rand() % 30, -40+rand() % 80);
        transpose(groupe.matrix[i]); // On transpose à la main
    }

    glGenBuffers(1, &groupe.matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);

    glBufferData(GL_ARRAY_BUFFER, nbInstances * 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // Avec un tableau dynamique, si on copie tout d'un coup (BufferData), 2 octets seront occupée entre chaque matrice
    // Je ne sais pas pk, le seul moyen trouvé est d'envoyer chaque matrice séparément
    for (i = 0 ; i < nbInstances ; i++ )
        glBufferSubData(GL_ARRAY_BUFFER, i * 16 * sizeof(float), 16 * sizeof(float), &(groupe.matrix[i][0]));

    // On ajoute les matrices au vao déjà créé du modèle
    // Comme les attributs ne peuvent être que des vec4, opengl distribue 4 index, 1 pour chaque colonne
    glBindVertexArray(mesh->vao);

    for (i = 0 ; i < 4 ; i++ )
    {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
        glVertexAttribDivisor(3 + i, 1);
        glEnableVertexAttribArray(3 + i);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return groupe;

}

void uploadMatrix(InstanceGroupe groupe) {

    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);
    int i;
    for (i = 0 ; i < groupe.nbInstances ; i++ )
        glBufferSubData(GL_ARRAY_BUFFER, ( i * 16 * 4), (16 * sizeof(float)), &(groupe.matrix[i][0]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
