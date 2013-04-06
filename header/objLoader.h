#ifndef _OBJLOADER_GUARD
#define _OBJLOADER_GUARD

#include <stdbool.h>
#include "vec2.h"
#include "vec3.h"

bool loadObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb);

bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices);
bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices);

bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb);
bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb);
bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices);

#endif // OBJLOADER

