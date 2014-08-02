#version 430 core

#extension GL_ARB_explicit_uniform_location : require

in vec2 TexCoord;

uniform layout (location = 0)  sampler2D tex_color;

out vec3 outputColor;

void main() {

    outputColor = (texture(tex_color, TexCoord).rgb);

}



