#ifndef _PLAN_GUARD
#define _PLAN_GUARD

#include "physics/collision.h"
#include "object3d.h"
#include "utils/matrix.h"
#include "shader.h"

typedef struct Plan {

    CollisionObject* collisionData;
    Object3D object;

} Plan;



void Plan_Draw(Plan plan, Renderer* renderer);
Plan Plan_Create(Mesh* mesh, CollisionPlan collisionPlan, const char* shader, GLuint texture);
void Plan_RotateBase(Plan* plan);

#endif // PLAN

