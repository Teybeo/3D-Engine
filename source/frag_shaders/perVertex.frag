#version 330 core

in vec2 TexCoord;
in float diffuseFactor;

uniform sampler2D tex_color;

out vec3 outputColor;

void main() {

   outputColor = diffuseFactor * texture(tex_color, TexCoord).rgb;

}




