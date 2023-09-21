#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientIntensity;
uniform vec3 lightPos;
uniform vec3 viewPos;

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

    vec3 result = (ambientComponent + diffuseComponent + specular) * objectColor;

    FragColor = vec4(result, 1.0);
}   