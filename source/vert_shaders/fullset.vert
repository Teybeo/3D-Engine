#version 330 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;
layout(location = 3) in vec3 attrTangent;
layout(location = 4) in vec3 attrBitangent;

uniform bool has_normal_map;

uniform mat4 modelWorld;
uniform mat4 worldCam;
uniform mat4 camClip;
uniform mat4 depth_mvp;

uniform vec3 sunDirection;

out vec3 f_normal_view;
out vec2 texCoord;
out vec3 fPosition_clip_fromLight;
out mat4 fWorldToView;
out mat3 f_tangentToView;
out vec3 f_position_view;
out vec3 f_sunDirection_viewspace;

void main(void){
    // Pour aller de [-1, 1] à [0, 1]
    mat4 clipToRGB = mat4
    (
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    gl_Position = camClip * worldCam * modelWorld * attrPosition;

    texCoord = attrTexcoord;
    fWorldToView = worldCam;

    vec4 position_worldspace = modelWorld * attrPosition;

    f_position_view = vec3(worldCam * position_worldspace);

    f_normal_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrNormal), 0)));
//    f_normal_view  =  normalize(vec3((transpose(inverse(worldCam * modelWorld))) * vec4(attrNormal, 0) ));

    mat3 viewToTangent;

    if (has_normal_map == true)
    {
        vec3 fTangent_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrTangent), 0)));
        vec3 fBitangent_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(attrBitangent), 0)));
        //vec3 fBitangent_view = normalize(vec3(worldCam * modelWorld * vec4(normalize(cross(attrNormal , attrTangent)), 0)));

        f_tangentToView = mat3(fTangent_view, fBitangent_view, f_normal_view);
    }

    fPosition_clip_fromLight = vec3(clipToRGB * depth_mvp * position_worldspace);

    // Sun directionnal light
    f_sunDirection_viewspace = normalize(vec3(fWorldToView * vec4(sunDirection, 0)));

}
