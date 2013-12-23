#version 330 core

in vec3 fPosition_view;
in vec3 fNormal_view;
in vec2 texCoord;
in mat4 fWorldToView;

layout(binding = 0) uniform sampler2D tex_color;
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

    vec3 diffColor = vec3(0);
    vec3 specColor = vec3(0);
    normal_view = normalize(fNormal_view);

    for (int i = 0; i < 10 ; i++)
    {
        Light light = computePointLight(lightPos[i], lightColor[i]);

        diffColor += computeDiffuse(light);
        specColor += computeSpecular(light);
    }
    Light sun = computeDirectionalLight(sunDirection, sunColor);

    diffColor += computeDiffuse(sun);
    specColor += computeSpecular(sun);

    outputColor = (texture(tex_color, texCoord).rgb * diffColor * matDiff) + (specColor * matSpec);
//    outputColor = (texture(shadowMap, texCoord).rgb*2-1);
//    outputColor = normal_view;
//    outputColor = lightPos[6];
//    outputColor = fPosition_view;
//    outputColor = vec3((1-fPosition_view.z)/1000.);
//    outputColor = vec3(gl_FragCoord.z);

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
    direcLight.surfaceToLight = normalize(lightDirection); // Lumière à l'infini
    direcLight.color = color;
    direcLight.intensity = 1.;
    return direcLight;
}

Light computePointLight(vec3 lightPos, vec3 color) {

    lightPos = vec3(fWorldToView * vec4(lightPos, 1));

    Light pointLight;
    pointLight.surfaceToLight = normalize(lightPos - fPosition_view);
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
    return light.intensity * light.color * pow( max( dot(reflectedLight, normalize(fPosition_view)), 0.), clamp(matShininess, 100., 1000));
}
