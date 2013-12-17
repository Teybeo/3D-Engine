#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

void main(void){

    // -------- Caméra vers homogènes
    gl_Position = camClip * worldCam * modelWorld * position;

}

