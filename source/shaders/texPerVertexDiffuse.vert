#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;
uniform vec3 lightDir;

out vec2 TexCoord;
out float diffuseFactor;

void main(void){

    gl_Position = camClip * worldCam * modelWorld * position;

    TexCoord = texcoord;

    vec3 lightVec = lightDir - (modelWorld*position);

    diffuseFactor = dot(normalize(lightVec), normal);
    diffuseFactor /= ( lightVec.length());
}



