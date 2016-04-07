#version 330

in vec3 colorf;

out vec4 color_out;

void main() {
  color_out = vec4(colorf, 1);
}
