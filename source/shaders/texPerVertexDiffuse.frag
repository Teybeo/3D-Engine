#version 420 core

in vec2 TexCoord;
in float diffuseFactor;

uniform sampler2D texture;

out vec3 outputColor;

void main() {

   outputColor = diffuseFactor * texture(texture, TexCoord).rgb;

}




