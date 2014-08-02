#include "renderer.h"
#include "shader_library.h"

#include <stdlib.h>
#include <stdio.h>

void updateShadowMatrix(Renderer* renderer);
void Renderer_SetPerspective(Renderer* renderer);
void Renderer_GenerateShadowMap(Renderer* renderer);
void Renderer_RenderMeshesShadowed(Renderer* renderer);

float identity[16];

void Renderer_Render(Renderer* renderer) {

    Scene* scene = renderer->scene;

    glClear(GL_DEPTH_BUFFER_BIT);

    Vec3* light = Light_Serialize(scene->lampe, 6);
    Shader_SendUniformArray(ShaderLibrary_Get("instance"), "lightPos[0]", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(ShaderLibrary_Get("instance"), "lightColor[0]", GL_FLOAT_VEC3, 6, &light[6].x);
    Shader_SendUniformArray(ShaderLibrary_Get("fullset"), "lightPos[0]", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(ShaderLibrary_Get("fullset"), "lightColor[0]", GL_FLOAT_VEC3, 6, &light[6].x);
    free(light);

//    for (i = 1 ; i <2 ; i++ )
//        Plan_Draw(scene->planes[i], scene->player.mondeToCam, scene->fenetre.camToClip);

    Renderer_GenerateShadowMap(renderer);

    Renderer_RenderMeshesShadowed(renderer);


//    for (i = 0 ; i < scene->nb ; i++ )
//        Sphere_Draw(scene->balle[i], scene->player.mondeToCam, renderer->camToClip);

    loadIdentity(scene->skybox.matrix);
    if (scene->player.camMode == CAMERAMODE_FREE)
        translateByVec(scene->skybox.matrix, scene->player.posCam);
    else
        translateByVec(scene->skybox.matrix, scene->player.posRobot);
    scale(scene->skybox.matrix, 2000, 2000, 2000);
    Object3D_Draw(scene->skybox, renderer);

    // shadowmap texture to quad
    glDisable(GL_DEPTH_TEST);
        Object3D_Draw(scene->objects[1], renderer);
    glEnable(GL_DEPTH_TEST);
}

void Renderer_GenerateShadowMap(Renderer* renderer) {

    renderer->depth_rendering = true;

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->framebuffer);
    glViewport(0, 0, SHADOWMAP_W, SHADOWMAP_H);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
        // Render shadowmap for object 0
        Object3D_Draw(renderer->scene->objects[0], renderer);
        Robot_draw(renderer->scene->robot, renderer);
        Plan_Draw(renderer->scene->planes[0], renderer);
        Plan_Draw(renderer->scene->planes[1], renderer);
        Plan_Draw(renderer->scene->planes[2], renderer);
        SphereGroupe_Draw(renderer->scene->sphere, renderer);

    renderer->depth_rendering = false;

}

void Renderer_RenderMeshesShadowed(Renderer* renderer) {

    Scene* scene = renderer->scene;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Renderer_SetPerspective(renderer);
    glViewport(0, 0, renderer->largeur, renderer->hauteur);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderer->shadowMap);
    glActiveTexture(GL_TEXTURE0);

    Shader_SendUniform(ShaderLibrary_Get("fullset"), "depth_mvp", GL_FLOAT_MAT4, MatxMat_GaucheVersDroite(renderer->depth_mondeToCam, renderer->depth_camToProj));

    // Render object 0 with shadows
    Object3D_Draw(scene->objects[0], renderer);
    Plan_Draw(scene->planes[0], renderer);
    Plan_Draw(scene->planes[1], renderer);
    Plan_Draw(scene->planes[2], renderer);

    int i;
    for (i = 0 ; i < 6 ; i++ )
        Object3D_Draw(scene->lampe[i].object, renderer);

    Robot_draw(scene->robot, renderer);

    SphereGroupe_Draw(scene->sphere, renderer);
    BulletGroupe_Draw(scene->bullet, renderer);
//    Object3DGroupe_Draw(scene->groupe, renderer);

}

