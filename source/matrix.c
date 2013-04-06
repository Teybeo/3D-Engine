#include "matrix.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>


// Les transformations se font de la gauche vers la droite
//                        |--------->
// L'opération est: pos = pos * A * B
// Example:
//      pos = pos * mondeToCam * camToClip;
//   => pos = pos * MatxMat_GaucheVersDroite(mondeToCam, camToClip);
// Multiplie chaque vecteur colonne de A par la matrice B
//
// -------- [GLSL utilise cette convention] -----------
//
float* MatxMat_GaucheVersDroite(float* a, float* b) {

    float* mat = calloc(sizeof(float), 16);

    Vec4 ligne = {};
    int i;
    for (i = 0 ; i < 4 ; i++ ) {
        ligne = Mat_Mul_Vec(b, (Vec4){a[i], a[i+4], a[i+8], a[i+12]} );
        mat[i] = ligne.x;
        mat[i+4] = ligne.y;
        mat[i+8] = ligne.z;
        mat[i+12] = ligne.w;
    }

    return mat;
}

// Les transformations se font de la droite vers la gauche
//                        <---------|
// L'opération est: pos = A * B * pos
// Example:
//      pos = camToClip * mondeToCam * pos;
//   => pos = MatxMat_DroiteVersGauche(camToClip, mondeToCam) * pos
// Multiplie chaque vecteur colonne de B par la matrice A
// Mat_Mul2(float* a, float* b)
float* MatxMat_DroiteVersGauche(float* a, float* b) {

    float* mat = calloc(sizeof(float), 16);

    Vec4 ligne = {};
    int i;
    for (i = 0 ; i < 4 ; i++ ) {
        ligne = Mat_Mul_Vec(a, (Vec4){b[i], b[i+4], b[i+8], b[i+12]} );
        mat[i] = ligne.x;
        mat[i+4] = ligne.y;
        mat[i+8] = ligne.z;
        mat[i+12] = ligne.w;
    }

    return mat;
}

void Mat_Mul_DroiteVersGauche2(float* a, float* b) {

    float* mat = MatxMat_DroiteVersGauche(a, b);

    memcpy(a, mat, sizeof(float)*16);

    free(mat);
}

void Mat_Mul_GaucheVersDroite2(float* a, float* b) {

    float* mat = MatxMat_GaucheVersDroite(a, b);

    memcpy(a, mat, sizeof(float)*16);

    free(mat);
}
void loadIdentity(float* mat) {

    float identity[16] = {
        1,    0,    0,    0,
        0,    1,    0,    0,
        0,    0,    1,    0,
        0,    0,    0,    1,
    };

    memcpy(mat, identity, sizeof(float)*16);

}

// GLSL différencie vec * mat et mat * vec
// Dans un sens colonne, l'autre ligne...
Vec4 Mat_Mul_Vec(float* mat, Vec4 pos) {

    Vec4 res = {};

    res.x = ( pos.x * mat[0]  ) + ( pos.y * mat[1]  ) + ( pos.z * mat[2]  ) + ( pos.w * mat[3]  );
    res.y = ( pos.x * mat[4]  ) + ( pos.y * mat[5]  ) + ( pos.z * mat[6]  ) + ( pos.w * mat[7]  );
    res.z = ( pos.x * mat[8]  ) + ( pos.y * mat[9]  ) + ( pos.z * mat[10] ) + ( pos.w * mat[11] );
    res.w = ( pos.x * mat[12] ) + ( pos.y * mat[13] ) + ( pos.z * mat[14] ) + ( pos.w * mat[15] );

    return res;

}
/*  x,  0,  0,  0,
    0,  y,  0,  0,
    0,  0,  z,  0,
    0,  0,  0,  1 */
void scale(float matrix[16], float x, float y, float z) {

    float scale[16];
    loadIdentity(scale);

    matrix[0] = x;
    matrix[5] = y;
    matrix[10] = z;

    Mat_Mul_DroiteVersGauche2(matrix, scale);
}

