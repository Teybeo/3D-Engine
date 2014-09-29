#version 420 core

layout(location = 0) in vec4 attrPosition;
layout(location = 1) in vec3 attrNormal;
layout(location = 2) in vec2 attrTexcoord;
// La matrice est décomposé en 4 vec4, chacun occupant un indice d'attribut, de 3 a 6 donc
layout(location = 3) in mat4 attrModelWorld;

uniform mat4 worldCam;
uniform mat4 camClip;
uniform vec3 sunDirection;
uniform mat4 depth_mvp;

out vec2 texCoord;
out vec3 f_normal_view;
out vec3 f_position_view;
out mat4 fWorldToView;
out vec3 f_sunToSurface_viewspace;
out vec3 fPosition_clip_fromLight;

// Pour aller de [-1, 1] à [0, 1]
const mat4 clipToRGB = mat4
(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
);

void main(void) {

    texCoord = attrTexcoord;
    fWorldToView = worldCam;

    f_position_view = vec3(worldCam * attrModelWorld * attrPosition);

    f_normal_view = vec3(normalize(worldCam * attrModelWorld * vec4(attrNormal, 0)));

    f_sunToSurface_viewspace = normalize(0 - vec3(fWorldToView * vec4(sunDirection, 0)));
    fPosition_clip_fromLight = vec3(clipToRGB * depth_mvp * attrModelWorld * attrPosition);

    gl_Position = camClip * vec4(f_position_view, 1);
}



