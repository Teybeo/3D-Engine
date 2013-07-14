#ifndef _OBJLOADER_GUARD
#define _OBJLOADER_GUARD

#include <stdbool.h>

#include "vec2.h"
#include "vec3.h"

bool loadObj(const char* filename, Vec3** verticesFinal, Vec2** uvsFinal, Vec3** normalsFinal, Vec2** rangesFinal, int* nbFinal, int* nbVertFinal, char* texFile);

bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, Vec2** ranges, int* nb, int* nbVertices, char* texFile);
bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, Vec2* ranges, int nb, int nbVertices, const char* texFile);

bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, Vec2** ranges, int* nb, int* nbVertices, char* texFile);
bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb);
bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices);

#endif // OBJLOADER