/* 1,   0,   0,   x,
   0,   1,   0,   y,
   0,   0,   1,   z,
   0,   0,   0,   1 */
void translate(float matrix[16], float x, float y, float z) {

    float translation[16];
    loadIdentity(translation);

    translation[3] = x;
    translation[7] = y;
    translation[11] = z;

    Mat_Mul_DroiteVersGauche2(matrix, translation);

}
/*     1,    0,     0,   0,
       0,  cos,  -sin,   0,
       0,  sin,   cos,   0,
       0,    0,     0,   1 */
void rotateX(float matrix[16], float angle) {

    angle *= M_PI/180;

    matrix[5] = cos(angle);
    matrix[6] = -sin(angle);
    matrix[9] = sin(angle);
    matrix[10] = cos(angle);
}

/*   cos,  0,  sin,  0,
       0,  1,    0,  0,
    -sin,  0,  cos,  0,
       0,  0,    0,  1 */
void rotateY(float matrix[16], float angle) {

    angle *= M_PI/180;

    matrix[0] = cos(angle);
    matrix[2] = sin(angle);
    matrix[8] = -sin(angle);
    matrix[10] = cos(angle);
}
/*  cos,  -sin,  0,  0,
    sin,   cos,  0,  0,
    0  ,  0   ,  1,  0,
    0  ,  0   ,  0,  1
*/
void rotateZ(float matrix[16], float angle) {

    angle *= M_PI/180;

    matrix[0] = cos(angle);
    matrix[1] = -sin(angle);
    matrix[4] = sin(angle);
    matrix[5] = cos(angle);
}

void rotate(float matrix[16], float x, float y, float z) {

    float xMatrix[16];
    float yMatrix[16];
    float zMatrix[16];

    loadIdentity(xMatrix);
    loadIdentity(yMatrix);
    loadIdentity(zMatrix);

    if (fabsf(x) > 0.00001)
    {
        rotateX(xMatrix, x);
        Mat_Mul_DroiteVersGauche2(matrix, xMatrix);
    }
    if (fabsf(y) > 0.00001)
    {
        rotateY(yMatrix, y);
        Mat_Mul_DroiteVersGauche2(matrix, yMatrix);
    }
    if (fabsf(z) > 0.00001)
    {
        rotateZ(zMatrix, z);
        Mat_Mul_DroiteVersGauche2(matrix, zMatrix);
    }

}

/*  distance/r,      0   ,       0     ,       0      ,
        0     ,  distance,       0     ,       0      ,
        0     ,      0   ,  -(f+n)/f-n),  -(2fn)/(f-n),
        0     ,      0   ,      -1     ,       0
*/
void projection(float matrix[16], float angle, float ratio, float zNear, float zFar) {

    // L'angle horizontal de vision définit la distance du plan de projection
    float distance = 1. / tan((angle/2) * M_PI/180);

    matrix[0] = distance/ratio;
    matrix[5] = distance;
    matrix[10] = -(zFar + zNear)/(zFar - zNear);
    matrix[11] =  -(2 * zFar * zNear) / (zFar - zNear);
    matrix[14] = -1; // W -> -z
    matrix[15] = 0;

}

void transpose(float matrix[16]) {

    float temp[16] = {};

    temp[0] = matrix[0];
    temp[4] = matrix[1];
    temp[8] = matrix[2];
    temp[12] = matrix[3];

    temp[1] = matrix[4];
    temp[5] = matrix[5];
    temp[9] = matrix[6];
    temp[13] = matrix[7];

    temp[2] = matrix[8];
    temp[6] = matrix[9];
    temp[10] = matrix[10];
    temp[14] = matrix[11];

    temp[3] = matrix[12];
    temp[7] = matrix[13];
    temp[11] = matrix[14];
    temp[15] = matrix[15];

    memcpy(matrix, temp, 16 * sizeof(float));
}
