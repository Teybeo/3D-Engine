#include "objLoader.h"

#include "utils/vec2.h"
#include "utils/vec3.h"
#include "utils/liste.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool loadObj(const char* filename, Vec3** verticesFinal, Vec2** uvsFinal, Vec3** normalsFinal, Vec2** rangesFinal, int* nbFinal, int* nbVertFinal, char* texFile) {

    puts("\n----------- Mesh ---------");
    printf("Loading '%s'\n", filename);

    Vec3* vertices = NULL;
    Vec3* normals = NULL;
    Vec2* uvs = NULL;
    Vec2* ranges = NULL;

    bool rawLoaded = false;
    bool indexedLoaded = false;

//    rawLoaded = loadRawObj(filename, &vertices, &uvs, &normals, &ranges, nbFinal, nbVertFinal, texFile);

    // Si pas de binaire, on charge le texte
    if (rawLoaded == false)
    {
        indexedLoaded = loadIndexedObj(filename, &vertices, &uvs, &normals, &ranges, nbFinal, nbVertFinal, texFile);

        // On génère un binaire pour aller plus vite les prochaines fois
        if (indexedLoaded == true)
        {
            writeRawObj(filename, vertices, uvs, normals, ranges, *nbFinal, *nbVertFinal, texFile);
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
    *rangesFinal = ranges;

    return true;
}

// Lecture d'un fichier en mode binaire, très rapide
bool loadRawObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, Vec2** ranges, int* nb, int* nbVertices, char* texFile) {

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

    fscanf(file, "%d %d\n", nbVertices, nb);

    *ranges = malloc(sizeof(Vec2) * *nb);
    *vertices = malloc(sizeof(Vec3) * *nbVertices);
    *uvs = malloc(sizeof(Vec2) * *nbVertices);
    *normals = malloc(sizeof(Vec3) * *nbVertices);

    fread(*ranges, sizeof(Vec2), *nb, file);
    fread(*vertices, sizeof(Vec3), *nbVertices, file);
    fread(*uvs, sizeof(Vec2), *nbVertices, file);
    fread(*normals, sizeof(Vec3), *nbVertices, file);

    fclose(file);

    puts("Ok");
    printf("\t%d objects for %d triangles\n", *nb, *nbVertices);

    return true;
}

// Lecture d'un fichier obj en mode texte, rapide
// Ces fichiers peuvent contenir des indices différents pour chaque attribut (vertex, uv, normal)
// OpenGl ne supporte pas cela, donc on désindexe les attributs en les mettant les uns à la suite des autres
bool loadIndexedObj(const char* filename, Vec3** vertices, Vec2** uvs, Vec3** normals, Vec2** ranges, int* nb, int* nbVertices, char* texFile) {

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
    ElemVec3* listeIntervalles = NULL;

    char lineHeader[128];
    Vec3 vecTemp;
    int i, currentIndex = 0;
    bool faceIsBeingDefined = false;

    while( 1 )
    {
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);

        // Indices des données de chaque triangle
        if ( strcmp( lineHeader, "f" ) == 0 && res != EOF)
        {
            if (faceIsBeingDefined == false)
            {
                faceIsBeingDefined = true;
                vecTemp.x = currentIndex;
                listeIntervalles = empiler(listeIntervalles, vecTemp);
            }

            for (i = 0 ; i < 3 ; i++ )
            {
                int matches = fscanf(file, "%f/%f/%f ", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                if (matches != 3)
                {
                    printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                    return false;
                }
                listeVertexIndices = empiler(listeVertexIndices, (Vec3){vecTemp.x, 0, 0});
                listeUvIndices = empiler(listeUvIndices, (Vec3){vecTemp.y, 0, 0});
                listeNormalIndices = empiler(listeNormalIndices, (Vec3){vecTemp.z, 0, 0});
            }

            currentIndex += 3;
        }
        else
        {
            if (faceIsBeingDefined)
            {
                faceIsBeingDefined = false;
                listeIntervalles->vec.y = currentIndex - listeIntervalles->vec.x;
            }
            // EOF = End Of File. Quit the loop.
            if (res == EOF)
                break;

            // Material file
            else if ( strcmp( lineHeader, "mtllib") == 0)
            {
                if (texFile != NULL)
                    fscanf(file, "%s", texFile);
            }
            // Vertex
            if ( strcmp( lineHeader, "v" ) == 0 )
            {
                fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                listeVertex = empiler(listeVertex, vecTemp);
            }
            // Texture Coordinate
            else if ( strcmp( lineHeader, "vt" ) == 0 )
            {
                fscanf(file, "%f %f\n", &vecTemp.x, &vecTemp.y );
                listeUv = empiler(listeUv, vecTemp);
            }
            // Normal
            else if ( strcmp( lineHeader, "vn" ) == 0 )
            {
                fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                listeNormal = empiler(listeNormal, vecTemp);
            }
            else
            {
                // Probably a comment, eat up the rest of the line
                char stupidBuffer[1000];
                fgets(stupidBuffer, 1000, file);
            }
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
    Vec2* uvsDump      = dumpVec2ListeToArray(listeUv);

    // On alloue les tableaux finaux, tous de la meme taille
    *vertices = malloc(sizeof(Vec3) * nbVertexUniques);
    *normals  = malloc(sizeof(Vec3) * nbVertexUniques);
    *uvs      = malloc(sizeof(Vec2) * nbVertexUniques);

    // Ici on va passer vertex par vertex
    for( i = 0; i < nbVertexUniques; i++ )
    {
        // On va récupèrer les données dans les tableaux avec les indices
        // Les données sont enregistrées dans les tableaux finaux
        (*vertices)[i] = verticesDump[vertexIndices[i]-1];
        (*normals) [i] = normalsDump[normalIndices[i]-1];
        (*uvs)     [i] = uvsDump[uvIndices[i]-1];
    }

    *ranges = dumpVec2ListeToArray(listeIntervalles);
    *nb = getElemNumber(listeIntervalles);
    *nbVertices = nbVertexUniques;

    free(vertexIndices);
    free(normalIndices);
    free(uvIndices);
    free(verticesDump);
    free(normalsDump);
    free(uvsDump);
    liberePile(listeVertex);
    liberePile(listeNormal);
    liberePile(listeUv);
    liberePile(listeVertexIndices);
    liberePile(listeNormalIndices);
    liberePile(listeUvIndices);
    liberePile(listeIntervalles);

    puts("Ok");
    printf("\t%d objects for %d triangles\n", *nb, nbVertexUniques);

    return true;

}

// Enregistre un modèle dans un fichier sous forme binaire, très rapide
bool writeRawObj(const char* filename, Vec3* vertices, Vec2* uvs, Vec3* normals, Vec2* ranges, int nb, int nbVertices, const char* texFile) {

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
    fprintf(file, "%d %d\n", nbVertices, nb);

    fwrite(ranges, sizeof(Vec2), nb, file);
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
