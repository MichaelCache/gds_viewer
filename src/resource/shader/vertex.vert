#version 330
layout(location = 0) in vec2 posVertex;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

out vec4 verPos;
void main() {
  gl_Position =
      cameraToView * worldToCamera * modelToWorld * vec4(posVertex, 0.0f, 1.0f);
  verPos = gl_Position;
}