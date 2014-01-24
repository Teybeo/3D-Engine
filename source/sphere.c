#include "sphere.h"

#include "utils/matrix.h"
#include "shader_library.h"

#include <stdlib.h>
#include <stdio.h>

SphereGroupe SphereGroupe_Create(int nbMax, Mesh* mesh, const char* shader, GLuint texture) {

    SphereGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.shader = ShaderLibrary_Get(shader);
    groupe.texture = texture;
    groupe.nbMax = nbMax;
    groupe.nbSpheres = 0;

    groupe.collisionData = calloc(nbMax, sizeof(CollisionObject));

    return groupe;

}

void Sphere_Add(SphereGroupe* sphereGroupe, Vec3 position, Vec3 direction) {

    static int index = 0;

    CollisionSphere newSphere = {};

    newSphere.particule = Particule_Init(0.8, 10);
    newSphere.rayon = 2;
    Particule_SetPosition(&newSphere.particule, position);
    Particule_SetVitesse(&newSphere.particule, Vec3_Mul_Scal_out(direction, newSphere.particule.masse));

    sphereGroupe->collisionData[index].sphere = newSphere;
    sphereGroupe->collisionData[index].type = COLLISION_SPHERE;

    index++;

    if (index == sphereGroupe->nbMax)
        index = 0;
    if (sphereGroupe->nbSpheres != sphereGroupe->nbMax)
        sphereGroupe->nbSpheres++;
}

void SphereGroupe_Draw(SphereGroupe sphereGroupe, Renderer* renderer) {

    Object3D balleObject3D = Object3D_Create(sphereGroupe.mesh, sphereGroupe.shader->name, sphereGroupe.texture);

    int i;
    for (i = 0 ; i < sphereGroupe.nbSpheres ; i++ )
    {
        loadIdentity(balleObject3D.matrix);
        translateByVec(balleObject3D.matrix, sphereGroupe.collisionData[i].sphere.particule.position);
        scale(balleObject3D.matrix, sphereGroupe.collisionData[i].sphere.rayon, sphereGroupe.collisionData[i].sphere.rayon, sphereGroupe.collisionData[i].sphere.rayon);

        Object3D_Draw(balleObject3D, renderer);
    }
}

void Sphere_Draw(Sphere sphere, Renderer* renderer) {

    loadIdentity(sphere.object.matrix);
    translateByVec(sphere.object.matrix, sphere.collisionData.particule.position);
    scale(sphere.object.matrix, sphere.collisionData.rayon, sphere.collisionData.rayon, sphere.collisionData.rayon);
    Object3D_Draw(sphere.object, renderer);
}

//Sphere* initGroupeSphere(Object3D object, int nombre) {
//
//    Sphere* balle = calloc(NB_BALLS_MAX, sizeof(Sphere));
//
//    int i, j;
//    bool superposition = false;
//
//    for (i = 0 ; i < nombre ; i++ )
//    {
//        balle[i].collisionData.particule = Particule_Init(0.8, 10 + i % 12);
//        balle[i].collisionData.rayon = 2;
//        balle[i].object = object;

//        Particule_AjouteForceRand(&balle[i].collisionData.particule, true, true);
//        Particule_SetPosition(&balle[i], (-FEN_L/2) + rand() % FEN_L, (-FEN_H/2) + rand() % FEN_H);
//        Particule_SetPosition(&balle[0].collisionData.particule, -5, 0, 0);
//        Particule_SetPosition(&balle[1].collisionData.particule, -5, 0, 1);
//        balle[1].collisionData.particule.vitesse.z = -1;
//
//        do {
//
//            Particule_SetPosition(&balle[i].collisionData.particule, MUR_GAUCHE + rand() % (abs(MUR_GAUCHE)+MUR_DROIT), rand() % MUR_HAUT, MUR_ARRIERE + rand() % (abs(MUR_ARRIERE)+MUR_AVANT));
//            Particule_SetPosition(&balle[i].collisionData.particule, (Vec3){-500 + rand() % (abs(-500)+500), rand() % 500, -500 + rand() % (abs(-500)+500)});
//
//            superposition = false; // On suppose qu'on l'a bien placé
//
//            for (j = 0 ; j < nombre ; j++ ) // On verifie pour chaque balle
//
//                if (i != j && CollisionGenerator_AreCollidingSphere(balle[i].collisionData, balle[j].collisionData) == true)
//                {
//                    superposition = true; // Il y a un problème
//                    break;
//                }
//
//
//        } while (superposition == true); // Si un problème on recommence
//
//    }
//
//    return balle;
//
//}

void SphereGroupe_Randomize(SphereGroupe* groupe) {

    int i, j;
    bool superposition = false;
    Vec3 pos;

    for (i = 0 ; i < groupe->nbMax ; i++ )
    {
        pos = Vec3_RandomPos(-20, 20, 50, 510, -20, 20);

        Sphere_Add(groupe, pos, (Vec3){0, 0, 0});
        //groupe.collisionData[i].sphere.particule = Particule_Init(0.8, 10 + i % 12);
        //groupe.collisionData[i].sphere.rayon = 2;

        //Particule_AjouteForceRand(&groupe.collisionData[i].sphere.particule, true, true);
        //Particule_SetPosition(&balle[i], (-FEN_L/2) + rand() % FEN_L, (-FEN_H/2) + rand() % FEN_H);
        //Particule_SetPosition(&balle[0].collisionData.particule, -5, 0, 0);
        //Particule_SetPosition(&balle[1].collisionData.particule, -5, 0, 1);
        //balle[1].collisionData.particule.vitesse.z = -1;

        do {

            //Particule_SetPosition(&balle[i].collisionData.particule, MUR_GAUCHE + rand() % (abs(MUR_GAUCHE)+MUR_DROIT), rand() % MUR_HAUT, MUR_ARRIERE + rand() % (abs(MUR_ARRIERE)+MUR_AVANT));
            pos = Vec3_RandomPos(-30, 30, 50, 510, -30, 30);

            Sphere_Add(groupe, pos, (Vec3){0, 0, 0});
            superposition = false; // On suppose qu'on l'a bien placé

            for (j = 0 ; j < groupe->nbMax ; j++ ) // On verifie pour chaque balle

                if (i != j && CollisionGenerator_AreCollidingSphere(groupe->collisionData[i].sphere, groupe->collisionData[j].sphere) == true)
                {
                    superposition = true; // Il y a un problème
                    break;
                }


        } while (superposition == true); // Si un problème on recommence

    }

}
