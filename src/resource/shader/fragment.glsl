#version 330 core
in vec3 fragColor;  // color from the vertex shader

out vec4 finalColor;  // output color of the pixel

void main() { finalColor = vec4(fragColor, 1.0f); }
