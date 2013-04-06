#ifndef _INSTANCE_GUARD
#define _INSTANCE_GUARD

#include "model.h"

typedef struct Instance {

    Model* model;
    float matrix[16];
    GLuint texture;
    GLuint program;

} Instance;

typedef struct InstanceGroupe {

    Model* model;
    int nbInstances;
    GLuint matrixVBO;
    float **matrix;
    GLuint program;
    GLuint texture;

} InstanceGroupe;

Instance Instance_Create(Model* model, GLuint program, GLuint texture);
void Instance_Draw(Instance object, float* mondeToCam, float* camToClip);

void InstanceGroupe_Draw(InstanceGroupe groupe, float* mondeToCam, float* camToClip);
InstanceGroupe InstanceGroupe_Create(Model* model, int nb, GLuint program, GLuint texture);
void uploadMatrix(InstanceGroupe groupe);

#endif // INSTANCE

