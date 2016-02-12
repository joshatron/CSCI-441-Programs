#version 330

in vec2 position;
in vec3 color_in;

uniform mat4 projection;

out vec4 color;

void main() {
  gl_Position = projection * vec4(position.x, position.y, 0, 1);
  color = vec4(color_in, 1.0);
}
