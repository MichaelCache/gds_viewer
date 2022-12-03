#version 330
layout(location = 0) in vec2 posVertex;
uniform mat4 uMvpMatrix;
out vec4 verPos;
void main() {
  gl_Position = uMvpMatrix * vec4(posVertex, 0.0f, 1.0f);
  verPos = gl_Position;
}