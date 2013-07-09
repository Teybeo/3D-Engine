#version 330 core

in vec3 position;
in mat4 frag_worldCam;
in vec3 fNormal;
in vec3 camNormal;
in vec2 texCoord;

uniform sampler2D texture;

uniform vec3 lightPos[10];
uniform vec3 lightColor[10];

vec3 normal;

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

void main() {

    vec3 diffColor = vec3(0);
    vec3 specColor = vec3(0);
    normal = normalize(fNormal);

    for (int i = 0; i < 10 ; i++)
    {
        Light light = computePointLight(lightPos[i], lightColor[i]);

        diffColor += computeDiffuse(light);
        specColor += computeSpecular(light);
    }

    outputColor = (texture(texture, texCoord).rgb * diffColor) + specColor;

}

#define ATTEN_CONST 0.01
#define ATTEN_LINEAR 0.01
#define ATTEN_QUADRA 0.000005

float calcAttenuation(vec3 lightVec) {

    float distance = length(lightVec);
//    if (distance > 200)
//        return 0.;
    return 1. / (ATTEN_CONST +
                (ATTEN_LINEAR * distance) +
                (ATTEN_QUADRA * distance * distance) );

}

Light computeDirectionalLight(vec3 vector, vec3 color) {

    Light direcLight;
    direcLight.pos = vector; // En théorie à l'infini, donc a vérifier cette ligne
    direcLight.vector = normalize(vector); // Lampe à l'infini
    direcLight.color = color;
    direcLight.intensity = 1;
    return direcLight;
}

Light computePointLight(vec3 lightPos, vec3 color) {

    Light pointLight;
    pointLight.pos = lightPos;
    pointLight.color = color;
    pointLight.vector = normalize(lightPos - position);
    pointLight.intensity = calcAttenuation(lightPos-position);
    return pointLight;
}

vec3 computeDiffuse(Light light) {

    return light.color * (light.intensity * max(dot(light.vector, normal), 0.));
}

vec3 computeSpecular(Light light) {

//    vec3 viewVector = normalize(-camPosition.xyz);
//    vec3 reflectedLight = 2.0 * dot(light.vector, normal) * normal - light.vector;

    vec4 camPosition = frag_worldCam * vec4(position, 1);
//    vec4 reflectedLight = reflect(-normalize(frag_worldCam * vec4(light.pos, 1) - camPosition), frag_worldCam * vec4(normal, 0));
    vec4 reflectedLight = reflect(-normalize(frag_worldCam * vec4(light.pos, 1) - camPosition), vec4(normalize(camNormal) , 0));
    if (dot(normal, light.vector) <= 0.0)
        return vec3(0);
    else
        return light.intensity * light.color * pow( max( dot(reflectedLight, normalize(-camPosition)), 0.), 100.);

}
