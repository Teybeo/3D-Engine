#ifndef _INSTANCE_GUARD
#define _INSTANCE_GUARD

#include "mesh.h"
#include "shader.h"

typedef struct Instance {

    Mesh* mesh;
    float matrix[16];
    Shader shader;

} Instance;

typedef struct InstanceGroupe {

    Mesh* mesh;
    int nbInstances;
    GLuint matrixVBO;
    float **matrix;
    Shader shader;
    GLuint texture;

} InstanceGroupe;

Instance Instance_Load(const char* objFile, Shader shader);
Instance Instance_Create(Mesh* mesh, Shader shader, GLuint texture);
void Instance_Draw(Instance object, float* mondeToCam, float* camToClip);

void InstanceGroupe_Draw(InstanceGroupe groupe, float* mondeToCam, float* camToClip);
InstanceGroupe InstanceGroupe_Create(Mesh* mesh, int nbInstances, Shader shader, GLuint texture);
void uploadMatrix(InstanceGroupe groupe);

#endif // INSTANCE

