#ifndef _PLAN_GUARD
#define _PLAN_GUARD

#include "physics/collision.h"
#include "utils/matrix.h"
#include "shader.h"

typedef struct Plan {

    CollisionObject* collisionData;
    Instance instance;

} Plan;

void Plan_Draw(Plan plan, float* mondeToCam, float* camToClip);
Plan Plan_Create(Mesh* mesh, CollisionPlan collisionPlan, Shader program, GLuint texture);
void Plan_RotateBase(Plan* plan);

#endif // PLAN

