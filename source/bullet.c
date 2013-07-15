#include "bullet.h"

#include "stdlib.h"
#include "utils/matrix.h"
#include "stdio.h"

BulletGroupe BulletGroupe_Create(int nbMax, Mesh* mesh, GLuint program, GLuint texture) {

    BulletGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.program = program;
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

void BulletGroupe_Draw(BulletGroupe bulletGroupe, float* mondeToCam, float* camToClip) {

    Instance bulletInstance = Instance_Create(bulletGroupe.mesh, bulletGroupe.program, bulletGroupe.texture);
    Vec3 color = {1, 1, 0};
    int i;
    for (i = 0 ; i < bulletGroupe.nbBullets ; i++ )
    {
        loadIdentity(bulletInstance.matrix);
        translateByVec(bulletInstance.matrix, bulletGroupe.collisionData[i].sphere.particule.position);
        scale(bulletInstance.matrix, bulletGroupe.collisionData[i].sphere.rayon, bulletGroupe.collisionData[i].sphere.rayon, bulletGroupe.collisionData[i].sphere.rayon);

        char name[50] = "";
        sprintf(name, "lightPos[%d]", i+6);
        glUniform3fv(glGetUniformLocation(bulletGroupe.program, name), 1, &bulletGroupe.collisionData[i].sphere.particule.position.x);
        sprintf(name, "lightColor[%d]", i+6);
        glUniform3fv(glGetUniformLocation(bulletGroupe.program, name), 1, &color.x);

        Instance_Draw(bulletInstance, mondeToCam, camToClip);
    }
}

void Bullet_Draw(Bullet bullet, float* mondeToCam, float* camToClip) {

    loadIdentity(bullet.instance.matrix);
    translateByVec(bullet.instance.matrix, bullet.collisionData.particule.position);
    scale(bullet.instance.matrix, bullet.collisionData.rayon, bullet.collisionData.rayon, bullet.collisionData.rayon);
    Instance_Draw(bullet.instance, mondeToCam, camToClip);
}
