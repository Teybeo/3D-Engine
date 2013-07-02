#include "player.h"

Player Player_init(Robot* robot) {

    Player player = {};

    setVec3(&player.posCam, 0, 10, 10);
//    setVec3(&player.posRobot, 0, 50, 0);
    player.angleY = 0;
    player.angleX = 0;
    player.camMode = CAMERAMODE_FREE;
    player.robotMat = malloc(16*4);
    player.robot = robot;
    loadIdentity(player.robotMat);
    memset(player.key, 0, sizeof(player.key));

    return player;

}

void computeBasisVectors(Player* player) {

	float angleY = player->angleY * TAU/360;
	float angleX = player->angleX * TAU/360;

	// Calcul du vecteur x de la caméra
	// Quelque soit le mode, se déplacer latéralement n'a pas d'effet sur l'altitude de la caméra
	// étant donné qu'il n'existe meme pas d'angle de rotation autour de l'axe Z.
	// Le vecteur latéral est indépendant de l'angle de rotation autour de l'axe X (angleX) car C'EST l'axe X.
	player->lateralVec.x = cos(angleY);
	player->lateralVec.y = 0;
	player->lateralVec.z = sin(angleY);

	if (player->camMode == CAMERAMODE_FREE)
    {
		// Calcul du vecteur -z de la caméra
		// En mode libre, la caméra doit pouvoir monter dans l'air
		// si on avance/recule avec un angleVertical non-nul
		player->frontalVec.x =  sin(angleY) * cos(angleX);
		player->frontalVec.y = -sin(angleX);
		player->frontalVec.z = -cos(angleY) * cos(angleX);

	}
	else
    {
		// Calcul du vecteur -z de la caméra
		// En mode 1ère ou 3ème personne, la caméra ne doit pas pouvoir monter dans l'air
		// si on avance/recule avec un angleVertical non-nul
		// On n'utilise donc pas l'angleX dans le calcul
		player->frontalVec.x = sin(angleY);
		player->frontalVec.y = 0;
		player->frontalVec.z = -cos(angleY);

	}

}

// Cette fonction utilise les vecteurs calculés précédement
// pour déplacer véritablement la position du joueur
void applyVectors(Player* player) {

    Vec3 deplacement = {};

    static float t = 0;
    static float vitesseAnim = 0.1;

    if (!player->key[AVANCER] && !player->key[RECULER])
    {
        player->robot->angleJambeDroite = 0;
        player->robot->angleJambeGauche = 0;
        t = 0;
    }

    if (player->key[RALENTI])
        vitesseAnim = 0.09;
    else
        vitesseAnim = 0.2;

	// On avance la caméra
    if (player->key[AVANCER])
    {
        Vec3_Add(&deplacement, player->frontalVec);

        if (player->camMode != CAMERAMODE_FREE)
        {
            player->robot->angleJambeDroite = t;
            player->robot->angleJambeGauche = t - M_PI;
        }

        t += vitesseAnim;
    }

    // On recule la caméra
    if (player->key[RECULER])
    {
        Vec3 reculer = player->frontalVec;
        Vec3_Mul_Scal(&reculer, -1);
        Vec3_Add(&deplacement, reculer);

        if (player->camMode != CAMERAMODE_FREE)
        {
            player->robot->angleJambeDroite = t;
            player->robot->angleJambeGauche = t - M_PI;
        }

        t -= vitesseAnim;
    }

    // On décale vers la droite la caméra
    if (player->key[DROITE])
    {
        Vec3_Add(&deplacement, player->lateralVec);
    }

    // On décale vers la gauche la caméra
    if (player->key[GAUCHE])
    {
        Vec3 left = player->lateralVec;
        Vec3_Mul_Scal(&left, -1);
        Vec3_Add(&deplacement, left);
    }

    if (player->key[RALENTI])
    {
        Vec3_Mul_Scal(&deplacement, 0.09);
    }

    if (player->camMode == CAMERAMODE_FREE)
        Vec3_Add(&player->posCam, deplacement);
    else
        Vec3_Add(&player->posRobot, deplacement);

}

void buildMatrix(Player* player) {

	loadIdentity  (player->mondeToCam);

    // Pour la matrice de la caméra, on inverse le signe du déplacement
    // car il n'y a pas réellement de caméra qui se déplace dans le monde,
    // c'est elle qui doit faire en sorte de déplacer le monde
    if (player->camMode == CAMERAMODE_FREE)
    {
        rotate        (player->mondeToCam, player->angleX, player->angleY, 0);
        translateByVec(player->mondeToCam, Vec3_Mul_Scal_out(player->posCam, -1));
    }
    else
    {
        rotate        (player->mondeToCam, player->angleX, player->angleY , 0);
        translateByVec(player->mondeToCam, Vec3_Mul_Scal_out(player->posRobot, -1));
        //rotate        (player->mondeToCam, 0, -180 , 0);
    }

	if (player->camMode != CAMERAMODE_FREE)
	{
        // Arrivé ici, la caméra est à la même position que le robot,
        // On la déplace un peu pour adapter la vue
        // Comme pour le placement de la caméra, les transformations sont inversées
        if (player->camMode == CAMERAMODE_FIRST_PERSON)
		{
			translate(player->mondeToCam, 0, -11, 0);
			translateByVec(player->mondeToCam, Vec3_Mul_Scal_out(player->frontalVec, -4));
		}
	    else if (player->camMode == CAMERAMODE_THIRD_PERSON )
		{
			translate(player->mondeToCam, 0, -15, 0);
			translateByVec(player->mondeToCam, Vec3_Mul_Scal_out(player->frontalVec, 15));
		}

        // On change l'orientation du robot seulement si on presse une touche de déplacement
		if (player->key[AVANCER] || player->key[RECULER] || player->key[GAUCHE] || player->key[DROITE])
            player->angleYRobot = player->angleY;

		// On est en mode 1ère ou 3ème personne donc on déplace aussi le robot
		// en construisant sa matrice

		loadIdentity  (player->robot->matrix);
		translateByVec(player->robot->matrix, player->posRobot);
        rotate        (player->robot->matrix, 0, -player->angleYRobot+180, 0);
	}

}

void Player_keyEvent(Player* player, SDL_KeyboardEvent keyEv) {

    switch (keyEv.keysym.sym) {

    case SDLK_z:
        player->key[AVANCER] = keyEv.state;
        break;
    case SDLK_s:
        player->key[RECULER] = keyEv.state;
        break;
    case SDLK_q:
        player->key[GAUCHE] = keyEv.state;
        break;
    case SDLK_d:
        player->key[DROITE] = keyEv.state;
        break;
    case SDLK_LSHIFT:
        player->key[RALENTI] = keyEv.state;
        break;
	case SDLK_c:
		if (keyEv.type == SDL_KEYDOWN) {
			player->camMode++;
			player->camMode %= NB_CAMERAMODES;
            (player->camMode == CAMERAMODE_FREE) ? puts("Camera libre") :
            player->camMode == CAMERAMODE_THIRD_PERSON ? puts("Camera 3eme personne") : puts("Camera 1ere personne");

		}
		break;
    }
}

void Player_mouseMotionEvent(Player* player, float dPhi, float dTheta) {

    player->angleY += dPhi * 0.1;
    player->angleX += dTheta * 0.1;
}

void Player_mouseButtonEvent(Player* player, SDL_MouseButtonEvent ev) {

    if (player && ev.button) {}

}

void Player_update(Player* player) {

	computeBasisVectors(player);
	applyVectors(player);
	buildMatrix(player);

}

