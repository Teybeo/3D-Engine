#ifndef _APP_GUARD
#define _APP_GUARD

#include "fenetre.h"
#include "robot.h"
#include "light.h"
#include "instance.h"
#include "player.h"

#include <stdbool.h>

#include "SDL.h"
#include "glew.h"

typedef struct App {

    Fenetre fenetre;
    GLint vertexShader;
    GLint fragmentShader;
    GLuint mainProgram;
    GLuint texProgram;
    GLuint texPerVertexDiffuseProgram;
    GLuint texPerFragmentDiffuseProgram;
    GLuint instanceTexPerFragmentDiffuseProgram;
    Player player;
    GLuint locProjMatrix;
    Robot robot;
    Instance objects[10000];
    InstanceGroupe objectGroupe;
    Instance skybox;
    Light halogene[10];

} App;

void App_Run(App* app);
bool App_Init(App* app);

#endif // APP
