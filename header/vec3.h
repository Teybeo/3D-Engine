#ifndef _VEC3_GUARD
#define _VEC3_GUARD

#define TAU 6.283185307179586476925286766559

typedef struct Vec3 {

    float x;
    float y;
    float z;

} Vec3;

void Vec3_Add(Vec3* a, Vec3 b);
void Vec3_Sub(Vec3* a, Vec3 b);
Vec3 Vec3_SubOut(Vec3 const a, Vec3 const b);

void Vec3_Mul(Vec3 *p, Vec3 a);
float Vec3_Mul_Out(Vec3 const a, Vec3 const b);


void Vec3_Mul_Scal(Vec3* vec, float scal);
Vec3 Vec3_Mul_Scal_out(Vec3 vec, float scal);

void Vec3_Div_Scal(Vec3* vec, float scal);

void Vec3_Normalise(Vec3* vec);
Vec3 Vec3_Normalise_Out(Vec3 const vec);

inline float Vec3_Distance2Points(Vec3 const a, Vec3 const b);
inline float Vec3_Distance2PointsCarree(Vec3 const a, Vec3 const b);

Vec3 Vec3_Project(Vec3 a, Vec3 b);

void setVec3(Vec3* vec, float x, float y, float z);
void afficheVec3(Vec3 a);

#endif // VEC3

