#version 330

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 indoorLights[24];

in vec3 position;
in vec3 normal;

out vec3 fcolor;
out vec3 fposition;
out vec3 fnormal;
out vec3 light;
out vec3 indoor[24];
out vec3 fpos;

void main() {
  gl_Position = projection * view * model * vec4(position, 1);
  fcolor = color;
  fnormal = vec3(transpose(inverse(view * model))*vec4(normal,1));
  fposition = vec3(view * model * vec4(position, 1));
  light = vec3(view * vec4(lightPos, 1));
  fpos = vec3(model * vec4(position, 1));

  for(int k = 0; k < 24; k++)
  {
    indoor[k] = vec3(view * vec4(indoorLights[k], 1));
  }
}
