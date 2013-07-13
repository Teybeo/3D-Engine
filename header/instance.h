#ifndef _INSTANCE_GUARD
#define _INSTANCE_GUARD

#include "mesh.h"

typedef struct Instance {

    Mesh* mesh;
    float matrix[16];
    GLuint texture;
    GLuint program;

} Instance;

typedef struct InstanceGroupe {

    Mesh* mesh;
    int nbInstances;
    GLuint matrixVBO;
    float **matrix;
    GLuint program;
    GLuint texture;

} InstanceGroupe;

Instance Instance_Load(const char* objFile, GLuint program);
Instance Instance_Create(Mesh* mesh, GLuint program, GLuint texture);
void Instance_Draw(Instance object, float* mondeToCam, float* camToClip);

void InstanceGroupe_Draw(InstanceGroupe groupe, float* mondeToCam, float* camToClip);
InstanceGroupe InstanceGroupe_Create(Mesh* mesh, int nb, GLuint program, GLuint texture);
void uploadMatrix(InstanceGroupe groupe);

#endif // INSTANCE

