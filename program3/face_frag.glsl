#version 330

in vec3 fcolor;
in vec3 fposition;
in vec3 fnormal;
in vec3 light;

uniform vec3 lightColor;
uniform float lightBrightness;
uniform float ambient;

out vec4 color_out;

void main() {
    vec3 normal = normalize(fnormal);
    vec3 l = normalize(light - fposition);
    vec3 r = 2 * dot(normal, l) * normal - l;
    vec3 v = normalize(-1 * fposition);

    float diffuse = .8 * max(0, dot(normal, l));

    float specular = 0;
    if(dot(normal, l) >= 0)
    {
        specular = .1 * pow(max(0, dot(v, r)), 5);
    }

    color_out = vec4((fcolor * (lightColor * lightBrightness * diffuse)) + (fcolor * (lightColor * lightBrightness * ambient)) + (lightColor * lightBrightness * vec3(specular)), 1);
}
