#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation
    float kc;
    float kl;
    float kq;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float u_time;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // emission
    vec3 emission = texture(material.emission, TexCoords+ vec2(0.0, u_time)).rgb * floor(vec3(1.f) - texture(material.specular, TexCoords).rgb);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.kc + light.kl * distance + light.kq * (distance * distance));

    vec3 result = attenuation * (ambient + diffuse + specular) + emission * (0.5f + 0.5f * sin(u_time));
    FragColor = vec4(result, 1.0);
}