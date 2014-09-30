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
#include <stdio.h>
#include <string.h>

void Object3D_DrawDepth(Object3D object, Renderer* renderer);

void SendMaterial(Shader* shader, Material* material, Renderer* renderer) {

    bool has_color_map = material->type & COLOR_MAP;
    bool has_normal_map = material->type & NORMAL_MAP;
    bool has_specular_map = material->type & SPECULAR_MAP;

    if (has_color_map && material->texture != renderer->currentColorTex)
    {
        glBindTexture(GL_TEXTURE_2D, material->texture);
        renderer->currentColorTex = material->texture;
    }
    if (has_normal_map && material->normalMap != renderer->currentNormalTex)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material->normalMap);
        renderer->currentNormalTex = material->normalMap;
        glActiveTexture(GL_TEXTURE0);
    }
    if (has_specular_map && material->specularMap != renderer->currentSpecularTex)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material->specularMap);
        renderer->currentSpecularTex = material->specularMap;
        glActiveTexture(GL_TEXTURE0);
    }

    Shader_SendUniform(shader, "has_normal_map", GL_BOOL, &has_normal_map);
    Shader_SendUniform(shader, "has_specular_map", GL_BOOL, &has_specular_map);
    Shader_SendUniform(shader, "matDiff", GL_FLOAT_VEC3, &material->diffuse.x);
    Shader_SendUniform(shader, "matSpec", GL_FLOAT_VEC3, &material->specular.x);
    Shader_SendUniform(shader, "matShininess", GL_INT, &material->exponent);

}

