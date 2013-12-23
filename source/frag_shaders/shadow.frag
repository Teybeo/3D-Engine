#version 330 core

in vec3 fPosition_view;
in vec3 fPosition_clip_fromLight;
in vec3 fNormal_view;
in vec2 texCoord;
in mat4 fWorldToView;

layout(binding = 0) uniform sampler2D colorTex;
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

    vec3 pos;
    vec3 vector; // Vecteur du fragment évalué à la source de lumière
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
vec3 sunPos = normalize(vec3(0, 0.1, 1));
sunPos = normalize(-sunDirection);
    vec3 diffColor = vec3(0.0);
    vec3 specColor = vec3(0);
    normal_view = normalize(fNormal_view);
//    normal_view += normalize((texture(normal, texCoord)));

    for (int i = 0; i < 10 ; i++)
    {
        Light light = computePointLight(lightPos[i], lightColor[i]);

        diffColor += computeDiffuse(light);
        specColor += computeSpecular(light);
    }
    Light sun = computeDirectionalLight(sunDirection, sunColor);

    float occlusion = 1;
    if (texture(shadowMap, vec2(fPosition_clip_fromLight)).z < fPosition_clip_fromLight.z -0.005)
        occlusion = 0.2;

    if (occlusion == 1)
        specColor += computeSpecular(sun);
    diffColor += (computeDiffuse(sun) * occlusion);

    outputColor = (texture(colorTex, texCoord).rgb * diffColor * matDiff) + (specColor * matSpec);
//    outputColor = texture(colorTex, texCoord).rgba;
//    outputColor = lightPos[6];
//    outputColor = vec3(occlusion);
//    outputColor = normal_view;
//    outputColor = (sunPos+1)/2;
//    outputColor = shadow2D(shadowMap, (fPosition_clip_fromLight), 0.05);
//    outputColor = vec3(fPosition_clip_fromLight.z+1)/2;

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

Light computeDirectionalLight(vec3 vector, vec3 color) {

    vector = vec3(fWorldToView * vec4(vector, 0));

    Light direcLight;
    direcLight.pos = vector; // En théorie à l'infini, donc a vérifier cette ligne
    direcLight.vector = normalize(vector); // Lampe à l'infini
    direcLight.color = color;
    direcLight.intensity = 1;
    return direcLight;
}

Light computePointLight(vec3 lightPos, vec3 color) {

    lightPos = vec3(fWorldToView * vec4(lightPos, 1));

    Light pointLight;
    pointLight.pos = lightPos;
    pointLight.color = color;
    pointLight.vector = normalize(lightPos - fPosition_view);
    pointLight.intensity = calcAttenuation(lightPos - fPosition_view);
    return pointLight;
}

vec3 computeDiffuse(Light light) {

    return light.color * (light.intensity * max(dot(light.vector, normal_view), 0.));
}

vec3 computeSpecular(Light light) {

//    vec3 viewVector = normalize(-camPosition.xyz);
//    vec3 reflectedLight = 2.0 * dot(light.vector, normal) * normal - light.vector;

//    vec4 camPosition = normalize(fWorldToView * vec4(position, 1));
//    vec4 reflectedLight = reflect(-normalize(fWorldToView * vec4(light.pos, 1) - camPosition), normalize(vec4(transpose(inverse(mat3(fWorldToView))) * normal, 0)));
    vec3 reflectedLight = reflect(-normalize(light.pos - fPosition_view), normal_view);
    if (dot(normal_view, light.vector) <= 0.0)
        return vec3(0);
    else
        return light.intensity * light.color * pow( max( dot(normalize(reflectedLight), normalize(-fPosition_view)), 0.), clamp(matShininess, 100., 1000));

}

