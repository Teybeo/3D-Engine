#version 330 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;
layout(location = 3) in vec3 attrTangent;
layout(location = 4) in vec3 attrBitangent;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;

uniform mat4 depth_mvp;

out vec3 fPosition_view;
out vec3 fPosition_clip_fromLight;
out vec3 fNormal_view;
out vec2 texCoord;
out mat4 fWorldToView;
out mat3 fViewToTangent;

void main(void){

    gl_Position = camClip * worldCam * modelWorld * attrPosition;
//    gl_Position = depth_mvp * attrPosition;

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
//    fPosition_view = fViewToTangent * fPosition_view;

    fNormal_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrNormal), 0)));
//    fNormal_view  =  normalize(vec3((transpose(inverse(worldCam * modelWorld))) * vec4(attrNormal, 0) ));
    vec3 fTangent_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrTangent), 0)));
    vec3 fBitangent_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrBitangent), 0)));

    fViewToTangent = transpose(mat3(fTangent_view, fBitangent_view, fNormal_view));

    fPosition_clip_fromLight = vec3(clipToRGB * depth_mvp * modelWorld * attrPosition);



}
