#version 330 core
out vec4 FragColor;

in vec3 FragPos;

in vec3 Normal;
in vec2 TexCoords;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLight;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_height1;
};

uniform Material material;

uniform vec3 viewPos;

void main()
{
    FragColor = vec4(0.04, 0.28, 0.26, 1.0);
}