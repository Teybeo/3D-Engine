#ifndef _PLAN_GUARD
#define _PLAN_GUARD

#include "collision.h"
#include "matrix.h"

typedef struct Plan {

    CollisionObject* collisionData;
    Instance instance;

} Plan;

void Plan_Draw(Plan plan, float* mondeToCam, float* camToClip);
Plan Plan_Create(Model* model, CollisionPlan collisionPlan, GLuint program, GLuint texture);
void Plan_RotateBase(Plan* plan);

#endif // PLAN

