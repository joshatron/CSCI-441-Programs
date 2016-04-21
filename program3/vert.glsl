#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;
uniform vec3 lightPos;

in vec3 position;
in vec3 normal;

out vec3 fcolor;
out vec3 fposition;
out vec3 fnormal;
out vec3 light;

void main() {
  gl_Position = projection * view * model * vec4(position, 1);
  fcolor = color;
  fnormal = vec3(transpose(inverse(view * model))*vec4(normal,1));
  fposition = vec3(view * model * vec4(position, 1));
  light = vec3(view * vec4(lightPos, 1));
}
