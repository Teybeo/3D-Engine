#ifndef _RENDERER_GUARD
#define _RENDERER_GUARD

#include "scene.h"

typedef struct _Renderer {

    Scene* scene;
    GLuint framebuffer;
    GLuint shadowMap;
    float depth_mondeToCam[16];
    float depth_camToProj[16];
    float camToClip[16];
    int largeur, hauteur;
    unsigned int currentShader;
    unsigned int currentColorTex;
    unsigned int currentNormalTex;
    unsigned int currentVAO;

} Renderer;

void Renderer_Update(Renderer* renderer);
bool Renderer_Init(Renderer* renderer, Scene* scene, int largeur, int hauteur);
void Renderer_Render(Renderer* renderer);
void Renderer_Resize(Renderer* renderer, int w, int h);

#endif // RENDERER

