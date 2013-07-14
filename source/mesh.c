#include "mesh.h"

#include "cube.h"
#include "carre.h"
#include "vec3.h"
#include "vec2.h"
#include "objLoader.h"

#include <stdio.h>
#include <stdlib.h>

void Mesh_CreateVAO(Mesh* mesh, int nbAttrib, int* attrib, int* offset, int* components) {

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    int i;
    for (i = 0 ; i < nbAttrib ; i++ )
    {
        glEnableVertexAttribArray(attrib[i]);
        glVertexAttribPointer(attrib[i], components[i], GL_FLOAT, GL_FALSE, 0, (void*)offset[i]);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh_CreateVBO(Mesh* mesh, int size, void* data) {

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void Mesh_CreateVBO2(Mesh* mesh, Vec3* vertices, Vec3* normals, Vec2* uvs, int nb) {

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    int sizeVertices = sizeof(Vec3) * nb;
    int sizeUvs      = sizeof(Vec2) * nb;
    int sizeNormals  = sizeof(Vec3) * nb;

    glBufferData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals + sizeUvs, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0                         , sizeVertices, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeVertices              , sizeNormals , normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeVertices + sizeNormals, sizeUvs     , uvs);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


Mesh* Mesh_Load(const char* filename) {

    return Mesh_FullLoad(filename, NULL);
}

// Charge et prépare directement un mesh
// Ecrit le chemin du fichier mtl dans texFile
Mesh* Mesh_FullLoad(const char* filename, char* texFile) {

    Mesh* mesh = malloc(sizeof(Mesh));

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    Vec2* range = NULL;

    int nb, nbVertices;
    if (loadObj(filename, &vertices, &uvs, &normals, &range, &nb, &nbVertices, texFile) == false)
        return NULL;

    mesh->primitiveType = GL_TRIANGLES;
    mesh->nb = nb;
    mesh->drawStart = malloc(sizeof(unsigned int) * nbVertices);
    mesh->drawCount = malloc(sizeof(unsigned int) * nbVertices);

    int i;
    for (i = 0 ; i < mesh->nb ; i++ )
    {
        mesh->drawStart[i] = range[i].x;
        mesh->drawCount[i] = range[i].y;
    }

    Mesh_CreateVBO2(mesh, vertices, normals, uvs, nbVertices);
    Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nbVertices, (sizeof(Vec3)*nbVertices) + sizeof(Vec3)*nbVertices}, (int[3]){3, 3, 2});

    free(vertices);
    free(normals);
    free(uvs);
    free(range);

    return mesh;
}

// Charge un mesh défini dans carre.h ou cube.h
Mesh* Mesh_LoadBuiltin(int type) {

    Mesh* mesh = malloc(sizeof(Mesh));

    mesh->drawStart = malloc(sizeof(unsigned int));
    mesh->drawCount = malloc(sizeof(unsigned int));
    mesh->drawStart[0] = 0;
    mesh->nb = 1;

    switch (type) {

    case MESH_CUBE:

        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cube), (void*)cube);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 1}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 3});

        break;

    case MESH_CUBE_TEX: // CUBE TEXTURE

        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTex), (void*)cubeTex);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 2}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CUBE_TEX_FLIP: // CUBE TEXTURE INVERSE (skybox)

        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTexFliped), (void*)cubeTexFliped);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 2}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CUBE_TEX_NORM: // CUBE TEXTURE + NORMAL

        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTex), (void*)cubeTex);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, 0, VBO_COLOR_OFFSET}, (int[3]){3, 3, 2});

        break;

    case MESH_CARRE_TEX: // CARRE TEXTURE

        mesh->drawCount[0] = 6;
        Mesh_CreateVBO(mesh, sizeof(carre), (void*)carre);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 1}, (int[2]){0, VBO_TEXCOORD_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CARRE_TEX_NORM: // CARRE TEXTURE + NORMAL

        mesh->drawCount[0] = 6;
        Mesh_CreateVBO(mesh, sizeof(carreNorm), (void*)carreNorm);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, VBO_NORMAL_OFFSET, VBO_TEXCOORD_OFFSET}, (int[3]){3, 3, 2});

        break;

    case MESH_CARRE_TEX_NORM2: // CARRE TEXTURE * 20 + NORMAL

        mesh->drawCount[0] = 6;
        Mesh_CreateVBO(mesh, sizeof(carreNorm20), (void*)carreNorm20);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, VBO_NORMAL_OFFSET, VBO_TEXCOORD_OFFSET}, (int[3]){3, 3, 2});

        break;

    default:
        printf("Erreur: MeshType inconnu: %d\n", type);
        return NULL;
        break;
    }

    mesh->primitiveType = GL_TRIANGLES;

    return mesh;
}


