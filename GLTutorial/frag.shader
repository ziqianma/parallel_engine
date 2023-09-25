#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;    
};

uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    vec3 ambient = material.ambient * light.ambient;

    vec3 lightDirection = normalize(light.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
            
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}   