#include "plan.h"

#include <stdlib.h>

Plan Plan_Create(Mesh* mesh, CollisionPlan collisionPlan, Shader program, GLuint texture) {

    Plan plan = {};
    plan.instance = Instance_Create(mesh, program, texture);

    plan.collisionData = calloc(1, sizeof(CollisionObject));
    plan.collisionData->plan = collisionPlan;
    plan.collisionData->type = COLLISION_PLAN;

    return plan;

}

void Plan_Draw(Plan plan, float* mondeToCam, float* camToClip) {

    loadIdentity(plan.instance.matrix);

    translateByVec(plan.instance.matrix, plan.collisionData->plan.pos);
    scale(plan.instance.matrix, plan.collisionData->plan.xLength, plan.collisionData->plan.xLength, plan.collisionData->plan.zLength);
    rotate(plan.instance.matrix, plan.collisionData->plan.angleX, plan.collisionData->plan.angleY, plan.collisionData->plan.angleZ);

    Instance_Draw(plan.instance, mondeToCam, camToClip);
}

void Plan_RotateBase(Plan* plan) {

    float base[16] = {};
    loadIdentity(base);
    rotate(base, plan->collisionData->plan.angleX, plan->collisionData->plan.angleY, plan->collisionData->plan.angleZ);

    matTo3Vec(base, &plan->collisionData->plan.x, &plan->collisionData->plan.normale, &plan->collisionData->plan.z);
}
