#version 330 core

in vec3 fPosition_view;
in vec3 fPosition_clip_fromLight;
in vec3 fNormal_view;
in vec2 texCoord;
in mat4 fWorldToView;
in mat3 fViewToTangent;

layout(binding = 0) uniform sampler2D colorTex;
layout(binding = 1) uniform sampler2D normalTex;
layout(binding = 2) uniform sampler2D shadowMap;

uniform vec3 lightPos[10];
uniform vec3 lightColor[10];
uniform vec3 matDiff;
uniform vec3 matSpec;
uniform int matShininess;
uniform vec3 sunDirection;

vec3 normal_view;

out vec3 outputColor;

struct Light {

    vec3 surfaceToLight; // Vecteur du fragment évalué à la source de lumière
    vec3 color;
    float intensity;
};

float calcAttenuation(vec3 lightVec);
vec3 computeDiffuse(Light light);
vec3 computeSpecular(Light light);
Light computePointLight(vec3 lightPos, vec3 color);
Light computeDirectionalLight(vec3 vector, vec3 color);


vec3 sunColor = vec3(1, .8, .7);

void main() {
//sunDirection = normalize(vec3(0.9, 1, 0.));
    vec3 diffColor = vec3(0.0);
    vec3 specColor = vec3(0);
    normal_view = normalize(fNormal_view);
    normal_view = normalize(texture(normalTex, texCoord).rgb*2 -1) ;
//    fPosition_view = fViewToTangent * fPosition_view;
    for (int i = 0; i < 10 ; i++)
    {
        Light light = computePointLight(lightPos[i], lightColor[i]);

//        diffColor += computeDiffuse(light);
        specColor += computeSpecular(light);
    }
    Light sun = computeDirectionalLight(sunDirection, sunColor);

    float occlusion = 1;
//    if (texture(shadowMap, vec2(fPosition_clip_fromLight)).z < fPosition_clip_fromLight.z -0.005)
//        occlusion = 0.;

//    if (occlusion == 1)
//        specColor += computeSpecular(sun);
//    diffColor += (computeDiffuse(sun) * occlusion);

    vec3 color = texture(colorTex, texCoord).rgb;
//    color = pow(color, vec3(1/2.2));
    outputColor = (color * diffColor * matDiff) + (specColor * matSpec);
//    outputColor = texture(colorTex, texCoord).rgba;
//    outputColor = lightPos[6];
//    outputColor = vec3(occlusion);
//    outputColor = vec3(specColor);
//    outputColor = normal_view;
//    outputColor = texture(normalTex, texCoord);
//    outputColor = (sunDirection+1)/2;
//    outputColor = shadow2D(shadowMap, (fPosition_clip_fromLight), 0.05);
//    outputColor = vec3(fPosition_clip_fromLight.z+1)/2;
//    outputColor = pow(outputColor, vec3(2.2));
}

#define ATTEN_CONST 1
#define ATTEN_LINEAR 0.001
#define ATTEN_QUADRA 0.0005

float calcAttenuation(vec3 lightVec) {

    float distance = length(lightVec);

    return 1. / (ATTEN_CONST +
                (ATTEN_LINEAR * distance) +
                (ATTEN_QUADRA * distance * distance) );

}

Light computeDirectionalLight(vec3 lightDirection, vec3 color) {

    lightDirection = vec3(fWorldToView * vec4(lightDirection, 0));

    Light direcLight;
    direcLight.surfaceToLight = fViewToTangent * normalize(lightDirection); // Lumière à l'infini
    direcLight.color = color;
    direcLight.intensity = 1;
    return direcLight;
}

Light computePointLight(vec3 lightPos, vec3 color) {

    lightPos = vec3(fWorldToView * vec4(lightPos, 1));

    Light pointLight;
    pointLight.surfaceToLight = fViewToTangent * normalize(lightPos - fPosition_view);
    pointLight.color = color;
    pointLight.intensity = calcAttenuation(lightPos - fPosition_view);
    return pointLight;
}

vec3 computeDiffuse(Light light) {

    return light.color * (light.intensity * max(dot(light.surfaceToLight, normal_view), 0.));
}

vec3 computeSpecular(Light light) {

    // On fait rebondir le rayon de lumière sur la surface grâce à sa normale
    vec3 reflectedLight = reflect(light.surfaceToLight, normal_view);

    // Plus les rayons refletés seront en direction de la caméra, plus il y aura de lumière à cet endroit
    // On est en espace caméra, cad que la caméra est en (0, 0, 0), donc la direction vers la caméra est surface - (0, 0, 0)
    return light.intensity * light.color * pow( max( dot(reflectedLight, normalize(fViewToTangent * fPosition_view)), 0.), clamp(matShininess, 10, 1000));
}

