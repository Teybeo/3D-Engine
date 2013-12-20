#ifndef _APP_GUARD
#define _APP_GUARD

#include "fenetre.h"
#include "scene.h"
#include "renderer.h"

#include <stdbool.h>

#include "SDL.h"
#include "glew.h"

#define NB_BULLETS_MAX 4

typedef struct App {

    Fenetre fenetre;
    Scene* scene;
    Renderer renderer;

} App;

void App_Run(App* app);
bool App_Init(App* app);

#endif // APP
