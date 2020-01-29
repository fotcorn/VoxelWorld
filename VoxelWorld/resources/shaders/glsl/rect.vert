#version 300 es

in vec3 vertex_position;

uniform mat4 mvp;
uniform vec3 color;

out vec3 frag_color;


void main() {
  frag_color = color;
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
