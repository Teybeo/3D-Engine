#include "sphere.h"

#include "stdlib.h"
#include "matrix.h"
#include "stdio.h"

SphereGroupe SphereGroupe_Create(int nbMax, Mesh* mesh, GLuint program, GLuint texture) {

    SphereGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.program = program;
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

void SphereGroupe_Draw(SphereGroupe sphereGroupe, float* mondeToCam, float* camToClip) {

    Instance balleInstance = Instance_Create(sphereGroupe.mesh, sphereGroupe.program, sphereGroupe.texture);

    int i;
    for (i = 0 ; i < sphereGroupe.nbSpheres ; i++ )
    {
        loadIdentity(balleInstance.matrix);
        translateByVec(balleInstance.matrix, sphereGroupe.collisionData[i].sphere.particule.position);
        scale(balleInstance.matrix, sphereGroupe.collisionData[i].sphere.rayon, sphereGroupe.collisionData[i].sphere.rayon, sphereGroupe.collisionData[i].sphere.rayon);

        Instance_Draw(balleInstance, mondeToCam, camToClip);
    }
}

void Sphere_Draw(Sphere sphere, float* mondeToCam, float* camToClip) {

    loadIdentity(sphere.instance.matrix);
    translateByVec(sphere.instance.matrix, sphere.collisionData.particule.position);
    scale(sphere.instance.matrix, sphere.collisionData.rayon, sphere.collisionData.rayon, sphere.collisionData.rayon);
    Instance_Draw(sphere.instance, mondeToCam, camToClip);
}

//Sphere* initGroupeSphere(Instance instance, int nombre) {
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
//        balle[i].instance = instance;

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
