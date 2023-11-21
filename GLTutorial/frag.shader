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
    vec3 textureAmbient = texture(material.texture_height1, TexCoords).rgb;
    vec3 textureDiffuse = texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 textureSpecular = texture(material.texture_specular1, TexCoords).rgb;

    vec3 ambientComponent = pointLight.ambient * textureAmbient;

    vec3 lightDir = normalize(pointLight.position - FragPos);
    vec3 normal = normalize(Normal);
    vec3 diffuseComponent = pointLight.diffuse * textureDiffuse * max(dot(normal, lightDir), 0.0f);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);        
    vec3 specularComponent = pointLight.specular * textureSpecular * max(pow(dot(viewDir, reflectDir), 128.0f), 0.0f);

    FragColor = vec4(ambientComponent + diffuseComponent + specularComponent, 1.0f);
}