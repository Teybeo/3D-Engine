#include "objLoader.h"

#include "texture.h"
#include "utils/vec2.h"
#include "utils/vec3.h"
#include "utils/liste.h"
#include "utils/listeString.h"
#include "shader_library.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool loadObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nb, Vec2** range, int* nbObjects, char*** matNames, char* texFile) {

    puts("\n----------- Mesh ---------");
    printf("Loading '%s'\n", filename);

    Vec3* _vertices = NULL;
    Vec3* _normals = NULL;
    Vec2* _uvs = NULL;
    Vec2* _range = NULL;
    char** _matNames = NULL;
    int _nb = 0;
    int _nbObjects = 0;

    bool rawLoaded = false;
    bool indexedLoaded = false;

//    rawLoaded = loadRawObj(filename, &vertices, &uvs, &normals, &ranges, nbFinal, nbVertFinal, texFile);

    // Si pas de binaire, on charge le texte
    if (rawLoaded == false)
    {
        indexedLoaded = loadIndexedObj(filename, &_vertices, &_normals, &_uvs, &_nb, &_range, &_nbObjects, &_matNames, texFile);

        // On génère un binaire pour aller plus vite les prochaines fois
        if (indexedLoaded == true)
        {
//            writeRawObj(filename, _vertices, _normals, _uvs, _nb, texFile);
//            writeUnindexedObj(filename, vertices, uvs, normals, nb); //Debug
        }
        else // Sinon, pas de chance
        {
            return false;
        }
    }

    *vertices = _vertices;
    *uvs      = _uvs;
    *normals  = _normals;
    *nb       = _nb;
    *range    = _range;
    *nbObjects = _nbObjects;
    *matNames = _matNames;


    return true;
}

