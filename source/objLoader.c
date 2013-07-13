#include "objLoader.h"

#include "vec2.h"
#include "vec3.h"
#include "liste.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool loadObj(const char* filename, Vec3** verticesFinal, Vec2** uvsFinal, Vec3** normalsFinal, int* nbFinal, char* texFile) {

    puts("\n----------- Mesh ---------");
    printf("Loading '%s'\n", filename);

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    int nb = 0;

    bool rawLoaded = false;
    bool indexedLoaded = false;

    //rawLoaded = loadRawObj(filename, &vertices, &uvs, &normals, &nb, texFile);

    // Si pas de binaire, on charge le texte
    if (rawLoaded == false)
    {
        indexedLoaded = loadIndexedObj(filename, &vertices, &uvs, &normals, &nb, texFile);

        // On génère un binaire pour aller plus vite les prochaines fois
        if (indexedLoaded == true)
        {
            writeRawObj(filename, vertices, uvs, normals, nb, texFile);
//            writeUnindexedObj(filename, vertices, uvs, normals, nb); //Debug
        }
        else // Sinon, pas de chance
        {
            return false;
        }
    }

    *verticesFinal = vertices;
    *uvsFinal = uvs;
    *normalsFinal = normals;
    *nbFinal = nb;

    return true;
}

// Lecture d'un fichier en mode binaire, très rapide
bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nbVertices, char* texFile) {

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

    char buffer[256] = "";

    fscanf(file, "%s\n", buffer);

    if (texFile != NULL)
        strcpy(texFile, buffer);

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
bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, int* nb, char* texFile) {

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

        if ( strcmp( lineHeader, "mtllib") == 0) // Material file
        {
            if (texFile != NULL)
                fscanf(file, "%s", texFile);
        }
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

    // On dump les listes d'indices dans des tableaux pour accélérer les accès
    unsigned int* vertexIndices = dumpListeToArray(listeVertexIndices);
    unsigned int* normalIndices = dumpListeToArray(listeNormalIndices);
    unsigned int* uvIndices     = dumpListeToArray(listeUvIndices);

    // On dump les listes de données dans des tableaux pour accélérer les accès
    Vec3* verticesDump = dumpVec3ListeToArray(listeVertex);
    Vec3* normalsDump  = dumpVec3ListeToArray(listeNormal);
    Vec2 *uvsDump      = dumpVec2ListeToArray(listeUv);

    // On alloue les tableaux finaux, tous de la meme taille
    *vertices = malloc(sizeof(Vec3) * nbVertexUniques);
    *normals  = malloc(sizeof(Vec3) * nbVertexUniques);
    *uvs      = malloc(sizeof(Vec2) * nbVertexUniques);

    int nbVertex  = getElemNumber(listeVertex);
    int nbNormals = getElemNumber(listeNormal);
    int nbUvs     = getElemNumber(listeUv);

    int i;
    // Ici on va passer vertex par vertex
    for( i = 0; i < nbVertexUniques; i++ )
    {
        // On récupère les indices de chaque donnée pour ce point
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];
        unsigned int uvIndex = uvIndices[i];

        // On va récupèrer les données dans les tableaux avec ces indices
        // Comme les données ont été empilées, les premières sont au bas de la pile
        // Les données sont enregistrées dans les tableaux finaux
        (*vertices)[nbVertexUniques - i - 1] = verticesDump[nbVertex - vertexIndex];
        (*normals)[nbVertexUniques - i - 1] = normalsDump[nbNormals - normalIndex];
        (*uvs)[nbVertexUniques - i - 1] = uvsDump[nbUvs - uvIndex];

    }

    *nb = nbVertexUniques;
    puts("Ok");
    printf("\t%d vertices in %d triangles\n", *nb, *nb/3);

    free(vertexIndices);
    free(normalIndices);
    free(uvIndices);
    free(verticesDump);
    free(normalsDump);
    free(uvsDump);

    return true;

}

// Enregistre un modèle dans un fichier sous forme binaire, très rapide
bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, int nbVertices, const char* texFile) {

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
    if (texFile == NULL)
        fputs("NULL\n", file);
    else
        fprintf(file, "%s\n", texFile);
    fprintf(file, "%d\n", nbVertices);

    fwrite(vertices, sizeof(Vec3), nbVertices, file);
    fwrite(uvs, sizeof(Vec2), nbVertices, file);
    fwrite(normals, sizeof(Vec3), nbVertices, file);

    fclose(file);
    puts("Ok");

    return true;

}

// Enregistre un modèle dans un fichier sous forme texte
// Ce format est différent du .obj original car les attributs ne sont pas indexés, lent
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
