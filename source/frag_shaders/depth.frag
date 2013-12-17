#version 330 core

out float depth;

void main() {

    gl_FragDepth = gl_FragCoord.z;
}



