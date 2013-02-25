#include "matrix.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>

void Mat_Mul(float* a, float* b) {

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

    memcpy(a, mat, sizeof(float)*16);

    free(mat);
}

float* Mat_Mul2(float* a, float* b) {

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

void loadIdentity(float* mat) {

    float identity[16] = {
        1,    0,    0,    0,
        0,    1,    0,    0,
        0,    0,    1,    0,
        0,    0,    0,    1,
    };

    memcpy(mat, identity, sizeof(float)*16);

}

Vec4 Mat_Mul_Vec(float* mat, Vec4 pos) {

    Vec4 res = {};

    res.x = ( pos.x * mat[0]  ) + ( pos.y * mat[1]  ) + ( pos.z * mat[2]  ) + ( pos.w * mat[3]  );
    res.y = ( pos.x * mat[4]  ) + ( pos.y * mat[5]  ) + ( pos.z * mat[6]  ) + ( pos.w * mat[7]  );
    res.z = ( pos.x * mat[8]  ) + ( pos.y * mat[9]  ) + ( pos.z * mat[10] ) + ( pos.w * mat[11] );
    res.w = ( pos.x * mat[12] ) + ( pos.y * mat[13] ) + ( pos.z * mat[14] ) + ( pos.w * mat[15] );

    return res;

}

/* 1,   0,   0,   x,
   0,   1,   0,   y,
   0,   0,   1,   z,
   0,   0,   0,   1
*/
void translate(float matrix[16], float x, float y, float z) {

    matrix[3] += x;
    matrix[7] += y;
    matrix[11] += z;

}
/*     1,    0,     0,   0,
       0,  cos,  -sin,   0,
       0,  sin,   cos,   0,
       0,    0,     0,   1
*/
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
       0,  0,    0,  1
*/
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
