#include "mesh.h"

#include "cube.h"
#include "carre.h"
#include "utils/vec3.h"
#include "utils/vec2.h"
#include "utils/hashMap.h"
#include "objLoader.h"
#include "shader_library.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* correctMtlPath(const char* path, const char* base_path);

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
    int sizeNormals  = sizeof(Vec3) * nb;
    int sizeUvs      = sizeof(Vec2) * nb;

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

void Mesh_CreateIndexVBO(Mesh* mesh, int* indices, int nb) {

    glGenBuffers(1, &mesh->vbo_indices);

    glBindVertexArray(mesh->vao);

    // Le binding du buffer a element_array_buffer est enregistré dans le vao
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * nb, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

// Charge et prépare directement un mesh
// Ecrit le chemin du fichier mtl dans mtlFile
Mesh* Mesh_Load(const char* objPath, bool loadMTL) {

    puts("\n----------- Mesh ---------");
    printf("Loading '%s'\n", objPath);

    Mesh* mesh = calloc(1, sizeof(Mesh));

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    Vec2* range = NULL;
    char mtlFile[256] = "";
    char** matNames = NULL;
    int nbVertices = 0, nbObjects = 0, nbMat = 0;
    Material* materialList = NULL;

    if (loadObj(objPath, &vertices, &normals, &uvs, &nbVertices, &range, &nbObjects, &matNames, mtlFile) == false)
        return NULL;

    if (loadMTL == MESH_LOAD_MTL && strlen(mtlFile) != 0)
    {
        char* mtlPath = correctMtlPath(objPath, mtlFile);

        if (loadMtl(mtlPath, &materialList, &nbMat) == false)
            return NULL;

        if (materialList != NULL && matNames != NULL)
            mesh->material = desindexeMaterial(materialList, nbMat, matNames, nbObjects);
    }
    else
    {
        mesh->material = malloc(sizeof(Material) * nbObjects);
        int i;
        for (i = 0 ; i < nbObjects ; i++ )
            mesh->material[i] = Material_GetDefault();
    }

    mesh->drawStart = malloc(sizeof(int) * nbObjects);
    mesh->drawCount = malloc(sizeof(int) * nbObjects);
    int i;
    int s = 0;
    for (i = 0 ; i < nbObjects ; i++ )
    {
//        printf("%d: [%s] start: %d count: %d vertices\n", i, mesh->material[i].nom, (int)range[i].x, (int)range[i].y);
        mesh->drawStart[i] = range[i].x;
        mesh->drawCount[i] = range[i].y;
    }

    mesh->primitiveType = GL_TRIANGLES;
    mesh->nb = nbObjects;

    int* indices = NULL;
    int nb_uniq_vertices = 0;

    if (true || mesh->material[0].type & NORMAL_MAP)
    {
        Vec3* tangents = malloc(sizeof(Vec3) * nbVertices);
        Vec3* bitangents = malloc(sizeof(Vec3) * nbVertices);
        computeTangentSpace(vertices, normals, uvs, nbVertices, tangents, bitangents);

        if (INDEXED_GEOMETRY)
        {
            puts("indexAttribsTBN");
//            indexAttribs_TBNSlow(&vertices, &normals, &uvs, &tangents, &bitangents, nbVertices, &indices, &nb_uniq_vertices);
            indexAttribs_TBN(&vertices, &normals, &uvs, &tangents, &bitangents, nbVertices, &indices, &nb_uniq_vertices);
            Mesh_CreateVBO3(mesh, vertices, normals, tangents, bitangents, uvs, nb_uniq_vertices);
            Mesh_CreateVAO(mesh, 5, (int[5]){0, 1, 3, 4, 2}, (int[5]){0, sizeof(Vec3)*nb_uniq_vertices, sizeof(Vec3)*nb_uniq_vertices*2, sizeof(Vec3)*nb_uniq_vertices*3, sizeof(Vec3)*nb_uniq_vertices*4}, (int[5]){3, 3, 3, 3, 2});
            Mesh_CreateIndexVBO(mesh, indices, nbVertices);

        }
        else
        {
            Mesh_CreateVBO3(mesh, vertices, normals, tangents, bitangents, uvs, nbVertices);
            Mesh_CreateVAO(mesh, 5, (int[5]){0, 1, 3, 4, 2}, (int[5]){0, sizeof(Vec3)*nbVertices, sizeof(Vec3)*nbVertices*2, sizeof(Vec3)*nbVertices*3, sizeof(Vec3)*nbVertices*4}, (int[5]){3, 3, 3, 3, 2});
        }

        mesh->bitangents = bitangents;
        mesh->tangents = tangents;
        mesh->normals = normals;
    }
    else if (INDEXED_GEOMETRY)
    {
//        indexAttribsSlow(&vertices, &normals, &uvs, nbVertices, &indices, &nb_uniq_vertices);
        indexAttribs(&vertices, &normals, &uvs, nbVertices, &indices, &nb_uniq_vertices);
        Mesh_CreateVBO2(mesh, vertices, normals, uvs, nb_uniq_vertices);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nb_uniq_vertices, sizeof(Vec3)*nb_uniq_vertices * 2}, (int[3]){3, 3, 2});
        Mesh_CreateIndexVBO(mesh, indices, nbVertices);
    }
    else
    {
        Mesh_CreateVBO2(mesh, vertices, normals, uvs, nbVertices);
        Mesh_CreateVAO(mesh, 3, (int[3]){0, 1, 2}, (int[3]){0, sizeof(Vec3)*nbVertices, (sizeof(Vec3)*nbVertices) + sizeof(Vec3)*nbVertices}, (int[3]){3, 3, 2});
    }

    mesh->vertices = vertices;
    mesh->indices = indices;

