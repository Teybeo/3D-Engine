#include "objLoader.h"

#include "vec2.h"
#include "vec3.h"
#include <stdio.h>
#include "liste.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool loadObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb) {

    if (loadRawObj(filename, vertices, uvs, normals, nb) == true)
    {
        return true;
    }
    else if (loadIndexedObj(filename, vertices, uvs, normals, nb) == true)
    {
        writeRawObj(filename, *vertices, *uvs, *normals, *nb);
        writeUnindexedObj(filename, *vertices, *uvs, *normals, *nb);

        return true;
    }
    else
        return false;

    return true;

}


bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb) {

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erreur ouverture de '%s'\n", filename);
        return false;
    }

    printf("Loading OBJ file '%s' ...\n", filename);

    ElemVec3* listeVertex = NULL;
    ElemVec3* listeUv = NULL;
    ElemVec3* listeNormal = NULL;

    ElemVec3* listeVertexIndices = NULL;
    ElemVec3* listeUvIndices = NULL;
    ElemVec3* listeNormalIndices = NULL;

    char lineHeader[128];
    Vec3 vecTemp;

    while( 1 )
    {

        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( lineHeader, "v" ) == 0 ) // Vertex
        {
            fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
            listeVertex = empiler(listeVertex, vecTemp);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 ) // Texture Coordinate
        {
            fscanf(file, "%f %f\n", &vecTemp.x, &vecTemp.y );
            listeUv = empiler(listeUv, vecTemp);
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 ) // Normal
        {
            fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
            listeNormal = empiler(listeNormal, vecTemp);
        }
        else if ( strcmp( lineHeader, "f" ) == 0 ) // Indices des vertex de chaque triangle
        {
            unsigned int triVertexIndex[3];
            unsigned int triUvIndex[3];
            unsigned int triNormalIndex[3];

            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &triVertexIndex[0], &triUvIndex[0], &triNormalIndex[0], &triVertexIndex[1], &triUvIndex[1], &triNormalIndex[1], &triVertexIndex[2], &triUvIndex[2], &triNormalIndex[2] );

            if (matches != 9)
            {
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }

            listeVertexIndices = empiler(listeVertexIndices, (Vec3){(float)triVertexIndex[0], 0, 0});
            listeVertexIndices = empiler(listeVertexIndices, (Vec3){(float)triVertexIndex[1], 0, 0});
            listeVertexIndices = empiler(listeVertexIndices, (Vec3){(float)triVertexIndex[2], 0, 0});

            listeUvIndices = empiler(listeUvIndices, (Vec3){(float)triUvIndex[0], 0, 0});
            listeUvIndices = empiler(listeUvIndices, (Vec3){(float)triUvIndex[1], 0, 0});
            listeUvIndices = empiler(listeUvIndices, (Vec3){(float)triUvIndex[2], 0, 0});

            listeNormalIndices = empiler(listeNormalIndices, (Vec3){(float)triNormalIndex[0], 0, 0});
            listeNormalIndices = empiler(listeNormalIndices, (Vec3){(float)triNormalIndex[1], 0, 0});
            listeNormalIndices = empiler(listeNormalIndices, (Vec3){(float)triNormalIndex[2], 0, 0});

        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }

    fclose(file);

    int nbVertexUniques = getElemNumber(listeVertexIndices);

    int nbVertex = getElemNumber(listeVertex);
    int nbUvs = getElemNumber(listeUv);
    int nbNormals = getElemNumber(listeNormal);

    *vertices = malloc(sizeof(Vec3) * nbVertexUniques);
    *uvs = malloc(sizeof(Vec2) * nbVertexUniques);
    *normals = malloc(sizeof(Vec3) * nbVertexUniques);

    unsigned int* vertexIndices = NULL;
    unsigned int* uvIndices = NULL;
    unsigned int* normalIndices = NULL;

    vertexIndices = dumpListeToArray(listeVertexIndices);
    normalIndices = dumpListeToArray(listeNormalIndices);
    uvIndices = dumpListeToArray(listeUvIndices);

    int i;
    // Ici on va passer vertex par vertex
    for( i = 0; i < nbVertexUniques; i++ )
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i ];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        Vec3 vertex = getElemByNumber(listeVertex, nbVertex - vertexIndex);
        Vec3 normal = getElemByNumber(listeNormal, nbNormals - normalIndex);
        Vec3 uv_temp = getElemByNumber(listeUv, nbUvs - uvIndex);
        Vec2 uv = (Vec2){uv_temp.x, uv_temp.y};

        (*vertices)[nbVertexUniques - i - 1] = vertex;
        (*normals)[nbVertexUniques - i - 1] = normal;
        (*uvs)[nbVertexUniques - i - 1] = uv;

    }

    *nb = nbVertexUniques;
    printf("%d vertices in %d triangles\n", *nb, *nb/3);
    printf("End '%s' loading\n", filename);

    free(vertexIndices);
    free(normalIndices);
    free(uvIndices);

    return true;

}


bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "raw");

    FILE* file = fopen(nameUnpacked, "wb");
    if (file == NULL)
    {
        printf("Erreur ouverture de '%s'\n", nameUnpacked);
        return false;
    }

    printf("Writing raw OBJ file '%s' ...\n", nameUnpacked);

//    char ligne[128] = "";
    fprintf(file, "%d\n", nbVertices);

    fwrite(vertices, sizeof(Vec3), nbVertices, file);
    fwrite(uvs, sizeof(Vec2), nbVertices, file);
    fwrite(normals, sizeof(Vec3), nbVertices, file);

    fclose(file);
    puts("End of writing");

    return true;

}

bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "raw");

    FILE* file = fopen(nameUnpacked, "rb");
    if (file == NULL)
    {
        printf("Erreur ouverture de '%s'\n", nameUnpacked);
        return false;
    }

    printf("Loading raw OBJ file '%s' ...\n", nameUnpacked);

    fscanf(file, "%d\n", nbVertices);

    *vertices = malloc(sizeof(Vec3) * *nbVertices);
    *uvs = malloc(sizeof(Vec2) * *nbVertices);
    *normals = malloc(sizeof(Vec3) * *nbVertices);

    fread(*vertices, sizeof(Vec3), *nbVertices, file);
    fread(*uvs, sizeof(Vec2), *nbVertices, file);
    fread(*normals, sizeof(Vec3), *nbVertices, file);

    fclose(file);

//    printf("%d vertices in %d triangles\n", *nb, *nb/3);
    printf("End '%s' loading\n", nameUnpacked);

    return true;
}

bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "unpacked");

    FILE* file = fopen(nameUnpacked, "w");
    if (file == NULL)
    {
        printf("Erreur ouverture de '%s'\n", nameUnpacked);
        return false;
    }

    printf("Writing unindexed OBJ file '%s' ...\n", nameUnpacked);

//    char ligne[128] = "";
    fprintf(file, "%d\n", nbVertices);

    int i;
    for (i = 0 ; i < nbVertices ; i++ )
    {
        fprintf(file, "%.4f %.4f %.4f | %.4f %.4f | %.4f %.4f %.4f\n", vertices[i].x, vertices[i].y, vertices[i].z, uvs[i].x, uvs[i].y, normals[i].x, normals[i].y, normals[i].z);
        if ((i+1) % 3 == 0)
            fputs("\n", file);
    }

    fclose(file);
    puts("End of writing");

    return true;

}

bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "unpacked");

    FILE* file = fopen(nameUnpacked, "r");
    if (file == NULL)
    {
        printf("Erreur ouverture de '%s'\n", nameUnpacked);
        return false;
    }

    printf("Loading OBJ unindexed file '%s' ...\n", nameUnpacked);

    fscanf(file, "%d\n", nbVertices);

    *vertices = malloc(sizeof(Vec3) * *nbVertices);
    *uvs = malloc(sizeof(Vec2) * *nbVertices);
    *normals = malloc(sizeof(Vec3) * *nbVertices);

    int matches;

    int i;
    for (i = 0 ; i < *nbVertices ; i++ )
    {
        matches = fscanf(file, "%f %f %f | %f %f | %f %f %f\n", &(*vertices)[i].x, &(*vertices)[i].y, &(*vertices)[i].z, &(*uvs)[i].x, &(*uvs)[i].y, &(*normals)[i].x, &(*normals)[i].y, &(*normals)[i].z);

        if (matches != 8)
        {
            printf("File can't be read by parser :-(\n");
            return false;
        }
    }

//    fseek(file, 0, SEEK_END);
//
//    printf("%ld\n", ftell(file));

    fclose(file);

//    printf("%d vertices in %d triangles\n", *nb, *nb/3);
    printf("End '%s' loading\n", nameUnpacked);

    return true;
}
