#include "mesh.h"

#include "cube.h"
#include "carre.h"
#include "utils/vec3.h"
#include "utils/vec2.h"
#include "objLoader.h"

#include <string.h>
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

void Mesh_CreateVBO3(Mesh* mesh, Vec3* vertices, Vec3* normals, Vec3* tangents, Vec3* bitangents, Vec2* uvs, int nb) {

    glGenBuffers(1, &mesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

    int size_Vec3 = sizeof(Vec3) * nb;
    int size_Vec2  = sizeof(Vec2) * nb;

    glBufferData(GL_ARRAY_BUFFER, size_Vec3 * 4 + size_Vec2, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0            , size_Vec3, vertices);
    glBufferSubData(GL_ARRAY_BUFFER, size_Vec3 * 1, size_Vec3, normals);
    glBufferSubData(GL_ARRAY_BUFFER, size_Vec3 * 2, size_Vec3, tangents);
    glBufferSubData(GL_ARRAY_BUFFER, size_Vec3 * 3, size_Vec3, bitangents);
    glBufferSubData(GL_ARRAY_BUFFER, size_Vec3 * 4, size_Vec2, uvs);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


Mesh* Mesh_Load(const char* filename) {

    return Mesh_FullLoad(filename, NULL);
}

// Charge et prépare directement un mesh
// Ecrit le chemin du fichier mtl dans mtlFile
Mesh* Mesh_FullLoad(const char* filename, char* mtlFile) {

    Mesh* mesh = calloc(1, sizeof(Mesh));

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    Vec2* range = NULL;
    char** matNames = NULL;
    int nbVertices = 0, nbObjects = 0, nbMat = 0;
    Material* materialList = NULL;

    if (loadObj(filename, &vertices, &normals, &uvs, &nbVertices, &range, &nbObjects, &matNames, mtlFile) == false)
        return NULL;

    if (mtlFile != NULL && strlen(mtlFile) != 0)
        if (loadMtl(mtlFile, &materialList, &nbMat) == false)
            return NULL;
    if (materialList != NULL && matNames != NULL)
        mesh->material = desindexeMaterial(materialList, nbMat, matNames, nbObjects);
    else
    {
        int i;
        mesh->material = malloc(sizeof(Material) * nbObjects);
        for (i = 0 ; i < nbObjects ; i++ )
            mesh->material[i] = Material_GetDefault();
    }

    mesh->drawStart = malloc(sizeof(int) * nbObjects);
    mesh->drawCount = malloc(sizeof(int) * nbObjects);
    int i;
    for (i = 0 ; i < nbObjects ; i++ )
    {
        mesh->drawStart[i] = range[i].x;
        mesh->drawCount[i] = range[i].y;
    }

    mesh->primitiveType = GL_TRIANGLES;
    mesh->nb = nbObjects;

    if (mesh->material[0].hasNormal == true)
    {
        Vec3* tangents = malloc(sizeof(Vec3) * nbVertices);
        Vec3* bitangents = malloc(sizeof(Vec3) * nbVertices);
        computeTangentSpace(vertices, normals, uvs, nbVertices, tangents, bitangents);
        Mesh_CreateVBO3(mesh, vertices, normals, tangents, bitangents, uvs, nbVertices);
        Mesh_CreateVAO(mesh, 5, (int[5]){0, 1, 3, 4, 2}, (int[5]){0, sizeof(Vec3)*nbVertices, sizeof(Vec3)*nbVertices*2, sizeof(Vec3)*nbVertices*3, sizeof(Vec3)*nbVertices*4}, (int[5]){3, 3, 3, 3, 2});
        mesh->bitangents = bitangents;
        mesh->tangents = tangents;
        mesh->normals = normals;
        mesh->vertices = vertices;
//        free(tangents);
//        free(bitangents);
    }
    else
    {
        mesh->bitangents = NULL;
        mesh->tangents = NULL;
        mesh->normals = NULL;
        mesh->vertices = vertices;
        Mesh_CreateVBO2(mesh, vertices, normals, uvs, nbVertices);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nbVertices, (sizeof(Vec3)*nbVertices) + sizeof(Vec3)*nbVertices}, (int[3]){3, 3, 2});
    }

//    free(vertices);
//    free(normals);
    free(uvs);
    free(range);

    return mesh;
}

// Charge un mesh défini dans carre.h ou cube.h
Mesh* Mesh_LoadBuiltin(int type) {

    Mesh* mesh = calloc(1, sizeof(Mesh));
    mesh->nb = 1;
    mesh->drawStart = malloc(sizeof(int));
    mesh->drawCount = malloc(sizeof(int));

    mesh->material = malloc(sizeof(Material));
    mesh->material[0] = Material_GetDefault();

    switch (type) {

    case MESH_CUBE:

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cube), (void*)cube);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 1}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 3});

        break;

    case MESH_CUBE_TEX: // CUBE TEXTURE

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTex), (void*)cubeTex);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 2}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CUBE_TEX_FLIP: // CUBE TEXTURE INVERSE (skybox)

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTexFliped), (void*)cubeTexFliped);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 2}, (int[2]){0, VBO_COLOR_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CUBE_TEX_NORM: // CUBE TEXTURE + NORMAL

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 36;
        Mesh_CreateVBO(mesh, sizeof(cubeTex), (void*)cubeTex);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, 0, VBO_COLOR_OFFSET}, (int[3]){3, 3, 2});

        break;

    case MESH_CARRE_TEX: // CARRE TEXTURE

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 6;
        Mesh_CreateVBO(mesh, sizeof(carre), (void*)carre);
        Mesh_CreateVAO(mesh, 2, (int[2]){0, 2}, (int[2]){0, VBO_TEXCOORD_OFFSET}, (int[2]){3, 2});

        break;

    case MESH_CARRE_TEX_NORM: // CARRE TEXTURE + NORMAL

        mesh->drawStart[0] = 0;
        mesh->drawCount[0] = 6;
        Mesh_CreateVBO(mesh, sizeof(carreNorm), (void*)carreNorm);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, VBO_NORMAL_OFFSET, VBO_TEXCOORD_OFFSET}, (int[3]){3, 3, 2});

        break;

    case MESH_CARRE_TEX_NORM2: // CARRE TEXTURE * 20 + NORMAL

        mesh->drawStart[0] = 0;
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

// A partir d'une liste des noms des matériaux de chaque sous-objet (usemtl, .obj)
// et de la liste complète de tous les matériaux disponibles (newmtl, .mtl)
// on crée un tableau de matériaux qui va associer chaque nom de matériau avec ses données propres
Material* desindexeMaterial(Material* material, int nbMat, char** mtlRef, int nbRef) {

    Material* res = calloc(sizeof(Material), nbRef);

    // Pas de matériaux définis
    if (nbMat == 0 || nbRef == 0)
        return res;

    int i, j;
    for (i = 0 ; i < nbRef ; i++ )
    {
        for (j = 0 ; j < nbMat ; j++ )

            if (strcmp(mtlRef[i], material[j].nom) == 0)
                res[i] = material[j];
    }

    return res;
}

Material Material_GetDefault() {

    Material mat = {};

    strcpy(mat.nom, "Default");
    mat.ambient = Vec3_Create(1, 1, 1);
    mat.diffuse = Vec3_Create(1, 1, 1);
    mat.specular = Vec3_Create(1, 1, 1);
    mat.exponent = 20;
    mat.hasTexture = false;
    mat.hasNormal = false;
    mat.texture = 0;
    mat.normalMap = 0;

    return mat;
}
