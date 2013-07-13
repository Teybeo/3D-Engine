#ifndef _MODEL_GUARD
#define _MODEL_GUARD

#include "glew.h"

#include <stdbool.h>

typedef enum ModelType {

    MODEL_OBJ,
    MODEL_CARRE,
    MODEL_CARRE_TEX,
    MODEL_CARRE_TEX_NORM,
    MODEL_CARRE_TEX_NORM2,
    MODEL_CUBE,
    MODEL_CUBE_TEX,
    MODEL_CUBE_TEX_NORM

} ModelType;

typedef struct Model {

    GLuint vao;
    GLuint vbo;
    GLenum primitiveType;
    GLint drawStart;
    GLint drawCount;

} Model;

Model* Model_Load(int type, const char* filename);
void Model_Draw(Model* model);

#endif // MODEL

