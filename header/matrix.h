#ifndef _MATRIX_GUARD
#define _MATRIX_GUARD

#include "vec4.h"

void Mat_Mul(float* a, float* b);
float* Mat_Mul2(float* a, float* b);
Vec4 Mat_Mul_Vec(float* mat, Vec4 pos);
void loadIdentity(float* mat);
void translate(float matrix[16], float x, float y, float z);
void rotateX(float matrix[16], float angle);
void rotateY(float matrix[16], float angle);
void rotateZ(float matrix[16], float angle);

#endif // MATRIX

