#include "model.h"

#include "cube.h"
#include "carre.h"
#include "vec3.h"
#include "vec2.h"
#include "objLoader.h"

#include <stdio.h>
#include <stdlib.h>

void Model_CreateVAO(Model* model, int nbAttrib, int* attrib, int* offset, int* components) {

    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    int i;
    for (i = 0 ; i < nbAttrib ; i++ )
    {
        glEnableVertexAttribArray(attrib[i]);
        glVertexAttribPointer(attrib[i], components[i], GL_FLOAT, GL_FALSE, 0, (void*)offset[i]);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Model_CreateVBO(Model* model, int size, void* data) {

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void Model_CreateVBO2(Model* model, Vec3* vertices, Vec3* normals, Vec2* uvs, int nb) {

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);

    int sizeVertices = sizeof(Vec3) * nb;
    int sizeUvs = sizeof(Vec2) * nb;
    int sizeNormals = sizeof(Vec3) * nb;

    glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals + sizeUvs, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0                         ,  sizeVertices, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeVertices              ,  sizeNormals, normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals, sizeUvs, uvs);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Model* Model_Load(int type, const char* filename) {

    Model* model = malloc(sizeof(Model));

    switch (type) {

    case MODEL_OBJ: // OBJ
        {
        Vec3* vertices = NULL;
        Vec3* normals = NULL;
        Vec2* uvs = NULL;
        int nb;
        if (loadObj(filename, &vertices, &uvs, &normals, &nb) == false)
            return NULL;

        model->drawStart = 0;
        model->drawCount = nb;
        Model_CreateVBO2(model, vertices, normals, uvs, nb);
        Model_CreateVAO(model, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nb, (sizeof(Vec3)*nb) + sizeof(Vec3)*nb}, (int[3]){3, 3, 2});
        }
        break;

    case MODEL_CUBE:

        model->drawStart = 0;
        model->drawCount = 36;
        Model_CreateVBO(model, sizeof(cube), (void*)cube);
        Model_CreateVAO(model, 2, (int[2]){0, 1}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 3});

        break;

    case MODEL_CUBE_TEX: // CUBE TEXTURE

        model->drawStart = 0;
        model->drawCount = 36;
        Model_CreateVBO(model, sizeof(cubeTex), (void*)cubeTex);
        Model_CreateVAO(model, 2, (int[2]){0, 1}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 2});

        break;

    case MODEL_CUBE_TEX_NORM: // CUBE TEXTURE + NORMAL

        model->drawStart = 0;
        model->drawCount = 36;
        Model_CreateVBO(model, sizeof(cubeTex), (void*)cubeTex);
        Model_CreateVAO(model, 3, (int[3]){0, 1, 2}, (int[3]){0, 0, VBO_COLOR_OFFSET}, (int[3]){3, 3, 2});

        break;

    case MODEL_CARRE_TEX: // CARRE TEXTURE

        model->drawStart = 0;
        model->drawCount = 6;
        Model_CreateVBO(model, sizeof(carre), (void*)carre);
        Model_CreateVAO(model, 2, (int[2]){0, 1}, (int[2]){0, VBO_TEXCOORD_OFFSET}, (int[2]){3, 2});

        break;

    case MODEL_CARRE_TEX_NORM: // CARRE TEXTURE + NORMAL

        model->drawStart = 0;
        model->drawCount = 6;
        Model_CreateVBO(model, sizeof(carreNorm), (void*)carreNorm);
        Model_CreateVAO(model, 3, (int[3]){0, 1, 2}, (int[3]){0, VBO_NORMAL_OFFSET, VBO_TEXCOORD_OFFSET}, (int[3]){3, 3, 2});

        break;

    default:
        printf("Erreur: ModelType inconnu: %d\n", type);
        return NULL;
        break;
    }

    model->primitiveType = GL_TRIANGLES;

    return model;
}


//void Model_SetProgram(Model* model, GLuint program) {
//
//    model->program = program;
//}
//
//void Model_SetTexture(Model* model, GLuint texture) {
//
//    model->texture = texture;
//}
