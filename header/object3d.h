#ifndef _OBJECT3D_GUARD
#define _OBJECT3D_GUARD

#include "mesh.h"
#include "shader.h"

typedef struct Object3D {

    Mesh* mesh;
    float matrix[16];
    Shader* shader;

} Object3D;

typedef struct Object3DGroupe {

    Mesh* mesh;
    int nbObject3Ds;
    GLuint matrixVBO;
    float **matrix;
    Shader* shader;
    GLuint texture;

} Object3DGroupe;

Object3D Object3D_Load(const char* objFile, const char* shader);
Object3D Object3D_Create(Mesh* mesh, const char* shader, GLuint texture);
void Object3D_Draw(Object3D object, bool onlyDepth, float* mondeToCam, float* camToClip, Shader* depthShader);

void Object3DGroupe_Draw(Object3DGroupe groupe, float* mondeToCam, float* camToClip);
Object3DGroupe Object3DGroupe_Create(Mesh* mesh, int nbObject3Ds, const char* shader, GLuint texture);
void uploadMatrix(Object3DGroupe groupe);

#endif // OBJECT3D

