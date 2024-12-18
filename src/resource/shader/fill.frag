#version 330
// out vec4 gl_FragColor;
uniform vec3 color;
in vec4 gl_FragCoord;

void main() {
  float a = 1.0;
  float deg = 45.;
  mat3 matrix = mat3(vec3(cos(deg), sin(deg), 0), vec3(-sin(deg), cos(deg), 0),
                     vec3(0, 0, 1));
  vec2 pp = (matrix * vec3(gl_FragCoord.x, gl_FragCoord.y, 1)).xy;
  if (mod(pp.x + 1.0, 10.0) - 1. > 0.) {
    a = 0.0;
  }
  gl_FragColor = vec4(color.rgb, 0.1);
}