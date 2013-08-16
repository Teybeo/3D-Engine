#include "light.h"

#include "utils/matrix.h"
#include <stdlib.h>

Light createLight(Object3D object, Vec3 pos, Vec3 color) {

    Light light = {};
    light.pos = pos;
    light.color = color;
    light.object = object;
    loadIdentity(light.object.matrix);

    return light;
}

void Light_SetPosColor(Light* light, Vec3 pos, Vec3 color) {

    light->pos = pos;
    light->color = color;

}
// Transforme un tableau de lampe en tableau de vec3 contenant les positions et les couleurs
Vec3* Light_Serialize(Light* light, int nb) {

    Vec3* tab = malloc(sizeof(Vec3) * nb * 2);
    int i;
    for (i = 0 ; i < nb ; i++ )
    {
        tab[i] = light[i].pos;
        tab[nb + i] = light[i].color;
    }

    return tab;
}
