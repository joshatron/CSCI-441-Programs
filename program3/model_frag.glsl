#version 330

in vec3 fcolor;
in vec3 fposition;
in vec3 fnormal;
in vec3 light;
in vec3 indoor[24];
in vec3 fpos;

uniform vec3 lightColor;
uniform float lightBrightness;
uniform float indoorBrightness[24];

out vec4 color_out;

void main() {
    vec3 lightOut = vec3(0,0,0);

    if(fpos.x < 79.9 && fpos.x > -79.9 && fpos.z < 79.9 && fpos.z > -79.9)
    {
        lightOut += fcolor * .3;
        for(int k = 0; k < 24; k++)
        {
            if(distance(indoor[k], fposition) < 50)
            {
                vec3 normal = normalize(fnormal);
                vec3 l = normalize(indoor[k] - fposition);
                vec3 r = 2 * dot(normal, l) * normal - l;
                vec3 v = normalize(-1 * fposition);

                float diffuse = .8 * max(0, dot(normal, l));

                float specular = 0;
                if(dot(normal, l) >= 0)
                {
                    specular = .1 * pow(max(0, dot(v, r)), 5);
                }

                lightOut += (20 / pow(distance(indoor[k], fposition), 2)) * lightColor * indoorBrightness[k] * fcolor * diffuse;
            }
        }
    }
    else
    {
        lightOut += fcolor * .5;
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

        lightOut += (fcolor * (lightColor * lightBrightness * diffuse)) + (lightColor * lightBrightness * vec3(specular));
    }

    color_out = vec4(lightOut, 1);
    color_out = vec4(0,0,0, 1);
}
