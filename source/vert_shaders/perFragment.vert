#version 330 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

out vec2 texCoord;
out vec3 fNormal;
out vec3 position;
out vec3 camNormal;
out mat4 frag_worldCam;

void main(void){

    texCoord = attrTexcoord;
    position = vec3(modelWorld * attrPosition);

    vec4 temp = modelWorld * vec4(attrNormal, 0);

    fNormal = normalize( vec3(temp) );
    camNormal  =  vec3(worldCam * temp);
    frag_worldCam = worldCam;

    gl_Position = camClip * worldCam * vec4(position, 1);
}
