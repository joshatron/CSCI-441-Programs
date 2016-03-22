#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 light;
uniform vec3 color;
uniform float brightness;

in vec3 position;
out vec3 colorf;

void main() {
  gl_Position = projection * view * model * light * vec4(position, 1);
  colorf = color * brightness;
}
