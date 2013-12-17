#ifndef _MATRIX_GUARD
#define _MATRIX_GUARD

#include "utils/vec4.h"
#include "utils/vec3.h"

float* MatxMat_GaucheVersDroite(float* a, float* b);

void Mat_Mul_GaucheVersDroite2(float* a, float* b);
void Mat_Mul_DroiteVersGauche2(float* a, float* b);
Vec4 Mat_Mul_Vec(float* mat, Vec4 pos);
void loadIdentity(float* mat);
void scale(float matrix[16], float x, float y, float z);
void translate(float matrix[16], float x, float y, float z);
void rotate(float matrix[16], float x, float y, float z);
void rotateX(float matrix[16], float angle);
void rotateY(float matrix[16], float angle);
void rotateZ(float matrix[16], float angle);
void projection(float matrix[16], float angle, float ratio, float zFar, float zNear);
void ortho(float matrix[16], const float left, const float right, const float bottom, const float top, const float zNear, const float zFar);
void transpose(float matrix[16]);
void translateByVec(float matrix[16], Vec3 vec);
void matTo3Vec(float matrix[16], Vec3* x, Vec3* y, Vec3* z);

#endif // MATRIX

