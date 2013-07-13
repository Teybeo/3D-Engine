#version 330 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

out vec2 texCoord;
out vec3 fPosition_view;
out vec3 fNormal_view;
out mat4 fWorldToView;

void main(void){

    texCoord = attrTexcoord;
    fWorldToView = worldCam;

    fPosition_view = vec3(worldCam * modelWorld * attrPosition);

    fNormal_view = vec3(normalize(worldCam * modelWorld * vec4(attrNormal, 0)));
//    fNormal_view  =  normalize(vec3((transpose(inverse(worldCam * modelWorld))) * vec4(attrNormal, 0) ));

    gl_Position = camClip * worldCam * modelWorld * attrPosition;
}
