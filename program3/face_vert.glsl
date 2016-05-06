#version 330

uniform mat4 projection;
uniform mat4 view;
uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 indoorLights[24];

in vec3 position;
in vec3 normal;
in vec2 uv;
in float ambient;

out vec3 fcolor;
out vec3 fposition;
out vec3 fnormal;
out vec3 light;
out vec2 fuv;
out float fambient;
out vec3 indoor[24];

void main() {
  gl_Position = projection * view * vec4(position, 1);
  fcolor = color;
  fnormal = vec3(transpose(inverse(view))*vec4(normal,1));
  fposition = vec3(view * vec4(position, 1));
  light = vec3(view * vec4(lightPos, 1));
  fuv = uv;
  fambient = ambient;

  for(int k = 0; k < 24; k++)
  {
    indoor[k] = vec3(view * vec4(indoorLights[k], 1));
  }
}
