#version 420 core

in vec3 position;
in mat4 frag_worldCam;
in vec3 normal;
in vec3 camNormal;
in vec2 texCoord;

uniform sampler2D texture;

uniform vec3 lightPos[10];
uniform vec3 lightColor[10];

const float constAttenuation = 1;
const float linearAttenuation = 0.2;
const float quadraticAttenuation = 0.05;

out vec3 outputColor;

void main() {

    float diffuseFactor = 0;
    float specFactor = 0;
    vec3 lightVec;
    vec4 reflectedLight;
    float distance;
    float diffuseAttenuation = 0;
    vec4 camPosition = frag_worldCam * vec4(position, 1);

    int i;
    for (i = 0; i < 6 ; i++)
    {
//        if (lightPos[i].y < -2)
//            continue;

        lightVec = lightPos[i] - position;

        diffuseFactor = max(0.0, dot(normalize(lightVec), normal));

//        reflectedLight = reflect(-normalize(frag_worldCam * vec4(lightPos[i], 1) - camPosition), vec4(camNormal, 0));
        reflectedLight = reflect(-normalize(frag_worldCam * vec4(lightPos[i], 1) - camPosition), normalize(frag_worldCam * vec4(normal, 0)));

        specFactor = pow(max(0.0, dot(normalize(-camPosition), reflectedLight) ), 100);

        distance = lightVec.length();
        diffuseAttenuation = 1. / (constAttenuation + (linearAttenuation*distance) + (quadraticAttenuation*distance*distance) );

        outputColor += lightColor[i] * ( specFactor + (diffuseFactor * diffuseAttenuation));
    }

    outputColor *= texture(texture, texCoord).rgb;

}



