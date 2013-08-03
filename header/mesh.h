#ifndef _MODEL_GUARD
#define _MODEL_GUARD

#include "utils/vec3.h"
#include "utils/vec2.h"

#include "glew.h"

#include <stdbool.h>

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
    GLuint texture;

} Material;

// Représente graphiquement 1 ou plusieurs objets.
// La géométrie est stockée dans un seul vbo.
typedef struct Mesh {

    GLuint vao;
    GLuint vbo;
    GLenum primitiveType;
    int nb;
    GLint* drawStart;
    GLint* drawCount;
    Material* material;
} Mesh;

Mesh* Mesh_LoadBuiltin(int type);
Mesh* Mesh_Load(const char* filename);
Mesh* Mesh_FullLoad(const char* filename, char* texFile);
void Mesh_Draw(Mesh* model);
Material Material_GetDefault();
Material* desindexeMaterial(Material* material, int nbMat, char** mtlRef, int nbRef);


#endif // MODEL

