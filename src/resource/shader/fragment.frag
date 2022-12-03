#version 330
out vec4 fragColor;
// in vec4 gl_FragCoord;
// vec4 strip() { if }

void main() {
  float a = 1.0;
  float deg = 45.;
  mat3 matrix = mat3(vec3(cos(deg), sin(deg), 0), vec3(-sin(deg), cos(deg), 0),
                     vec3(0, 0, 1));
  vec2 pp = (matrix * vec3(gl_FragCoord.x, gl_FragCoord.y, 1)).xy;
  if (mod(pp.x + 1.0, 10.0) - 1. > 0.) {
    a = 0.0;
  }
  fragColor = vec4(gl_FragCoord.xyz, a);
}