void Object3D_Draw(Object3D object, Renderer* renderer) {

    if (renderer->depth_rendering == true)
        return Object3D_DrawDepth(object, renderer);

    Shader* shader = NULL;
    Mesh* mesh = object.mesh;

    float* mondeToCam = renderer->scene->player.mondeToCam;
    float* camToClip = renderer->camToClip;

    if (mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(mesh->vao);
        renderer->currentVAO = mesh->vao;
    }

    int i;
    for (i = 0 ; i < mesh->nb ; i++ )
    {

        shader = mesh->material[i].shader;

        if (shader->id != renderer->currentShader)
        {
            glUseProgram(shader->id);
            renderer->currentShader = shader->id;
            Shader_SendUniform(shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(shader, "camClip", GL_FLOAT_MAT4, camToClip);
        }

        Shader_SendUniform(shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

        }

        SendMaterial(shader, &mesh->material[i], renderer);

        if (mesh->material[0].type & NORMAL_MAP && (renderer->debug_tangents || renderer->debug_bitangents || renderer->debug_normals)
            && mesh->vbo_indices == 0)
        {
            Shader* debug_shader = ShaderLibrary_Get("noTexNoLight");
            glUseProgram(debug_shader->id);
            Shader_SendUniform(debug_shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(debug_shader, "camClip", GL_FLOAT_MAT4, camToClip);
            Shader_SendUniform(debug_shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

            glBegin(GL_LINES);
            int j;
            for (j=0; j < mesh->drawCount[i] ; j++)
            {
                int k = j + mesh->drawStart[i];
                Vec3 vertex = mesh->vertices[k];

                if (renderer->debug_tangents)
                {
                    glColor3f(1, 0, 0);
                    glVertex3fv(&vertex.x);
                    Vec3 tangent = mesh->tangents[k];
                    tangent = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(tangent, .2));
                    glVertex3fv(&tangent.x);
                }

                if (renderer->debug_bitangents)
                {
                    glColor3f(0, 1, 0);
                    glVertex3fv(&vertex.x);
                    Vec3 bitangent = mesh->bitangents[k];
                    bitangent = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(bitangent, .2));
                    glVertex3fv(&bitangent.x);
                }

                if (renderer->debug_normals)
                {
                    glColor3f(0, 0, 1);
                    glVertex3fv(&vertex.x);
                    Vec3 normal = mesh->normals[k];
                    normal = Vec3_AddOut(vertex, Vec3_Mul_Scal_out(normal, .2));
                    glVertex3fv(&normal.x);
                }

            }
            glEnd();
            glUseProgram(renderer->currentShader);
        }
        if (renderer->debug_wireframe == true && mesh->vertices != NULL && mesh->vbo_indices == 0)
        {
            Shader* debug_shader = ShaderLibrary_Get("noTexNoLight");
            glUseProgram(debug_shader->id);
            Shader_SendUniform(debug_shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
            Shader_SendUniform(debug_shader, "camClip", GL_FLOAT_MAT4, camToClip);
            Shader_SendUniform(debug_shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

            glBegin(GL_LINES);
            glColor3f(.7, .7, .7);
            int j;
            for (j=0; j < mesh->drawCount[i] ; j += 3)
            {
                int k = j + mesh->drawStart[i];

                glVertex3fv(&(mesh->vertices[k].x));
                glVertex3fv(&(mesh->vertices[k+1].x));

                glVertex3fv(&mesh->vertices[k].x);
                glVertex3fv(&mesh->vertices[k+2].x);

                glVertex3fv(&mesh->vertices[k+1].x);
                glVertex3fv(&mesh->vertices[k+2].x);
            }
            glEnd();
            glUseProgram(renderer->currentShader);

        }
        else if (mesh->vbo_indices != 0)
            glDrawElements(mesh->primitiveType, mesh->drawCount[i], GL_UNSIGNED_INT, NULL+mesh->drawStart[i]*sizeof(GL_UNSIGNED_INT));
        else
            glDrawArrays(mesh->primitiveType, mesh->drawStart[i], mesh->drawCount[i]);
    }

}

void Object3D_DrawDepth(Object3D object, Renderer* renderer) {

    Shader* shader = ShaderLibrary_Get("depth");
    Mesh* mesh = object.mesh;

    float* mondeToCam = renderer->depth_mondeToCam;
    float* camToClip = renderer->depth_camToProj;

    if (mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(mesh->vao);
        renderer->currentVAO = mesh->vao;
    }

    if (shader->id != renderer->currentShader)
    {
        glUseProgram(shader->id);
        renderer->currentShader = shader->id;
        Shader_SendUniform(shader, "worldCam", GL_FLOAT_MAT4, mondeToCam);
        Shader_SendUniform(shader, "camClip", GL_FLOAT_MAT4, camToClip);
    }

    Shader_SendUniform(shader, "modelWorld", GL_FLOAT_MAT4, object.matrix);

    // If we're depth rendering, we don't care about the materials
    // So just draw the object in one big draw call and return
    // This only works when all submeshes are contiguous and in the same vbo
    int j;
    int count = 0;
    for (j = 0 ; j < mesh->nb ; j++ )
        count += mesh->drawCount[j];

    if (mesh->vbo_indices != 0)
        glDrawElements(mesh->primitiveType, count, GL_UNSIGNED_INT, NULL);
    else
        glDrawArrays(mesh->primitiveType, 0, count);

    return;
}

// Crée un object complet (mesh + matériaux + shaders) à partir d'un seul fichier .obj
Object3D Object3D_Load(const char* objFile) {

    Object3D object = {};

    object.mesh = Mesh_Load(objFile, MESH_LOAD_MTL);
    // TODO: meilleur gestion d'erreur
    if (object.mesh == NULL)
        return object;

    loadIdentity(object.matrix);

    return object;
}

// Force tous les shaders des matériaux du mesh à un même shader
void Object3D_ForceShader(Object3D* object, const char* shader_name) {

    Shader* shader = ShaderLibrary_Get(shader_name);

    int i;
    for (i = 0 ; i < object->mesh->nb ; i++ )
        object->mesh->material[i].shader = shader;

}

// Crée une object à partir d'un mesh et d'une texture déjà chargé en mémoire
Object3D Object3D_Create(Mesh* mesh, const char* shader, GLuint texture) {

    Object3D object = {};

    object.mesh = mesh;
    object.mesh->material[0].shader = ShaderLibrary_Get(shader);
    object.mesh->material[0].texture = texture;
    object.mesh->material[0].type |= COLOR_MAP;

    loadIdentity(object.matrix);

    return object;

}

void Object3DGroupe_Draw(Object3DGroupe groupe, Renderer* renderer) {

    Shader* shader = NULL;

    if (groupe.mesh->vao != renderer->currentVAO)
    {
        glBindVertexArray(groupe.mesh->vao);
        renderer->currentVAO = groupe.mesh->vao;
    }

    int i;
    for (i = 0 ; i < groupe.mesh->nb ; i++ )
    {
        shader = groupe.mesh->material[i].shader;
        if (shader->id != renderer->currentShader)
        {
            glUseProgram(shader->id);
            renderer->currentShader = shader->id;
            Shader_SendUniform(shader, "worldCam", GL_FLOAT_MAT4, renderer->scene->player.mondeToCam);
            Shader_SendUniform(shader, "camClip", GL_FLOAT_MAT4, renderer->camToClip);
        }
        SendMaterial(shader, &groupe.mesh->material[i], renderer);
        if (groupe.mesh->vbo_indices != 0)
            glDrawElementsInstanced(groupe.mesh->primitiveType, groupe.mesh->drawCount[i], GL_UNSIGNED_INT, NULL+groupe.mesh->drawStart[i]*4, groupe.nbObject3Ds);
        else
            glDrawArraysInstanced(groupe.mesh->primitiveType, groupe.mesh->drawStart[i], groupe.mesh->drawCount[i], groupe.nbObject3Ds);
    }
}

Object3DGroupe Object3DGroupe_Create(Mesh* mesh, int nbObject3Ds, const char* shader, GLuint texture) {

    Object3DGroupe groupe = {};

    groupe.mesh = mesh;
    groupe.mesh->material[0].type |= COLOR_MAP;
    groupe.mesh->material[0].texture = texture;

    groupe.nbObject3Ds = nbObject3Ds;
    groupe.mesh->material[0].shader = ShaderLibrary_Get(shader);
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

