#include "bullet.h"

#include "utils/matrix.h"
#include "shader_library.h"

#include <stdlib.h>
#include <stdio.h>

BulletGroupe BulletGroupe_Create(int nbMax, Mesh* mesh, const char* shader, GLuint texture) {

    BulletGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.shader = ShaderLibrary_Get(shader);
    groupe.texture = texture;
    groupe.nbMax = nbMax;
    groupe.nbBullets = 0;

    groupe.collisionData = calloc(nbMax, sizeof(CollisionObject));

    return groupe;

}

void Bullet_Add(BulletGroupe* bulletGroupe, Vec3 position, Vec3 direction) {

    static int index = 0;

    CollisionSphere newBullet = {};

    newBullet.particule = Particule_Init(0.8, 1);
    newBullet.rayon = 5;
    Particule_SetPosition(&newBullet.particule, position);
    Particule_SetVitesse(&newBullet.particule, Vec3_Div_Scal_Out(direction, newBullet.particule.masse));

    bulletGroupe->collisionData[index].sphere = newBullet;
    bulletGroupe->collisionData[index].type = COLLISION_SPHERE;

    index++;
    index %= bulletGroupe->nbMax;

    if (bulletGroupe->nbBullets != bulletGroupe->nbMax)
        bulletGroupe->nbBullets++;
}

void BulletGroupe_Draw(BulletGroupe bulletGroupe, Renderer* renderer) {

    Object3D bulletObject3D = Object3D_Create(bulletGroupe.mesh, bulletGroupe.shader->name, bulletGroupe.texture);
    Vec3 color = {1, 1, 0};
    int i;
    for (i = 0 ; i < bulletGroupe.nbBullets ; i++ )
    {
        loadIdentity(bulletObject3D.matrix);
        translateByVec(bulletObject3D.matrix, bulletGroupe.collisionData[i].sphere.particule.position);
        scale(bulletObject3D.matrix, bulletGroupe.collisionData[i].sphere.rayon, bulletGroupe.collisionData[i].sphere.rayon, bulletGroupe.collisionData[i].sphere.rayon);

        char name[50] = "";
        sprintf(name, "lightPos[%d]", i+6);
        glUniform3fv(glGetUniformLocation(bulletGroupe.shader->id, name), 1, &bulletGroupe.collisionData[i].sphere.particule.position.x);
        sprintf(name, "lightColor[%d]", i+6);
        glUniform3fv(glGetUniformLocation(bulletGroupe.shader->id, name), 1, &color.x);

        Object3D_Draw(bulletObject3D, renderer);
    }
}

void Bullet_Draw(Bullet bullet, Renderer* renderer) {

    loadIdentity(bullet.object.matrix);
    translateByVec(bullet.object.matrix, bullet.collisionData.particule.position);
    scale(bullet.object.matrix, bullet.collisionData.rayon, bullet.collisionData.rayon, bullet.collisionData.rayon);
    Object3D_Draw(bullet.object, renderer);
}