void Renderer_Update(Renderer* renderer) {

//    memcpy(&renderer->scene->player.posRobot, &renderer->scene->lampe[0].pos, sizeof(Vec3));
    Player_update(&renderer->scene->player);
    updateShadowMatrix(renderer);
//    memcpy(&renderer->scene->player.mondeToCam, &renderer->depth_mondeToCam, sizeof(float)*16);
//    memcpy(&renderer->camToClip, &renderer->depth_camToProj, sizeof(float)*16);

    ShaderLibrary_Refresh();

}

bool Renderer_Init(Renderer* renderer, Scene* scene, int largeur, int hauteur) {

    renderer->scene = scene;
    renderer->largeur = largeur;
    renderer->hauteur = hauteur;

    Renderer_SetPerspective(renderer);

    glGenFramebuffers(1, &renderer->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->framebuffer);

    glGenTextures(1, &renderer->shadowMap);
    glBindTexture(GL_TEXTURE_2D, renderer->shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOWMAP_W, SHADOWMAP_H, 0, GL_DEPTH_COMPONENT, GL_HALF_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderer->shadowMap, 0);

    scene->objects[1].mesh->material[0].texture = renderer->shadowMap;

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        puts("Framebuffer incomplete");
        return false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void updateShadowMatrix(Renderer* renderer) {

    static float angleY = 0;
    static float angleX = 40;

    loadIdentity(renderer->depth_mondeToCam);
    rotate(renderer->depth_mondeToCam, angleX, angleY, 0);

    Shader_SendUniform(ShaderLibrary_Get("instance"), "sunDirection", GL_FLOAT_VEC3, &renderer->depth_mondeToCam[8]);
    Shader_SendUniform(ShaderLibrary_Get("fullset"), "sunDirection", GL_FLOAT_VEC3, &renderer->depth_mondeToCam[8]);

    angleY += 0.1;
//    angleX += 0.1;
//    memcpy(renderer->depth_mondeToCam, renderer->player.mondeToCam, sizeof(float)*16);
            loadIdentity(renderer->depth_camToProj);
//            memcpy(renderer->depth_camToProj, renderer->fenetre.camToClip, sizeof(float)*16);
//            ortho(renderer->depth_camToProj, -15, 15,-15, 15, -5, 10);
            ortho(renderer->depth_camToProj, -150, 150,-150, 150, -120, 150);
//            ortho(renderer->depth_camToProj, -150, 150,-150, 150, -270, 150); // Sponza
    Shader_SendUniform(ShaderLibrary_Get("fullset"), "depth_mvp", GL_FLOAT_MAT4, MatxMat_GaucheVersDroite(renderer->depth_mondeToCam, renderer->depth_camToProj));
    Shader_SendUniform(ShaderLibrary_Get("instance"), "depth_mvp", GL_FLOAT_MAT4, MatxMat_GaucheVersDroite(renderer->depth_mondeToCam, renderer->depth_camToProj));

}

void Renderer_SetPerspective(Renderer* renderer) {

    glViewport(0, 0, renderer->largeur, renderer->hauteur);

    loadIdentity(renderer->camToClip);

    projection(renderer->camToClip, 70, (float)renderer->largeur/renderer->hauteur, 1, 5000);
}

void Renderer_Resize(Renderer* renderer, int w, int h) {

    renderer->largeur = w;
    renderer->hauteur = h;

    Renderer_SetPerspective(renderer);
}

void Renderer_keyEvent(Renderer* renderer, SDL_KeyboardEvent key) {

    if (key.type == SDL_KEYDOWN) {

        if (key.keysym.sym == SDLK_w)
            renderer->debug_wireframe = !renderer->debug_wireframe;

        if (key.keysym.sym == SDLK_b)
            renderer->debug_bitangents = !renderer->debug_bitangents;

        if (key.keysym.sym == SDLK_t)
            renderer->debug_tangents = !renderer->debug_tangents;

        if (key.keysym.sym == SDLK_n)
            renderer->debug_normals = !renderer->debug_normals;
    }

}
