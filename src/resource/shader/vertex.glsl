#version 330 core

layout(location = 0) in vec2 position;  // vertex position
layout(location = 1) in vec3 color;     // vertex color

out vec3 fragColor;  // pass color to fragment shader

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main() {
  gl_Position =
      cameraToView * worldToCamera * modelToWorld * vec4(position, 0.0f, 1.0f);
  // gl_Position = vec4(position, 100.0f, 1.0f);
  fragColor = color;
}
