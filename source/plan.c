#include "plan.h"

#include <stdlib.h>

Plan Plan_Create(Mesh* mesh, CollisionPlan collisionPlan, const char* shader, GLuint texture) {

    Plan plan = {};
    plan.object = Object3D_Create(mesh, shader, texture);

    plan.collisionData = calloc(1, sizeof(CollisionObject));
    plan.collisionData->plan = collisionPlan;
    plan.collisionData->type = COLLISION_PLAN;

    return plan;

}

void Plan_Draw(Plan plan, Renderer* renderer) {

    loadIdentity(plan.object.matrix);

    translateByVec(plan.object.matrix, plan.collisionData->plan.pos);
    scale(plan.object.matrix, plan.collisionData->plan.xLength, plan.collisionData->plan.xLength, plan.collisionData->plan.zLength);
    rotate(plan.object.matrix, plan.collisionData->plan.angleX, plan.collisionData->plan.angleY, plan.collisionData->plan.angleZ);

    Object3D_Draw(plan.object, renderer);
}

void Plan_RotateBase(Plan* plan) {

    float base[16] = {};
    loadIdentity(base);
    rotate(base, plan->collisionData->plan.angleX, plan->collisionData->plan.angleY, plan->collisionData->plan.angleZ);

    matTo3Vec(base, &plan->collisionData->plan.x, &plan->collisionData->plan.normale, &plan->collisionData->plan.z);
}
