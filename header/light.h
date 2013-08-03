#ifndef _LIGHT_GUARD
#define _LIGHT_GUARD

#include "utils/vec3.h"
#include "instance.h"

typedef struct Light {

    Vec3 pos;
    Vec3 color;
    Instance instance;

} Light;

void Light_SetPosColor(Light* light, Vec3 pos, Vec3 color);
Light createLight(Instance instance, Vec3 pos, Vec3 color);
Vec3* Light_Serialize(Light* light, int nb);

#endif // LIGHT

