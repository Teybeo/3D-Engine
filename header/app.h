#ifndef _APP_GUARD
#define _APP_GUARD

#include "fenetre.h"
#include "shader.h"
#include "robot.h"
#include "light.h"
#include "instance.h"
#include "player.h"
#include "physics/particule.h"
#include "physics/collision.h"
#include "sphere.h"
#include "bullet.h"
#include "plan.h"

#include <stdbool.h>

#include "SDL.h"
#include "glew.h"

#define NB_BULLETS_MAX 4

typedef struct App {

    Fenetre fenetre;
    Shader noTexNoLight;
    Shader onlyTex;
    Shader perVertex;
    Shader perFragment;
    Shader instancePerFragment;
    Player player;
    Robot robot;
    Instance objects[10000];
    InstanceGroupe objectGroupe;
    Instance skybox;
    Light lampe[6];
    SphereGroupe sphereGroupe;
    BulletGroupe bulletGroupe;
    CollisionObject wall[6];
    Plan planes[3];

} App;

void App_Run(App* app);
bool App_Init(App* app);

#endif // APP