// Lecture d'un fichier en mode binaire, très rapide
bool loadRawObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nbVertices, char* texFile) {

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

// Lecture d'un fichier obj en mode texte, rapide
// Ces fichiers peuvent contenir des indices différents pour chaque attribut (vertex, uv, normal)
// OpenGl ne supporte pas cela, donc on désindexe les attributs en les mettant les uns à la suite des autres
bool loadIndexedObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nb, Vec2** range, int* nbObjects, char*** matNames, char* mtlFile) {

    printf("\tLoading OBJ file '%s' ...", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

    bool faceConstruction = false;

    ElemVec3* listeVertex = NULL;
    ElemVec3* listeUv = NULL;
    ElemVec3* listeNormal = NULL;

    ElemVec3* listeVertexIndices = NULL;
    ElemVec3* listeUvIndices = NULL;
    ElemVec3* listeNormalIndices = NULL;
    ElemVec3* listeRange = NULL;
    ElemString* listeMatNames = NULL;

    char lineHeader[128];
    char currentMat[128];
    int currentIndex = 0;
    int i;
    Vec3 vecTemp;

    while( 1 )
    {

        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);

        // Indices des données de chaque triangle
        if ( strcmp( lineHeader, "f" ) == 0 && res != EOF)
        {
            if (faceConstruction == false)
            {
                faceConstruction = true;
                listeRange = empiler(listeRange, Vec3_Create(currentIndex, 0, 0));
                listeMatNames = empilerStr(listeMatNames, currentMat);
                // Empile le nom du matériau actuel (usemtl)
                // Empile un nouvel élém d'intervalle avec l'index actuel pour début
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
            if (faceConstruction == true)
            {
                faceConstruction = false;
                // x Stocke l'index de départ, y stocke le nombre d'éléments
                listeRange->vec.y = currentIndex - listeRange->vec.x;
                // Met l'index de fin de l'intervalle actuel à la valeur actuelle de l'index
            }

            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.

            // Vertex
            if ( strcmp( lineHeader, "v" ) == 0 )
            {
                fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                listeVertex = empiler(listeVertex, vecTemp);
            }
            // Texture coordinate
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

            // Material file
            else if ( strcmp( lineHeader, "mtllib") == 0)
            {
                if (mtlFile != NULL)
                    fscanf(file, "%s", mtlFile);
            }
            // Material name
            else if ( strcmp( lineHeader, "usemtl") == 0)
            {
                fscanf(file, "%s", currentMat);
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

    *nb = nbVertexUniques;
    *nbObjects = getElemNumber(listeRange);
    *range = dumpVec2ListeToArray(listeRange);
    *matNames = dumpListeToArrayStr(listeMatNames);


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
    liberePile(listeRange);
    liberePileStr(listeMatNames);

    puts("Ok");
    printf("\t%d objects for %d vertices\n", *nbObjects, nbVertexUniques);

    return true;
}

bool loadMtl(char* filename, Material** material, int* nbFinal) {

    printf("\tLoading MTL file '%s' ...", filename);

    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        puts("Error");
        return false;
    }

    ElemString* pileName = NULL;
    ElemString* pileTexture = NULL;
    ElemString* pileNormalMap = NULL;
    ElemString* pileSpecularMap = NULL;
    ElemVec3* pileAmbient = NULL;
    ElemVec3* pileDiffuse = NULL;
    ElemVec3* pileSpecular = NULL;
    ElemVec3* pileExponent = NULL;

    char lineHeader[128];
    char chaine[128] = "";
    Vec3 vecTemp;
    int i;
    bool mtlIsBeingDefined = false;

    while( 1 )
    {
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);

        // Nouveau matériau, on stocke le nom
        if ( strcmp( lineHeader, "newmtl" ) == 0 && res != EOF)
        {
            if (mtlIsBeingDefined == false)
            {
                mtlIsBeingDefined = true;
            }

            fscanf(file, "%s", chaine);
            pileName = empilerStr(pileName, chaine);
            pileTexture = empilerStr(pileTexture, "");
            pileNormalMap = empilerStr(pileNormalMap, "");
            pileSpecularMap = empilerStr(pileSpecularMap, "");
            pileExponent = empiler(pileExponent, (Vec3){20, 0, 0});
            pileSpecular = empiler(pileSpecular, (Vec3){1, 1, 1});
            pileDiffuse = empiler(pileDiffuse, (Vec3){1, 1, 1});
            pileAmbient = empiler(pileAmbient, (Vec3){1, 1, 1});

        }
        else
        {
            if (mtlIsBeingDefined)
            {
                mtlIsBeingDefined = false;

            }
            // EOF = End Of File. Quit the loop.
            if (res == EOF)
                break;

            // Texture diffuse
            else if ( strcmp( lineHeader, "map_Kd" ) == 0 )
            {
                fscanf(file, "%s", chaine);
                strcpy(pileTexture->chaine, chaine);
            }
            // Normal map
            else if ( strcmp( lineHeader, "map_bump" ) == 0 || strcmp( lineHeader, "map_Disp" ) == 0 )
            {
                fscanf(file, "%s", chaine);
                strcpy(pileNormalMap->chaine, chaine);
            }
            // Specular map
            else if ( strcmp( lineHeader, "map_Ks" ) == 0)
            {
                fscanf(file, "%s", chaine);
                strcpy(pileSpecularMap->chaine, chaine);
            }

            // Couleur ambiante
            else if ( strcmp( lineHeader, "Ka") == 0)
            {
                fscanf(file, "%f %f %f", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                pileAmbient->vec = vecTemp;
            }
            // Couleur diffuse
            else if ( strcmp( lineHeader, "Kd") == 0)
            {
                fscanf(file, "%f %f %f", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                pileDiffuse->vec = vecTemp;
            }
            // Couleur spéculaire
            if ( strcmp( lineHeader, "Ks" ) == 0 )
            {
                fscanf(file, "%f %f %f\n", &vecTemp.x, &vecTemp.y, &vecTemp.z );
                pileSpecular->vec = vecTemp;
            }
            // Exponent spéculaire
            else if ( strcmp( lineHeader, "Ns" ) == 0 )
            {
                fscanf(file, "%f", &vecTemp.x);
                pileExponent->vec = vecTemp;
            }

            else
            {
                // Probably a comment, eat up the rest of the line
                char stupidBuffer[1000];
                fgets(stupidBuffer, 1000, file);
            }
        }

    }

    int nb = getElemNumberStr(pileName);

    char** names = dumpListeToArrayStr(pileName);
    char** textures = dumpListeToArrayStr(pileTexture);
    char** normalMaps = dumpListeToArrayStr(pileNormalMap);
    char** specularMaps = dumpListeToArrayStr(pileSpecularMap);
    Vec3* ambient = dumpVec3ListeToArray(pileAmbient);
    Vec3* diffuse = dumpVec3ListeToArray(pileDiffuse);
    Vec3* specular = dumpVec3ListeToArray(pileSpecular);
    unsigned int* exponent = dumpListeToArray(pileExponent);

    *material = malloc(sizeof(Material) * nb);

    for (i = 0 ; i < nb ; i++ )
    {
        (*material)[i].type = NONE;

        strcpy((*material)[i].nom, names[i]);
        if (strlen(textures[i]) > 0)
        {
            (*material)[i].texture = chargerTexture(textures[i], GL_LINEAR);
            (*material)[i].type |= COLOR_MAP;
        }

        if (strlen(normalMaps[i]) > 0)
        {
            (*material)[i].normalMap = chargerTexture(normalMaps[i], GL_LINEAR);
            (*material)[i].type |= NORMAL_MAP;
        }

        if (strlen(specularMaps[i]) > 0)
        {
            (*material)[i].specularMap = chargerTexture(specularMaps[i], GL_LINEAR);
            (*material)[i].type |= SPECULAR_MAP;
        }

        if ((*material)[i].type == NONE)
        {
            (*material)[i].shader = ShaderLibrary_Get("noTexNoLight");
            printf("Shader noTexNoLight choosen for [%s]\n", names[i]);
        }
        else
        {
            (*material)[i].shader = ShaderLibrary_Get("fullset");
            printf("Shader fullset choosen for [%s]\n", names[i]);
        }

        (*material)[i].ambient = ambient[i];
        (*material)[i].diffuse = diffuse[i];
        (*material)[i].specular = specular[i];
        (*material)[i].exponent = exponent[i];
    }

    *nbFinal = nb;

    fclose(file);

    puts("Ok");
    printf("\t%d Matériaux chargés\n", nb);

    return true;
}

// Les performances des .obj sont relativement suffisantes, ce code est à réparer
#if 0
// Enregistre un modèle dans un fichier sous forme binaire, très rapide
bool writeRawObj(const char* filename, Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices, const char* texFile) {

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
#endif
// Enregistre un modèle dans un fichier sous forme texte
// Ce format est différent du .obj original car les attributs ne sont pas indexés, lent
bool writeUnindexedObj(const char* filename, Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices) {

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
bool loadUnindexedObj(const char* filename, Vec3** vertices, Vec3** normals, Vec2** uvs, int* nbVertices) {

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

// TODO: compute the tangent space here, by computing a tangent an binormal for every vertex //
void computeTangentSpace(Vec3* vertices, Vec3* normals, Vec2* uvs, int nbVertices, Vec3* tangents, Vec3* bitangents) {

    int i;
    int j;
    Vec3 positions[3];
    Vec2 texCoords[3];

	for(i = 0; i < nbVertices - 2; i += 3){

		for( j = 0; j < 3; j++)
		{
			positions[j] = vertices[i + j];
            texCoords[j] = uvs[i + j];
        }

		Vec3 Q1 = Vec3_SubOut(positions[1], positions[0]);
		Vec3 Q2 = Vec3_SubOut(positions[2], positions[0]);

        Vec2 deltaUV_1 = Vec2_SubOut(texCoords[1], texCoords[0]);
        Vec2 deltaUV_2 = Vec2_SubOut(texCoords[2], texCoords[0]);

		float det = deltaUV_1.x * deltaUV_2.y - deltaUV_1.y * deltaUV_2.x;


		Vec3 tangent = Vec3_Create(1, 0, 0);
		Vec3 bitangent = Vec3_Create(0, 1, 0);
		if (det > 0.0000005) {
			tangent = Vec3_Mul_Scal_out(Vec3_SubOut(Vec3_Mul_Scal_out(Q1, deltaUV_2.y), Vec3_Mul_Scal_out(Q2, deltaUV_1.y)), (1.0f / det));
            bitangent = Vec3_Mul_Scal_out(Vec3_SubOut(Vec3_Mul_Scal_out(Q2, deltaUV_1.x), Vec3_Mul_Scal_out(Q1, deltaUV_2.x)), (1.0f / det));
		}

        tangent = Vec3_Normalise_Out(Vec3_SubOut(tangent, Vec3_Mul_Scal_out(normals[i], Vec3_Mul_Out(normals[i], tangent))));

		for ( j = 0; j < 3; j++)
        {
            tangents[i + j] = tangent;
            bitangents[i + j] = Vec3_Normalise_Out(bitangent);
		}

	}
	// Ende der Tangent Berechnung


//	unsigned int indexCount = meshData.indices.size();
//	for(unsigned int i = 0; i <(indexCount - 2); i += 3){
//		glm::vec3 v[3];
//		glm::vec2 t[3];
//
//		int positionIndex[3];
//		int textureIndex[3];
//		for( int j = 0; j < 3; ++j){
//
//			positionIndex[j] = 3 * meshData.indices[i+j];
//			textureIndex[j] = 2 * meshData.indices[i+j];
//
//
//			v[j] = glm::vec3(
//								meshData.vertex_position[positionIndex[j] + 0],
//								meshData.vertex_position[positionIndex[j] + 1],
//								meshData.vertex_position[positionIndex[j] + 2]
//							);
//
//			t[j] = glm::vec2(
//								meshData.vertex_texcoord[textureIndex[j] + 0],
//								meshData.vertex_texcoord[textureIndex[j] + 1]
//							);
//		}
//
//		glm::vec3 Q1 = v[1] - v[0];
//		glm::vec3 Q2 = v[2] - v[0];
//
//
//
//		GLfloat du1 = t[1].x - t[0].x;
//		GLfloat dv1 = t[1].y - t[0].y;
//
//		GLfloat du2 = t[2].x - t[0].x;
//		GLfloat dv2 = t[2].y - t[0].y;
//
//
//
//		GLfloat det = du1 * dv2 - dv1 * du2;
//
//
//		glm::vec3 tangent(1,0,0);
//		if(det != 0){
//			tangent = (Q1 * dv2 - Q2 *dv1) *  (1.0f / det);
//		}
//
//
//		for(int j = 0; j < 3; ++j){
//			meshData.vertex_tangent[positionIndex[j] +0] += tangent.x;
//			meshData.vertex_tangent[positionIndex[j] +1] += tangent.y;
//			meshData.vertex_tangent[positionIndex[j] +2] += tangent.z;
//		}
//	}
//	// Ende der Tangent Berechnung


	// Gram Schmidt apprach for reothogonalize tangent to normal
//
//	unsigned int vertexCount = meshData.vertex_position.size();
//	for(unsigned int i = 0; i < (vertexCount - 2); i += 3){
//		Vec3 tangent = Vec3(
//										meshData.vertex_tangent[i +0],
//										meshData.vertex_tangent[i +1],
//										meshData.vertex_tangent[i +2]
//									 );
//
//		tangent = glm::normalize(tangent);
//
//		Vec3 normal = Vec3(
//										meshData.vertex_normal[i +0],
//										meshData.vertex_normal[i +1],
//										meshData.vertex_normal[i +2]
//									);
//
//		GLfloat NdotT = glm::dot(normal, tangent);
//
//		for(unsigned int j = 0; j < 3; ++j){
//			tangent[j] = tangent[j] - NdotT * normal[j];
//		}
//		tangent = glm::normalize(tangent);
//
//
//		Vec3 binormal = glm::normalize(glm::cross(tangent, normal));
//
//
//
//		for(int j = 0; j < 3; ++j){
//			meshData.vertex_tangent[i+j] = tangent[j];
//			meshData.vertex_binormal[i+j] = binormal[j];
//		}
//
//
//	}
}
