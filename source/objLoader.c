#include "objLoader.h"

#include "vec2.h"
#include "vec3.h"
#include <stdio.h>
#include "liste.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool loadObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb) {

    puts("\n----------- Model ---------");
    printf("Loading '%s'\n", filename);

    bool rawLoaded = false;
    bool indexedLoaded = false;

    rawLoaded = loadRawObj(filename, vertices, uvs, normals, nb);

    // Si pas de binaire, on charge le texte
    if (rawLoaded == false)
    {
        indexedLoaded = loadIndexedObj(filename, vertices, uvs, normals, nb);

        // On génère un binaire pour aller plus vite les prochaines fois
        if (indexedLoaded == true)
        {
            writeRawObj(filename, *vertices, *uvs, *normals, *nb);
            //writeUnindexedObj(filename, *vertices, *uvs, *normals, *nb); Debug
        }
        else // Sinon, pas de chance
        {
            return false;
        }
    }

    return true;
}

// Lecture d'un fichier en mode binaire, très rapide
bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "raw");

    printf("\tLoading raw OBJ file '%s' ... ", nameUnpacked);

    FILE* file = fopen(nameUnpacked, "rb");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

    fscanf(file, "%d\n", nbVertices);

    *vertices = malloc(sizeof(Vec3) * *nbVertices);
    *uvs = malloc(sizeof(Vec2) * *nbVertices);
    *normals = malloc(sizeof(Vec3) * *nbVertices);

    fread(*vertices, sizeof(Vec3), *nbVertices, file);
    fread(*uvs, sizeof(Vec2), *nbVertices, file);
    fread(*normals, sizeof(Vec3), *nbVertices, file);

    fclose(file);

//    printf("%d vertices in %d triangles\n", *nb, *nb/3);
    puts("Ok");

    return true;
}

// Lecture d'un fichier obj en mode texte, très lent
// Ces fichiers peuvent contenir des indices différents pour chaque attribut (vertex, uv, normal)
// OpenGl ne supporte pas cela, donc on désindexe les attributs en les mettant les uns à la suite des autres
bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb) {

    printf("\tLoading OBJ file '%s' ...", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

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
    puts("Ok");
    printf("\t%d vertices in %d triangles\n", *nb, *nb/3);

    free(vertexIndices);
    free(normalIndices);
    free(uvIndices);

    return true;

}

// Enregistre un modèle dans un fichier sous forme binaire, très rapide
bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "raw");

    printf("\tWriting raw OBJ file '%s' ...", nameUnpacked);

    FILE* file = fopen(nameUnpacked, "wb");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

//    char ligne[128] = "";
    fprintf(file, "%d\n", nbVertices);

    fwrite(vertices, sizeof(Vec3), nbVertices, file);
    fwrite(uvs, sizeof(Vec2), nbVertices, file);
    fwrite(normals, sizeof(Vec3), nbVertices, file);

    fclose(file);
    puts("Ok");

    return true;

}

// Enregistre un modèle dans un fichier sous forme texte
// Ce format est différent du .obj original car les attributs en sont pas indexés, lent
bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "unpacked");

    printf("\tWriting unindexed OBJ file '%s' ...", nameUnpacked);

    FILE* file = fopen(nameUnpacked, "w");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

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
    puts("Ok");

    return true;

}

// Charge un modèle à partir d'un fichier texte généré avec writeUnidexedObj
// Attributs non-indexés, lent
bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices) {

    char nameUnpacked[128] = "";
    strcpy(nameUnpacked, filename);
    strcat(nameUnpacked, "unpacked");

    printf("\tLoading OBJ unindexed file '%s' ...", nameUnpacked);

    FILE* file = fopen(nameUnpacked, "r");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

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
    puts("Ok");

    return true;
}

#if 0
bool loadOBJ(const char * path,
        std::vector<glm::vec3> & out_vertices,
        std::vector<glm::vec2> & out_uvs,
        std::vector<glm::vec3> & out_normals) {

    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> listeVertex;
    std::vector<glm::vec2> listeUv;
    std::vector<glm::vec3> listeNormal;


    FILE * file = fopen(path, "r");
    if( file == NULL ){
            printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
            return false;
    }

    while( 1 ){

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(file, "%s", lineHeader);
    if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

    // else : parse lineHeader

    if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            listeVertex.push_back(vertex);
    }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            listeUv.push_back(uv);
    }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            listeNormal.push_back(normal);
    }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                    printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                    return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
    }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
    }

    }

    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){

            // Get the indices of its attributes
            unsigned int vertexIndex = vertexIndices[i];
            unsigned int uvIndex = uvIndices[i];
            unsigned int normalIndex = normalIndices[i];

            // Get the attributes thanks to the index
            glm::vec3 vertex = listeVertex[ vertexIndex-1 ];
            glm::vec2 uv = listeUv[ uvIndex-1 ];
            glm::vec3 normal = listeNormal[ normalIndex-1 ];

            // Put the attributes in buffers
            out_vertices.push_back(vertex);
            out_uvs     .push_back(uv);
            out_normals .push_back(normal);

    }

    return true;
}
#endif
