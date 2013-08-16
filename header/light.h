#ifndef _LIGHT_GUARD
#define _LIGHT_GUARD

#include "utils/vec3.h"
#include "object3d.h"

typedef struct Light {

    Vec3 pos;
    Vec3 color;
    Object3D object;

} Light;

void Light_SetPosColor(Light* light, Vec3 pos, Vec3 color);
Light createLight(Object3D object, Vec3 pos, Vec3 color);
Vec3* Light_Serialize(Light* light, int nb);

#endif // LIGHT

