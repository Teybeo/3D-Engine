
// Nombre d'octets à partir duquel commence les données des coordonées de texture
#define VBO_TEXCOORD_OFFSET (72)
// 1 * 2 * 3 * 3 * 4 = 72
// 1 face...
// de 2 triangles...
// de 3 points...
// de 3 coordonées...
// de 4 octets...

const float carre[] =
{
	// Avant
	-1,  1,  0, // Haut gauche
	-1, -1,  0, // Bas gauche
	 1, -1,  0, // Bas droite

	 1, -1,  0, // Bas droite
	 1,  1,  0, // Haut droite
	-1,  1,  0, // Haut gauche

    // Avant
    0, 1,
    0, 0,
    1, 0,

    1, 0,
    1, 1,
    0, 1,

};

//typedef struct Carre {
//
//    int drawStart = 0;
//    int drawCount = 6;
//    int nbAttrib = 2;
//    int locAttrib[2] = {0, 1};
//    int offsetAttrib[2] = {0, VBO_TEXCOORD_OFFSET};
//    int attribComponents[2] = {3, 2};
//
//} Carre;

const float carreNorm20[] =
{
     1,  0, -1, // Fond Droite
	-1,  0, -1, // Fond Gauche
    -1,  0,  1, // Avant Gauche

    -1,  0,  1, // Avant Gauche
	 1,  0,  1, // Avant Droite
	 1,  0, -1, // Fond Droite

    100, 100,
     0, 100,
     0,  0,

     0,  0,
    100,  0,
    100, 100,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

};

const float carreNorm[] =
{
     1,  0, -1, // Fond Droite
	-1,  0, -1, // Fond Gauche
    -1,  0,  1, // Avant Gauche

    -1,  0,  1, // Avant Gauche
	 1,  0,  1, // Avant Droite
	 1,  0, -1, // Fond Droite

    1, 1,
    0, 1,
    0, 0,

    0, 0,
    1, 0,
    1, 1,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

    0, 1, 0,
    0, 1, 0,
    0, 1, 0,

};
#define VBO_NORMAL_OFFSET (VBO_TEXCOORD_OFFSET + (6*2*4))
