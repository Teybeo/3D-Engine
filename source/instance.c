#include "instance.h"

#include "objLoader.h"
#include "shader.h"
#include "texture.h"
#include "utils/matrix.h"
#include "utils/vec3.h"
#include "utils/vec2.h"

#include <stdlib.h>

static GLuint activeShader = -1;
static GLuint activeVAO = -1;
static GLuint activeTexture = -1;

void Object3D_Draw(Object3D object, float* mondeToCam, float* camToClip) {

    if (object.shader.id != activeShader)
    {
        glUseProgram(object.shader.id);
        activeShader = object.shader.id;
        Shader_SendUniform(object.shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
        Shader_SendUniform(object.shader, "camClip", GL_FLOAT_MAT4, camToClip);
    }
    if (object.mesh->vao != activeVAO)
    {
        glBindVertexArray(object.mesh->vao);
        activeVAO = object.mesh->vao;
    }

    Shader_SendUniform(object.shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);
//    glUniformMatrix4fv(modelWorldLoc, 1, GL_TRUE, object.matrix);

    int i;
    for (i = 0 ; i < object.mesh->nb ; i++ )
    {
        if (object.mesh->material[i].hasTexture && object.mesh->material[i].texture != activeTexture)
        {
            glBindTexture(GL_TEXTURE_2D, object.mesh->material[i].texture);
            activeTexture = object.mesh->material[i].texture;
        }

        Shader_SendUniform(object.shader, "matDiff", GL_FLOAT_VEC3, &object.mesh->material[i].diffuse.x);
        Shader_SendUniform(object.shader, "matSpec", GL_FLOAT_VEC3, &object.mesh->material[i].specular.x);
        Shader_SendUniform(object.shader, "matShininess", GL_INT, &object.mesh->material[i].exponent);

        glDrawArrays(object.mesh->primitiveType, object.mesh->drawStart[i], object.mesh->drawCount[i]);
    }

//    glBindTexture(GL_TEXTURE_2D, 0);
//    glBindVertexArray(0);
//    glUseProgram(0);

}

// Charge un fichier obj complet (mesh + texture) et en fait une object
Object3D Object3D_Load(const char* objFile, Shader shader) {

    Object3D object = {};
    object.shader = shader;

    char texFile[256] = "";

    object.mesh = Mesh_FullLoad(objFile, texFile);
    // TODO: meilleur gestion d'erreur
    if (object.mesh == NULL)
        return object;

    loadIdentity(object.matrix);

    return object;
}

// Crée une object à partir d'un mesh et d'une texture déjà chargé en mémoire
Object3D Object3D_Create(Mesh* mesh, Shader shader, GLuint texture) {

    Object3D object = {};

    mesh->material[0].texture = texture;
    mesh->material[0].hasTexture = true;
    object.mesh = mesh;
    object.shader = shader;
    object.mesh->material[0].texture = texture;
    object.mesh->material[0].hasTexture = true;

    loadIdentity(object.matrix);

    return object;

}

void Object3DGroupe_Draw(Object3DGroupe groupe, float* mondeToCam, float* camToClip) {

    if (groupe.shader.id != activeShader)
    {
        glUseProgram(groupe.shader.id);
        activeShader = groupe.shader.id;
        Shader_SendUniform(groupe.shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
        Shader_SendUniform(groupe.shader, "camClip", GL_FLOAT_MAT4, camToClip);
    }
    if (groupe.mesh->vao != activeVAO)
    {
        glBindVertexArray(groupe.mesh->vao);
        activeVAO = groupe.mesh->vao;
    }

    int i;
    for (i = 0 ; i < groupe.mesh->nb ; i++ )
    {
        if (groupe.mesh->material[i].hasTexture && groupe.mesh->material[i].texture != activeTexture)
        {
            glBindTexture(GL_TEXTURE_2D, groupe.mesh->material[i].texture);
            activeTexture = groupe.mesh->material[i].texture;
        }

        Shader_SendUniform(groupe.shader, "matDiff", GL_FLOAT_VEC3, &groupe.mesh->material[i].diffuse.x);
        Shader_SendUniform(groupe.shader, "matSpec", GL_FLOAT_VEC3, &groupe.mesh->material[i].specular.x);
        Shader_SendUniform(groupe.shader, "matShininess", GL_INT, &groupe.mesh->material[i].exponent);

        glDrawArraysInstanced(groupe.mesh->primitiveType, groupe.mesh->drawStart[i], groupe.mesh->drawCount[i], groupe.nbObject3Ds);
    }
}

Object3DGroupe Object3DGroupe_Create(Mesh* mesh, int nbObject3Ds, Shader shader, GLuint texture) {

    Object3DGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.mesh->material[0].hasTexture = true;
    groupe.mesh->material[0].texture = texture;

    groupe.nbObject3Ds = nbObject3Ds;
    groupe.shader = shader;
    groupe.texture = texture;

    groupe.matrix = malloc(sizeof(float*) * nbObject3Ds);
    int i;
    for (i = 0 ; i < nbObject3Ds ; i++ )
    {
        groupe.matrix[i] = malloc(sizeof(float)*16);
        loadIdentity(groupe.matrix[i]);
        translate(groupe.matrix[i], -400 + rand() % 800, rand() % 30, -400+rand() % 800);
        transpose(groupe.matrix[i]); // On transpose à la main
    }

    glGenBuffers(1, &groupe.matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);

    glBufferData(GL_ARRAY_BUFFER, nbObject3Ds * 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // Avec un tableau dynamique, si on copie tout d'un coup (BufferData), 2 octets seront occupée entre chaque matrice
    // Je ne sais pas pk, le seul moyen trouvé est d'envoyer chaque matrice séparément
    for (i = 0 ; i < nbObject3Ds ; i++ )
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

void uploadMatrix(Object3DGroupe groupe) {

    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);
    int i;
    for (i = 0 ; i < groupe.nbObject3Ds ; i++ )
        glBufferSubData(GL_ARRAY_BUFFER, ( i * 16 * 4), (16 * sizeof(float)), &(groupe.matrix[i][0]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
