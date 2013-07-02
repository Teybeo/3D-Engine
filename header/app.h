#ifndef _APP_GUARD
#define _APP_GUARD

#include "fenetre.h"
#include "robot.h"
#include "light.h"
#include "instance.h"
#include "player.h"
#include "particule.h"
#include "collision.h"
#include "sphere.h"
#include "bullet.h"
#include "plan.h"

#include <stdbool.h>

#include "SDL.h"
#include "glew.h"

#define NB_BALLS_MAX 1000
#define NB_BULLETS_MAX 5

typedef struct App {

    Fenetre fenetre;
    GLint vertexShader;
    GLint fragmentShader;
    GLuint noTexNoLightProgram;
    GLuint texProgram;
    GLuint perVertexProgram;
    GLuint perFragmentProgram;
    GLuint instancePerFragmentProgram;
    Player player;
    GLuint locProjMatrix;
    Robot robot;
    Instance objects[10000];
    InstanceGroupe objectGroupe;
    Instance skybox;
    Light lampe[10];
    SphereGroupe sphereGroupe;
    BulletGroupe bulletGroupe;
    CollisionObject wall[6];
    Plan planes[3];

} App;

void App_Run(App* app);
bool App_Init(App* app);

#endif // APP
