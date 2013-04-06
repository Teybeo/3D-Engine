#version 420 core

layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texcoord;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

out vec2 TexCoord;

void main(void){

    gl_Position = camClip * worldCam * modelWorld * position;

    TexCoord = texcoord;

}


