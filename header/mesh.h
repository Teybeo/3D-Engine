#ifndef _MODEL_GUARD
#define _MODEL_GUARD

#include "utils/vec3.h"
#include "utils/vec2.h"

#include "glew.h"

#include <stdbool.h>

#define INDEXED_GEOMETRY 1

typedef enum MeshType {

    MESH_CARRE,
    MESH_CARRE_TEX,
    MESH_CARRE_TEX_NORM,
    MESH_CARRE_TEX_NORM2,
    MESH_CUBE,
    MESH_CUBE_TEX,
    MESH_CUBE_TEX_NORM,
    MESH_CUBE_TEX_FLIP,

} MeshType;

typedef struct Material {

    char nom[128];
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    unsigned int exponent;
    bool hasTexture;
    bool hasNormal;
    GLuint texture;
    GLuint normalMap;

} Material;

// Représente graphiquement 1 ou plusieurs objets.
// La géométrie est stockée dans un seul vbo.
typedef struct Mesh {

    GLuint vao;
    GLuint vbo;
    GLuint vbo_indices;
    GLenum primitiveType;
    int nb;
    GLint* drawStart; // Offset des attributs de sommet de chaque sub-mesh dans le vbo
    GLint* drawCount; // Nombre de sommets de chaque sub-mesh
    Material* material;
    Vec3* tangents;
    Vec3* bitangents;
    Vec3* normals;
    Vec3* vertices;
    int* indices;

} Mesh;

Mesh* Mesh_LoadBuiltin(int type);
Mesh* Mesh_Load(const char* filename);
Mesh* Mesh_FullLoad(const char* filename, char* texFile);
void Mesh_Draw(Mesh* model);
Material Material_GetDefault();
Material* desindexeMaterial(Material* material, int nbMat, char** mtlRef, int nbRef);
void indexAttribs(Vec3** vertices, Vec3** normals, Vec2** uvs, int nbVertices, int** indices_out, int* uniq_vertices_out);
void indexAttribs_TBN(Vec3** vertices, Vec3** normals, Vec2** uvs, Vec3** tangents, Vec3** bitangents, int nbVertices, int** indices_out, int* uniq_vertices_out);

#endif // MODEL

