#ifndef _MODEL_GUARD
#define _MODEL_GUARD

#include "vec3.h"
#include "vec2.h"

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

typedef struct Mesh {

    GLuint vao;
    GLuint vbo;
    GLenum primitiveType;
    GLint drawStart;
    GLint drawCount;

} Mesh;

Mesh* Mesh_LoadBuiltin(int type);
Mesh* Mesh_Load(const char* filename);
Mesh* Mesh_FullLoad(const char* filename, char* texFile);
void Mesh_Draw(Mesh* model);

#endif // MODEL

