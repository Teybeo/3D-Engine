#version 330 core

in vec2 TexCoord;

uniform layout (location = 0)  sampler2D tex_color;

out vec3 outputColor;

void main() {

    outputColor = (texture(tex_color, TexCoord).rgb);

}



