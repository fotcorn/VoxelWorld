#version 410

const int TEXTURE_WIDTH = 128;

in vec3 vertex_position;
in vec2 texture_coordinate;

out vec2 frag_texture_coordinate;

uniform mat4 mvp;
uniform int texture_offset;

void main() {
  frag_texture_coordinate = vec2(texture_coordinate.x + texture_offset * TEXTURE_WIDTH, texture_coordinate.y);
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
