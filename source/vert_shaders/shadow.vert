#version 330 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

uniform mat4 depth_mvp;

out vec3 fPosition_view;
out vec3 fPosition_clip_fromLight;
out vec3 fNormal_view;
out vec2 texCoord;
out mat4 fWorldToView;

void main(void){

    // Pour aller de [-1, 1] à [0, 1]
    mat4 clipToRGB = mat4
    (
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    texCoord = attrTexcoord;
    fWorldToView = worldCam;
    fPosition_view = vec3(worldCam * modelWorld * attrPosition);

    fNormal_view = vec3(normalize(worldCam * modelWorld * vec4(attrNormal, 0)));
//    fNormal_view  =  normalize(vec3((transpose(inverse(worldCam * modelWorld))) * vec4(attrNormal, 0) ));

    gl_Position = camClip * worldCam * modelWorld * attrPosition;
//    gl_Position = depth_mvp * attrPosition;

    fPosition_clip_fromLight = vec3(clipToRGB * depth_mvp * modelWorld * attrPosition);

}
