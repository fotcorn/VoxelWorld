#version 410

in vec3 vertex_position;
in vec3 vertex_color;

out vec3 color;

uniform mat4 mvp;

void main() {
  color = vertex_color;
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