//    free(vertices);
//    free(normals);
    free(uvs);
    free(range);
//    free(indices);

    puts("\n---------");

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
            {
                res[i] = material[j];
                break;
            }
    }

    return res;
}

Material Material_GetDefault() {

    Material mat = {};

    strcpy(mat.nom, "Default");
    mat.shader = ShaderLibrary_Get("noTexNoLight");
    mat.ambient = Vec3_Create(1, 1, 1);
    mat.diffuse = Vec3_Create(1, 1, 1);
    mat.specular = Vec3_Create(1, 1, 1);
    mat.exponent = 20;
    mat.type = NONE;
    mat.texture = 0;
    mat.normalMap = 0;
    mat.specularMap = 0;

    return mat;
}

void indexAttribs(Vec3** vertices, Vec3** normals, Vec2** uvs, int nbVertices, int** indices_out, int* uniq_vertices_out) {

    clock_t start = clock();

    // On crée des nouveaux tableaux de même taille que les anciens pour être sur que les données rentreront
    // Au final seulement 70-80% de l'espace sera utilisé mais ils seront redimensionnés à la fin de la fonction
    // On fait cela car on suppose que des listes chainées auraient été plus lentes
    Vec3* new_vertices = malloc(sizeof(Vec3) * nbVertices);
    Vec3* new_normals = malloc(sizeof(Vec3) * nbVertices);
    Vec2* new_uvs = malloc(sizeof(Vec2) * nbVertices);

    // Par contre cette taille est correcte
    int* indices = malloc(sizeof(int) * nbVertices);

    HashMap* hashmap = HashMap_create(nbVertices);

    Vertex vertex = {};
    int index;

    int uniq_vertices = 0;

    int i;
    for (i = 0 ; i < nbVertices ; i++ )
    {
        // On cherche la combinaison des 3 attributs d'un vertex dans les nouveaux tableaux
        vertex = (Vertex){(*vertices)[i], (*normals)[i], (*uvs)[i]};

        index = HashMap_get(hashmap, vertex);

        // Si pas trouvé, on rentre cette combinaison dans les nouveaux tableaux et on rentre son index
        if (index == -1)
        {
            new_vertices[uniq_vertices] = (*vertices)[i];
            new_normals[uniq_vertices] = (*normals)[i];
            new_uvs[uniq_vertices] = (*uvs)[i];
            indices[i] = uniq_vertices;
            HashMap_add(hashmap, vertex, indices[i]);
            uniq_vertices++;
        }
        // Si trouvé, alors on a juste à rentrer l'index de la combinaison
        else
        {
            indices[i] = index;
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

    printf("\tVertex reuse: %d of %d => %.2f%% reutilisation rate\n", uniq_vertices, i, ((float)uniq_vertices/i)*100);
    printf("\tTime used: %.3f seconds\n", ((float)clock() - start) / CLOCKS_PER_SEC);
    HashMap_printCollisions(hashmap);

    HashMap_Delete(hashmap);

}

void indexAttribs_TBN(Vec3** vertices, Vec3** normals, Vec2** uvs, Vec3** tangents, Vec3** bitangents, int nbVertices, int** indices_out, int* uniq_vertices_out/*, int const* const vertex_offsets*/) {

    clock_t start = clock();

    // On crée des nouveaux tableaux de même taille que les anciens pour être sur que les données rentreront
    // Au final seulement 70-80% de l'espace sera utilisé mais ils seront redimensionnés à la fin de la fonction
    // On fait cela car on suppose que des listes chainées auraient été plus lentes
    Vec3* new_vertices = malloc(sizeof(Vec3) * nbVertices);
    Vec3* new_normals = malloc(sizeof(Vec3) * nbVertices);
    Vec2* new_uvs = malloc(sizeof(Vec2) * nbVertices);
    Vec3* new_tangents = malloc(sizeof(Vec3) * nbVertices);
    Vec3* new_bitangents = malloc(sizeof(Vec3) * nbVertices);

    // Par contre cette taille est correcte
    int* indices = malloc(sizeof(int) * nbVertices);

    HashMap* hashmap = HashMap_create(nbVertices);

    Vertex vertex = {};
    int index;

    int uniq_vertices = 0;

    int i;
    for (i = 0 ; i < nbVertices ; i++ )
    {
        // On cherche la combinaison des 3 attributs d'un vertex dans les nouveaux tableaux
        vertex = (Vertex){(*vertices)[i], (*normals)[i], (*uvs)[i]};

        index = HashMap_get(hashmap, vertex);

        // Si pas trouvé, on rentre cette combinaison dans les nouveaux tableaux et on rentre son index
        if (index == -1)
        {
            new_vertices[uniq_vertices] = (*vertices)[i];
            new_normals[uniq_vertices] = (*normals)[i];
            new_uvs[uniq_vertices] = (*uvs)[i];
            new_tangents[uniq_vertices] = (*tangents)[i];
            new_bitangents[uniq_vertices] = (*bitangents)[i];
            indices[i] = uniq_vertices;
            HashMap_add(hashmap, vertex, indices[i]);
            uniq_vertices++;
        }
        // Si trouvé, alors on a juste à rentrer l'index de la combinaison
        else
        {
            indices[i] = index;
        }

    }

    new_vertices = realloc(new_vertices, sizeof(Vec3) * uniq_vertices);
    new_normals = realloc(new_normals, sizeof(Vec3) * uniq_vertices);
    new_uvs = realloc(new_uvs, sizeof(Vec2) * uniq_vertices);
    new_tangents = realloc(new_tangents, sizeof(Vec3) * uniq_vertices);
    new_bitangents = realloc(new_bitangents, sizeof(Vec3) * uniq_vertices);

    *indices_out = indices;
    *uniq_vertices_out = uniq_vertices;
    free(*vertices);
    free(*normals);
    free(*uvs);
    free(*tangents);
    free(*bitangents);
    *vertices = new_vertices;
    *normals = new_normals;
    *uvs = new_uvs;
    *tangents = new_tangents;
    *bitangents = new_bitangents;

    printf("\tVertex reuse: %d of %d => %.2f%% reutilisation rate\n", uniq_vertices, i, ((float)uniq_vertices/i)*100);
    printf("\tTime used: %.3f seconds\n", ((float)clock() - start) / CLOCKS_PER_SEC);
    HashMap_printCollisions(hashmap);

    HashMap_Delete(hashmap);
}

char* getDirectoryPath(const char* path);

// Get the directory path of the obj file (relative to models/) and insert it to the mtl filename
// The .obj can be in a directory and we expect the .mtl to be along it
// But we must reconstruct the mtl path because the .obj typically only gives the name of the .mtl, without its path
// TODO: detect and fix the case where the .obj gives the correct .mtl path
char* correctMtlPath(const char* objPath, const char* mtlFile) {

    char* objDirPath = getDirectoryPath(objPath);

	char final_length = strlen(objDirPath) + strlen(mtlFile);

	char* final_path = (char*)calloc(sizeof(char), final_length + 1);

	strncpy(final_path, objDirPath, strlen(objDirPath));
	strcat(final_path, mtlFile);

    free(objDirPath);

	return final_path;
}

char* getDirectoryPath(const char* path) {

    char* pointer = strrchr(path, '/');

    if (pointer == NULL)
    	return NULL;

    int base_path_length = pointer - path + 1;

    char* base_path = (char*)calloc(sizeof(char) , base_path_length + 1);

    strncpy(base_path, path, base_path_length);

    return base_path;
}
