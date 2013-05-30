#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 couleur;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

out vec4 Color;

void main(void){

    vec4 newPos = position;

// --------- Ce bloc marche ------------

//     ------- Modèle vers monde
//    newPos *= modelWorld;
//
//     ------- Monde vers caméra
//    newPos *= worldCam;
//
//     -------- Caméra vers homogènes
//    gl_Position = camClip * newPos;

// -----------------------------------------------

    // -------- Caméra vers homogènes
    gl_Position = camClip * worldCam * modelWorld * newPos;

//    gl_Position = camClip * worldCam * modelWorld * newPos;

    Color = couleur;

}

