#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientIntensity;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec3 ambientComponent = ambientIntensity * lightColor;

    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);

    vec3 diffuseComponent = max(dot(lightDirection, normal), 0.0) * lightColor;
            
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambientComponent + diffuseComponent + specular) * vec3(texture(texture2, texCoord));

    FragColor = vec4(result, 1.0);
}   