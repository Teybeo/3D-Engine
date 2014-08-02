#include "sphere.h"

#include "utils/matrix.h"
#include "shader_library.h"
#include "renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

SphereGroupe SphereGroupe_Create(int nbMax, Mesh* mesh, const char* shader, GLuint texture) {

    SphereGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.shader = ShaderLibrary_Get(shader);
    groupe.texture = texture;
    groupe.nbMax = nbMax;
    groupe.nbSpheres = 0;

    groupe.collisionData = calloc(nbMax, sizeof(CollisionObject));


    groupe.matrix = malloc(sizeof(float*) * nbMax);
    int i;
    for (i = 0 ; i < nbMax ; i++ )
    {
        groupe.matrix[i] = malloc(sizeof(float)*16);
        loadIdentity(groupe.matrix[i]);
//        translate(groupe.matrix[i], -400 + rand() % 800, rand() % 30, -400+rand() % 800);
//        transpose(groupe.matrix[i]); // On transpose à la main
    }

    glGenBuffers(1, &groupe.matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);

    glBufferData(GL_ARRAY_BUFFER, nbMax * 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // Avec un tableau dynamique, si on copie tout d'un coup (BufferData), 2 octets seront occupés entre chaque matrice
    // Je ne sais pas pk, le seul moyen trouvé est d'envoyer chaque matrice séparément
    for (i = 0 ; i < nbMax ; i++ )
        glBufferSubData(GL_ARRAY_BUFFER, i * 16 * sizeof(float), 16 * sizeof(float), &(groupe.matrix[i][0]));

    // On ajoute les matrices au vao déjà créé du modèle
    // Comme les attributs ne peuvent être que des vec4, opengl distribue 4 index, 1 pour chaque colonne
    glBindVertexArray(mesh->vao);

    for (i = 0 ; i < 4 ; i++ )
    {
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(i * 4 * sizeof(float)));
        glVertexAttribDivisor(3 + i, 1);
        glEnableVertexAttribArray(3 + i);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
void SphereGroupe_Draw(SphereGroupe groupe, Renderer* renderer) {

    Shader* shader = NULL;
    float* mondeToCam = renderer->scene->player.mondeToCam;
    float* camToClip = renderer->camToClip;

    if (groupe.mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(groupe.mesh->vao);
        renderer->currentVAO = groupe.mesh->vao;
    }

    int i;
    for (i = 0 ; i < groupe.mesh->nb ; i++ )
    {
//        shader = groupe.mesh->material[i].shader;
        shader = groupe.shader;

        if (renderer->depth_rendering == true)
        {
            shader = ShaderLibrary_Get("depthInstance");
            mondeToCam = renderer->depth_mondeToCam;
            camToClip = renderer->depth_camToProj;
        }

        if (shader->id != renderer->currentShader)
        {
            glUseProgram(shader->id);
            renderer->currentShader = shader->id;
            Shader_SendUniform(shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(shader, "camClip", GL_FLOAT_MAT4, camToClip);
        }

        if (renderer->depth_rendering == false)
            SendMaterial(shader, &groupe.mesh->material[i], renderer);

        if (groupe.mesh->vbo_indices != 0)
            glDrawElementsInstanced(groupe.mesh->primitiveType, groupe.mesh->drawCount[i], GL_UNSIGNED_INT, NULL+groupe.mesh->drawStart[i]*4, groupe.nbSpheres);
        else
            glDrawArraysInstanced(groupe.mesh->primitiveType, groupe.mesh->drawStart[i], groupe.mesh->drawCount[i], groupe.nbSpheres);
    }
}

void SphereGroupe_Update(SphereGroupe groupe) {

    float* matrices = malloc(groupe.nbMax*16*sizeof(float));
    int i;
    for (i = 0 ; i < groupe.nbSpheres ; i++ )
    {
        loadIdentity(&matrices[i*16]);
        translateByVec(&matrices[i*16], groupe.collisionData[i].sphere.particule.position);
        scale(&matrices[i*16], groupe.collisionData[i].sphere.rayon, groupe.collisionData[i].sphere.rayon, groupe.collisionData[i].sphere.rayon);
        transpose(&matrices[i*16]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);
//    for (i = 0 ; i < groupe.nbSpheres ; i++ )
//        glBufferSubData(GL_ARRAY_BUFFER, ( i * 16 * 4), (16 * sizeof(float)), &(groupe.matrix[i][0]));
    glBufferData(GL_ARRAY_BUFFER, groupe.nbMax * 16 * sizeof(float), matrices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    free(matrices);
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
