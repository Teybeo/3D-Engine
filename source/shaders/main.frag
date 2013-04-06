#version 420 core

in vec4 Color;
out vec4 outputColor;

void main()
{
   outputColor = Color*0.8;
}


