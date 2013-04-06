#include "light.h"

#include "matrix.h"

Light createLight(Instance instance, Vec3 pos, Vec3 color) {

    Light light = {};
    light.pos = pos;
    light.color = color;
    light.instance = instance;
    loadIdentity(light.instance.matrix);

    return light;
}

void Light_SetPosColor(Light* light, Vec3 pos, Vec3 color) {

    light->pos = pos;
    light->color = color;

}
