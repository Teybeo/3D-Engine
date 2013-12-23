#include "renderer.h"
#include "shader_library.h"

#define SHADOWMAP_W 1024
#define SHADOWMAP_H SHADOWMAP_W

void updateShadowMatrix(Renderer* renderer);
void Renderer_SetPerspective(Renderer* renderer);

float identity[16];

void Renderer_Render(Renderer* renderer) {

    Scene* scene = renderer->scene;

    glClear(GL_DEPTH_BUFFER_BIT);

    int i;
    /*glUseProgram(scene->instancePerFragmentProgram);
        for (i = 0 ; i < 6 ; i++ )
        {
            char name[50] = "";
            sprintf(name, "lightPos[%d]", i);
            glUniform3fv(glGetUniformLocation(scene->instancePerFragment, name), 1, &scene->lampe[i].pos.x);
            sprintf(name, "lightColor[%d]", i);
            glUniform3fv(glGetUniformLocation(scene->instancePerFragment, name), 1, &scene->lampe[i].color.x);
        }
    glUseProgram(0);
    */
//    glUseProgram(scene->perFragment.id);
//        for (i = 0 ; i < 6 ; i++ )
//        {
//            char name[50] = "";
//            sprintf(name, "lightPos[%d]", i);
//            glUniform3fv(glGetUniformLocation(scene->perFragment.id, name), 1, &scene->lampe[i].pos.x);
//            sprintf(name, "lightColor[%d]", i);
//            glUniform3fv(glGetUniformLocation(scene->perFragment.id, name), 1, &scene->lampe[i].color.x);
//        }

    Vec3* light = Light_Serialize(scene->lampe, 6);
    Shader_SendUniformArray(ShaderLibrary_Get("perFragment"), "lightPos[0]", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(ShaderLibrary_Get("perFragment"), "lightColor[0]", GL_FLOAT_VEC3, 6, &light[6].x);
    Shader_SendUniformArray(ShaderLibrary_Get("instance"), "lightPos[0]", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(ShaderLibrary_Get("instance"), "lightColor[0]", GL_FLOAT_VEC3, 6, &light[6].x);
    Shader_SendUniformArray(ShaderLibrary_Get("shadow"), "lightPos[0]", GL_FLOAT_VEC3, 6, &light->x);
    Shader_SendUniformArray(ShaderLibrary_Get("shadow"), "lightColor[0]", GL_FLOAT_VEC3, 6, &light[6].x);
    free(light);

//        for (i = 0 ; i < 6 ; i++ )
//        {
//            char name[50] = "";
//            sprintf(name, "lightInput[%d].pos", i);
//            Shader_SendUniform(scene->perFragment, name, GL_FLOAT_VEC3, &scene->lampe[i].pos.x);
//            sprintf(name, "lightInput[%d].color", i);
//            Shader_SendUniform(scene->perFragment, name, GL_FLOAT_VEC3, &scene->lampe[i].color.x);
//        }
//    glUseProgram(0);

//    for (i = 1 ; i <2 ; i++ )
//        Plan_Draw(scene->planes[i], scene->player.mondeToCam, scene->fenetre.camToClip);

//    Object3DGroupe_Draw(scene->objectGroupe, scene->player.mondeToCam, scene->fenetre.camToClip);

    glBindFramebuffer(GL_FRAMEBUFFER, renderer->framebuffer);
    glViewport(0, 0, SHADOWMAP_W, SHADOWMAP_H);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
        // Render shadowmap for object 0
        Object3D_Draw(scene->objects[0], true, renderer->depth_mondeToCam, renderer->depth_camToProj, ShaderLibrary_Get("depth"));
        SphereGroupe_Draw(scene->sphere, true, renderer->depth_mondeToCam, renderer->depth_camToProj, ShaderLibrary_Get("depth"));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Renderer_SetPerspective(renderer);

    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, renderer->shadowMap);
    glActiveTexture(GL_TEXTURE0);

    Shader* shadow = ShaderLibrary_Get("shadow");
    Shader_SendUniform(shadow, "depth_mvp", GL_FLOAT_MAT4, MatxMat_GaucheVersDroite(renderer->depth_mondeToCam, renderer->depth_camToProj));

    // Render object 0 with shadows
    Object3D_Draw(scene->objects[0], false, scene->player.mondeToCam, renderer->camToClip, NULL);
    Plan_Draw(scene->planes[0], scene->player.mondeToCam, renderer->camToClip);

    for (i = 0 ; i < 6 ; i++ )
        Object3D_Draw(scene->lampe[i].object, false, scene->player.mondeToCam, renderer->camToClip, NULL);

    Robot_draw(scene->robot, scene->player.mondeToCam, renderer->camToClip);

    SphereGroupe_Draw(scene->sphere, false, scene->player.mondeToCam, renderer->camToClip, NULL);
    BulletGroupe_Draw(scene->bullet, scene->player.mondeToCam, renderer->camToClip);
    // shadowmap texture to quad
    glDisable(GL_DEPTH_TEST);
        Object3D_Draw(scene->objects[1], false, identity, renderer->camToClip, NULL);
    glEnable(GL_DEPTH_TEST);

//    for (i = 0 ; i < scene->nb ; i++ )
//        Sphere_Draw(scene->balle[i], scene->player.mondeToCam, renderer->camToClip);

    loadIdentity(scene->skybox.matrix);
    if (scene->player.camMode == CAMERAMODE_FREE)
        translateByVec(scene->skybox.matrix, scene->player.posCam);
    else
        translateByVec(scene->skybox.matrix, scene->player.posRobot);
    scale(scene->skybox.matrix, 2000, 2000, 2000);
    Object3D_Draw(scene->skybox, false, scene->player.mondeToCam, renderer->camToClip, NULL);

}

void Renderer_GenerateShadowMap(Renderer* renderer) {


}

void Renderer_Update(Renderer* renderer) {

//    memcpy(&app->player.posRobot, &app->lampe[0].pos, sizeof(Vec3));
    Player_update(&renderer->scene->player);
    updateShadowMatrix(renderer);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOWMAP_W, SHADOWMAP_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void updateShadowMatrix(Renderer* renderer) {

    static float angleY = 0;
    static float angleX = 40;
    loadIdentity(renderer->depth_mondeToCam);
    rotate(renderer->depth_mondeToCam, angleX, angleY, 0);

    Shader_SendUniform(ShaderLibrary_Get("shadow"), "sunDirection", GL_FLOAT_VEC3, &renderer->depth_mondeToCam[8]);
    Shader_SendUniform(ShaderLibrary_Get("perFragment"), "sunDirection", GL_FLOAT_VEC3, &renderer->depth_mondeToCam[8]);

    angleY += 0.1;
//    angleX += 0.1;
//    memcpy(renderer->depth_mondeToCam, renderer->player.mondeToCam, sizeof(float)*16);
            loadIdentity(renderer->depth_camToProj);
//            memcpy(renderer->depth_camToProj, renderer->fenetre.camToClip, sizeof(float)*16);
//            ortho(renderer->depth_camToProj, -15, 15,-15, 15, -5, 10);
            ortho(renderer->depth_camToProj, -150, 150,-150, 150, -120, 150);
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
