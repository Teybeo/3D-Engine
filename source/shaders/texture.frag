#version 420 core

in vec2 TexCoord;

uniform sampler2D texture;

out vec3 outputColor;

void main() {

   outputColor = texture(texture, TexCoord).rgb;

}



