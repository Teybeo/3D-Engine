#version 420 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;
// La matrice est décomposé en 4 vec4, chacun occupant un indice d'attribut, de 3 a 6 donc
layout(location = 3) in mat4 attrModelWorld;

uniform mat4 worldCam;
uniform mat4 camClip;

out vec2 texCoord;
out vec3 fNormal_view;
out vec3 fPosition_view;
out mat4 fWorldToView;

void main(void){

    texCoord = attrTexcoord;

    fPosition_view = vec3(worldCam * attrModelWorld * attrPosition);

    fNormal_view = vec3(normalize(worldCam * attrModelWorld * vec4(attrNormal, 0)));

    fWorldToView = worldCam;

    gl_Position = camClip * vec4(fPosition_view, 1);
}



