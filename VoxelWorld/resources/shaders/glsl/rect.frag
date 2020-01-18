#version 410

in vec3 frag_color;

out vec4 fragmentColor;

void main() {
  fragmentColor = vec4(frag_color, 1.0);
}
