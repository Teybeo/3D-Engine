#include "object3d.h"

#include "renderer.h"
#include "objLoader.h"
#include "shader.h"
#include "shader_library.h"
#include "texture.h"
#include "utils/matrix.h"
#include "utils/vec3.h"
#include "utils/vec2.h"

#include <stdlib.h>

void SendMaterial(Shader* shader, Material* material, Renderer* renderer) {

    if (material->hasTexture && material->texture != renderer->currentColorTex)
    {
        glBindTexture(GL_TEXTURE_2D, material->texture);
        renderer->currentColorTex = material->texture;
    }
    if (material->hasNormal && material->normalMap != renderer->currentNormalTex)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material->normalMap);
        renderer->currentNormalTex = material->normalMap;
        glActiveTexture(GL_TEXTURE0);
    }

    Shader_SendUniform(shader, "matDiff", GL_FLOAT_VEC3, &material->diffuse.x);
    Shader_SendUniform(shader, "matSpec", GL_FLOAT_VEC3, &material->specular.x);
    Shader_SendUniform(shader, "matShininess", GL_INT, &material->exponent);

}

void Object3D_Draw(Object3D object, Renderer* renderer) {

    Shader* shader = object.shader;
    float* mondeToCam = renderer->scene->player.mondeToCam;
    float* camToClip = renderer->camToClip;

    if (renderer->depth_rendering == true)
    {
        shader = ShaderLibrary_Get("depth");
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
    if (object.mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(object.mesh->vao);
        renderer->currentVAO = object.mesh->vao;
    }

    Shader_SendUniform(shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

    int i;
    for (i = 0 ; i < object.mesh->nb ; i++ )
    {
        if (renderer->depth_rendering == false)
            SendMaterial(shader, &object.mesh->material[i], renderer);

        if (object.mesh->material[0].hasNormal == true && (renderer->debug_tangents || renderer->debug_bitangents || renderer->debug_normals)
            && object.mesh->vbo_indices == 0)
        {
            Shader* debug_shader = ShaderLibrary_Get("noTexNoLight");
            glUseProgram(debug_shader->id);
            Shader_SendUniform(debug_shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(debug_shader, "camClip", GL_FLOAT_MAT4, camToClip);
            Shader_SendUniform(debug_shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

            glBegin(GL_LINES);
            int j;
            for (j=0; j < object.mesh->drawCount[i] ; j++)
            {
                int k = j + object.mesh->drawStart[i];
                Vec3 vertex = object.mesh->vertices[k];

                if (renderer->debug_tangents)
                {
                    glColor3f(1, 0, 0);
                    glVertex3fv(&vertex.x);
                    Vec3 tangent = object.mesh->tangents[k];
                    tangent = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(tangent, .1));
                    glVertex3fv(&tangent.x);
                }

                if (renderer->debug_bitangents)
                {
                    glColor3f(0, 1, 0);
                    glVertex3fv(&vertex.x);
                    Vec3 bitangent = object.mesh->bitangents[k];
                    bitangent = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(bitangent, .1));
                    glVertex3fv(&bitangent.x);
                }

                if (renderer->debug_normals)
                {
                    glColor3f(0, 0, 1);
                    glVertex3fv(&vertex.x);
                    Vec3 normal = object.mesh->normals[k];
                    normal = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(normal, .1));
                    glVertex3fv(&normal.x);
                }

            }
            glEnd();
            glUseProgram(renderer->currentShader);
        }
        if (renderer->debug_wireframe == true && object.mesh->vertices != NULL && object.mesh->vbo_indices == 0)
        {
            Shader* debug_shader = ShaderLibrary_Get("noTexNoLight");
            glUseProgram(debug_shader->id);
            Shader_SendUniform(debug_shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(debug_shader, "camClip", GL_FLOAT_MAT4, camToClip);
            Shader_SendUniform(debug_shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

            glBegin(GL_LINES);
            glColor3f(.7, .7, .7);
            int j;
            for (j=0; j < object.mesh->drawCount[i] ; j += 3)
            {
                int k = j + object.mesh->drawStart[i];

                glVertex3fv(&(object.mesh->vertices[k].x));
                glVertex3fv(&(object.mesh->vertices[k+1].x));

                glVertex3fv(&object.mesh->vertices[k].x);
                glVertex3fv(&object.mesh->vertices[k+2].x);

                glVertex3fv(&object.mesh->vertices[k+1].x);
                glVertex3fv(&object.mesh->vertices[k+2].x);
            }
            glEnd();
            glUseProgram(renderer->currentShader);

        }
        else if (object.mesh->vbo_indices != 0)
            glDrawElements(object.mesh->primitiveType, object.mesh->drawCount[i], GL_UNSIGNED_INT, NULL+object.mesh->drawStart[i]*4);
        else
            glDrawArrays(object.mesh->primitiveType, object.mesh->drawStart[i], object.mesh->drawCount[i]);
    }

}

// Charge un fichier obj complet (mesh + texture) et en fait une object
Object3D Object3D_Load(const char* objFile, const char* shader) {

    Object3D object = {};
    object.shader = ShaderLibrary_Get(shader);

    char texFile[256] = "";

    object.mesh = Mesh_FullLoad(objFile, texFile);
    // TODO: meilleur gestion d'erreur
    if (object.mesh == NULL)
        return object;

    loadIdentity(object.matrix);

    return object;
}

// Crée une object à partir d'un mesh et d'une texture déjà chargé en mémoire
Object3D Object3D_Create(Mesh* mesh, const char* shader, GLuint texture) {

    Object3D object = {};

    mesh->material[0].texture = texture;
    mesh->material[0].hasTexture = true;
    object.mesh = mesh;
    object.shader = ShaderLibrary_Get(shader);
    object.mesh->material[0].texture = texture;
    object.mesh->material[0].hasTexture = true;

    loadIdentity(object.matrix);

    return object;

}

void Object3DGroupe_Draw(Object3DGroupe groupe, Renderer* renderer) {

    if (groupe.shader->id != renderer->currentShader)
    {
        glUseProgram(groupe.shader->id);
        renderer->currentShader = groupe.shader->id;
        Shader_SendUniform(groupe.shader, "worldCam", GL_FLOAT_MAT4, renderer->scene->player.mondeToCam);
        Shader_SendUniform(groupe.shader, "camClip", GL_FLOAT_MAT4, renderer->camToClip);
    }
    if (groupe.mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(groupe.mesh->vao);
        renderer->currentVAO = groupe.mesh->vao;
    }

    int i;
    for (i = 0 ; i < groupe.mesh->nb ; i++ )
    {
        SendMaterial(groupe.shader, &groupe.mesh->material[i], renderer);
        if (groupe.mesh->vbo_indices != 0)
            glDrawElementsInstanced(groupe.mesh->primitiveType, groupe.mesh->drawCount[i], GL_UNSIGNED_INT, NULL+groupe.mesh->drawStart[i]*4, groupe.nbObject3Ds);
        else
            glDrawArraysInstanced(groupe.mesh->primitiveType, groupe.mesh->drawStart[i], groupe.mesh->drawCount[i], groupe.nbObject3Ds);
    }
}

Object3DGroupe Object3DGroupe_Create(Mesh* mesh, int nbObject3Ds, const char* shader, GLuint texture) {

    Object3DGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.mesh->material[0].hasTexture = true;
    groupe.mesh->material[0].texture = texture;

    groupe.nbObject3Ds = nbObject3Ds;
    groupe.shader = ShaderLibrary_Get(shader);
    groupe.texture = texture;

    groupe.matrix = malloc(sizeof(float*) * nbObject3Ds);
    int i;
    for (i = 0 ; i < nbObject3Ds ; i++ )
    {
        groupe.matrix[i] = malloc(sizeof(float)*16);
        loadIdentity(groupe.matrix[i]);
        translate(groupe.matrix[i], -400 + rand() % 800, rand() % 30, -400+rand() % 800);
        transpose(groupe.matrix[i]); // On transpose à la main
    }

    glGenBuffers(1, &groupe.matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);

    glBufferData(GL_ARRAY_BUFFER, nbObject3Ds * 16 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // Avec un tableau dynamique, si on copie tout d'un coup (BufferData), 2 octets seront occupée entre chaque matrice
    // Je ne sais pas pk, le seul moyen trouvé est d'envoyer chaque matrice séparément
    for (i = 0 ; i < nbObject3Ds ; i++ )
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

void uploadMatrix(Object3DGroupe groupe) {

    glBindBuffer(GL_ARRAY_BUFFER, groupe.matrixVBO);
    int i;
    for (i = 0 ; i < groupe.nbObject3Ds ; i++ )
        glBufferSubData(GL_ARRAY_BUFFER, ( i * 16 * 4), (16 * sizeof(float)), &(groupe.matrix[i][0]));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
