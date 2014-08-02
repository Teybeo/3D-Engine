#version 420 core

layout(location = 0) in vec4 attrPosition;
// La matrice est décomposé en 4 vec4, chacun occupant un indice d'attribut, de 3 a 6 donc
layout(location = 3) in mat4 attrModelWorld;

uniform mat4 worldCam;
uniform mat4 camClip;

void main(void){

    gl_Position = camClip * worldCam * attrModelWorld * attrPosition;
}



