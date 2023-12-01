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

    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[4];

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

uniform vec3 viewPos;
uniform samplerCube skybox;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
    
void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos); 
    vec3 result = vec3(0.0);

    for (int i = 0; i < 4; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * material.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * material.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * material.specular * spec * vec3(texture(material.texture_specular1, TexCoords));

    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}