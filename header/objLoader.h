#ifndef _OBJLOADER_GUARD
#define _OBJLOADER_GUARD

#include <stdbool.h>

#include "utils/vec2.h"
#include "utils/vec3.h"
#include "mesh.h"

bool loadObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nb, Vec2** range, int* nbObjects, char*** matNames, char* mtlFile);

bool loadRawObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nbVertices, char* mtlFile);
bool writeRawObj(const char* filename, Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices, const char* mtlFile);

bool loadIndexedObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nb, Vec2** range, int* nbObjects, char*** matNames, char* mtlFile);
bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nb);
bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices);

bool loadMtl(char* filename, Material** material, int* nbFinal);

void computeTangentSpace(Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices, Vec3* tangents, Vec3* bitangents);

#endif // OBJLOADER

