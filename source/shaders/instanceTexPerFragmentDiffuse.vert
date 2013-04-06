#version 420 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;
// La matrice est décomposé en 4 vec4, chacun occupant un indice d'attribut, de 3 a 6 donc
layout(location = 3) in mat4 attrModelWorld;

uniform mat4 worldCam;
uniform mat4 camClip;

out vec2 texCoord;
out vec3 normal;
out vec3 position;
out vec3 camNormal;
out mat4 frag_worldCam;

void main(void){

    texCoord = attrTexcoord;

    position = vec3(attrModelWorld * attrPosition);

    vec4 temp = attrModelWorld * vec4(attrNormal, 0);

    normal = normalize( vec3(temp) );

    camNormal  = normalize( vec3(worldCam * temp) );

    frag_worldCam = worldCam;

    gl_Position = camClip * worldCam * vec4(position, 1);
}



