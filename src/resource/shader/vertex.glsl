#version 330 core

layout(location = 0) in vec2 position;  // vertex position
layout(location = 1) in vec3 color;     // vertex color

out vec3 fragColor;  // pass color to fragment shader

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
  gl_Position = projMatrix * viewMatrix * vec4(position, 0.0f, 1.0f);
  fragColor = color;
}
