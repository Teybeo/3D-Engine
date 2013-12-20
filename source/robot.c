#include "robot.h"

#include "utils/matrix.h"
#include "texture.h"

#include <stdbool.h>

#include "glew.h"

void draw_jambe(Robot* robot, Partie jambe) {

    loadIdentity(robot->partie[jambe].matrix);

    float angle = 0;
    if (jambe == JAMBE_DROITE)
    {
        angle = robot->angleJambeDroite;
        translate (robot->partie[jambe].matrix, 1, 1.5, 0);  // Cube monté un peu dessus du milieu de la jambe
    }
    else if (jambe == JAMBE_GAUCHE)
    {
        angle = robot->angleJambeGauche;
        translate (robot->partie[jambe].matrix, -1, 1.5, 0);  // Cube monté un peu dessus du milieu de la jambe
    }

    rotate    (robot->partie[jambe].matrix, 40*sin(angle), 0, 0); // Cube orienté
    translate (robot->partie[jambe].matrix, 0, -0.8, 0); // Le centre du cube est éloigné de son centre de rotation
    scale     (robot->partie[jambe].matrix, 0.7, 2.5, 0.7); // Allongement du cube

}

void draw_bras(Robot* robot, Partie bras) {

    float cote = 0, angle = 0;
    if (bras == BRAS_DROIT)
    {
        angle = robot->angleJambeDroite;
        cote = 1;
    }
    else if (bras == BRAS_GAUCHE)
    {
        angle = robot->angleJambeGauche;
        cote = -1;
    }

    loadIdentity(robot->partie[bras].matrix);
    loadIdentity(robot->partie[bras+2].matrix);

    translate (robot->partie[bras].matrix, 2.1*cote, 8.1, 0);
    rotate    (robot->partie[bras].matrix, 40*sin(angle), 0, 0);
    translate (robot->partie[bras].matrix, 0, -1.7, 0);

    memcpy    (robot->partie[bras+2].matrix, robot->partie[bras].matrix, 16*sizeof(float));
    scale     (robot->partie[bras+2].matrix, 0.8, 0.8, 0.8);
    translate (robot->partie[bras+2].matrix, 0, -0.7, 0);
    rotate    (robot->partie[bras+2].matrix, -20+20*sin(angle), 0, 0);
    translate (robot->partie[bras+2].matrix, 0, -1.8, 0);

}

void draw_tete(Robot* robot) {

    loadIdentity(robot->partie[TETE].matrix);
    translate(robot->partie[TETE].matrix, 0, 9.5, 1);
    scale(robot->partie[TETE].matrix, 2, 2, 2);

}

void draw_corps(Robot* robot) {

    loadIdentity(robot->partie[CORPS].matrix);
    translate(robot->partie[CORPS].matrix, 0, 6, 0);
    scale(robot->partie[CORPS].matrix, 1.8, 1.8, 1.8);
}

void Robot_draw(Robot* robot, float* worldCam, float* camClip) {

    draw_jambe(robot, JAMBE_DROITE);
    draw_jambe(robot, JAMBE_GAUCHE);

    draw_bras(robot, BRAS_DROIT);
    draw_bras(robot, BRAS_GAUCHE);

    draw_tete(robot);
    draw_corps(robot);

    int i;
    for (i = 0 ; i < PARTIE_NB; i++ )
    {
        Mat_Mul_GaucheVersDroite2(robot->partie[i].matrix, robot->matrix);
        Object3D_Draw(robot->partie[i], false, worldCam, camClip, NULL);
    }

}

bool Robot_init(Robot* robot, const char* shader) {

    GLuint jambeTex = chargerTexture("../images/steve3.png", GL_NEAREST);
    if (jambeTex == 0)
        return false;

    GLuint skinTex = chargerTexture("../images/skin.bmp", GL_LINEAR);
    if (skinTex == 0)
        return false;

    Mesh* cubeMesh = Mesh_LoadBuiltin(MESH_CUBE_TEX_NORM);
    if (cubeMesh == NULL)
        return false;

    Mesh* brasMesh = Mesh_Load("../models/bras.obj");
    if (brasMesh == NULL)
        return false;

    Object3D tete = Object3D_Load("../models/tete.obj", shader);
    Object3D cube = Object3D_Create(cubeMesh, shader, jambeTex);
    Object3D corps = Object3D_Load("../models/corps.obj", shader);
    Object3D bras = Object3D_Create(brasMesh, shader, skinTex);

    loadIdentity(robot->matrix);

    int i;
    for (i = 0 ; i < PARTIE_NB ; i++ )
    {
        robot->partie[i] = cube;
//        loadIdentity(robot->partie[i].matrix);
    }

    robot->partie[TETE] = tete;
    robot->partie[CORPS] = corps;
    robot->partie[BRAS_DROIT] = bras;
    robot->partie[BRAS_GAUCHE] = bras;
    robot->partie[AVANT_BRAS_DROIT] = bras;
    robot->partie[AVANT_BRAS_GAUCHE] = bras;

    robot->collisionObject.type = COLLISION_SPHERE;
    robot->collisionObject.sphere.rayon = 10;
    robot->collisionObject.sphere.particule = Particule_Init(0.9, 99999);
    setVec3(&robot->collisionObject.sphere.particule.position, 0, 2, 0);

    return true;

}
