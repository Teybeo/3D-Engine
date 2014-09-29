#version 330 core

#define GAMMA 2.2

in vec3 fPosition_clip_fromLight;
in vec3 f_normal_view;
in vec2 texCoord;
in mat4 fWorldToView;
in mat3 f_tangentToView;
in vec3 f_position_view;
in vec3 f_sunToSurface_viewspace;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D shadowMap;
layout(binding = 3) uniform sampler2D specularMap;

uniform bool has_color_map;
uniform bool has_normal_map;
uniform bool has_specular_map;
uniform int matShininess;
uniform vec3 matDiff;
uniform vec3 matSpec;
uniform vec3 lightColor[10];
uniform vec3 lightPos[10];

float computeDiffuse(vec3 surfaceToLight, vec3 normal);
float computeSpecular(vec3 surfaceToCamera, vec3 lightToSurface_viewspace, vec3 normal);
float calcAttenuation(vec3 lightToSurface);

vec3 sun_color = vec3(1, .7, .6);
vec3 normal;
vec3 lightToSurface_viewspace;

out vec3 outputColor;

void main() {

    float occlusion = 1.0;

    // Compare what can be seen at the surface location from the light with the actual distance from the light
    // If the sun see something nearer (smaller depth value) than the surface, this means there is occlusion
    if (texture(shadowMap, vec2(fPosition_clip_fromLight) ).x < fPosition_clip_fromLight.z -0.0005)
        occlusion = 0.0;

    vec3 diffColor = vec3(0);
    vec3 specColor = vec3(0);
    vec3 material_diff = vec3(1);
    vec3 material_spec = vec3(1);

    if (has_color_map == true)
        material_diff = pow(texture(colorTex, texCoord).rgb, vec3(GAMMA));
    else
        material_diff = matDiff;

    if (has_normal_map == true)
        normal = normalize(f_tangentToView * (texture(normalTex, texCoord).rgb*2 -1));
    else
        normal = normalize(f_normal_view);

    if (has_specular_map == true)
        material_spec = texture(specularMap, texCoord).rgb;
    else
        material_spec = matSpec;

    // On est en espace caméra, cad que la caméra est en (0, 0, 0),
    // Donc la direction vers la caméra est: (0, 0, 0) - surface
    vec3 surfaceToCamera = normalize(0 - f_position_view);

    for (int i = 7; i < 8; i++)
    {
        lightToSurface_viewspace = f_position_view - vec3(fWorldToView * vec4(lightPos[i], 1));

        vec3 baseColor = lightColor[i] * calcAttenuation(lightToSurface_viewspace);

        lightToSurface_viewspace = normalize(lightToSurface_viewspace);

        diffColor += baseColor * computeDiffuse(lightToSurface_viewspace, normal);
        specColor += baseColor * 2.0 * computeSpecular(surfaceToCamera, lightToSurface_viewspace, normal);
    }

    if (occlusion == 1.0)
        specColor += sun_color * 1.0 * computeSpecular(surfaceToCamera, f_sunToSurface_viewspace, normal);

    diffColor += sun_color * computeDiffuse(f_sunToSurface_viewspace, normal) * occlusion;

    outputColor = material_diff * (0.02 + diffColor  + (specColor * material_spec));
//    outputColor = texture(normalTex, texCoord).rgb;
//    outputColor = vec3(specColor * material_spec);
//    outputColor =  vec3(occlusion);
//    outputColor = ((specColor * material_spec) + diffColor) * material_diff;
//    outputColor = lightToSurface_viewspace[2];
//    outputColor = normalize(f_tangentToView * surfaceToCamera);
//    outputColor = texture(shadowMap, vec2(fPosition_clip_fromLight)).xyz;
//    outputColor = vec3(1.0);
//    outputColor = vec3(fPosition_clip_fromLight.z+1)/2;
//    outputColor = vec3(has_normal_map);
//
//    tonemap(outputColor);

    outputColor = pow(outputColor, vec3(1.0/GAMMA));

}

float computeDiffuse(vec3 surfaceToLight, vec3 normal) {

    return max(dot(-surfaceToLight, normal), 0.);
}

float computeSpecular(vec3 surfaceToCamera, vec3 lightToSurface_viewspace, vec3 normal) {

    // On fait rebondir le rayon de lumière sur la surface grâce à sa normale
    vec3 reflectedLight = reflect(lightToSurface_viewspace, normal);

    // Plus les rayons refletés seront en direction de la caméra, plus il y aura de lumière à cet endroit
    return pow( max( dot(reflectedLight, surfaceToCamera), 0.), clamp(matShininess, 0, 1024));
}

#define ATTEN_CONST .2
#define ATTEN_LINEAR 0.1
#define ATTEN_QUADRA 0.01

float calcAttenuation(vec3 lightToSurface) {

    float distance = length(lightToSurface);

    return 1. / (ATTEN_CONST +
                (ATTEN_LINEAR * distance) +
                (ATTEN_QUADRA * distance * distance) );

}
