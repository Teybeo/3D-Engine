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


Mesh* Mesh_Load(const char* filename) {

    return Mesh_FullLoad(filename, NULL);
}

// Charge et prépare directement un mesh
// Ecrit le chemin du fichier mtl dans mtlFile
Mesh* Mesh_FullLoad(const char* filename, char* mtlFile) {

    Mesh* mesh = malloc(sizeof(Mesh));

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    Vec2* range = NULL;
    char** matNames = NULL;
    int nbVertices = 0, nbObjects = 0, nbMat = 0;
    Material* materialList = NULL;

    if (loadObj(filename, &vertices, &normals, &uvs, &nbVertices, &range, &nbObjects, &matNames, mtlFile) == false)
        return NULL;

    if (mtlFile)
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

    int* indices = NULL;
    int nb_uniq_vertices = 0;

    indexAttribs(&vertices, &normals, &uvs, nbVertices, &indices, &nb_uniq_vertices);

    Mesh_CreateVBO2(mesh, vertices, normals, uvs, nbVertices);
    Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nb_uniq_vertices, sizeof(Vec3)*nb_uniq_vertices * 2}, (int[3]){3, 3, 2});

    glBindVertexArray(mesh->vao);
    glGenBuffers(1, &mesh->vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbVertices * sizeof(int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
    free(normals);
    free(uvs);
    free(range);
    free(indices);

    return mesh;
}

// Charge un mesh défini dans carre.h ou cube.h
Mesh* Mesh_LoadBuiltin(int type) {

    Mesh* mesh = malloc(sizeof(Mesh));
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
    mat.texture = 0;

    return mat;
}

int searchVec3(Vec3* array, int nb, Vec3 data) {

    int i;
    for (i = 0 ; i < nb ; i++ )
        if (memcmp(&array[i], &data, sizeof(Vec3)) == 0)
            return i;

    return -1;
}

int searchVec2(Vec2* array, int nb, Vec2 data) {

    int i;
    for (i = 0 ; i < nb ; i++ )
        if (memcmp(&array[i], &data, sizeof(Vec2)) == 0)
            return i;

    return -1;
}

void indexAttribs(Vec3** vertices, Vec3** normals, Vec2** uvs, int nbVertices, int** indices_out, int* uniq_vertices_out) {

    // On crée des nouveaux tableaux de même taille que les anciens pour être sur que les données rentreront
    // Au final seulement 70-80% de l'espace sera utilisé mais ils seront redimensionnés à la fin de la fonction
    // On fait cela car on suppose que des listes chainées auraient été plus lentes
    Vec3* new_vertices = malloc(sizeof(Vec3) * nbVertices);
    Vec3* new_normals = malloc(sizeof(Vec3) * nbVertices);
    Vec2* new_uvs = malloc(sizeof(Vec2) * nbVertices);

    // Par contre cette taille est correcte
    int* indices = malloc(sizeof(int) * nbVertices);

    int pos_vertex_stored = 0;
    int pos_normal_stored = 0;
    int pos_uv_stored = 0;

    int uniq_vertices = 0;

    int i;
    for (i = 0 ; i < nbVertices ; i++ )
    {
        // On regarde si un vertex avec ces 3 attributs existe déjà
        pos_vertex_stored = searchVec3(new_vertices, uniq_vertices, (*vertices)[i]);
        pos_normal_stored = searchVec3(new_normals, uniq_vertices, (*normals)[i]);
        pos_uv_stored = searchVec2(new_uvs, uniq_vertices, (*uvs)[i]);

        // Si oui, on les indexe dans indices
        if ((pos_vertex_stored == pos_normal_stored) && (pos_vertex_stored == pos_uv_stored) && pos_vertex_stored != 0)
        {
            indices[i] = pos_vertex_stored;
        }
        // Si non, on les enregistre et on les indexe
        else
        {
            new_vertices[uniq_vertices] = (*vertices)[i];
            new_normals[uniq_vertices] = (*normals)[i];
            new_uvs[uniq_vertices] = (*uvs)[i];
            indices[i] = uniq_vertices;
            uniq_vertices++;
        }

    }

    new_vertices = realloc(new_vertices, sizeof(Vec3) * uniq_vertices);
    new_normals = realloc(new_normals, sizeof(Vec3) * uniq_vertices);
    new_uvs = realloc(new_uvs, sizeof(Vec2) * uniq_vertices);

    *indices_out = indices;
    *uniq_vertices_out = uniq_vertices;
    free(*vertices);
    free(*normals);
    free(*uvs);
    *vertices = new_vertices;
    *normals = new_normals;
    *uvs = new_uvs;

}
